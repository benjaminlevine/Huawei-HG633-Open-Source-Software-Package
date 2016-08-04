/*
 *	Forwarding decision
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/netfilter_bridge.h>
#include "br_private.h"
#if defined(CONFIG_MIPS_BRCM)
#include <linux/blog.h>
#endif
#if defined(CONFIG_MIPS_BRCM)
#include <linux/ip.h>
#include <linux/igmp.h>
#endif /* for IGMP */
/* Start of viewed by f00120964 for qos function 2012-1-6 */
#if defined(CONFIG_IMQ) || defined(CONFIG_IMQ_MODULE)
#include <linux/imq.h>
#endif
/* End of viewed by f00120964 for qos function 2012-1-6 */
#include "atp_interface.h"

#if defined(CONFIG_BR_IGMP_STATISTIC)
#include <linux/igmp_statistic.h>
#endif

#ifdef CONFIG_ATP_BR_VLAN_MODE
#include "br_vlan.h"
#endif

#include <linux/if_arp.h>
#include <linux/inetdevice.h>

/* BEGIN: Added by h00169677, 2012/5/28   PN:DTS2012021404606 */
#ifdef CONFIG_IPV6
#include <net/ndisc.h>
#include <linux/ipv6.h>
#include <net/ipv6.h>
extern int	ipv6_get_lladdr(struct net_device *dev,struct in6_addr *addr,unsigned char banned_flags);
#endif
/* END:   Added by h00169677, 2012/5/28 */
/*start add by z00228368 for DTS2013101800046 630 ttnet add grefon fun @20131017 */
#ifdef CONFIG_ATP_GRE_FON
extern int g_ssid2_vid;
extern int g_ssid3_vid;
#endif
/*end add by z00228368 for DTS2013101800046 630 ttnet add grefon fun @20131017 */
#ifdef CONFIG_SUPPORT_ATP

static inline int is_igmp_packet(struct sk_buff *skb)
{
    struct ethhdr *pstHdr = NULL;
    if (!skb)
    {
        return 0;
    }

    pstHdr = eth_hdr(skb);
    if ((pstHdr->h_proto == htons(ETH_P_IP)) 
            && (skb->data[9] == IPPROTO_IGMP))
    {
        return 1;
    }
    
    if ((pstHdr->h_proto == htons(ETH_P_8021Q))
            && (skb->data[13] == IPPROTO_IGMP))
    {
        return 1;
    }

    return 0;
}


#ifdef CONFIG_IGMP_DROP_LEAVE_LAN2LAN
/*允许lan lan leave报文转发的特殊组地址*/
static unsigned long g_IGMPAddrSp[] = {
    0xEFFFFFFAU,    /*upnp*/
    0xE0000009U,    /*ripv2*/
    0xE00000FBU     /*mdns*/
};

#define IGMPADDRSPNUM ((sizeof(g_IGMPAddrSp)) / (sizeof(g_IGMPAddrSp[0])))

static int is_need_drop_group(unsigned long GroupAddr)
{
    int index = 0;

    for (index = 0; index < IGMPADDRSPNUM; index++)
    {
        if (GroupAddr == __constant_htonl(g_IGMPAddrSp[index]))
        {
            return 0;
        }
    }

    return 1;
}


static int is_forward_from_lan_to_lan(const struct net_bridge_port *p, 
                                      const struct sk_buff *skb)
{
    if (NULL == p)
    {
        return 0;
    }

    if (NULL == skb)
    {
        return 0;
    }
    
    if (!IS_LAN_DEV(skb->dev->name))
    {
        return 0;
    }

    if (!IS_LAN_DEV(p->dev->name))
    {
        return 0;
    }

    return 1;
}


static int is_igmp_v3_leave_packet(struct sk_buff *skb, 
                                   struct iphdr *pip, struct igmpv3_report *pstRepV3)
{
    int k;
    struct igmpv3_grec   *pstGrec;
    int is_leave = 0;

    pstGrec = &pstRepV3->grec[0];
    for (k = 0; k < pstRepV3->ngrec; k++)
    {
        
        if ((IGMPV3_CHANGE_TO_INCLUDE == pstGrec->grec_type && pstGrec->grec_nsrcs == 0) || 
                 (pstGrec->grec_nsrcs == 0 && IGMPV3_MODE_IS_INCLUDE == pstGrec->grec_type) || 
                 (IGMPV3_BLOCK_OLD_SOURCES == pstGrec->grec_type))
        {
            /*一个IGMP报文包含多个leave和join时，只要有一个leave，就不在lan侧转发*/
            is_leave = is_need_drop_group(pstGrec->grec_mca);
            if (is_leave)
            {
                break;
            }
        }
        
        pstGrec = (struct igmpv3_grec *)((char *)pstGrec + sizeof(struct igmpv3_grec) + pstGrec->grec_nsrcs * sizeof(struct in_addr)); 
    }
    return is_leave;
}


