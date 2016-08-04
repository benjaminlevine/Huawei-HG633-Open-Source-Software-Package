/* RTCP extension for IP connection tracking.
 *by kongchengcheng
 */

#include <linux/module.h>
#include <linux/ctype.h>
#include <linux/skbuff.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <linux/udp.h>
#include <linux/netfilter.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_nat_helper.h>
#include <net/netfilter/nf_nat.h>

static int rtcp_debug = 0;
module_param(rtcp_debug, int, 0600);
MODULE_PARM_DESC(rtcp_debug, "rtcp_debug=1 is turn on debug messages");
#define RTCPDEBUGP(fmt, arg...)  if (rtcp_debug) printk (KERN_DEBUG "RTCP[%u]: " fmt , __LINE__ , ## arg)


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christian Hentschel <chentschel@arnet.com.ar>");
MODULE_DESCRIPTION("RTCP connection tracking helper");
MODULE_ALIAS("nf_conntrack_rtcp");

//#define RTCP_PORT    8888
#define RTCP_PORT    8027
#define MAX_PORTS    8

static struct nf_conntrack_helper rtcp[MAX_PORTS][2] __read_mostly;
static char rtcp_names[MAX_PORTS][2][sizeof("rtcp-65535")] __read_mostly;
static struct nf_conntrack_expect_policy rtcp_exp_policy;

static int ports_c;
static unsigned short ports[MAX_PORTS];
module_param_array(ports, ushort, &ports_c, 0400);
MODULE_PARM_DESC(ports_c, "port numbers of rtcp servers");

extern int rtcp_help(struct sk_buff *skb,
                unsigned int protoff,
                struct nf_conn *ct,
                enum ip_conntrack_info ctinfo);

extern int (*nf_conntrack_set_rtp_hook)(struct sk_buff *skb,
                struct nf_conn *ct,
                enum ip_conntrack_info ctinfo,
                const char *dptr);


#ifdef CONFIG_MIPS_BRCM
extern int (*bcm_nat_rtcp_help_hook)(struct sk_buff *skb,
                unsigned int protoff,
                struct nf_conn *ct,
                enum ip_conntrack_info ctinfo);
#endif

static void nf_nat_follow_rtp(struct nf_conn *ct,struct nf_conntrack_expect *exp)
{
	struct nf_nat_range range;

	/* This must be a fresh one. */
	BUG_ON(ct->status & IPS_NAT_DONE_MASK);

	/* Change src to where new ct comes from */
	range.flags = IP_NAT_RANGE_MAP_IPS;
	range.min_ip = range.max_ip =
		ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip;
	nf_nat_setup_info(ct, &range, IP_NAT_MANIP_SRC);
	 
	/* For DST manip, map port here to where it's expected. */
	range.flags = (IP_NAT_RANGE_MAP_IPS | IP_NAT_RANGE_PROTO_SPECIFIED);
	range.min = range.max = exp->saved_proto;
	range.min_ip = range.max_ip = exp->saved_ip;
	nf_nat_setup_info(ct, &range, IP_NAT_MANIP_DST);
}

static unsigned int set_expected(struct sk_buff *skb,
                   enum ip_conntrack_info ctinfo,
                   struct nf_conntrack_expect *exp,
                   const char *dptr)
{
    struct nf_conn *ct = exp->master;
    enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);/* = IP_CT_DIR_ORIGINAL */
    u_int16_t port_tmp;
    
    exp->tuple.dst.u3.ip = ct->tuplehash[!dir].tuple.dst.u3.ip;
    //exp->mask.dst.u3.ip = 0xFFFFFFFF;

    port_tmp = ct->tuplehash[!dir].tuple.dst.u.all;
    port_tmp = ntohs(port_tmp);
    port_tmp = port_tmp - 1;
    port_tmp = htons(port_tmp);
    exp->tuple.dst.u.all = port_tmp;
    //exp->mask.dst.u.all = 0xFFFF;
    
    exp->dir = !dir;
    exp->tuple.src.u3.ip = 0;
    exp->mask.src.u3.ip = 0;
    port_tmp = ntohs(ct->tuplehash[dir].tuple.src.u.all) - 1;
    exp->saved_proto.all = htons(port_tmp);
    exp->saved_ip = ct->tuplehash[dir].tuple.src.u3.ip;
      
    /* When you see the packet, we need to NAT it the same as the
       this one. */
    exp->expectfn = nf_nat_follow_rtp;
    //exp->flags = NF_CT_EXPECT_PERMANENT;
        
    return NF_ACCEPT;
}

