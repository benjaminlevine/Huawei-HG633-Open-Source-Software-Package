/* RTSP helper for connection tracking. */

/* (C) 2008 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/ctype.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <net/checksum.h>
#include <net/tcp.h>

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <linux/netfilter/nf_conntrack_rtsp.h>
#include <linux/iqos.h>
#ifdef CONFIG_NF_CONNTRACK_PRI
#include <linux/netfilter/nf_conntrack_pri.h>
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("RTSP connection tracking helper");
MODULE_ALIAS("ip_conntrack_rtsp");



#if defined(CONFIG_SYSCTL) && defined(CONFIG_NF_CONNTRACK_RTSP) && defined(CONFIG_NF_CONNTRACK_PROC_COMPAT)
extern int alg_rtsp_enable;
#endif



#define RTSP_PORT 554

/* This is slow, but it's simple. --RR */
static char *rtsp_buffer;

static DEFINE_SPINLOCK(nf_rtsp_lock);

#define MAX_PORTS 8
static u_int16_t ports[MAX_PORTS];
static unsigned int ports_c;
module_param_array(ports, ushort, &ports_c, 0400);
MODULE_PARM_DESC(ports, "port numbers of RTSP servers");

#define RTSP_CHANNEL_MAX 8
static int max_outstanding = RTSP_CHANNEL_MAX;
module_param(max_outstanding, int, 0600);
MODULE_PARM_DESC(max_outstanding,
		 "max number of outstanding SETUP requests per RTSP session");

#if defined(CONFIG_NF_CONNTRACK_RTCP) || (defined(CONFIG_NF_CONNTRACK_RTCP_MODULE) && defined(CONFIG_MODULES))
#include <net/netfilter/nf_nat_helper.h>
#define NF_NEED_STRNCASECMP
#define NF_NEED_STRTOU16
#include <linux/netfilter_helpers.h>

#define SKIP_WSPACE(ptr,len,off) while(off < len && isspace(*(ptr+off))) { off++; }

//#define RTCP_TIMEOUT 300
//static unsigned int rtcp_timeout = RTCP_TIMEOUT;

static int rtsp_debug = 0;
module_param(rtsp_debug, int, 0600);
MODULE_PARM_DESC(rtsp_debug, "rtsp_debug=1 is turn on debug messages");
#define RTSP_DEBUG_MSG(fmt, arg...)  if (rtsp_debug) printk (KERN_DEBUG "RTSP[%u]: " fmt , __LINE__ , ## arg)

struct nf_conntrack_helper nf_conntrack_rtcp_helper;
EXPORT_SYMBOL_GPL(nf_conntrack_rtcp_helper);

unsigned int (*nf_nat_rtcp_hook)(struct sk_buff **pskb,
                enum ip_conntrack_info ctinfo,
                struct nf_conn *ct,
                const char *dptr,
                __be32 newip,
                u_int16_t port) __read_mostly;
EXPORT_SYMBOL_GPL(nf_nat_rtcp_hook);

int (*nf_conntrack_set_rtp_hook)(struct sk_buff *skb,
                struct nf_conn *ct,
                enum ip_conntrack_info ctinfo,
                const char *dptr) __read_mostly;
EXPORT_SYMBOL_GPL(nf_conntrack_set_rtp_hook);

#define RTCP_RSR_PT                     0xcd    /* 205 */
#define RTCP_RSR_FMT                    5       /* RSR */
#define RTCP_NACK_PT                    0xcd    /* 205 */
#define RTCP_NACK_FMT                   1       /* NACK */

#define RTCP_BYE_PT                     0xcb    /* 203 */
#define RTCP_BYE_FMT                    1       /* NACK */

#define RTCP_UDP_HDR_LEN                8
#define RTCP_IPV4_LEN                   4
#define RTCP_HEAD_LEN(len)              (((len)+1)*4)

typedef struct rtcp_hdr
{
#if defined( __LITTLE_ENDIAN_BITFIELD )
    unsigned char  fmt : 5;
    unsigned char  pad : 1;
    unsigned char  ver : 2;
#elif defined (__BIG_ENDIAN_BITFIELD)
    unsigned char  ver : 2;
    unsigned char  pad : 1;
    unsigned char  fmt : 5;
#endif
    unsigned char pt;
    unsigned short len;
}rtcp_hdr;

typedef struct rtcp_rsr_data
{
    unsigned long   ssrc_sender;
    unsigned long   ssrc_media_source;
    unsigned char   adaptive[RTCP_IPV4_LEN];
    unsigned char   ssrc_current[RTCP_IPV4_LEN];
    unsigned long   user_ip;
    unsigned short  user_port;
    unsigned short  reserve;
}rtcp_rsr_data;

typedef struct rtcp_nack_data
{
    unsigned short   reserve;
    unsigned short   client_port;
    unsigned long    client_ip;
}rtcp_nack_data;

static inline void dump_tuple_ip(const struct nf_conntrack_tuple *t)
{
    if (0 == rtsp_debug)
        return;
    
	printk("    tuple %p: %u %pI4:%hu -> %pI4:%hu\n",
	       t, t->dst.protonum,
	       &t->src.u3.ip, ntohs(t->src.u.all),
	       &t->dst.u3.ip, ntohs(t->dst.u.all));
}