static int is_igmp_leave_packet(struct sk_buff *skb)
{
    struct igmpv3_report *pstRepV3 = NULL;
    struct igmphdr * pstRepV2 = NULL;
    struct iphdr *pip = NULL;
    int is_leave = 0;

    if (NULL == skb)
    {
        return is_leave;
    }
    
    pip = ip_hdr(skb);
    pstRepV3 = (struct igmpv3_report *)(skb->data + (pip->ihl << 2));

    if (IGMPV3_HOST_MEMBERSHIP_REPORT == pstRepV3->type)
    {
        return is_igmp_v3_leave_packet(skb, pip, pstRepV3);
    }

    pstRepV2 = (struct igmphdr *)pstRepV3;
    if (IGMP_HOST_LEAVE_MESSAGE == pstRepV2->type)
    {
        is_leave = is_need_drop_group(pstRepV2->group);
    }
    
    return is_leave;
}


static int should_igmp_deliver(const struct net_bridge_port *p, 
                               const struct sk_buff *skb)
{
    /*只关注lan to lan的报文*/
    if (!is_forward_from_lan_to_lan(p, skb))
    {
        return 1;
    }

    /*只关注igmp报文*/
    if (!is_igmp_packet(skb))
    {            
        return 1;
    }

    /*如果是igmp leave报文，不允许lan lan转发*/
    if (is_igmp_leave_packet(skb))
    {
        return 0;
    }

    return 1;
}
#endif

static inline int should_eth_deliver(const struct net_bridge_port *p, 
				 const struct sk_buff *skb)
{
    unsigned char *dest;
    dest = NULL;
	
    dest = eth_hdr(skb)->h_dest;

    if (dest != NULL)
    {        
        if ( (dest[0] == 0xff) && (dest[1] ==0xff) && (dest[2] == 0xff)
              &&(dest[3] == 0xff) && (dest[4] ==0xff) && (dest[5] == 0xff)
            )
       {
            return  1;
       }

        /* IPV6 */
        if ((dest[0] == 0x33) && (dest[1] ==0x33))
        {
             return  1;
        }
    }

#ifdef CONFIG_IGMP_DROP_LEAVE_LAN2LAN
    /*igmp leave报文不在lan lan转发*/
    if (0 == should_igmp_deliver(p, skb))
    {
        return 0;
    }
#endif
    return 1;
}

#ifdef CONFIG_WAN_LAN_BIND
/* WAN 服务类型，与应用层保持一致，目前只用到INTERNET和OTHER*/
typedef enum
{
    ATP_WAN_SERVICE_INTERNET = 1,
    ATP_WAN_SERVICE_TR069 = 2,
    ATP_WAN_SERVICE_VOICE = 4,
    ATP_WAN_SERVICE_OTHER = 8,
    ATP_WAN_SERVICE_SHARENET = 16,
}ATP_WAN_SERVICE_EN;

static int should_deliver_other_bridge(const struct net_bridge_port *p,
				 const struct sk_buff *skb)
{
    /*允许不同BR下LAN到LAN的报文*/
    if(IS_LAN_DEV(skb->dev->name) && IS_LAN_DEV(p->dev->name))
    {
        return 1;
    }
    
    /*允许BR0下LAN口到其它br下INTERNET WAN口的报文*/
    if((p->dev->flags & IFF_ALLOWPASS) && skb->dev->br_port && skb->dev->br_port->br
        && 0 == strcmp(skb->dev->br_port->br->dev->name, ATP_DEFAULT_BRIDGE_NAME))
    {
        return 1;
    }

    /*允许INTERNET WAN口的报文到br0*/
    if((skb->dev->flags & IFF_ALLOWPASS)
        && 0 == strcmp(p->br->dev->name, ATP_DEFAULT_BRIDGE_NAME))
    {
        return 1;
    }

    return 0;
}
#endif

/* Don't forward packets to originating port or forwarding diasabled */
static inline int should_deliver(const struct net_bridge_port *p,
				 const struct sk_buff *skb)
{
 	if (skb->dev == p->dev ||
	    p->state != BR_STATE_FORWARDING)
		return 0;
          
    if (IS_BRIDGE_DEV(skb->dev->name) && IS_WAN_DEVICE(p->dev->name))
    {
        return 0;
    }
    
	if ((*(p->dev->name)    == 'n') &&
	    (*(skb->dev->name)  == 'n')) 
		return 0;

#ifdef CONFIG_WAN_LAN_BIND    
#ifdef CONFIG_BRIDGE_PORT_RELAY
    /*开启relay时不下发RA*/
    if (IS_BRIDGE_DEV(skb->dev->name) && IsSkbfromDhcpsPort(skb, 1) 
        && DevDhcpRelayOn(p->br->dev->name))
    {
        return 0;
    }
#endif

    /*不同br间的转发限制*/
    if(skb->dev->br_port && skb->dev->br_port->br != p->br)
    {
        return should_deliver_other_bridge(p, skb);
    }
#endif

	return 1;
}