static int rtcp_port_is_static(unsigned short port)
{
    int idx = 0;
    
    for (idx = 0; idx < ports_c; idx ++)
    {
        if( ports[idx] == port )
        {
            return 1;
        }
    }

    return 0;
}

/* 设置RTCP生成的RTP期待连接 */ 
static int set_expected_rtp(struct sk_buff *skb,
                struct nf_conn *ct,
                enum ip_conntrack_info ctinfo,
                const char *dptr)
{
    enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo); /* = IP_CT_DIR_ORIGINAL */
    unsigned short dst_port;
    struct nf_conntrack_expect *exp;
    unsigned int exp_class = NF_CT_EXPECT_CLASS_DEFAULT;
    struct nf_conn_help *master_help = nfct_help(ct);
    int ret = NF_ACCEPT;
    
    if ((NULL != master_help) && (NULL != master_help->helper) 
        && (0 == strcmp("BCM-NAT", master_help->helper->name)))
    {
        exp_class = NF_CT_EXPECT_CLASS_DEFAULT + 1;
    }

    /* 只对第一次上行 ct 创建 rtp exp ，即不刷新 timeout */      
	if (dir != IP_CT_DIR_ORIGINAL ||
	    master_help->expecting[exp_class])
    {   
        RTCPDEBUGP("rtp exp for CoreNAT is exist, just return.\n");
		return ret;
    }
    
    /* 静态监听的端口,创建rtp的期待连接 */
    dst_port = ntohs(ct->tuplehash[dir].tuple.dst.u.all);
    if( !rtcp_port_is_static(dst_port))
    {
        RTCPDEBUGP("dynamic rtcp port: %d\r\n", dst_port);
        return ret;
    }

    exp = nf_ct_expect_alloc(ct);
    if (exp == NULL)
    {
        RTCPDEBUGP("alloc rtp exp failed.\n");
        return NF_DROP;
    }

    nf_ct_expect_init(exp, exp_class, nf_ct_l3num(ct),/*family,*/
                 &ct->tuplehash[!dir].tuple.src.u3, &ct->tuplehash[!dir].tuple.dst.u3,
                 IPPROTO_UDP, NULL, &ct->tuplehash[!dir].tuple.dst.u.all);
    
    RTCPDEBUGP("rtp exp from : src[%d.%d.%d.%d : %d] -> dst[%d.%d.%d.%d : %d]\n", 
        NIPQUAD(exp->tuple.src.u3), ntohs(exp->tuple.src.u.all),
        NIPQUAD(exp->tuple.dst.u3), ntohs(exp->tuple.dst.u.all));
    
    if (ct->status & IPS_NAT_MASK)
    {
        ret = set_expected(skb, ctinfo, exp, dptr);
    }    
    
    RTCPDEBUGP("rtp exp to   : src[%d.%d.%d.%d : %d] -> dst[%d.%d.%d.%d : %d]\n", 
        NIPQUAD(exp->tuple.src.u3), ntohs(exp->tuple.src.u.all),
        NIPQUAD(exp->tuple.dst.u3), ntohs(exp->tuple.dst.u.all));

    if (nf_ct_expect_related(exp) != 0)
    {
        RTCPDEBUGP("related rtp exp return NO-zero.");
    }
    else
    {
        RTCPDEBUGP("related rtp exp ok\n");
    }
    nf_ct_expect_put(exp);

    return ret;
}

