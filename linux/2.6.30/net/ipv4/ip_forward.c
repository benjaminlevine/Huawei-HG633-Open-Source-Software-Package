/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		The IP forwarding functionality.
 *
 * Authors:	see ip.c
 *
 * Fixes:
 *		Many		:	Split from ip.c , see ip_input.c for
 *					history.
 *		Dave Gregorich	:	NULL ip_rt_put fix for multicast
 *					routing.
 *		Jos Vos		:	Add call_out_firewall before sending,
 *					use output device for accounting.
 *		Jos Vos		:	Call forward firewall after routing
 *					(always use output device).
 *		Mike McLagan	:	Routing by source
 */

#include <linux/types.h>
#include <linux/mm.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/netdevice.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/icmp.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/netfilter_ipv4.h>
#include <net/checksum.h>
#include <linux/route.h>
#include <net/route.h>
#include <net/xfrm.h>

	/* Start of modified by f00120964 for qos function 2012-4-2*/
#ifdef CONFIG_DT_QOS
	static unsigned char s_dtQosMarkToTOS[] = {0x0, 0x60, 0x80, 0xA0, 0xC0, 0x0, 0x0, 0x0};
#endif
    /* End of modified by f00120964 for qos function 2012-4-2*/

#ifdef CONFIG_ATP_HYBRID
#define DHCPC_UDP_PORT        68
#endif

#ifdef CONFIG_TCP_GATHER_ACK
#define IP_FORWARD_TCP_GATHER_PKT_LEN 60

extern struct sk_buff_head	g_fwd_tcp_ack_queue;

int ip_forward_gather_tcp_ack(struct sk_buff *skb)
{
	struct iphdr *ip_h = NULL;
	struct tcphdr *tcp_h = NULL;

	/*非LAN侧的包不处理*/
	if (0 != strncmp(skb->dev->name, "br", 2))
	{
		return 0;
	}

	ip_h = ip_hdr(skb);
	if (IPPROTO_TCP != ip_h->protocol)
	{
		return 0;
	}

	if (IP_FORWARD_TCP_GATHER_PKT_LEN < skb->len)
	{
		return 0;
	}

	tcp_h = (struct tcphdr*)((__u32 *)ip_h + ip_h->ihl);
	if (((u_int8_t *)tcp_h)[13] & 0x10)
	{
		/*record short ack to queue*/
		skb_queue_tail(&g_fwd_tcp_ack_queue, skb);
		return 1;
	}

	return 0;
}
#endif

static int ip_forward_finish(struct sk_buff *skb)
{
	struct ip_options * opt	= &(IPCB(skb)->opt);

	IP_INC_STATS_BH(dev_net(skb->dst->dev), IPSTATS_MIB_OUTFORWDATAGRAMS);

	if (unlikely(opt->optlen))
		ip_forward_options(skb);

	/*TCP优化处理*/
#ifdef CONFIG_TCP_GATHER_ACK
	if (ip_forward_gather_tcp_ack(skb))
	{
		return 0;
	}
#endif

	return dst_output(skb);
}

int ip_forward(struct sk_buff *skb)
{
	struct iphdr *iph;	/* Our header */
	struct rtable *rt;	/* Route we use */
#ifdef CONFIG_ATP_HYBRID     
    struct udphdr *uh;	/* do not send exceed icmp when recv the dhcp from HAAP DTS2013101509929 */ 
#endif
	struct ip_options * opt	= &(IPCB(skb)->opt);

	if (skb_warn_if_lro(skb))
		goto drop;

	if (!xfrm4_policy_check(NULL, XFRM_POLICY_FWD, skb))
		goto drop;

	if (IPCB(skb)->opt.router_alert && ip_call_ra_chain(skb))
		return NET_RX_SUCCESS;

	if (skb->pkt_type != PACKET_HOST)
		goto drop;

	skb_forward_csum(skb);

	/*
	 *	According to the RFC, we must first decrease the TTL field. If
	 *	that reaches zero, we must reply an ICMP control message telling
	 *	that the packet's lifetime expired.
	 */
	if (ip_hdr(skb)->ttl <= 1)
		goto too_many_hops;

	if (!xfrm4_route_forward(skb))
		goto drop;

	rt = skb->rtable;

	if (opt->is_strictroute && rt->rt_dst != rt->rt_gateway)
		goto sr_failed;

	if (unlikely(skb->len > dst_mtu(&rt->u.dst) && !skb_is_gso(skb) &&
		     (ip_hdr(skb)->frag_off & htons(IP_DF))) && !skb->local_df) {
		IP_INC_STATS(dev_net(rt->u.dst.dev), IPSTATS_MIB_FRAGFAILS);
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED,
			  htonl(dst_mtu(&rt->u.dst)));
		goto drop;
	}

	/* We are about to mangle packet. Copy it! */
	if (skb_cow(skb, LL_RESERVED_SPACE(rt->u.dst.dev)+rt->u.dst.header_len))
		goto drop;
	iph = ip_hdr(skb);

	/* Decrease ttl after skb cow done */
	ip_decrease_ttl(iph);

	/* Start of modified by f00120964 for qos function 2012-4-2*/
#ifdef CONFIG_DT_QOS
	if ((0x0 != (skb->mark & 0x7)) && (0x0 == iph->tos)) {
		/* Without TOS value, mark it from skb->mark */
		u_int16_t diffs[2];
		diffs[0] = htons(iph->tos) ^ 0xffff;
		iph->tos = s_dtQosMarkToTOS[(skb->mark & 0x7)];
		diffs[1] = htons(iph->tos);
		iph->check = csum_fold(csum_partial((char *)diffs,
			sizeof(diffs), iph->check^0xffff));		
	}
#endif
    /* End of modified by f00120964 for qos function 2012-4-2*/

	/*
	 *	We now generate an ICMP HOST REDIRECT giving the route
	 *	we calculated.
	 */
	if (rt->rt_flags&RTCF_DOREDIRECT && !opt->srr && !skb_sec_path(skb))
		ip_rt_send_redirect(skb);

	skb->priority = rt_tos2priority(iph->tos);

	return NF_HOOK(PF_INET, NF_INET_FORWARD, skb, skb->dev, rt->u.dst.dev,
		       ip_forward_finish);

sr_failed:
	/*
	 *	Strict routing permits no gatewaying
	 */
	 icmp_send(skb, ICMP_DEST_UNREACH, ICMP_SR_FAILED, 0);
	 goto drop;

too_many_hops:
#ifdef CONFIG_ATP_HYBRID 
    /* start do not send exceed icmp when recv the dhcp from HAAP DTS2013101509929 */ 
    if (IPPROTO_UDP == ip_hdr(skb)->protocol)
    {
        __skb_pull(skb, ip_hdrlen(skb));
	    skb_reset_transport_header(skb);   
        uh = udp_hdr(skb);
        if (DHCPC_UDP_PORT == ntohs(uh->dest))
        {
            goto drop;
        }
    }
    /* end do not send exceed icmp when recv the dhcp from HAAP DTS2013101509929 */ 
#endif    
	/* Tell the sender its packet died... */
	IP_INC_STATS_BH(dev_net(skb->dst->dev), IPSTATS_MIB_INHDRERRORS);
	icmp_send(skb, ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, 0);
drop:
	kfree_skb(skb);
	return NET_RX_DROP;
}