static void __br_deliver(const struct net_bridge_port *to, struct sk_buff *skb);

#define ATP_IPPROTO_UDP    17
static void br_deliver_nonas(const struct net_bridge_port * to, struct sk_buff * skb)
{
   /*
    *解决br0下存在nas接口三层组播(IGMP Proxy)出现马赛克问题
    *原因: 三层udp数据报文向二层转发广播进入nas接口, 数据流处理不及时
    *将进入桥下nas口的数据包直接丢弃不做任何处理
    *
    */
    const unsigned char *dest = NULL;
    
    if (IS_BRIDGE_DEV(skb->dev->name) && IS_WAN_DEVICE(to->dev->name))
    {
        kfree_skb(skb);
        return ;
    }

    dest = eth_hdr(skb)->h_dest;
    if(((skb->data[9] == ATP_IPPROTO_UDP 
            &&(dest[0] == 0x01 && dest[1] == 0x00 && dest[2] == 0x5e))
        )
        && (*(to->dev->name) == 'n' && *(to->dev->name + 1) == 'a' &&
        *(to->dev->name + 2) == 's'))
    {
        kfree_skb(skb);
        return ;
    }
    
    __br_deliver(to,skb);
}

#endif

static inline unsigned packet_length(const struct sk_buff *skb)
{
	return skb->len - (skb->protocol == htons(ETH_P_8021Q) ? VLAN_HLEN : 0);
}

int br_dev_queue_push_xmit(struct sk_buff *skb)
{
	/* drop mtu oversized packets except gso */
	if (packet_length(skb) > skb->dev->mtu && !skb_is_gso(skb))
		kfree_skb(skb);
	else {
		/* ip_refrag calls ip_fragment, doesn't copy the MAC header. */
		if (nf_bridge_maybe_copy_header(skb))
			kfree_skb(skb);
		else {
			skb_push(skb, ETH_HLEN);
#ifdef CONFIG_ATP_BR_VLAN_MODE
            if(IS_LAN_DEV(skb->dev->name))
            {
                br_vlan_lan_xmit(skb);
            }
            else
#endif
            {
			    dev_queue_xmit(skb);
            }
		}
	}

	return 0;
}

/*start add by z00228368 for DTS2013101800046 630 ttnet add grefon fun @20131017 */
#ifdef CONFIG_ATP_GRE_FON
static inline struct sk_buff *put_vlan_tag(struct sk_buff *skb, unsigned short tag)
{
	struct vlan_ethhdr *veth;

	if (skb_headroom(skb) < VLAN_HLEN) 
	{
		struct sk_buff *sk_tmp = skb;
		skb = skb_realloc_headroom(sk_tmp, VLAN_HLEN);
		kfree_skb(sk_tmp);
		if (!skb) {
			printk(KERN_ERR "vlan: failed to realloc headroom\n");
			return NULL;
		}
	} else {
		skb = skb_unshare(skb, GFP_ATOMIC);
		if (!skb) {
			printk(KERN_ERR "vlan: failed to unshare skbuff\n");
			return NULL;
		}
	}
    
	veth = (struct vlan_ethhdr *)skb_push(skb, VLAN_HLEN);

	/* Move the mac addresses to the beginning of the new header. */
	memmove(skb->data, skb->data + VLAN_HLEN, 2 * VLAN_ETH_ALEN);

	/* first, the ethernet type */
	veth->h_vlan_proto = __constant_htons(ETH_P_8021Q);

	/* now, the tag */
	veth->h_vlan_TCI = htons(tag);

	skb->protocol = __constant_htons(ETH_P_8021Q);
	skb->mac_header -= VLAN_HLEN;
	skb->network_header -= VLAN_HLEN;

	return skb;
}

/**
 *	struct vlan_dev_info_fon - VLAN private device data
 *	@nr_ingress_mappings: number of ingress priority mappings
 *	@ingress_priority_map: ingress priority mappings
 *	@nr_egress_mappings: number of egress priority mappings
 *	@egress_priority_map: hash of egress priority mappings
 *	@vlan_id: VLAN identifier
 *	@flags: device flags
 *	@real_dev: underlying netdevice
 *	@real_dev_addr: address of underlying netdevice
 *	@dent: proc dir entry
 *	@cnt_inc_headroom_on_tx: statistic - number of skb expansions on TX
 *	@cnt_encap_on_xmit: statistic - number of skb encapsulations on TX
 */