#ifdef CONFIG_MIPS_BRCM
int bcm_nat_rtcp_help(struct sk_buff *skb,
            unsigned int protoff,
            struct nf_conn *ct,
            enum ip_conntrack_info ctinfo)
{
    int ret = NF_ACCEPT;
    enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);

    if (ports[0] == ntohs(ct->tuplehash[dir].tuple.dst.u.all))
    {
	    ret = rtcp_help(skb, protoff, ct, ctinfo); 
    }    

    return ret;
}
#endif

static void nf_conntrack_rtcp_fini(void)
{
    int i, j;

    for (i = 0; i < ports_c; i++) {
        for (j = 0; j < 2; j++) {
            if (rtcp[i][j].me == NULL)
                continue;
            //nf_conntrack_helper_unregister(&rtcp[i][j]);
        }
    }

    nf_conntrack_set_rtp_hook = NULL;
    
#ifdef CONFIG_MIPS_BRCM    
    bcm_nat_rtcp_help_hook = NULL;
#endif
    printk("\r\n ---unregister conntrack rtcp alg ko succeed! \r\n");
}

static int __init nf_conntrack_rtcp_init(void)
{
    int i, j, ret = 0;
    char *tmpname;
    
    BUG_ON(nf_conntrack_set_rtp_hook);
    nf_conntrack_set_rtp_hook = set_expected_rtp;

#ifdef CONFIG_MIPS_BRCM
    BUG_ON(bcm_nat_rtcp_help_hook);
    bcm_nat_rtcp_help_hook = bcm_nat_rtcp_help;
#endif    
    
    if (ports_c == 0)
    {
        ports[ports_c++] = RTCP_PORT;
#if 0        
        ports[ports_c++] = RTCP_PORT + 1;
        ports[ports_c++] = RTCP_PORT + 2;
        ports[ports_c++] = RTCP_PORT + 3;
        ports[ports_c++] = RTCP_PORT + 4;
        ports[ports_c++] = RTCP_PORT + 5;
        ports[ports_c++] = RTCP_PORT + 6;
        ports[ports_c++] = RTCP_PORT + 7; 
#endif        
    }

	rtcp_exp_policy.max_expected = 8;
    rtcp_exp_policy.timeout	= 3 * 60;

    for (i = 0; i < ports_c; i++) {
        memset(&rtcp[i], 0, sizeof(rtcp[i]));

        rtcp[i][0].tuple.src.l3num = AF_INET;
        rtcp[i][1].tuple.src.l3num = AF_INET6;
        for (j = 0; j < 2; j++) {
            rtcp[i][j].tuple.src.u.udp.port = htons(ports[i]);
            rtcp[i][j].tuple.dst.protonum = IPPROTO_UDP;
            //rtcp[i][j].mask.src.l3num = 0xFFFF;
            //rtcp[i][j].mask.src.u.udp.port = htons(0xFFFF);
            //rtcp[i][j].mask.dst.protonum = 0xFF;
    		rtcp[i][j].expect_policy = &rtcp_exp_policy;
    		rtcp[i][j].expect_class_max = 1;
    		rtcp[i][j].me = THIS_MODULE;
    		rtcp[i][j].help = rtcp_help;
            //rtcp[i][j].max_expected = 8;
            //rtcp[i][j].timeout = 3 * 60; /* 3 minutes */
            //rtcp[i][j].me = THIS_MODULE;
            tmpname = &rtcp_names[i][j][0];
            if (ports[i] == RTCP_PORT)
                sprintf(tmpname, "rtcp");
            else
                sprintf(tmpname, "rtcp-%u", i);
            rtcp[i][j].name = tmpname;

    		pr_debug("nf_ct_rtcp: registering helper for port %d\n",
    		       	 ports[i]);
            
            //ret = nf_conntrack_helper_register(&rtcp[i][j]);
            if (ret) {
                printk("nf_ct_rtcp: failed to register helper "
                       "for pf: %u port: %u\n",
                       rtcp[i][j].tuple.src.l3num, ports[i]);
                nf_conntrack_rtcp_fini();
                return ret;
            }
        }
    }
    
    printk("nf_conntrack_rtcp loading, port = %d\n", ports[0]);
    
    return 0;
}

module_init(nf_conntrack_rtcp_init);
module_exit(nf_conntrack_rtcp_fini);