/*
eg1:
    rtp_ct -->    
        kill_fccport: org src[10.251.64.245 : 8026] -> dst[10.66.0.223 : 3028]          
        kill_fccport: rep src[192.168.1.4 : 3028] -> dst[10.251.64.245 : 8026]    
        
    ct(ct_master) -->
        kill_fccport: org src[192.168.1.4 : 3029] -> dst[10.251.64.245 : 8027]          
        kill_fccport: rep src[10.251.64.245 : 8027] -> dst[10.66.0.223 : 3029] 

eg2:
    ct_master -->
        kill_fccport: org src[192.168.1.4 : 3013] -> dst[10.251.50.139 : 8027]          
        kill_fccport: rep src[10.251.50.139 : 8027] -> dst[10.66.0.223 : 3013] 

    ct -->
        kill_fccport: org src[10.251.64.252 : 8027] -> dst[10.66.0.223 : 3013]          
        kill_fccport: rep src[192.168.1.4 : 3013] -> dst[10.251.64.252 : 8027]   

    rtp_ct --> 
        kill_fccport: org src[10.251.64.252 : 8026] -> dst[10.66.0.223 : 3012]          
        kill_fccport: rep src[192.168.1.4 : 3012] -> dst[10.251.64.252 : 8026]          
*/
static inline bool match_rtp_ct(const struct nf_conn *ct_master, struct nf_conn *node_ct)
{
    /* NOT rtp */
    if (8026 != ntohs(node_ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u.udp.port))
        return false;

    if ((ct_master->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip == node_ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u3.ip)
        && (ntohs(ct_master->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u.udp.port) 
            == (ntohs(node_ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u.udp.port) + 1)))
    {
        RTSP_DEBUG_MSG("match_rtp_ct: &ct_master = %p, &node_ct->master = %p, a matched ct: \n", ct_master, node_ct->master);                
        return true;
    }

    RTSP_DEBUG_MSG("match_rtp_ct: &ct_master = %p, &node_ct->master = %p, a un-matched ct: \n", ct_master, node_ct->master); 
    dump_tuple_ip(&node_ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
    dump_tuple_ip(&node_ct->tuplehash[IP_CT_DIR_REPLY].tuple);    
    
	return false;
}

static unsigned int nf_nat_rtcp_out(struct sk_buff *skb, struct nf_conn *ct,
                                    enum ip_conntrack_info ctinfo, 
                                    const char *dptr)
{
    struct rtcp_hdr * rtcp_hdr = NULL;
    unsigned long data_off = 0;
    unsigned long data_len = 0;
    unsigned long new_ip = 0;
    unsigned short new_port = 0;
    unsigned long matchoff = 0;
    unsigned char buffer[6] = {0};
    enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);
    int rc = NF_ACCEPT;
    
    /* UDP净荷数据的长度 */
    data_len = (skb->data + skb->len) - (unsigned char *)dptr;
    
    /* 连接跟踪中的WAN IP和PORT */
    new_ip = ct->tuplehash[!dir].tuple.dst.u3.ip;
    new_port = ct->tuplehash[!dir].tuple.dst.u.all;

    RTSP_DEBUG_MSG("nf_nat_rtcp_out: new ip:port = %u.%u.%u.%u:%u\n", NIPQUAD(new_ip), new_port);

    data_off = 0;
    while (data_off < data_len)
    {        
        rtcp_hdr = (struct rtcp_hdr *)(dptr + data_off);
        
        data_off += RTCP_HEAD_LEN(ntohs(rtcp_hdr->len));
        if (0 == ntohs(rtcp_hdr->len))
        {
            RTSP_DEBUG_MSG("rtcp_hdr's len is invalid\r\n");
            rc = NF_ACCEPT;
            break;
        }
        
        if (data_off > data_len)
        {
            break;
        }

        /* RTCP RSR报文 */
        if (RTCP_RSR_PT == rtcp_hdr->pt && RTCP_RSR_FMT == rtcp_hdr->fmt)
        {
            struct rtcp_rsr_data * rsr_data = 
                (struct rtcp_rsr_data *)((char *)rtcp_hdr + sizeof(struct rtcp_hdr));

            //igmp_send_join(skb->dev, rsr_data->ssrc_media_source);
            
            matchoff = ((char *)&(rsr_data->user_ip)) - (char *)dptr;
            
            memcpy(buffer, &new_ip, sizeof(new_ip));
            memcpy(buffer+4, &new_port, sizeof(new_port));
            
            rc = nf_nat_mangle_udp_packet(skb, ct, ctinfo, matchoff, sizeof(buffer), buffer, sizeof(buffer));
            if(NF_ACCEPT != rc)
            {
                RTSP_DEBUG_MSG("nf_nat_mangle_udp_packet rsr packet failed\r\n");
                break;
            }

        	struct nf_conntrack_tuple_hash *h = NULL;
        	struct nf_conn *_ct = NULL;
        	struct hlist_nulls_node *n = NULL;
            unsigned int bucket = 0;            
            struct net *net = nf_ct_net(ct);
            int match_num = 0;
            extern int nf_conntrack_clean;

            spin_lock_bh(&nf_conntrack_lock);
        	for (; bucket < nf_conntrack_htable_size; bucket++) {
        		hlist_nulls_for_each_entry(h, n, &net->ct.hash[bucket], hnnode) {
        			_ct = nf_ct_tuplehash_to_ctrack(h);
                    if (match_rtp_ct(ct, _ct))
                    {
        			    set_bit(27, &_ct->status);
                        dump_tuple_ip(&_ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
                        dump_tuple_ip(&_ct->tuplehash[IP_CT_DIR_REPLY].tuple);
                        match_num++;
                    }
        		}
        	}
        	spin_unlock_bh(&nf_conntrack_lock);
            
            //if (0 != match_num)
            {
                nf_conntrack_clean = 8027;
            }
            
            RTSP_DEBUG_MSG("nf_nat_mangle_udp_packet rsr packet ok\r\n");
        }
        /* RTCP NACK报文 */
        else if (RTCP_NACK_PT == rtcp_hdr->pt && RTCP_NACK_FMT == rtcp_hdr->fmt)
        {
            struct rtcp_nack_data * nack_data = 
                (struct rtcp_nack_data *)(dptr + data_off - sizeof(struct rtcp_nack_data));

            /* 判断该报文是否带了扩展 */
            if ( nack_data->client_ip != ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip )
            {
                /* 没有扩展 */
                continue;
            }
            
            matchoff = ((char *)&(nack_data->client_port)) - (char *)dptr;;
            
            memcpy(buffer, &new_port, sizeof(new_port));
            memcpy(buffer+2, &new_ip, sizeof(new_ip));
            
            rc = nf_nat_mangle_udp_packet(skb, ct, ctinfo, matchoff, sizeof(buffer), buffer, sizeof(buffer));
            if(NF_ACCEPT != rc)
            {
                RTSP_DEBUG_MSG("nf_nat_mangle_udp_packet nack packet failed\r\n");
                break;
            }

            RTSP_DEBUG_MSG("nf_nat_mangle_udp_packet nack packet \r\n");
        }        
        /* Goodbye 报文 */
        else if (RTCP_BYE_PT == rtcp_hdr->pt && RTCP_BYE_FMT == rtcp_hdr->fmt)
        {      
        	struct nf_conntrack_tuple_hash *h = NULL;
        	struct nf_conn *_ct = NULL;
        	struct hlist_nulls_node *n = NULL;
            unsigned int bucket = 0;            
            struct nf_conn *ct_master = ct->master;
            struct net *net = nf_ct_net(ct);
            extern int nf_conntrack_clean;
            
            if (NULL == ct_master)
            {
                ct_master = ct;
                RTSP_DEBUG_MSG("nf_nat_rtcp_out: goodbye packet(one rtcp ct) \n");
            }     
            else
            {
                //nf_ct_remove_expectations(ct);
                set_bit(27, &ct->status);
                RTSP_DEBUG_MSG("nf_nat_rtcp_out: goodbye packet(two rtcp ct) \n");
            }
            
            //nf_ct_remove_expectations(ct_master);
            set_bit(27, &ct_master->status);

            spin_lock_bh(&nf_conntrack_lock);
        	for (; bucket < nf_conntrack_htable_size; bucket++) {
        		hlist_nulls_for_each_entry(h, n, &net->ct.hash[bucket], hnnode) {
        			_ct = nf_ct_tuplehash_to_ctrack(h);
                    if ((ct_master == _ct->master)
                        ||(match_rtp_ct(ct_master, _ct)))
                    {
        			    set_bit(27, &_ct->status);
                        dump_tuple_ip(&_ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
                        dump_tuple_ip(&_ct->tuplehash[IP_CT_DIR_REPLY].tuple);                        
                        RTSP_DEBUG_MSG("nf_nat_rtcp_out: goodbye packet(a rtp ct) \n");
                    }
        		}
        	}
        	spin_unlock_bh(&nf_conntrack_lock);            
            nf_conntrack_clean = 8027;
            
            RTSP_DEBUG_MSG("nf_nat_rtcp_out: goodbye packet \r\n");
        }
    }
    
    return NF_ACCEPT;
}

int rtcp_help(struct sk_buff *skb,
            unsigned int protoff,
            struct nf_conn *ct,
            enum ip_conntrack_info ctinfo)
{
    int ret = NF_ACCEPT;
    enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);
    unsigned int dataoff;
    const char *dptr;
    //typeof(nf_nat_rtcp_hook) nf_nat_rtcp;  
    typeof(nf_conntrack_set_rtp_hook) nf_set_rtcp_exp_rtp;      

    RTSP_DEBUG_MSG("rtcp_help proc\n");

    /* 只对上行的rtcp报文做处理 */
    if (ct->tuplehash[dir].tuple.src.u3.ip == ct->tuplehash[!dir].tuple.dst.u3.ip)
    {
        goto out;
    }

    /* 这里protoff为ip头长度，计算ip + udp头部总长度 */
    dataoff = protoff + sizeof(struct udphdr);
    if (dataoff >= skb->len)
        return NF_ACCEPT;

    //nf_ct_refresh(ct, skb, rtcp_timeout * HZ);

    dump_tuple_ip(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
    dump_tuple_ip(&ct->tuplehash[IP_CT_DIR_REPLY].tuple);

    if (!skb_is_nonlinear(skb))
        dptr = skb->data + dataoff;
    else {
        RTSP_DEBUG_MSG("Copy of skbuff not supported yet.\n");
        goto out;
    }

    /* 只针对RTCP RSR报文创建期望连接 */
    if (sizeof(struct rtcp_hdr) < ((skb->data + skb->len) - (unsigned char *)dptr))
    {        
        struct rtcp_hdr * rtcp_hdr = (struct rtcp_hdr *)dptr;
        if (RTCP_RSR_PT == rtcp_hdr->pt && RTCP_RSR_FMT == rtcp_hdr->fmt)
        {
            nf_set_rtcp_exp_rtp = rcu_dereference (nf_conntrack_set_rtp_hook);
            if (nf_set_rtcp_exp_rtp && ct->status & IPS_NAT_MASK)
                ret = nf_set_rtcp_exp_rtp(skb, ct, ctinfo, dptr);

            RTSP_DEBUG_MSG("rtcp_help rtcp_exp_rtp ret = %d \n", ret);
        }
    }
    
    RTSP_DEBUG_MSG("rtcp_help nat_rtcp\n");
    
    //nf_nat_rtcp = rcu_dereference (nf_nat_rtcp_hook);
    //if (nf_nat_rtcp && ct->status & IPS_NAT_MASK)
    //    ret = nf_nat_rtcp(pskb, ctinfo, ct, dptr, 0, 0);
    if (ct->status & IPS_NAT_MASK)
        ret = nf_nat_rtcp_out(skb, ct, ctinfo, dptr);

out:
    return ret;

}
EXPORT_SYMBOL_GPL(rtcp_help);

static struct nf_conntrack_expect_policy rtcp_exp_policy = {
        .max_expected   = 8, 
        .timeout        = 300,
};

struct nf_conntrack_helper nf_conntrack_rtcp_helper __read_mostly = {
    .name            = "rtcp_help",
    .me            = THIS_MODULE,
    .expect_policy = &rtcp_exp_policy,
    .tuple.dst.protonum    = IPPROTO_UDP,
    //.mask.src.u.udp.port    = __constant_htons(0xFFFF),
    //.mask.dst.protonum    = 0xFF,
    .help            = rtcp_help
};
#endif

/* Single data channel */
int (*nat_rtsp_channel_hook) (struct sk_buff *skb, struct nf_conn *ct,
			      enum ip_conntrack_info ctinfo,
			      unsigned int matchoff, unsigned int matchlen,
			      struct nf_conntrack_expect *exp);
EXPORT_SYMBOL_GPL(nat_rtsp_channel_hook);

/* A pair of data channels (RTP/RTCP) */
int (*nat_rtsp_channel2_hook) (struct sk_buff *skb, struct nf_conn *ct,
			       enum ip_conntrack_info ctinfo,
			       unsigned int matchoff, unsigned int matchlen,
			       struct nf_conntrack_expect *rtp_exp,
			       struct nf_conntrack_expect *rtcp_exp,
			       char dash);
EXPORT_SYMBOL_GPL(nat_rtsp_channel2_hook);

/* Modify parameters like client_port in Transport for single data channel */
int (*nat_rtsp_modify_port_hook) (struct sk_buff *skb, struct nf_conn *ct,
			      	  enum ip_conntrack_info ctinfo,
			      	  unsigned int matchoff, unsigned int matchlen,
			      	  __be16 rtpport);
EXPORT_SYMBOL_GPL(nat_rtsp_modify_port_hook);

/* Modify parameters like client_port in Transport for multiple data channels*/
int (*nat_rtsp_modify_port2_hook) (struct sk_buff *skb, struct nf_conn *ct,
			       	   enum ip_conntrack_info ctinfo,
			       	   unsigned int matchoff, unsigned int matchlen,
			       	   __be16 rtpport, __be16 rtcpport,
				   char dash);
EXPORT_SYMBOL_GPL(nat_rtsp_modify_port2_hook);

/* Modify parameters like destination in Transport */
int (*nat_rtsp_modify_addr_hook) (struct sk_buff *skb, struct nf_conn *ct,
			 	  enum ip_conntrack_info ctinfo,
			 	  int matchoff, int matchlen);
EXPORT_SYMBOL_GPL(nat_rtsp_modify_addr_hook);

/* Modify parameters like destination in FCC x-NAT */
int (*rtsp_mangle_xnataddr_hook)(struct sk_buff *skb, struct nf_conn *ct,
        			       enum ip_conntrack_info ctinfo, 
        			       unsigned int protoff, uint xnatoff, uint xnatlen, char *nat_buffer);
EXPORT_SYMBOL_GPL(rtsp_mangle_xnataddr_hook);

/*Init the tcp_buffer*/
int (*rtsp_init_tcp_buffer_hook)(const char * tcpbuf, uint tcplen);
EXPORT_SYMBOL_GPL(rtsp_init_tcp_buffer_hook);

/*Mangle the tcp_content*/
int (*rtsp_mangle_tcp_content_hook)(struct sk_buff *skb, struct nf_conn *ct,
								enum ip_conntrack_info ctinfo);
EXPORT_SYMBOL_GPL(rtsp_mangle_tcp_content_hook);

static int memmem(const char *haystack, int haystacklen,
		  const char *needle, int needlelen)
{
	const char *p = haystack;
	int l = haystacklen - needlelen + 1;
	char c = *needle;

	while(l-- > 0) { /* "!=0" won't handle haystacklen less than needlelen, need ">" */
		if (*p++ == c) {
			if (memcmp(p, needle+1, needlelen-1) == 0)
				return p - haystack - 1;
		}
	}
	return -1;
}

static int memstr(const char *haystack, int haystacklen,
		  const char *needle, int needlelen)
{
	const char *p = haystack;
	int l = haystacklen - needlelen + 1;
	char c = *needle;

	if (isalpha(c)) {
		char lower = __tolower(c);
		char upper = __toupper(c);

		while(l-- > 0) {  /* "!=0" won't handle haystacklen less than needlelen, need ">" */
			if (*p == lower || *p == upper) {
				if (strncasecmp(p, needle, needlelen) == 0)
					return p - haystack;
			}
			p++;
		}
	} else {
		while(l-- > 0) {
			if (*p++ == c) {
				if (strncasecmp(p, needle+1, needlelen-1) == 0)
					return p - haystack - 1;
			}
		}
	}
	return -1;
}
#if defined(CONFIG_MIPS_BRCM)
static int get_cseq(const char *str)
{
	unsigned long cseq = 0, i = 0;
	char c = *str;
	while(i++ < 10 && c && c != 0xd && c>='0' && c <= '9'){
		cseq = (cseq * 10) + (c - '0');
		c = *(str + i);
	}
	if(!cseq)
		cseq = -1;
	return (int) cseq;
}
#endif
/* Get next message in a packet */
static int get_next_message(const char *tcpdata, int tcpdatalen,
			    int *msgoff, int *msglen, int *msghdrlen)
{
	if (*msglen == 0) { /* The first message */
		*msgoff = 0;
	} else {
		*msgoff += *msglen;
#if defined(CONFIG_MIPS_BRCM)
		if ((*msgoff + 4) >= tcpdatalen) /* No more message */
#else
		if (*msgoff >= tcpdatalen) /* No more message */
#endif
			return 0;
	}

	/* Get message header length */
	*msghdrlen = memmem(tcpdata+*msgoff, tcpdatalen-*msgoff, "\r\n\r\n", 4);
	if (*msghdrlen < 0) {
		*msghdrlen = *msglen = tcpdatalen - *msgoff;
	} else {
		/* Get message length including SDP */
		int cloff = memstr(tcpdata+*msgoff, *msghdrlen, "Content-Length: ", 16);
		if (cloff < 0) {
			*msglen = *msghdrlen + 4;
		} else {
			unsigned long cl = simple_strtoul(tcpdata+*msgoff+cloff+16, NULL, 10);
			*msglen = *msghdrlen + 4 + cl;
		}
	}

	return 1;
}

/* Get next client_port parameter in a Transport header */
static int get_next_client_port(const char *tcpdata, int tpoff, int tplen,
				int *portoff, int *portlen,
				__be16 *rtpport, __be16 *rtcpport,
				char *dash)
{
	int off;
	char *p;

	if (*portlen == 0) { /* The first client_port */
		*portoff = tpoff;
	} else {
		*portoff += *portlen;
		if (*portoff >= tpoff + tplen) /* No more data */
			return 0;
	}

	off = memmem(tcpdata+*portoff, tplen-(*portoff-tpoff),
		     ";client_port=", 13);
	if (off < 0)
		return 0;
	*portoff += off + 13;

	*rtpport = htons((unsigned short)simple_strtoul(tcpdata+*portoff,
							&p, 10));
	if (*p != '-' && *p != '/') {
		*dash = 0;
	} else {
		*dash = *p++;
		*rtcpport = htons((unsigned short)simple_strtoul(p, &p, 10));
	}
	*portlen = p - tcpdata - *portoff;
	return 1;
}

/* Get next destination=<ip>:<port> parameter in a Transport header
 * This is not a standard parameter, so far, it's only seen in some customers'
 * products.
 */
static int get_next_dest_ipport(const char *tcpdata, int tpoff, int tplen,
				int *destoff, int *destlen, __be32 *dest,
				int *portoff, int *portlen, __be16 *port)
{
	int off;
	char *p;

	if (*destlen == 0) { /* The first destination */
		*destoff = tpoff;
	} else {
		*destoff += *destlen + 1 + *portlen;
		if (*destoff >= tpoff + tplen) /* No more data */
			return 0;
	}

	off = memmem(tcpdata+*destoff, tplen-(*destoff-tpoff),
		     ";destination=", 13);
	if (off < 0)
		return 0;
	*destoff += off + 13;

        if (in4_pton(tcpdata+*destoff, tplen-(*destoff-tpoff), (u8 *)dest,
                     -1, (const char **)&p) == 0) {
		return 0;
	}
	*destlen = p - tcpdata - *destoff;

	if (*p != ':') {
		return 0;
	}
	*portoff = p - tcpdata + 1;

	*port = htons((unsigned short)simple_strtoul(tcpdata+*portoff, &p, 10));
	*portlen = p - tcpdata - *portoff;

	return 1;
}

/* Get next destination parameter in a Transport header */
static int get_next_destination(const char *tcpdata, int tpoff, int tplen,
				int *destoff, int *destlen, __be32 *dest)
{
	int off;
	char *p;

	if (*destlen == 0) { /* The first destination */
		*destoff = tpoff;
	} else {
		*destoff += *destlen;
		if (*destoff >= tpoff + tplen) /* No more data */
			return 0;
	}

	off = memmem(tcpdata+*destoff, tplen-(*destoff-tpoff),
		     ";destination=", 13);
	if (off < 0)
		return 0;
	*destoff += off + 13;

        if (in4_pton(tcpdata+*destoff, tplen-(*destoff-tpoff), (u8 *)dest,
                     -1, (const char **)&p)) {
		*destlen = p - tcpdata - *destoff;
		return 1;
	} else {
		return 0;
	}
}

#if defined(CONFIG_NF_CONNTRACK_RTCP) || (defined(CONFIG_NF_CONNTRACK_RTCP_MODULE) && defined(CONFIG_MODULES))
static int get_next_server_port(const char *tcpdata, int tpoff, int tplen,
				int *portoff, int *portlen,
				__be16 *rtpport, __be16 *rtcpport,
				char *dash)
{
	int off;
	char *p;

	if (*portlen == 0) { /* The first client_port */
		*portoff = tpoff;
	} else {
		*portoff += *portlen;
		if (*portoff >= tpoff + tplen) /* No more data */
			return 0;
	}

	off = memmem(tcpdata+*portoff, tplen-(*portoff-tpoff),
		     ";server_port=", 13);
	if (off < 0)
		return 0;
	*portoff += off + 13;

	*rtpport = htons((unsigned short)simple_strtoul(tcpdata+*portoff,
							&p, 10));
	if (*p != '-' && *p != '/') {
		*dash = 0;
	} else {
		*dash = *p++;
		*rtcpport = htons((unsigned short)simple_strtoul(p, &p, 10));
	}
	*portlen = p - tcpdata - *portoff;
	return 1;
}

static int set_expected_rtcp(struct sk_buff *skb, struct nf_conn *ct,
                                enum ip_conntrack_info ctinfo, 
                                unsigned short rtcpport, unsigned short server_rtcpport)
{
    int ret = 0;
    int dir = CTINFO2DIR(ctinfo);
    struct nf_conntrack_expect *rtcp_exp;
    struct nf_conntrack_tuple try_rtcp_tuple;
    struct net *net = nf_ct_net(ct);

    if ((0 == rtcpport) || (0 == server_rtcpport)) {
    	return -1;
    }

    /* 查询RTCP的期望连接 */
    memset(&try_rtcp_tuple, 0, sizeof(try_rtcp_tuple));
    try_rtcp_tuple.dst.u3.ip = ct->tuplehash[dir].tuple.dst.u3.ip;        
    try_rtcp_tuple.dst.protonum = IPPROTO_UDP;
    try_rtcp_tuple.dst.u.all = htons(rtcpport);
    try_rtcp_tuple.dst.dir = dir;
    try_rtcp_tuple.src.l3num = ct->tuplehash[dir].tuple.src.l3num;

    rtcp_exp = nf_ct_expect_find_get(net, &try_rtcp_tuple);
    if(NULL == rtcp_exp)
    {
        RTSP_DEBUG_MSG("nf_ct_rtsp: find expect failed\r\n");
        return -1;
    }

	/* Create expect for RTCP */
	if ((rtcp_exp = nf_ct_expect_alloc(ct)) == NULL) {
		return -1;
	}

    RTSP_DEBUG_MSG("nf_ct_rtsp: set expect RTCP\r\n");
    
#if defined(CONFIG_MIPS_BRCM)
	nf_ct_expect_init(rtcp_exp, NF_CT_EXPECT_CLASS_DEFAULT, nf_ct_l3num(ct),
			  &ct->tuplehash[!dir].tuple.src.u3,
			  &ct->tuplehash[dir].tuple.src.u3,
			  IPPROTO_UDP, NULL, &server_rtcpport);
#else
	nf_ct_expect_init(rtcp_exp, NF_CT_EXPECT_CLASS_DEFAULT, nf_ct_l3num(ct),
			  &ct->tuplehash[!dir].tuple.src.u3,
			  &ct->tuplehash[dir].tuple.src.u3,
			  IPPROTO_UDP, NULL, &server_rtcpport);
#endif

    RTSP_DEBUG_MSG("nf_ct_rtsp: set expect RTCP ok\r\n");

    rtcp_exp->dir = !dir;
    rtcp_exp->expectfn = NULL;
    rtcp_exp->helper = &nf_conntrack_rtcp_helper;
    rtcp_exp->flags = NF_CT_EXPECT_PERMANENT;

	if (nf_ct_expect_related(rtcp_exp) == 0) {
		pr_debug("nf_ct_rtsp: expect RTCP ");
		nf_ct_dump_tuple(&rtcp_exp->tuple);
	} else {
		ret = -1;
	}

	nf_ct_expect_put(rtcp_exp);

	return ret;
}
#endif

static int expect_rtsp_channel(struct sk_buff *skb, struct nf_conn *ct,
			       enum ip_conntrack_info ctinfo,
			       int portoff, int portlen,
			       __be16 rtpport)
{
	int ret = 0;
	int dir = CTINFO2DIR(ctinfo);
	struct nf_conntrack_expect *rtp_exp;
	typeof(nat_rtsp_channel_hook) nat_rtsp_channel;

	if (rtpport == 0)
		return -1;

	/* Create expect for RTP */
	if ((rtp_exp = nf_ct_expect_alloc(ct)) == NULL)
		return -1;
#if defined(CONFIG_MIPS_BRCM)
	nf_ct_expect_init(rtp_exp, NF_CT_EXPECT_CLASS_DEFAULT, nf_ct_l3num(ct),
			  NULL, &ct->tuplehash[!dir].tuple.dst.u3,
			  IPPROTO_UDP, NULL, &rtpport);
#else
	nf_ct_expect_init(rtp_exp, NF_CT_EXPECT_CLASS_DEFAULT, nf_ct_l3num(ct),
			  &ct->tuplehash[!dir].tuple.src.u3,
			  &ct->tuplehash[!dir].tuple.dst.u3,
			  IPPROTO_UDP, NULL, &rtpport);
#endif
	if ((nat_rtsp_channel = rcu_dereference(nat_rtsp_channel_hook)) &&
	    ct->status & IPS_NAT_MASK) {
		/* NAT needed */
		ret = nat_rtsp_channel(skb, ct, ctinfo, portoff, portlen,
				       rtp_exp);
	} else {		/* Conntrack only */
		if (nf_ct_expect_related(rtp_exp) == 0) {
			pr_debug("nf_ct_rtsp: expect RTP ");
			nf_ct_dump_tuple(&rtp_exp->tuple);
		} else
			ret = -1;
	}

	nf_ct_expect_put(rtp_exp);

	return ret;
}

static int expect_rtsp_channel2(struct sk_buff *skb, struct nf_conn *ct,
				enum ip_conntrack_info ctinfo,
				int portoff, int portlen,
				__be16 rtpport, __be16 rtcpport,
				char dash)
{
	int ret = 0;
	int dir = CTINFO2DIR(ctinfo);
	struct nf_conntrack_expect *rtp_exp;
	struct nf_conntrack_expect *rtcp_exp;
	typeof(nat_rtsp_channel2_hook) nat_rtsp_channel2;

	if (rtpport == 0 || rtcpport == 0)
		return -1;

	/* Create expect for RTP */
	if ((rtp_exp = nf_ct_expect_alloc(ct)) == NULL)
		return -1;
	nf_ct_expect_init(rtp_exp, NF_CT_EXPECT_CLASS_DEFAULT ,nf_ct_l3num(ct),
			  &ct->tuplehash[!dir].tuple.src.u3,
			  &ct->tuplehash[!dir].tuple.dst.u3,
			  IPPROTO_UDP, NULL, &rtpport);

	/* Create expect for RTCP */
	if ((rtcp_exp = nf_ct_expect_alloc(ct)) == NULL) {
		nf_ct_expect_put(rtp_exp);
		return -1;
	}
#if defined(CONFIG_MIPS_BRCM)
	nf_ct_expect_init(rtcp_exp, NF_CT_EXPECT_CLASS_DEFAULT, nf_ct_l3num(ct),
			  NULL, &ct->tuplehash[!dir].tuple.dst.u3,
			  IPPROTO_UDP, NULL, &rtcpport);
#else
	nf_ct_expect_init(rtcp_exp, NF_CT_EXPECT_CLASS_DEFAULT, nf_ct_l3num(ct),
			  &ct->tuplehash[!dir].tuple.src.u3,
			  &ct->tuplehash[!dir].tuple.dst.u3,
			  IPPROTO_UDP, NULL, &rtcpport);
#endif
	if ((nat_rtsp_channel2 = rcu_dereference(nat_rtsp_channel2_hook)) &&
	    ct->status & IPS_NAT_MASK) {

#if defined(CONFIG_NF_CONNTRACK_RTCP) || (defined(CONFIG_NF_CONNTRACK_RTCP_MODULE) && defined(CONFIG_MODULES))
        rtcp_exp->helper = &nf_conntrack_rtcp_helper;
        rtcp_exp->flags = NF_CT_EXPECT_PERMANENT;
#endif        
        
		/* NAT needed */
		ret = nat_rtsp_channel2(skb, ct, ctinfo, portoff, portlen,
				   	rtp_exp, rtcp_exp, dash);
	} else {		/* Conntrack only */
		if (nf_ct_expect_related(rtp_exp) == 0) {
			if (nf_ct_expect_related(rtcp_exp) == 0) {
				pr_debug("nf_ct_rtsp: expect RTP ");
				nf_ct_dump_tuple(&rtp_exp->tuple);
				pr_debug("nf_ct_rtsp: expect RTCP ");
				nf_ct_dump_tuple(&rtcp_exp->tuple);
			} else {
				nf_ct_unexpect_related(rtp_exp);
				ret = -1;
			}
		} else
			ret = -1;
	}

	nf_ct_expect_put(rtp_exp);
	nf_ct_expect_put(rtcp_exp);

	return ret;
}

static void set_normal_timeout(struct nf_conn *ct, struct sk_buff *skb)
{
	struct nf_conn *child;

	/* nf_conntrack_lock is locked inside __nf_ct_refresh_acct, locking here results in a deadlock */
	/* write_lock_bh(&nf_conntrack_lock); */ 
	list_for_each_entry(child, &ct->derived_connections, derived_list) {
		child->derived_timeout = 5*HZ;
		nf_ct_refresh(child, skb, 5*HZ);
	}
	/* write_unlock_bh(&nf_conntrack_lock); */
}

static void set_long_timeout(struct nf_conn *ct, struct sk_buff *skb)
{
	struct nf_conn *child;

	/* write_lock_bh(&nf_conntrack_lock); */
	list_for_each_entry(child, &ct->derived_connections, derived_list) {
#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BLOG)
		if ( (child->blog_key[IP_CT_DIR_ORIGINAL] != BLOG_KEY_NONE)
			|| (child->blog_key[IP_CT_DIR_REPLY] != BLOG_KEY_NONE) )
		{
			/* remove flow from flow cache */
                        blog_notify(DESTROY_FLOWTRACK, (void*)child,
                                                (uint32_t)child->blog_key[IP_CT_DIR_ORIGINAL],
                                                (uint32_t)child->blog_key[IP_CT_DIR_REPLY]);

                        /* Safe: In case blog client does not set key to 0 explicilty */
                        child->blog_key[IP_CT_DIR_ORIGINAL] = BLOG_KEY_NONE;
                        child->blog_key[IP_CT_DIR_REPLY]    = BLOG_KEY_NONE;
		}
#endif
		nf_ct_refresh(child, skb, 3600*HZ);
	}
	/*	write_unlock_bh(&nf_conntrack_lock); */
}

static int help(struct sk_buff *skb, unsigned int protoff,
		struct nf_conn *ct, enum ip_conntrack_info ctinfo)
{
	int dir = CTINFO2DIR(ctinfo);
	struct nf_conn_help *hlp = nfct_help(ct);
	struct tcphdr _tcph, *th;
	unsigned int tcpdataoff, tcpdatalen;
	char *tcpdata;
	int msgoff, msglen, msghdrlen;
	int tpoff, tplen;
	int portlen = 0;
	int portoff = 0;
	__be16 rtpport = 0;
	__be16 rtcpport = 0;
	char dash = 0;
	int destlen = 0;
	int destoff = 0;
	__be32 dest = 0;
    int ret = 0;
	typeof(nat_rtsp_modify_addr_hook) nat_rtsp_modify_addr;
	typeof(nat_rtsp_modify_port_hook) nat_rtsp_modify_port;
	typeof(nat_rtsp_modify_port2_hook) nat_rtsp_modify_port2;
    typeof(rtsp_mangle_xnataddr_hook) rtsp_mangle_xnataddr;
	typeof(rtsp_init_tcp_buffer_hook) rtsp_init_tcp_buffer;
    typeof(rtsp_mangle_tcp_content_hook) rtsp_mangle_tcp_content;
    
#if defined(CONFIG_SYSCTL) && defined(CONFIG_NF_CONNTRACK_RTSP) && defined(CONFIG_NF_CONNTRACK_PROC_COMPAT)
    if (!alg_rtsp_enable)
    {
        return NF_ACCEPT;
    }
#endif

	/* Until there's been traffic both ways, don't look in packets. */
	if (ctinfo != IP_CT_ESTABLISHED
	    && ctinfo != IP_CT_ESTABLISHED+IP_CT_IS_REPLY) {
		return NF_ACCEPT;
	}
	pr_debug("nf_ct_rtsp: skblen = %u\n", skb->len);

	/* Get TCP header */
	th = skb_header_pointer(skb, protoff, sizeof(_tcph), &_tcph);
	if (th == NULL) {
		return NF_ACCEPT;
	}

	/* Get TCP payload offset */
	tcpdataoff = protoff + th->doff * 4;
	if (tcpdataoff >= skb->len) { /* No data? */
		return NF_ACCEPT;
	}

	/* Get TCP payload length */
	tcpdatalen = skb->len - tcpdataoff;

	spin_lock_bh(&nf_rtsp_lock);

	/* Get TCP payload pointer */
	tcpdata = skb_header_pointer(skb, tcpdataoff, tcpdatalen, rtsp_buffer);
	BUG_ON(tcpdata == NULL);

	rtsp_init_tcp_buffer = rcu_dereference(rtsp_init_tcp_buffer_hook);
    if (rtsp_init_tcp_buffer)
    {
    	rtsp_init_tcp_buffer(tcpdata, tcpdatalen);
    }
    
	/* There may be more than one message in a packet, check them
	 * one by one */
	msgoff = msglen = msghdrlen = 0;
	while(get_next_message(tcpdata, tcpdatalen, &msgoff, &msglen,
			       &msghdrlen)) {
	    /*start by d00191326 of DTS2012072304247 :增加DSLITE隧道时的RTSP ALG*/
		/* Messages from LAN side through MASQUERADED connections */
		if (memcmp(&ct->tuplehash[dir].tuple.src.u3,
			   &ct->tuplehash[!dir].tuple.dst.u3,
			   sizeof(ct->tuplehash[dir].tuple.src.u3)) != 0
			   || (0 != strstr(skb->dev->name, "ip6tnl") && IP_CT_DIR_ORIGINAL == dir)) {
	    /*end by d00191326 of DTS2012072304247 :增加DSLITE隧道时的RTSP ALG*/
			if(memcmp(tcpdata+msgoff, "PAUSE ", 6) == 0) {
#if defined(CONFIG_MIPS_BRCM)
				int cseq = memmem(tcpdata+msgoff, msglen, "CSeq: ", 6);
				if(cseq == -1) {
				        /* Fix the IOP issue with DSS on Drawin system */
				        cseq = memmem(tcpdata+msgoff, msglen, "Cseq: ", 6);
				        if(cseq == -1) {
					   pr_debug("nf_ct_rtsp: wrong PAUSE msg\n");
				        } else {
					   cseq = get_cseq(tcpdata+msgoff+cseq+6);
				        }
				} else {
					cseq = get_cseq(tcpdata+msgoff+cseq+6);
				}
				
				pr_debug("nf_ct_rtsp: PAUSE, CSeq=%d\n", cseq);
				hlp->help.ct_rtsp_info.paused = cseq;
#else
				pr_debug("nf_ct_rtsp: PAUSE\n");
				hlp->help.ct_rtsp_info.paused = 1;
#endif
				continue;
			} else {
				hlp->help.ct_rtsp_info.paused = 0;
			}
			if(memcmp(tcpdata+msgoff, "TEARDOWN ", 9) == 0) {
				pr_debug("nf_ct_rtsp: TEARDOWN\n");
				set_normal_timeout(ct, skb);
				continue;
			} else if(memcmp(tcpdata+msgoff, "SETUP ", 6) != 0) {
				continue;
			}
			
			/* Now begin to process SETUP message */
			pr_debug("nf_ct_rtsp: SETUP\n");

#if defined(CONFIG_NF_CONNTRACK_RTCP) || (defined(CONFIG_NF_CONNTRACK_RTCP_MODULE) && defined(CONFIG_MODULES))
            int x_natoff = 0;
            x_natoff = memmem(tcpdata+msgoff+6, msghdrlen-6, "x-NAT_Address:", 14);
            if (0 <= x_natoff)
            {
                int x_natlen = 0;        
                RTSP_DEBUG_MSG("nf_ct_rtsp: find x-NAT_Address\n");
                
                x_natoff += msgoff + 6;            
                x_natlen = memmem(tcpdata + x_natoff, msghdrlen - (x_natoff - msgoff), "\r\n", 2);
        		if (x_natlen < 0)
                {
                    RTSP_DEBUG_MSG("nf_ct_rtsp: wrong x-NAT_Address message");
                }
                else
                {
                    x_natlen += 2;
                    
                    /* x-NAT_Address解析 */
                    rtsp_mangle_xnataddr = rcu_dereference(rtsp_mangle_xnataddr_hook);
                    if (rtsp_mangle_xnataddr)
                    {
	                    if (!rtsp_mangle_xnataddr(skb, ct, ctinfo, protoff, x_natoff, x_natlen, rtsp_buffer))
	                    {
	                        RTSP_DEBUG_MSG("nf_ct_rtsp: x-NAT_Address mangle failed");
	                    }
                    }
                }
             
            }
#endif

		/* Reply message that's from WAN side. */
		} else {
			/* We only check replies */
			if(memcmp(tcpdata+msgoff, "RTSP/", 5) != 0)
				continue;
			
			pr_debug("nf_ct_rtsp: Reply message\n");

		 	/* Response to a previous PAUSE message */
			if (hlp->help.ct_rtsp_info.paused) {
#if defined(CONFIG_MIPS_BRCM)
				int cseq = memmem(tcpdata+msgoff, msglen, "CSeq: ", 6);
				if(cseq == -1) {
				        /* Fix the IOP issue with DSS on Drawin system */
				        cseq = memmem(tcpdata+msgoff, msglen, "Cseq: ", 6);
				        if(cseq == -1) {
					   pr_debug("nf_ct_rtsp: wrong reply msg\n");
				        } else {
					   cseq = get_cseq(tcpdata+msgoff+cseq+6);
				        }
				} else {
					cseq = get_cseq(tcpdata+msgoff+cseq+6);
				}
				if(cseq == hlp->help.ct_rtsp_info.paused) {
					pr_debug("nf_ct_rtsp: Reply to PAUSE\n");
					set_long_timeout(ct, skb);
					hlp->help.ct_rtsp_info.paused = 0;
					goto end;
				}
#else
				pr_debug("nf_ct_rtsp: Reply to PAUSE\n");
				set_long_timeout(ct, skb);
				hlp->help.ct_rtsp_info.paused = 0;
				goto end;
#endif
			}
			
			/* Now begin to process other reply message */
		}

		/* Get Transport header offset */
		tpoff = memmem(tcpdata+msgoff+6, msghdrlen-6,
			       "\r\nTransport: ", 13);
		if (tpoff < 0)
			continue;
		tpoff += msgoff + 6 + 13;

		/* Get Transport header length */
		tplen = memmem(tcpdata+tpoff, msghdrlen-(tpoff - msgoff),
			       "\r\n", 2);
		if (tplen < 0)
			tplen = msghdrlen - (tpoff - msgoff);

		/* There maybe more than one client_port parameter in this
		 * field, we'll process each of them. I know not all of them
		 * are unicast UDP ports, but that is the only situation we
		 * care about so far. So just KISS. */
		portoff = portlen = 0;
		while(get_next_client_port(tcpdata, tpoff, tplen,
					   &portoff, &portlen,
					   &rtpport, &rtcpport, &dash)) {
            /*start by d00191326 of DTS2012072304247 :增加DSLITE隧道时的RTSP ALG*/
			if (memcmp(&ct->tuplehash[dir].tuple.src.u3,
			   	   &ct->tuplehash[!dir].tuple.dst.u3,
			   	   sizeof(ct->tuplehash[dir].tuple.src.u3))
			    != 0 || (0 != strstr(skb->dev->name, "ip6tnl") && IP_CT_DIR_ORIGINAL == dir)) {
			/*end by d00191326 of DTS2012072304247 :增加DSLITE隧道时的RTSP ALG*/
				/* LAN to WAN */
				if (dash == 0) {
					/* Single data channel */
					ret = expect_rtsp_channel(skb, ct,
								  ctinfo,
			 					  portoff,
								  portlen,
								  rtpport);
				} else {
					/* A pair of data channels (RTP/RTCP)*/
					ret = expect_rtsp_channel2(skb, ct,
								   ctinfo,
								   portoff,
								   portlen,
								   rtpport,
								   rtcpport,
								   dash);
				}
			} else {
				nat_rtsp_modify_port = rcu_dereference(
					nat_rtsp_modify_port_hook);
				nat_rtsp_modify_port2 = rcu_dereference(
					nat_rtsp_modify_port2_hook);
				/* WAN to LAN */
				if (dash == 0 ) {
					/* Single data channel */
					if (nat_rtsp_modify_port) {
					ret = nat_rtsp_modify_port(skb, ct,
								   ctinfo,
								   portoff,
								   portlen,
								   rtpport);
					}
				} else {
#if defined(CONFIG_NF_CONNTRACK_RTCP) || (defined(CONFIG_NF_CONNTRACK_RTCP_MODULE) && defined(CONFIG_MODULES))
                    __be16 server_rtpport = 0;
                    __be16 server_rtcpport = 0;
                	int server_portlen = 0;
                	int server_portoff = 0;
                    char server_dash = 0;
                    if (0 < get_next_server_port(tcpdata, tpoff, tplen,
					   &server_portoff, &server_portlen,
					   &server_rtpport, &server_rtcpport, &dash))   
                    {

                        RTSP_DEBUG_MSG("nf_ct_rtsp: ports = (%d,%hu,%hu), server_ports = (%d,%hu,%hu)\n",
                               (int)dash, rtpport, rtcpport,
                               (int)server_dash, server_rtpport, server_rtcpport);
                        
                        /* 建立上行期待连接用于对上行动态端口发出的RTCP报文进行ALG处理 */
                        if (0 > set_expected_rtcp(skb, ct, ctinfo, rtcpport, server_rtcpport))
                        {
                            RTSP_DEBUG_MSG("nf_ct_rtsp: set expect RTCP done\r\n");
                        }
                    }
#endif                    
                
					/* A pair of data channels (RTP/RTCP)*/
					if (nat_rtsp_modify_port2) {
					ret = nat_rtsp_modify_port2(skb, ct,
								    ctinfo,
								    portoff,
								    portlen,
								    rtpport,
								    rtcpport,
								    dash);
					}
				}
			}

            /* register the RTP ports with ingress QoS classifier */
//            printk("\n RTP Port = %d, RTCP Port = %d\n", rtpport, rtcpport);
#if (defined CONFIG_MIPS_BRCM || defined CONFIG_HSAN)
            iqos_add_L4port(IPPROTO_UDP, rtpport, IQOS_ENT_DYN, IQOS_PRIO_HIGH);
            iqos_add_L4port(IPPROTO_UDP, rtcpport, IQOS_ENT_DYN, IQOS_PRIO_HIGH);
#endif
            
			if (ret < 0)
				goto end;
		}

		/* Process special destination=<ip>:<port> parameter in 
		 * Transport header. This is not a standard parameter,
		 * so far, it's only seen in some customers' products.
 		 */
		while(get_next_dest_ipport(tcpdata, tpoff, tplen,
					   &destoff, &destlen, &dest,
					   &portoff, &portlen, &rtpport)) {            
            /*start by d00191326 of DTS2012072304247 :增加DSLITE隧道时的RTSP ALG*/
			/* Process the port part */
			if (memcmp(&ct->tuplehash[dir].tuple.src.u3,
			   	   &ct->tuplehash[!dir].tuple.dst.u3,
			   	   sizeof(ct->tuplehash[dir].tuple.src.u3))
			    != 0 || (0 != strstr(skb->dev->name, "ip6tnl") && IP_CT_DIR_ORIGINAL == dir)) {
		    /*end by d00191326 of DTS2012072304247 :增加DSLITE隧道时的RTSP ALG*/
				/* LAN to WAN */
				ret = expect_rtsp_channel(skb, ct, ctinfo,
							  portoff, portlen,
							  rtpport);
			} else {
				/* WAN to LAN */
				if ((nat_rtsp_modify_port = rcu_dereference(
				    nat_rtsp_modify_port_hook))) {
					ret = nat_rtsp_modify_port(skb, ct,
								   ctinfo,
								   portoff,
								   portlen,
								   rtpport);
				}
			}
            
            /* register the RTP ports with ingress QoS classifier */
//            printk("\n RTP Port = %d\n", rtpport);
#if (defined CONFIG_MIPS_BRCM || defined CONFIG_HSAN)
            iqos_add_L4port(IPPROTO_UDP, rtpport, IQOS_ENT_DYN, IQOS_PRIO_HIGH);
#endif
            
			if (ret < 0)
				goto end;

			/* Then the IP part */
			if (dest != ct->tuplehash[dir].tuple.src.u3.ip)
				continue;
			if ((nat_rtsp_modify_addr =
			     rcu_dereference(nat_rtsp_modify_addr_hook)) &&
			    ct->status & IPS_NAT_MASK) {
			}
			/* NAT needed */
			ret = nat_rtsp_modify_addr(skb, ct, ctinfo,
						   destoff, destlen);
			if (ret < 0)
				goto end;
		}

		if ((nat_rtsp_modify_addr =
		     rcu_dereference(nat_rtsp_modify_addr_hook)) &&
		    ct->status & IPS_NAT_MASK) {
			destoff = destlen = 0;
			while(get_next_destination(tcpdata, tpoff, tplen,
					   	   &destoff, &destlen, &dest)) {				
				if (dest != ct->tuplehash[dir].tuple.src.u3.ip)
					continue;

				/* NAT needed */
				ret = nat_rtsp_modify_addr(skb, ct, ctinfo,
							   destoff, destlen);
				if (ret < 0)
					goto end;
			}
		}
	}    
end:
    rtsp_mangle_tcp_content = rcu_dereference(rtsp_mangle_tcp_content_hook);
    if (rtsp_mangle_tcp_content)
    {
    	rtsp_mangle_tcp_content(skb, ct, ctinfo);
    }
	spin_unlock_bh(&nf_rtsp_lock);
	return NF_ACCEPT;
}

static struct nf_conntrack_helper rtsp[MAX_PORTS];
static char rtsp_names[MAX_PORTS][sizeof("rtsp-65535")];
static struct nf_conntrack_expect_policy rtsp_exp_policy;


#ifdef CONFIG_RTSP_QOS
static int is_rtsp_control_tuple(const struct nf_conntrack_tuple *orig)
{
	int i;

	if (NULL == orig)
	{
		return 0;
	}

	if (IPPROTO_TCP != orig->dst.protonum)
	{
		return 0;
	}

	for (i = 0; i < ports_c; i++)
	{
		if (orig->dst.u.tcp.port == htons(ports[i]))
		{
			return 1;
		}
	}

	return 0;
}


static int is_rtsp_data_tuple(struct net *net, const struct nf_conntrack_tuple *orig)
{
	int is_rtsp = 0;

	struct nf_conntrack_expect *exp = NULL;

	exp = nf_ct_find_expectation_safe(net, orig);
	if (NULL == exp)
	{
		return 0;
	}

	is_rtsp = is_rtsp_control_tuple(&exp->master->tuplehash[IP_CT_DIR_ORIGINAL].tuple);

	nf_ct_expect_put(exp);

	return is_rtsp;
}


static int is_rtsp_data_ct(struct nf_conn *ct)
{
	int i = 0;

	if (NULL == ct)
	{
		return 0;
	}

	if (NULL == ct->master)
	{
		return 0;
	}

	if (IPPROTO_TCP != ct->master->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.protonum)
	{
		return 0;
	}

	for (i = 0; i < ports_c; i++)
	{
		if (ct->master->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.tcp.port == htons(ports[i]))
		{
			return 1;
		}
	}

	return 0;
}


int is_rtsp_tuple(struct net *net, const struct nf_conntrack_tuple *orig)
{
	if (is_rtsp_control_tuple(orig))
	{
		return 1;
	}

	/*ct还未建立，使用tuple查找是否属于RTSP数据连接*/
	if (is_rtsp_data_tuple(net, orig))
	{
		return 1;
	}

	return 0;
}


int is_rtsp_ct(struct nf_conn *ct)
{
	if (NULL == ct)
	{
		return 0;
	}

	if (is_rtsp_control_tuple(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple))
	{
		return 1;
	}

	if (is_rtsp_data_ct(ct))
	{
		return 1;
	}

	return 0;
}

static struct nf_conntrack_proto_pri rtsp_pri __read_mostly = {
   .name = "rtsp",
   .check_by_tuple = is_rtsp_tuple,
   .check_by_ct = is_rtsp_ct,
};

#endif


/* don't make this __exit, since it's called from __init ! */
static void nf_conntrack_rtsp_fini(void)
{
	int i;

	for (i = 0; i < ports_c; i++) {
		if (rtsp[i].me == NULL)
			continue;

        /* unregister the RTSP ports with ingress QoS classifier */
#if (defined CONFIG_MIPS_BRCM || defined CONFIG_HSAN)
        iqos_rem_L4port( rtsp[i].tuple.dst.protonum, 
                         rtsp[i].tuple.src.u.tcp.port, IQOS_ENT_STAT );
#endif
		pr_debug("nf_ct_rtsp: unregistering helper for port %d\n",
		       	 ports[i]);
		nf_conntrack_helper_unregister(&rtsp[i]);
	}

	kfree(rtsp_buffer);

#if defined(CONFIG_RTSP_QOS) && defined(CONFIG_NF_CONNTRACK_PRI)
	nf_conntrack_pri_unregister(&rtsp_pri);
#endif
}

static int __init nf_conntrack_rtsp_init(void)
{
	int i, ret = 0;
	char *tmpname;

	rtsp_buffer = kmalloc(4000, GFP_KERNEL);
	if (!rtsp_buffer)
		return -ENOMEM;

#if defined(CONFIG_RTSP_QOS) && defined(CONFIG_NF_CONNTRACK_PRI)
	nf_conntrack_pri_register(&rtsp_pri);
#endif

	if (ports_c == 0)
		ports[ports_c++] = RTSP_PORT;

	rtsp_exp_policy.max_expected = max_outstanding;
#if defined(CONFIG_NF_CONNTRACK_RTCP) || (defined(CONFIG_NF_CONNTRACK_RTCP_MODULE) && defined(CONFIG_MODULES))
	rtsp_exp_policy.timeout	= (24 * 3600);
#else	
    rtsp_exp_policy.timeout	= 5 * 60;
#endif
	for (i = 0; i < ports_c; i++) {
		rtsp[i].tuple.src.l3num = PF_INET;
		rtsp[i].tuple.src.u.tcp.port = htons(ports[i]);
		rtsp[i].tuple.dst.protonum = IPPROTO_TCP;
		rtsp[i].expect_policy = &rtsp_exp_policy;
		rtsp[i].expect_class_max = 1;
		rtsp[i].me = THIS_MODULE;
		rtsp[i].help = help;
		tmpname = &rtsp_names[i][0];
		if (ports[i] == RTSP_PORT)
			sprintf(tmpname, "rtsp");
		else
			sprintf(tmpname, "rtsp-%d", ports[i]);
		rtsp[i].name = tmpname;

		pr_debug("nf_ct_rtsp: registering helper for port %d\n",
		       	 ports[i]);
		ret = nf_conntrack_helper_register(&rtsp[i]);
		if (ret) {
			printk("nf_ct_rtsp: failed to register helper "
			       "for port %d\n", ports[i]);
			nf_conntrack_rtsp_fini();
			return ret;
		}

        /* register the RTSP ports with ingress QoS classifier */        
#if (defined CONFIG_MIPS_BRCM || defined CONFIG_HSAN)
        iqos_add_L4port( IPPROTO_TCP, ports[i], IQOS_ENT_STAT, IQOS_PRIO_HIGH );
#endif
	}

	return 0;
}

module_init(nf_conntrack_rtsp_init);
module_exit(nf_conntrack_rtsp_fini);