struct vlan_dev_info_fon {
	unsigned int				nr_ingress_mappings;
	u32					ingress_priority_map[8];
	unsigned int				nr_egress_mappings;
	struct vlan_priority_tci_mapping	*egress_priority_map[16];

	u16					vlan_id;
	u16					flags;

	struct net_device			*real_dev;
	unsigned char				real_dev_addr[ETH_ALEN];

	struct proc_dir_entry			*dent;
	unsigned long				cnt_inc_headroom_on_tx;
	unsigned long				cnt_encap_on_xmit;
#if defined(CONFIG_MIPS_BRCM)
#ifdef CONFIG_BLOG
	BlogStats_t bstats; /* stats when the blog promiscuous layer has consumed packets */
	struct net_device_stats cstats; /* Cummulative Device stats (rx-bytes, tx-pkts, etc...) */
#endif
    int nfmark_to_priority;
#endif
	
};

//static inline struct sk_buff *vlan_check_reorder_header(struct sk_buff *skb)
static inline struct vlan_dev_info_fon *vlan_dev_info_fon(const struct net_device *dev)
{
	return netdev_priv(dev);
}

static inline struct sk_buff *vlan_check_reorder_header_fon(struct sk_buff *skb)
{
    //去掉判断条件，使得wifi模块也能接受到带vlan的报文
	if (skb_cow(skb, skb_headroom(skb)) < 0)
		skb = NULL;
	if (skb) {
		/* Lifted from Gleb's VLAN code... */
		memmove(skb->data - ETH_HLEN,
			skb->data - VLAN_ETH_HLEN, 12);
		skb->mac_header += VLAN_HLEN;
	}

	return skb;
}
#endif

#ifdef CONFIG_ATP_GRE_FON
#define WLAN2_MARK 0x60000000
#define WLAN3_MARK 0x70000000
#endif

int br_forward_finish(struct sk_buff *skb)
{
#ifdef CONFIG_ATP_GRE_FON
	unsigned short vlan_id = 0;
	struct net_device *brdevfon = NULL;
	struct net_bridge_port *p = rcu_dereference(skb->dev->br_port);
	//int i = 0;
	u16 tmpProto = 0; 

	if (p)
	{
		brdevfon = p->br->dev;	
		if(brdevfon && ('1' == brdevfon->name[2]))
		{
			/* packet from nasgre to wl0.x,debug,current allow eth*/
			if ('n' != skb->dev->name[0])
			{
			    //printk("get a skb to LAN[%s], skb->protocol=0x%04x, skb->len=%d\n",
			    //skb->dev->name, skb->protocol, skb->len);
			    #if 0
			    printk("before, skb->protocol=%04x:\n", skb->protocol);
				for (i = 0; i < skb->len; i++)
				{
				    printk("%02x ", skb->data[i]);
				}
				printk("\n\n");
				#endif
				/* Take off the VLAN header (4 bytes currently), if exist*/
				if (htons(ETH_P_8021Q) == skb->protocol)
				{
				    tmpProto = *(__be16*)(skb->data + 2);
				    //printk("tmpProto=0x%04x\n", tmpProto);
				    skb_pull_rcsum(skb, VLAN_HLEN);
				    skb = vlan_check_reorder_header_fon(skb);
				    skb->protocol = tmpProto;
				    #if 0
    				printk("after untag vlan, skb->protocol=0x%04x, skb->len=%d:\n", 
    				       skb->protocol, skb->len);
    				for (i = 0; i < skb->len; i++)
    				{
    				    printk("%02x ", skb->data[i]);
    				}
    				printk("\n\n");
    				#endif
				}
				goto SEND_PACKET;
			}
			/*packet from wifi lan to nasgre, add vlan tag if configured*/
			else if ('n' == skb->dev->name[0])
			{
			    switch(skb->mark & 0xF0000000)
			    {
			        //case LAN1_MARK:暂时用LAN1调
			        case WLAN2_MARK:
                        vlan_id = g_ssid2_vid;
                        break;
                    case WLAN3_MARK:
                        vlan_id = g_ssid3_vid;
                        break;
                    default:
                        break;
			    }
			    if (vlan_id)
			    {
			        skb_push(skb, ETH_HLEN);
			        skb = put_vlan_tag(skb, vlan_id);
			        skb_pull(skb, ETH_HLEN);
			    }
			}
		}
	}
	
SEND_PACKET:	
#endif
/*end add by z00228368 for DTS2013101800046 630 ttnet add grefon fun @20131017 */
	return NF_HOOK(PF_BRIDGE, NF_BR_POST_ROUTING, skb, NULL, skb->dev,
		       br_dev_queue_push_xmit);

}

static void __br_deliver(const struct net_bridge_port *to, struct sk_buff *skb)
{
#if defined(CONFIG_BR_IGMP_STATISTIC)
    int flag = 0;
    if ( !is_multicast_ether_addr(eth_hdr(skb)->h_dest) )
    {
        if ( NULL != to && NULL != to->dev && NULL != skb->dev )
        {
            if ( IS_BRIDGE_DEV(skb->dev->name) && IS_LAN_ETH_DEVICE(to->dev->name) )
            {
                /* 下行流进入设备数据报文统计 */
                flag = 1;
            }
        }
    }
#endif

	skb->dev = to->dev;

#if defined(CONFIG_BR_IGMP_STATISTIC)
    if ( 1 == flag )
    {
        br_igmp_statistic_inc(skb,IGMP_DOWN_RETRANSMIT_PACKET);
    }
#endif

	NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_OUT, skb, NULL, skb->dev,
			br_forward_finish);
}

#ifdef CONFIG_ATP_DROP_ARP
/*start of Add by t00163975 20110527 for 不转发WAN侧发来的与br0接口IP同网段的ARP */
/*Start:add by caoxiafei cKF24361 20100827 for downstream arp packets forward incorrect*/
static inline int should_drop_arp_pakcet(struct sk_buff *skb)
{
    struct net_device *dev = skb->dev;
    struct arphdr *arp = NULL;
    unsigned char *arp_ptr2 = NULL;
    unsigned char *arp_ptr = NULL;

    if (__constant_htons(ETH_P_8021Q) == skb->protocol)
    {
        struct vlan_hdr *vhdr = (struct vlan_hdr *)skb->data;
        if (vhdr->h_vlan_encapsulated_proto ==  ntohs(ETH_P_ARP))
        {
            arp = (struct arphdr *)(vhdr + 1);
        }
    }
    else
    {
        __be16 ucprotocol = *(__be16 *)&(skb->data[12]);
    	if ((htons(ETH_P_ARP) == ucprotocol) || (htons(ETH_P_ARP) == skb->protocol))
    	{
        	arp = arp_hdr(skb);
    	}
    }

    if(NULL == arp)
    {
    	return 0;
    }

    /* 如果是从nas接口下来的ARP 包,而且目的IP或是源IP与br0接口IP相同的网段，则将其丢弃 */
    if ('n' == skb->dev->name[0]) // && 'a' == skb->dev->name[1] && 's' == skb->dev->name[2])
    {
		arp_ptr = (unsigned char *)(arp+1);
		arp_ptr += dev->addr_len;
		arp_ptr2 = arp_ptr;
		arp_ptr2 += 4 + dev->addr_len;
        
             return check_arp_for_all_br(arp_ptr,arp_ptr2,1);
	}
	return 0;
}

/*End:add by caoxiafei cKF24361 20100827 for downstream arp packets forward incorrect*/
/*end of Add by t00163975 20110527 for 不转发WAN侧发来的与br0接口IP同网段的ARP */
#endif

/* BEGIN: Added by h00169677, 2012/5/28   PN:DTS2012060900287 */
#ifdef CONFIG_IPV6
#define VLAN_HEAD_LEN 4
#define IPV6_HEAD_LEN 40
#define NS_TYPE_OFFSET 40
#define ICMPV6_OFFSET 6
static inline int should_drop_icmpv6ns_pakcet(struct sk_buff *skb)
{
    struct net_device *dev ;
	struct net *net;
	struct ipv6hdr *ipv6hd = NULL;
	struct nd_msg *msg = NULL;
	struct in6_addr addr_buf;
	
	memset(&addr_buf, 0, sizeof(struct in6_addr));
    if(!IS_LAN_DEV(skb->dev->name))
    {
        return 0;
    }
    
    if (__constant_htons(ETH_P_8021Q) == skb->protocol)
    {
        struct vlan_hdr *vhdr = (struct vlan_hdr *)skb->data;
        if (vhdr->h_vlan_encapsulated_proto !=  ntohs(ETH_P_IPV6))
        {
            return 0;
        }
        
        ipv6hd = (struct ipv6hdr *)(vhdr + 1);
        if ((ipv6_addr_any(&ipv6hd->saddr)) || 
            (IPPROTO_ICMPV6 != skb->data[VLAN_HEAD_LEN+ICMPV6_OFFSET]) ||
            (0x87 != skb->data[VLAN_HEAD_LEN+NS_TYPE_OFFSET]))
        {
            return 0;
        }
            
        msg = (struct nd_msg *)(skb->data+VLAN_HEAD_LEN+IPV6_HEAD_LEN);
    }
    else if (__constant_htons(ETH_P_IPV6) == skb->protocol)
    {
        ipv6hd = (struct ipv6hdr *)(skb->data);
        if ((ipv6_addr_any(&ipv6hd->saddr)) || 
            (IPPROTO_ICMPV6 != skb->data[ICMPV6_OFFSET]) ||
            (0x87 != skb->data[NS_TYPE_OFFSET]))
        {
            return 0;
        }
        
        msg = (struct nd_msg *)(skb->data+IPV6_HEAD_LEN);

    }
    else
    {
        return 0;
    }
    
    for_each_net(net) 
    {
        for_each_netdev(net, dev) 
    	{
    		if(IS_WAN_DEVICE(dev->name) || IS_BRIDGE_DEV(dev->name))
    		{
    			if (!ipv6_get_lladdr(dev, &addr_buf, IFA_F_TENTATIVE)) 
    			{
    				if ((NULL != msg) && (ipv6_addr_equal(&(msg->target),&addr_buf)))
    				{
    					return 1;
    				}
    			}
    		}
        }
    }
	return 0;
}
#endif
/* END:   Added by h00169677, 2012/5/28 */

static void __br_forward(const struct net_bridge_port *to, struct sk_buff *skb)
{
	struct net_device *indev;

	if (skb_warn_if_lro(skb)) {
		kfree_skb(skb);
		return;
	}
#ifdef CONFIG_ATP_DROP_ARP
    /*start of Add by t00163975 20110527 for 不转发WAN侧发来的与br0接口IP同网段的ARP */
    if(should_drop_arp_pakcet(skb))
    {
//            printk("File[%s],Line[%d]:packet from %s to %s drop\n",__FILE__, __LINE__, skb->dev->name, to->dev->name);
            kfree_skb(skb);
            return;
    }
    /*end of Add by t00163975 20110527 for 不转发WAN侧发来的与br0接口IP同网段的ARP */
#endif

	/* BEGIN: Added by h00169677, 2012/5/28   PN:DTS2012021404606 */
#ifdef CONFIG_IPV6
	if(should_drop_icmpv6ns_pakcet(skb))
	{
		kfree_skb(skb);
        return;
	}
#endif
	/* END:   Added by h00169677, 2012/5/28 */
	
	indev = skb->dev;

/* BEGIN: Added by z67728, 2010/9/2 统计WAN侧下行流和LAN侧上行流*/
#if defined(CONFIG_BR_IGMP_STATISTIC)
    if (is_multicast_ether_addr(eth_hdr(skb)->h_dest) && !is_broadcast_ether_addr(eth_hdr(skb)->h_dest))
    {
        if ( NULL != to && NULL != to->dev && NULL != indev )
        {
            if ( IS_WAN_DEVICE(indev->name) && IS_LAN_DEV(to->dev->name) )
            {
                /* 下行流进入设备数据报文统计 */
                br_igmp_statistic_inc(skb,IGMP_DOWN_RETRANSMIT_PACKET);
            }
        }
    }
#endif
/* END:   Added by z67728, 2010/9/2 */

    //indev = skb->dev;
	skb->dev = to->dev;
	skb_forward_csum(skb);

/* Start of viewed by f00120964 for qos function 2012-1-6 */
#if defined(CONFIG_IMQ) || defined(CONFIG_IMQ_MODULE)
    skb->mark |= QOS_DEFAULT_MARK;
#endif
/* End of viewed by f00120964 for qos function 2012-1-6 */

/* BEGIN: Added by z67728, 2010/9/2 统计LAN侧下行流和WAN侧上行流*/
#if defined(CONFIG_BR_IGMP_STATISTIC)
    if (is_multicast_ether_addr(eth_hdr(skb)->h_dest) && !is_broadcast_ether_addr(eth_hdr(skb)->h_dest))
    {
        if ( NULL != to && NULL != to->dev && NULL != indev )
        {
            if ( IS_WAN_DEVICE(indev->name) && IS_LAN_DEV(to->dev->name) )
            {
                /* 下行流转发设备数据报文统计 */
                br_igmp_statistic_inc(skb,IGMP_DOWN_RETRANSMIT_PACKET);
            }
            else if ( IS_LAN_DEV(indev->name) && IS_WAN_DEVICE(to->dev->name) )
            {
                /* 上行流转发设备数据报文统计 */
                br_igmp_statistic_inc(skb,IGMP_UP_REQUEST_PACKET);

                /* 上行流转发设备协议报文统计 */
                br_igmp_statistic_inc(skb,IGMP_BUFF);
            }
        }
    }
#endif
/* END:   Added by z67728, 2010/9/2 */

	NF_HOOK(PF_BRIDGE, NF_BR_FORWARD, skb, indev, skb->dev,
			br_forward_finish);
}

/* called with rcu_read_lock */
void br_deliver(const struct net_bridge_port *to, struct sk_buff *skb)
{
	if (should_deliver(to, skb)) {
		__br_deliver(to, skb);
		return;
	}

	kfree_skb(skb);
}

/* called with rcu_read_lock */
void br_forward(const struct net_bridge_port *to, struct sk_buff *skb)
{
	if (should_deliver(to, skb)) {
		__br_forward(to, skb);
		return;
	}

	kfree_skb(skb);
}

#ifdef CONFIG_WAN_LAN_BIND
/*向其它br广播报文*/
static void br_flood_other_bridge(struct net_bridge *br, struct sk_buff *skb,
	void (*__packet_hook)(const struct net_bridge_port *p,
			      struct sk_buff *skb))
{
    struct net_bridge_port *p;
    struct net_device *dev;
    struct net_bridge *brtmp;
    struct sk_buff *skb2;

#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BLOG)
	Blog_t * blog_p = blog_ptr(skb);
	if ( blog_p && !blog_p->rx.info.multicast)
		blog_skip(skb);
#endif

    for_each_netdev(&init_net,dev)
    {
        brtmp = netdev_priv(dev);
        if(IS_BRIDGE_DEV(dev->name) && brtmp != br)
        {
            list_for_each_entry_rcu(p, &brtmp->port_list, list) {
                if (should_deliver(p, skb)
                        && should_eth_deliver(p, skb)) {
#ifdef CONFIG_ATH_WRAP
        			if(PTYPE_IS_WRAP(p->type))
        				skb->mark |= WRAP_BR_MARK_FLOOD;
#endif
       				
        				if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
        					brtmp->dev->stats.tx_dropped++;
        					kfree_skb(skb);
        					return;
        				}
#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BLOG)
        				blog_clone(skb, blog_ptr(skb2));
#endif

        				__packet_hook(p, skb2);

        		}
        	}
        }
    }

}
#endif

/* called under bridge lock */
static void br_flood(struct net_bridge *br, struct sk_buff *skb,
	void (*__packet_hook)(const struct net_bridge_port *p,
			      struct sk_buff *skb))
{
	struct net_bridge_port *p;
	struct net_bridge_port *prev;

#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BLOG)
	Blog_t * blog_p = blog_ptr(skb);
	if ( blog_p && !blog_p->rx.info.multicast)
		blog_skip(skb);
#endif
	prev = NULL;


/*check MARK for RTK CPU rx mark for unicast flooding only*/
#ifdef CONFIG_RTK_EXT_SWITCH 
	/*check all packet toRTK switch and the packet which been marked as also rx from RTK switch.*/
	if((( 0 == (strncmp(skb->dev->name, "eth0.",5)) ) || ( 0 == (strncmp(skb->dev->name, "rtksw",5)) ))
		&& (skb->mark  & 0x00800000))
	{
	}
	else
	{
		skb->mark = skb->mark & 0xFF7FFFFF;	
	}
#endif


	list_for_each_entry_rcu(p, &br->port_list, list) {
/* BEGIN: Added by z67728, 2010/7/12 Add for BT forbid LAN Multicast stream transmit to WAN */
#if 1
        if (should_deliver(p, skb)
                && should_eth_deliver(p, skb)) {
#ifdef CONFIG_ATH_WRAP
			if(PTYPE_IS_WRAP(p->type))
				skb->mark |= WRAP_BR_MARK_FLOOD;
#endif
#else
        if (should_deliver(p, skb)
         && should_eth_deliver(p, skb)
         && br_floodtowan_ok(p, skb)) {
#endif
/* END:   Added by z67728, 2010/7/12 */
			if (prev != NULL) {
				struct sk_buff *skb2;

				if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
					br->dev->stats.tx_dropped++;
					kfree_skb(skb);
					return;
				}
#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BLOG)
				blog_clone(skb, blog_ptr(skb2));
#endif

				__packet_hook(prev, skb2);
			}

			prev = p;
		}
	}

#ifdef CONFIG_WAN_LAN_BIND
    /*flood to other bridge*/
    br_flood_other_bridge(br, skb, __packet_hook);
#endif

	if (prev != NULL) {
/* BEGIN: Added by z67728, 2010/7/12 Add for BT forbid LAN Multicast stream transmit to WAN */
#if 0
        if ( br_floodtowan_ok(prev, skb) ){                    
			__packet_hook(prev, skb);
        }
        else {
            goto drop;
        }
#else
        __packet_hook(prev, skb);
#endif
/* END:   Added by z67728, 2010/7/12 */
		return;
	}
#if 0    
drop:
#endif
	kfree_skb(skb);
    return;
}


#ifdef CONFIG_SUPPORT_ATP
void br_flood_deliver(struct net_bridge *br, struct sk_buff *skb)
{
	br_flood(br, skb, br_deliver_nonas);
}
#else
/* called with rcu_read_lock */
void br_flood_deliver(struct net_bridge *br, struct sk_buff *skb)
{
	br_flood(br, skb, __br_deliver);
}
#endif

/* START ADD BY D00167548 2012.12.13 不转发不透传目的地址是br0 IP  的ARP */
#ifdef CONFIG_WLAN_APCLI_DROP_ARP	
static inline int should_not_flood_local_arp(struct sk_buff *skb)
{
    struct net_device *dev = skb->dev;
    struct arphdr *arp = NULL;
    struct net_device *brdev = NULL;
    struct in_device *indev = NULL;
    struct in_ifaddr *ifa = NULL;
    unsigned char *arp_ptr2 = NULL;
    unsigned char *arp_ptr = NULL;
    struct net *net = dev_net(dev);
	
	if (__constant_htons(ETH_P_8021Q) == skb->protocol)
    {
        struct vlan_hdr *vhdr = (struct vlan_hdr *)skb->data;
        if (vhdr->h_vlan_encapsulated_proto ==  ntohs(ETH_P_ARP))
        {
            arp = (struct arphdr *)(vhdr + 1);
        }
    }
    else
    {
        __be16 ucprotocol = *(__be16 *)&(skb->data[12]);
    	if ((htons(ETH_P_ARP) == ucprotocol) || (htons(ETH_P_ARP) == skb->protocol))
    	{
        	arp = arp_hdr(skb);
    	}
    }
    if(NULL == arp)
    {
    	return false;
    }

	/*不转发不透传目的地址是br0 IP 的 ARP*/
	arp_ptr = (unsigned char *)(arp+1);
	arp_ptr += dev->addr_len; /*ARP 源IP*/
	arp_ptr2 = arp_ptr;
	arp_ptr2 += 4 + dev->addr_len;/*ARP 目的IP*/

	brdev = __dev_get_by_name(net, "br0");	
	if (brdev)
	{
		indev = __in_dev_get_rtnl(brdev);
		if (indev && (NULL != (ifa = indev->ifa_list)))
		{
			for (; ifa; ifa = ifa->ifa_next)
			{
				if (ifa->ifa_local  == (*((unsigned int*)arp_ptr2)))
				{
					return true;
				}
			}
		}
	}
	return false;
}
#endif
/*END ADD BY D00167548 2012.12.13 不转发不透传目的地址是br0 IP  的ARP */

/* called under bridge lock */
void br_flood_forward(struct net_bridge *br, struct sk_buff *skb)
{
	/* START ADD BY D00167548 2012.12.13 不转发不透传目的地址是br0 IP  的ARP */
#ifdef CONFIG_WLAN_APCLI_DROP_ARP	
	if (should_not_flood_local_arp(skb))
	{
		return;
	}
#endif
	/*END ADD BY D00167548 2012.12.13 不转发不透传目的地址是br0 IP  的ARP */
	
	br_flood(br, skb, __br_forward);
}

#ifdef CONFIG_IGMP_SNOOPING
/* DT c4163 add for AU8D04182, add by z67728 for BT IGMP function. This allow interface which don't join currrent multicast group reciving IGMP report packet. */
extern unsigned int br_igmp_snooping_no_v1v2report(const struct net_bridge_port *dev_port, struct sk_buff *skb);

static void br_forward_igmpv1v2report(const struct net_bridge_port * to, struct sk_buff * skb)
{
	if (to != 0 && br_igmp_snooping_no_v1v2report(to, skb)) {
		__br_forward(to, skb);
		return;
	}
	kfree_skb(skb);
}

void br_flood_forward_igmpv1v2report(struct net_bridge *br, struct sk_buff *skb)
{
	br_flood(br, skb, br_forward_igmpv1v2report);
}
/* DT c4163 end */

/* BEGIN: Added by z67728, 2010/10/29 For BT IGMPv3 query and send report*/
static void br_send_igmpv3report(const struct net_bridge_port * to, struct sk_buff * skb)
{
	if (to != NULL && to->dev != NULL && IS_WAN_DEVICE(to->dev->name)) {
		__br_forward(to, skb);
		return;
	}

	kfree_skb(skb);
}
void br_forward_igmpv3report(struct net_bridge *br, struct sk_buff *skb)
{
	br_flood(br, skb, br_send_igmpv3report);
}
/* END:   Added by z67728, 2010/10/29 */
#endif

