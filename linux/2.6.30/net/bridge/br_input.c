/*
 *	Handle incoming frames
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
#include <linux/etherdevice.h>
#include <linux/netfilter_bridge.h>
#include "br_private.h"
#if defined(CONFIG_MIPS_BRCM)
#include <linux/if_vlan.h>
#include <linux/timer.h>
#include <linux/igmp.h>
#include <linux/ip.h>
#include <linux/blog.h>
#endif
#include "br_private.h"
#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BR_IGMP_SNOOP)
#include "br_igmp.h"
#endif
#ifdef CONFIG_IGMP_SNOOPING
#include "br_igmp_snooping.h"
#endif
#ifdef CONFIG_MLD_SNOOPING
#include "br_mld_snooping.h"
#endif

#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BR_MLD_SNOOP)
#include "br_mld.h"
#endif

#if defined(CONFIG_MIPS_BRCM)
#if defined(CONFIG_BROADCOM_RELEASE_VERSION_4_12_L08)
extern int pktc_tx_enabled;
#endif
#endif

#if defined(CONFIG_BR_IGMP_STATISTIC)
#include <linux/igmp_statistic.h>
#endif

#include <linux/if_arp.h>
#include <linux/if_vlan.h>
#include <linux/inetdevice.h>

/* BEGIN: Added by h00169677, 2012/6/8   PN:DTS2012052606695 */
#ifdef CONFIG_IPV6
#include <net/ndisc.h>
#include <linux/ipv6.h>
#include <net/ipv6.h>
extern int	ipv6_get_lladdr(struct net_device *dev,struct in6_addr *addr,unsigned char banned_flags);
static inline int should_drop_ns_fromwan(struct sk_buff *skb);
#endif
#include "atp_interface.h"
/* END:   Added by h00169677, 2012/6/8 */
/**
 * Features changed due to FAP power up/down
**/
/*Start ADD: w00178730 on 2013/11/26 for DTS2013101800046*/
#ifdef CONFIG_ATP_GRE_FON
extern unsigned char g_acAgentCircuitIDOpenSSID[64];
extern unsigned char g_acAgentCircuitIDClosedSSID[64];
extern unsigned char g_acAgentRemoteID[32];
#endif
/*End ADD: w00178730 on 2013/11/26 for DTS2013101800046*/

/* Bridge group multicast address 802.1d (pg 51). */
const u8 br_group_address[ETH_ALEN] = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x00 };

#if defined(CONFIG_BRIDGE_DEV_RELAY) || defined(CONFIG_BRIDGE_PORT_RELAY) || defined(CONFIG_DHCPS_OPTION125)
//iIptype表示IP的版本，0:IPV6 1:IPV4
#define IPV4_TYPE_CODE   0
#define IPV6_TYPE_CODE   1
#define IPBOTH_TYPE_CODE 2
#ifdef CONFIG_WAN_LAN_BIND
#define PORT_RELAY_TYPE IPBOTH_TYPE_CODE
#else
#define PORT_RELAY_TYPE IPV4_TYPE_CODE
#endif
int IsSkbfromDhcpsPort(struct sk_buff *skb, int iIptype)
{
    int iIsdhcpsport = 0;
    unsigned short usIpHeadLen = 0;

    if (NULL == skb)
    {
        return iIsdhcpsport;
    }

    usIpHeadLen = (skb->data[0] & 0x0f) * 4;

    //IPV6
#ifdef CONFIG_IPV6
    if ((htons(ETH_P_IPV6) == eth_hdr(skb)->h_proto) && (IPV6_TYPE_CODE == iIptype || IPBOTH_TYPE_CODE == iIptype))
    {
        if (IPPROTO_UDP == skb->data[6])
        {
            /*IPV6的情况下端口在skb中所处的位置*/
            if (((0x02 == skb->data[40]) && (0x22 == skb->data[41]))
                && ((0x02 == skb->data[42]) && (0x23 == skb->data[43])))
            {
                iIsdhcpsport = 1;
            }
        }
        else if (IPPROTO_ICMPV6 == skb->data[6])          /* RS/RA */
        {
            if ((0x85 == skb->data[40]) || (0x86 == skb->data[40]))
            {
                iIsdhcpsport = 1;
            }
        }
    }
    else
#endif
    {
        //IPV4
        if((skb->data[9] == IPPROTO_UDP) && (IPV4_TYPE_CODE == iIptype || IPBOTH_TYPE_CODE == iIptype))
        {
            if(skb->data[usIpHeadLen] == 0x00 && skb->data[usIpHeadLen + 1] == 0x44 
            && skb->data[usIpHeadLen + 2] == 0x00 && skb->data[usIpHeadLen + 3] == 0x43)
            {
                iIsdhcpsport = 1;
            }
        }
    }

    return iIsdhcpsport;
}
#endif

#ifdef CONFIG_BRIDGE_PORT_RELAY
int DevDhcpRelayOn(const char* name)
{
    int i = 0;
    int iEnable = 0;
    
    for( i = 0; i < PORT_RELAY_NUM; i++)
    {
        if (strstr(name, g_portrelay.acWanName[i]))
        {
            iEnable = g_portrelay.iEnable[i];
            break;
        }
    }

    return iEnable;
}

static int Br_port_dhcp_relay(struct sk_buff *skb)
{
    struct net_bridge_port *p;
    int iEnable = 0;

	if(NULL == skb)
	{
		return iEnable;
	}

    /*判断是否开启端口relay*/
    if(IS_LAN_DEV(skb->dev->name) && DevDhcpRelayOn(skb->dev->name))
    {
        return 1;
    }

    /*遍历接口所在的bridge是否有WAN开启DHCP Relay*/
    if(skb->dev->br_port)
    {
        /*判断br是否要开启relay*/
        if(DevDhcpRelayOn(skb->dev->br_port->br->dev->name))
        {
            return 1;
        }
    }
    return iEnable;
}
#endif

/*START: add by h00190880 for CT dhcps devrelay at 2013-1-17*/
#ifdef CONFIG_BRIDGE_DEV_RELAY
#define OPTION_FIELD       0
#define FILE_FIELD         1
#define SNAME_FIELD        2

#define OPT_CODE           0
#define OPT_LEN            1
#define OPT_DATA           2
#define DHCP_VENDOR        0x3c
#define OPT_MAX_LEN        308
#define OPT_CODE_LEN_MAX   2
#define OPT_FIFLD_MAX      128
#define OPT_SNAME_MAX      64

#define DHCP_PADDING       0x00
#define DHCP_OPTION_OVER   0x34
#define DHCP_END           0xFF

typedef enum
{
    DHCP_DEVTYPE_STB = 0,
    DHCP_DEVTYPE_PHONE,
    DHCP_DEVTYPE_CAMERA,
    DHCP_DEVTYPE_COMPUTER,
    /* more add here */
    DHCP_DEVTYPE_BUTT
}DHCP_DEVTYPE_EN;

static unsigned char *get_option_vendor(unsigned char* data, int code)
{
    int i, length;
    static char err[] = "bogus packet, option fields too long."; /* save a few bytes */
    unsigned char *optionptr;
    int over = 0, done = 0, curr = OPTION_FIELD;

    if (NULL == data)
    {
        return NULL;
    }

    optionptr = data;
    i = 0;
    length = OPT_MAX_LEN;
    while (!done) 
    {
        if (i >= length) 
        {
            return NULL;
        }

        if (optionptr[i + OPT_CODE] == code) 
        {
            if (i + 1 + optionptr[i + OPT_LEN] >= length) 
            {
                return NULL;
            }
            return optionptr + i + OPT_CODE_LEN_MAX;
        }

        switch (optionptr[i + OPT_CODE])
        {
            case DHCP_PADDING:
                i++;
                break;
            case DHCP_OPTION_OVER:
                if (i + 1 + optionptr[i + OPT_LEN] >= length) 
                {
                    return NULL;
                }
                over = optionptr[i + 3];
                i += optionptr[OPT_LEN] + OPT_CODE_LEN_MAX;
                break;
            case DHCP_END:
                if (curr == OPTION_FIELD && over & FILE_FIELD)
                {
                    //optionptr = packet->file;
                    i = 0;
                    length = OPT_FIFLD_MAX;
                    curr = FILE_FIELD;
                } 
                else if (curr == FILE_FIELD && over & SNAME_FIELD) 
                {
                    //optionptr = packet->sname;
                    i = 0;
                    length = OPT_SNAME_MAX;
                    curr = SNAME_FIELD;
                } else done = 1;
                break;
            default:
                i += optionptr[OPT_LEN + i] + OPT_CODE_LEN_MAX;
        }
    }
    return NULL;
}

static int find_relay_index(unsigned char *buff)
{
    int index;

    if(!memcmp(buff, "STB", 3))
    {
       index = DHCP_DEVTYPE_STB;
    }
    else if(!memcmp(buff, "Phone", 5))
    {
       index = DHCP_DEVTYPE_PHONE;
    }
    else if(!memcmp(buff, "Camera", 6))
    {
       index = DHCP_DEVTYPE_CAMERA;
    }
    else
    {
      index = DHCP_DEVTYPE_COMPUTER;
    }    
    
    return index;
}

static int Br_dev_dhcp_relay(struct sk_buff *skb)
{
    int iRetCode = 0;
    int relay_index = 0;
    int index = 0;
    int lOptLength = 0;
    static unsigned char tmpbuff[255];
    unsigned char *option = NULL;
    unsigned short usIpHeadLen = 0;

    if (NULL == skb)
    {
        return iRetCode;
    }
	memset(tmpbuff, 0, sizeof(tmpbuff));
    usIpHeadLen = (skb->data[0] & 0x0f) * 4;

    //判断OPTION60是否携带了STB,PHONE,CAMERA,COMPUTER这些设备类型，子option从usIpHeadLen + 248开始
    option = get_option_vendor(&skb->data[usIpHeadLen + 248], DHCP_VENDOR);
    if(NULL != option)
    {
        unsigned char *tmppoint = option;
        
        tmppoint += 2;
        for(index = DHCP_DEVTYPE_STB; index < DHCP_DEVTYPE_BUTT; index++)
        {
            if(NULL != tmppoint && lOptLength < *(option - 1))
            {
                if(*tmppoint == 2)
                {
                    memcpy(tmpbuff, tmppoint + 2, *(tmppoint + 1));
                    break;
                }
            }
            else
            {
                index = DHCP_DEVTYPE_STB;
                break;
            }
            tmppoint = tmppoint + *(tmppoint + 1) + 2;
            lOptLength = tmppoint - option;
        }

        if(index == DHCP_DEVTYPE_STB)
        {
            memcpy(tmpbuff, option, *(option -1));
        }

        relay_index = find_relay_index(tmpbuff); 
    }
    else
    {
        relay_index = DHCP_DEVTYPE_COMPUTER;
    }

    if(g_devrelay.iEnable[relay_index])
    {
        iRetCode = 1;
    }

    return iRetCode;
}
#endif
/*END: add by h00190880 for CT dhcps devrelay at 2013-1-17*/

#ifdef CONFIG_ATP_DROP_ARP

/* BEGIN: Added by z67728, 2010/11/18 过滤转发的非网关发送的包含网关br0地址ip的arp报文*/
static inline int IsNeedDropArp(struct sk_buff *skb)
{
    struct net_device *dev = skb->dev;
    struct arphdr *arp = NULL;
    unsigned char *arp_ptr2 = NULL;
    unsigned char *arp_ptr = NULL;

    if (__constant_htons(ETH_P_8021Q) == skb->protocol)
    {
        struct vlan_ethhdr *vhdr = (struct vlan_ethhdr *)skb->data;
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

    if( NULL == arp )
    {
    	return 0;
    }

    //printk("\r\n OPT[%x] dev[%s]\r\n",htons(arp->ar_op),dev->name);
    /* 防止伪装arp应答和arp请求 */
	arp_ptr   = (unsigned char *)(arp+1);
	arp_ptr  += dev->addr_len;
    arp_ptr2  = arp_ptr;
    arp_ptr2 += 4 + dev->addr_len;

    return check_arp_for_all_br(arp_ptr,arp_ptr2,0);

}
/* END:   Added by z67728, 2010/11/18 */
#endif

#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BLOG)&& !defined(CONFIG_BR_IGMP_SNOOP)
#define PPPOE_UDPPROTOID_OFFSET        17    
#define PPPOE_UDPADDR_OFFSET           24
static void blog_skip_ppp_multicast(struct sk_buff *skb, const unsigned char *dest)
{
	unsigned short usProtocol;
	unsigned short usProtocol1;
	__u32 *usMulticastIp;

    if(!is_multicast_ether_addr(dest) )
    {
        usProtocol = ((u16 *) skb->data)[-1];
        usProtocol1 = skb->data[PPPOE_UDPPROTOID_OFFSET];
        usMulticastIp = (__u32*)(skb->data + PPPOE_UDPADDR_OFFSET);
        if((usProtocol == __constant_htons(ETH_P_PPP_SES))  
            && (usProtocol1 == IPPROTO_UDP))
        {
            if(MULTICAST((*usMulticastIp)))
            {
                blog_skip(skb);
            }
        }
    }
}
#endif



/*start of 电信增加dhcp server回复报文中添加Option125携带vlan信息by h00190880 2013/1/7*/
#if defined(CONFIG_DHCPS_OPTION125)
enum
{
    DHCPS_INTERFACE_NONE = 0,
    DHCPS_INTERFACE_ETH1,
    DHCPS_INTERFACE_ETH2,
    DHCPS_INTERFACE_ETH3,
    DHCPS_INTERFACE_ETH4,
    DHCPS_INTERFACE_WL1,
    DHCPS_INTERFACE_WL2,
    DHCPS_INTERFACE_WL3,
    DHCPS_INTERFACE_WL4,
    DHCPS_INTERFACE_END,
};
#define DHCPS_OPTION_LAN_CODE       0xfe
#define DHCPS_OPTION_VALUE_LEN      0x01
#define DHCPS_OPTION_LEN            3
#define DHCPS_VLANID_LEN            4
//将Lan口信息加入到dhcp报文中的option字段内，上报给dhcps使用
static void Br_dhcps_add_port_option(struct sk_buff *skb)
{
    int iInterface = 0;
    int i;
    unsigned int tmp, iplen;
    int iIndex = 0;
    int iVlanLen = 0;
    static short ip_header[10] = {0};
    long int  check_base = 0;
    static short upd_check[512] = {0};
    short check_sum = 0;
    /*UDP协议的标示位*/
    static unsigned char upd_protocal[2] = {0x00, 0x11};
    long int  udp_length = 0;

    if(NULL == skb)
    {
    	return;
    }

    iplen =  (skb->data[0] & 0x0f) * 4;                          //计算ip报头的长度

    //获取名称为eth0.x从lan口过来的报文信息
    if('e' == skb->dev->name[0])
    {
        /*带vlan的报文从eth0.x.xxxx口传往上层，不带vlan的报文从eth0.x_x传往上层*/
        if (8 < strlen(skb->dev->name))
        {
            iVlanLen = strlen(skb->dev->name) - 7;
        }
        switch(skb->dev->name[5])
        {
            case '2':
                iInterface = DHCPS_INTERFACE_ETH4;
                break;
            case '3':
                iInterface = DHCPS_INTERFACE_ETH3;
                break;
            case '4':
                iInterface = DHCPS_INTERFACE_ETH2;
                break;
            case '5':
                iInterface = DHCPS_INTERFACE_ETH1;
                break;
            default:
                iInterface = DHCPS_INTERFACE_NONE;
        }
        
        //printk("br_input g_cTvLedState :%x\n",g_cTvLedState);
    }
    //获取名称为wl0.x从wifi口过来的报文信息
    else if('w' == skb->dev->name[0])
    {
        /*带vlan的报文从wl0.x.xxxx或wl0.xxxx口传往上层，不带vlan的报文从wl0.x_x或者wl0传往上层*/
        if (8 < strlen(skb->dev->name))
        {
            iVlanLen = strlen(skb->dev->name) - 6;
        }
        else if (8 == strlen(skb->dev->name))
        {
            iVlanLen = strlen(skb->dev->name) - 4;
        }

        if('.' == skb->dev->name[3])
        {
            switch(skb->dev->name[4])
            {
                case '1':
                    iInterface = DHCPS_INTERFACE_WL2;
                    break;
                case '2':
                    iInterface = DHCPS_INTERFACE_WL3;
                    break;
                case '3':
                    iInterface = DHCPS_INTERFACE_WL4;
                    break;
                default:
                    iInterface = DHCPS_INTERFACE_NONE;
                    break;
            }
        }
        else
        {
            iInterface = DHCPS_INTERFACE_WL1;
        }
    }
    /*将lan口信息加入报文尾部，作为option上报到dhcps
      这里考虑udp末尾可能会加入0x0作为padding的情况
      加入2字节数据，不用再考虑padding*/
    //win7发送的报文中末尾携带多个0x0，导致异常
    i = -1;
    while (skb->tail[i] == 0x00)
        i--;
    if (0xff != skb->tail[i])
    {
        return;
    }
    skb->tail[i++] = DHCPS_OPTION_LAN_CODE;
    skb->tail[i++] = DHCPS_OPTION_VALUE_LEN + iVlanLen;    
    skb->tail[i++] = iInterface;
    for (iIndex = strlen(skb->dev->name) - iVlanLen; iIndex < strlen(skb->dev->name); iIndex++)
    {
        //将字符格式的vlan转换为十六进制写入
        skb->tail[i++] = skb->dev->name[iIndex] - 0x30;
    }
    skb->tail[i++] = 0xff;//末尾endoption字段
    while (DHCPS_OPTION_LEN + iVlanLen > i)
        skb->tail[i++] = 0x00;
    //重新计算报文长度
    skb->tail += (DHCPS_OPTION_LEN + iVlanLen);

    skb->data[3] += (DHCPS_OPTION_LEN + iVlanLen);
    if (skb->data[3] < DHCPS_OPTION_LEN + iVlanLen)
        skb->data[2]++;
    skb->data[iplen + 5] += (DHCPS_OPTION_LEN + iVlanLen);
    if (skb->data[iplen + 5] < DHCPS_OPTION_LEN + iVlanLen)
        skb->data[iplen + 4]++;

    //重新计算ip报头校验值
    skb->data[10] = 0x00;
    skb->data[11] = 0x00;
    /*2. copy ip_header*/
    memcpy(ip_header, &(skb->data[0]), sizeof(ip_header));
    for(i = 0; i < 10; i++)
    {
        check_base += (long int)(ip_header[i] & 0xffff);
    }

    // take only 16 bits out of the 32 bit sum and add up the carries
    while (check_base>>16)
    {
        check_base = (check_base & 0xFFFF)+(check_base >> 16);
    }

    check_sum = 0xffff - (short)(check_base);
    memcpy(&(skb->data[10]), &check_sum, sizeof(check_sum));

    //将udp校验值置为0，取消校验
    skb->data[iplen + 7] = 0x00;
    skb->data[iplen + 6] = 0x00;
    skb->len += DHCPS_OPTION_LEN + iVlanLen;  

    return;
}
#endif
/*end of 电信增加dhcp server回复报文中添加Option125携带vlan信息by h00190880 2013/1/7*/

/*ipcheck优化，socket接收时携带lan indev信息*/
#define PACKET_SKB_CB(__skb)    ((struct packet_skb_cb *)((__skb)->cb))

struct packet_skb_cb {
    unsigned int origlen;
    union {
        struct sockaddr_pkt pkt;
        struct sockaddr_ll ll;
    } sa;
};

static int is_arp_pkt(const struct sk_buff *skb)
{
    __be16 ucprotocol = 0;
    struct vlan_ethhdr *vhdr = NULL;

    if (NULL == skb)
    {
        return 0;
    }

    if (__constant_htons(ETH_P_8021Q) == skb->protocol)
    {
        vhdr = (struct vlan_ethhdr *)skb->data;
        if (ntohs(ETH_P_ARP) == vhdr->h_vlan_encapsulated_proto) 
        {   
            return 1;
        }

        return 0;
    }

    ucprotocol = *(__be16 *)&(skb->data[12]);
    if ((htons(ETH_P_ARP) == ucprotocol) || (htons(ETH_P_ARP) == skb->protocol))
    {
        return 1;
    }

    return 0;
}
/*ipcheck 优化新增代码结束*/

static void br_pass_frame_up(struct net_bridge *br, struct sk_buff *skb)
{
	struct net_device *indev, *brdev = br->dev;
    struct sockaddr_pkt *spkt = NULL;

#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BLOG)
	blog_link(IF_DEVICE, blog_ptr(skb), (void*)br->dev, DIR_RX, skb->len);
#endif

#if defined(CONFIG_BR_IGMP_STATISTIC)
    if ( !is_multicast_ether_addr(eth_hdr(skb)->h_dest) )
    {
        if ( IS_LAN_DEV(skb->dev->name) )
        {
            /* 上行流转发设备数据报文统计 */
            br_igmp_statistic_inc(skb,IGMP_UP_REQUEST_PACKET);
        }
    }
#endif    
	brdev->stats.rx_packets++;
	brdev->stats.rx_bytes += skb->len;

    /*ipcheck接收arp的lan indev*/
    if (is_arp_pkt(skb))
    {
        spkt = &PACKET_SKB_CB(skb)->sa.pkt;
        strlcpy(spkt->spkt_device, skb->dev->name, sizeof(spkt->spkt_device));
    }

	indev = skb->dev;
    /* add lan interface (eth0.5) in iptables 20090217 */
    skb->lanindev = indev;
	skb->dev = brdev;
#ifdef CONFIG_WAN_LAN_BIND
    /*所有报文都发给br0*/
    skb->dev = dev_get_by_name(&init_net, ATP_DEFAULT_BRIDGE_NAME);
#endif

	NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_IN, skb, indev, NULL,
		netif_receive_skb);
}
/*Start ADD: w00178730 on 2013/11/26 for DTS2013101800046*/
#ifdef CONFIG_ATP_GRE_FON
#define ADD_OPTION82_FAIL    0
#define ADD_OPTION82_SUCCESS 1
#define ADD_OPTION82_NONEED  2

//#define GRE_OPTION82_DEBUG 1
#ifdef GRE_OPTION82_DEBUG
#define ADD_OPTION82_DEBUG(format, args...) {printk("\nFILE: %s LINE: %d: ", __FILE__, __LINE__);printk(format, ##args);printk("\n");}
#else
#define ADD_OPTION82_DEBUG(format, args...)
#endif

/******************************************************************************
  函数名称：checksumcalc
  功能描述：计算报文ip head checksum
  输入参数：buf:指向skb buffer data
            nword:ip head长度(单位:16 bits)
  输出参数：NULL
  返 回 值：ip head checksum
  其它说明：
******************************************************************************/
static unsigned short checksumcalc(unsigned short *buf, unsigned short nword)
{
    unsigned long sum = 0;
    unsigned short addCount = 0;

    for (sum = 0; nword > 0; nword--)
    {   
        /*  重新计算ip head checksum时，要将原先的checksum置0 */
        if (5 == addCount)
        {
            sum += 0;
            buf++;
        }
        else
        {
            sum += *buf++;
        }
        addCount++;
    }

    /*  Fold 32-bit sum to 16 bits */
    sum = (sum>>16) + (sum&0xffff);
    sum += (sum>>16);

    return ~sum;
}


/******************************************************************************
  函数名称：br_add_option82_for_gre_ssid
  功能描述：STA连接GRE FON SSID，认证通过后，STA发送DHCP报文，网关截获DHCP报文，
            并添加OPTION 82的相关信息
  输入参数：br，skb
  输出参数：NULL
  返 回 值：ADD_OPTION82_SUCCESS -- 成功插入option 82
            ADD_OPTION82_FAIL -- 插入option 82失败
            ADD_OPTION82_NONEED -- 无需插入option 82
  其它说明：
******************************************************************************/
static unsigned int br_add_option82_for_gre_ssid(struct net_bridge *br, struct sk_buff *skb)
{
    unsigned short usIpHeadLen = 0;
    char acAgentCircuitID[64] = {0};
    char acAgentRemoteID[32] = {0};
    unsigned int ulOption82Len = 0;
    unsigned short checksum = 0;
    unsigned char *pTmp = NULL;
    int i = 0;

    if (NULL == br || NULL == skb)
    {
        ADD_OPTION82_DEBUG("br or skb is null.");
        return ADD_OPTION82_FAIL;
    }
    
    // check ip protocol, udp is what we want
    if (IPPROTO_UDP == skb->data[9])
    {
        // get ip head length
        usIpHeadLen = (skb->data[0] & 0x0f) * 4;
        
        // check bootp kind, bootp request is what we want
        if (0x00 == skb->data[usIpHeadLen] && 0x44 == skb->data[usIpHeadLen + 1] //bootpc 68
                   && 0x00 == skb->data[usIpHeadLen + 2] && 0x43 == skb->data[usIpHeadLen + 3]) //bootps 67
        {
            ADD_OPTION82_DEBUG("skb->dev->name is %s", skb->dev->name);

            //只往来自于wl0.1和wl0.2的DHCP请求报文里插入option82
            if (('w' == skb->dev->name[0]) && ('.' == skb->dev->name[3]) && ('1' == skb->dev->name[4]))//wl0.1
            {
                //配置SSID2 Agent Circuit ID: AP-NAME-STRING+CHAR(0)+SSID-STRING+CHAR(0)+SSID-TYPE
                snprintf(acAgentCircuitID, sizeof(acAgentCircuitID), "%s", g_acAgentCircuitIDOpenSSID);
            }
            else if (('w' == skb->dev->name[0]) && ('.' == skb->dev->name[3]) && ('2' == skb->dev->name[4]))
            {
                //配置SSID3 Agent Circuit ID: AP-NAME-STRING+CHAR(0)+SSID-STRING+CHAR(0)+SSID-TYPE
                snprintf(acAgentCircuitID, sizeof(acAgentCircuitID), "%s", g_acAgentCircuitIDClosedSSID);
            }
            else
            {
                return ADD_OPTION82_NONEED;
            }
            //配置Agent Remote ID
            //Remote-id MUST be a string containing UE’s MAC address with format of "xx:xx:xx:xx:xx:xx".
            snprintf(acAgentRemoteID, sizeof(acAgentRemoteID), "%s", g_acAgentRemoteID);

            ADD_OPTION82_DEBUG("Agent Circuit ID on %s is %s", skb->dev->name, acAgentCircuitID);
            ADD_OPTION82_DEBUG("Agent Remote ID on %s is %s", skb->dev->name, acAgentRemoteID);

            //增加skb数据空间:option 82长度
            ulOption82Len = strlen(acAgentCircuitID) + strlen(acAgentRemoteID) + 6;
            ADD_OPTION82_DEBUG("Option82 Len is %d", ulOption82Len);

            pTmp = skb_put(skb, ulOption82Len);
            memset(pTmp, 0, ulOption82Len);

            // 往新增的空间填充option 82内容
            pTmp = skb->tail - ulOption82Len;
            while (0x0 == *pTmp)
            {
                pTmp--;
            }
            if (0xff != *pTmp)
            {
                ADD_OPTION82_DEBUG("Invalid DHCP packet!");
                return ADD_OPTION82_FAIL;
            }
            *(pTmp++) = 0x52; // type
            *(pTmp++) = (ulOption82Len - 2); // length

            *(pTmp++) = 0x01; // Agent Circuit ID type
            *(pTmp++) = strlen(acAgentCircuitID); // Agent Circuit ID length
            for (i=0; i<strlen(acAgentCircuitID); i++)
            {
                *(pTmp++) = acAgentCircuitID[i];
            }

            *(pTmp++) = 0x02; // Agent Remote ID type
            *(pTmp++) = strlen(acAgentRemoteID); // Agent Remote ID length
            for (i=0; i<strlen(acAgentRemoteID); i++)
            {
                *(pTmp++) = acAgentRemoteID[i];
            }
            *pTmp = 0xff; // end option

            //重新计算IP头部长度字段
            skb->data[3] += ulOption82Len;
            if (skb->data[3] < ulOption82Len)
            {
                skb->data[2]++;
            }

            //重新计算UDP头部长度字段
            skb->data[usIpHeadLen+5] += ulOption82Len;
            if (skb->data[usIpHeadLen+5] < ulOption82Len)
            {
                skb->data[usIpHeadLen+4]++;
            }

            //重新计算ip头部checksum
            checksum = checksumcalc((unsigned short *)skb->data, usIpHeadLen/2);
            ADD_OPTION82_DEBUG("ip check sum is 0x%X", checksum);
            skb->data[10] = (unsigned char)(checksum&0xff);
            skb->data[11] = (unsigned char)(checksum>>8);

            //将udp校验值置为0，取消校验
            skb->data[usIpHeadLen+7] = 0;
            skb->data[usIpHeadLen+6] = 0;
#if 0 // for debug
            ADD_OPTION82_DEBUG("after add option 82, content as follow:");

            for (i = 0; i < skb->len; i++)
            {
                printk("%02x ", skb->data[i]);
            }
            printk("\n\n");
#endif
            return ADD_OPTION82_SUCCESS;
        }
    }

    return ADD_OPTION82_NONEED;
}
#endif
/*End ADD: w00178730 on 2013/11/26 for DTS2013101800046*/

/* note: already called with rcu_read_lock (preempt_disabled) */
int br_handle_frame_finish(struct sk_buff *skb)
{
	const unsigned char *dest = eth_hdr(skb)->h_dest;
	struct net_bridge_port *p = rcu_dereference(skb->dev->br_port);
	struct net_bridge *br;
	struct net_bridge_fdb_entry *dst;
	struct sk_buff *skb2;
#if defined(CONFIG_MIPS_BRCM)
	struct iphdr *pip = NULL;
	__u8 igmpTypeOffset = 0;
#endif
    int iRelayOff = 0;

	if (!p || p->state == BR_STATE_DISABLED)
		goto drop;

#if defined(CONFIG_MIPS_BRCM)
	if ( vlan_eth_hdr(skb)->h_vlan_proto == ETH_P_IP )
	{
		pip = ip_hdr(skb);
		igmpTypeOffset = (pip->ihl << 2);
	}
	else if ( vlan_eth_hdr(skb)->h_vlan_proto == ETH_P_8021Q )
	{
		if ( vlan_eth_hdr(skb)->h_vlan_encapsulated_proto == ETH_P_IP )
		{
			pip = (struct iphdr *)(skb_network_header(skb) + sizeof(struct vlan_hdr));
			igmpTypeOffset = (pip->ihl << 2) + sizeof(struct vlan_hdr);
		}
	}

	if ((pip) && (pip->protocol == IPPROTO_IGMP))
	{
#if defined(CONFIG_BCM_GPON_MODULE)
		struct igmphdr *ih = (struct igmphdr *)&skb->data[igmpTypeOffset];

		/* drop IGMP v1 report packets */
		if (ih->type == IGMP_HOST_MEMBERSHIP_REPORT)
		{
			goto drop;
		}

		/* drop IGMP v1 query packets */
		if ((ih->type == IGMP_HOST_MEMBERSHIP_QUERY) &&
		    (ih->code == 0))
		{
			goto drop;
		}

		/* drop IGMP leave packets for group 0.0.0.0 */
		if ((ih->type == IGMP_HOST_LEAVE_MESSAGE) &&
          (0 == ih->group) )
		{
			goto drop;
		}
#endif
		/* rate limit IGMP */
		br = p->br;
		if ( br->igmp_rate_limit )
		{
			ktime_t      curTime;
			u64          diffUs;
			unsigned int usPerPacket;
			unsigned int temp32;
			unsigned int burstLimit;

			/* add tokens to the bucket - compute in microseconds */
			curTime     = ktime_get();
			usPerPacket = (1000000 / br->igmp_rate_limit);
			diffUs      = ktime_to_us(ktime_sub(curTime, br->igmp_rate_last_packet));
			diffUs     += br->igmp_rate_rem_time;

			/* allow 25% burst */
			burstLimit = br->igmp_rate_limit >> 2;
			if ( 0 == burstLimit)
			{
				burstLimit = 1;
			}

			if ( diffUs > 1000000 )
			{
				br->igmp_rate_bucket   = burstLimit;
				br->igmp_rate_rem_time = 0;
			}
			else
			{
				temp32 = (unsigned int)diffUs / usPerPacket;
				br->igmp_rate_bucket += temp32;
				if (temp32)
				{
					br->igmp_rate_rem_time = diffUs - (temp32 * usPerPacket);
				}
			}
			if (br->igmp_rate_bucket > burstLimit)
			{
				br->igmp_rate_bucket   = burstLimit;
				br->igmp_rate_rem_time = 0;
			}

			/* if bucket is empty drop the packet */
			if (0 == br->igmp_rate_bucket)
			{
				goto drop;
			}
			br->igmp_rate_bucket--;
			br->igmp_rate_last_packet.tv64 = curTime.tv64;
		}
	}
#endif

	/* insert into forwarding database after filtering to avoid spoofing */
	br = p->br;
#ifdef CONFIG_ATH_WRAP
	/*Skip updating fdb on MPVAP, if dest is mcast*/
	if(unlikely(PTYPE_IS_WRAP(p->type))){
		int type = p->type & PTYPE_MASK;
		int update=1;
		if(!WRAP_PTYPE_HAS_ISO(p->type)){			
			if((type != WRAP_PTYPE_MPVAP) 
		  		||(!is_multicast_ether_addr(dest))){

				if((WRAP_PTYPE_PETH ==type)||(WRAP_PTYPE_PVAP==type))
				{
					update = compare_ether_addr(p->dev->dev_addr, eth_hdr(skb)->h_source);
				}

				if(1 == update) {
					br_fdb_update(br, p, eth_hdr(skb)->h_source);
				}
				else {
					goto drop;
				}
			}
		}	
	}else
#endif		
	{	
		br_fdb_update(br, p, eth_hdr(skb)->h_source);
	}

	if (p->state == BR_STATE_LEARNING)
		goto drop;
	
	/* The packet skb2 goes to the local host (NULL to skip). */
    /*Start ADD: w00178730 on 2013/11/26 for DTS2013101800046*/
#ifdef CONFIG_ATP_GRE_FON
    if (ADD_OPTION82_FAIL == br_add_option82_for_gre_ssid(br,skb))
    {
        ADD_OPTION82_DEBUG("br_add_option82_for_gre_ssid fail.");
    }
#endif
    /*End ADD: w00178730 on 2013/11/26 for DTS2013101800046*/
	skb2 = NULL;

	if (br->dev->flags & IFF_PROMISC)
		skb2 = skb;

	dst = NULL;

	
	/* BEGIN: Added by h00169677, 2012/6/8   PN:DTS2012052606695 */
#ifdef CONFIG_IPV6
	if (should_drop_ns_fromwan(skb))
	{
		goto drop;
	}
#endif
	/* END:   Added by h00169677, 2012/6/8 */

   /* BEGIN: Added by h00169677, 2012/6/21   PN:DTS2012061205128 */
   if ('n' == skb->dev->name[0])
   {
        if ( IsNeedDropArp(skb) )
	    {
	        goto drop;
        }
   }
   /* END:   Added by h00169677, 2012/6/21 */
	
#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BR_MLD_SNOOP)
	if((0x33 == dest[0]) && (0x33 == dest[1])) {
		br->statistics.multicast++;
		skb2 = skb;
		if (br_mld_mc_forward(br, skb, 1, 0))
		{
			skb = NULL;
			skb2 = NULL;
		}
	} else 
#endif
	if (is_multicast_ether_addr(dest)) {
		br->dev->stats.multicast++;
		skb2 = skb;
#if defined(CONFIG_MLD_SNOOPING)
        if ((0x33 == dest[0]) && (0x33 == dest[1]))
        {
            if ((NULL != skb) && (br_mld_snooping_forward(skb,br,dest, 1)))
            {
                skb = NULL;
            }
        }
#endif
        if ((0x33 != dest[0]) || (0x33 != dest[1]))
        {
#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BR_IGMP_SNOOP)
    		if ((NULL != skb) && (br_igmp_mc_forward(br, skb, 1, 0)))
    		{
    			skb = NULL;
    			skb2 = NULL;
    		}
#elif defined(CONFIG_IGMP_SNOOPING)
            if ((NULL != skb) && (br_igmp_snooping_forward(skb, br, (unsigned char *)dest, 1)))
            {
                skb = NULL;
            }
#endif
        }
#if defined(CONFIG_BR_IGMP_STATISTIC)
        /* 此处只统计IGMP协议报文和总报文数 */
        if (!is_broadcast_ether_addr(dest)) {
            br_igmp_statistic_inc(skb2,IGMP_BUFF);
        }
#endif
	} else {
		dst = __br_fdb_get(br, dest);
#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BLOG)
		blog_link(BRIDGEFDB, blog_ptr(skb),
					(void*)__br_fdb_get(br, eth_hdr(skb)->h_source),
					BLOG_PARAM1_SRCFDB, 0);
		blog_link(BRIDGEFDB, blog_ptr(skb), (void*)dst, BLOG_PARAM1_DSTFDB, 0);

		/* wlan pktc */
		if ((dst != NULL) && (!dst->is_local))
		{
#if defined(CONFIG_BROADCOM_RELEASE_VERSION_4_12_L08)
			u8 from_wl_to_switch=0, from_switch_to_wl=0;
			struct net_bridge_fdb_entry *src = __br_fdb_get(br, eth_hdr(skb)->h_source);
			
			if ((src->dst->dev->path.hw_port_type == BLOG_WLANPHY) &&
			    (dst->dst->dev->path.hw_port_type == BLOG_ENETPHY))
				from_wl_to_switch = 1;

			else if ((src->dst->dev->path.hw_port_type == BLOG_ENETPHY) &&
			    (dst->dst->dev->path.hw_port_type == BLOG_WLANPHY) &&
				pktc_tx_enabled)
				from_switch_to_wl = 1;

			if ((from_wl_to_switch || from_switch_to_wl) && !(dst->dst->dev->priv_flags & IFF_WANDEV) && netdev_path_is_root(dst->dst->dev)) { 
				/* also check for non-WAN and non-VLAN cases */
				ctf_brc_hot_t *chainEntry = (ctf_brc_hot_t *)blog_pktc(UPDATE_BRC_HOT, (void*)dst, (uint32_t)dst->dst->dev, 0);
				if (chainEntry)
				{
					//Update chainIdx in blog
					// chainEntry->tx_dev will always be NOT NULL as we just added that above
					if (skb->blog_p != NULL) 
					{
						skb->blog_p->wlTxChainIdx = chainEntry->idx;
						//   printk("Added ChainTableEntry Idx %d Dev %s blogSrcAddr 0x%x blogDstAddr 0x%x DstMac %x:%x:%x:%x:%x:%x\n", 
						//        chainEntry->idx, dst->dst->dev->name, skb->blog_p->rx.tuple.saddr, skb->blog_p->rx.tuple.daddr, dst->addr.addr[0],
						//      dst->addr.addr[1], dst->addr.addr[2], dst->addr.addr[3], dst->addr.addr[4], dst->addr.addr[5]);
					}
				}
			}
#else
            u8 from_wl=0, to_switch=0;
			struct net_bridge_fdb_entry *src = __br_fdb_get(br, eth_hdr(skb)->h_source);
			
			if (src->dst->dev->path.hw_port_type == BLOG_WLANPHY)
				from_wl = 1;
			if (dst->dst->dev->path.hw_port_type == BLOG_ENETPHY)
				to_switch = 1;

			if (from_wl && to_switch && !(dst->dst->dev->priv_flags & IFF_WANDEV) && netdev_path_is_root(dst->dst->dev)) { 
				/* also check for non-WAN and non-VLAN cases */
				blog_pktc(UPDATE_BRC_HOT, (void*)dst, (uint32_t)dst->dst->dev, 0);
			}
#endif
		}
#endif
		if ((dst != NULL) && dst->is_local) 
		{
			skb2 = skb;
			/* Do not forward the packet since it's local. */
			skb = NULL;
		}
	}

#ifdef CONFIG_BRIDGE_PORT_RELAY
    /*目前只进行V4的DHCP RELAY*/
    if(IsSkbfromDhcpsPort(skb, PORT_RELAY_TYPE))
    {
        /*start of ATP 2012-1-9 增加 for DHCP端口绑定二层透传 by h00190880 */
        if(Br_port_dhcp_relay(skb))
        {
            iRelayOff = 0;
            skb2 = NULL;
        }
        else
        {
            iRelayOff = 1;
        }
    }
#endif

    /*START: add by h00190880 for CT dhcps devrelay at 2013-1-17*/
#ifdef CONFIG_BRIDGE_DEV_RELAY
    /*根据dhcp报文中的option60的VENDOR判断是否进行二层透传(只支持IPV4) by h00190880*/
    if (IsSkbfromDhcpsPort(skb, IPV4_TYPE_CODE))
    {
        if (Br_dev_dhcp_relay(skb))
        {
            iRelayOff = 0;
            skb2 = NULL;
        }
    }
#endif
    /*END: add by h00190880 for CT dhcps devrelay at 2013-1-17*/

	if((skb != NULL) && (skb2 == skb))
	{
#ifdef CONFIG_DHCPS_OPTION125
        if (IsSkbfromDhcpsPort(skb, IPV4_TYPE_CODE))
        {
            Br_dhcps_add_port_option(skb);
        }
#endif

		skb2 = skb_clone(skb, GFP_ATOMIC);
	}

	if (skb2) 
		br_pass_frame_up(br, skb2);

    
	if (skb) {
        if (iRelayOff)
        {
            goto drop;
        }

#ifdef CONFIG_ATP_DROP_ARP
	    /* BEGIN: Added by z67728, 2010/9/19 PN:DTS2010091403656,ARP欺骗防御*/
	    if ( IsNeedDropArp(skb) )
	    {
	        goto drop;
            }
            /* END:   Added by z67728, 2010/9/19 */
#endif
		if (dst)
	    {
#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BLOG)&& !defined(CONFIG_BR_IGMP_SNOOP)
#ifndef CONFIG_BCM96362
            blog_skip_ppp_multicast(skb, dest);
#endif
#endif
			br_forward(dst->dst, skb);
	    }
		else
			br_flood_forward(br, skb);
	}

out:
	return 0;
drop:
	kfree_skb(skb);
	goto out;
}

/* note: already called with rcu_read_lock (preempt_disabled) */
static int br_handle_local_finish(struct sk_buff *skb)
{
	struct net_bridge_port *p = rcu_dereference(skb->dev->br_port);

	if (p)
		br_fdb_update(p->br, p, eth_hdr(skb)->h_source);
	return 0;	 /* process further */
}

/* Does address match the link local multicast address.
 * 01:80:c2:00:00:0X
 */
static inline int is_link_local(const unsigned char *dest)
{
	__be16 *a = (__be16 *)dest;
	static const __be16 *b = (const __be16 *)br_group_address;
	static const __be16 m = cpu_to_be16(0xfff0);

	return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | ((a[2] ^ b[2]) & m)) == 0;
}

/*
 * Called via br_handle_frame_hook.
 * Return NULL if skb is handled
 * note: already called with rcu_read_lock (preempt_disabled)
 */
struct sk_buff *br_handle_frame(struct net_bridge_port *p, struct sk_buff *skb)
{
	const unsigned char *dest = eth_hdr(skb)->h_dest;
	int (*rhook)(struct sk_buff *skb);

	if (!is_valid_ether_addr(eth_hdr(skb)->h_source))
		goto drop;

	skb = skb_share_check(skb, GFP_ATOMIC);
	if (!skb)
		return NULL;

	if (unlikely(is_link_local(dest))) {
		/* Pause frames shouldn't be passed up by driver anyway */
		if (skb->protocol == htons(ETH_P_PAUSE))
			goto drop;

		/* If STP is turned off, then forward */
		if (p->br->stp_enabled == BR_NO_STP && dest[5] == 0)
			goto forward;

		if (NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_IN, skb, skb->dev,
			    NULL, br_handle_local_finish))
			return NULL;	/* frame consumed by filter */
		else
			return skb;	/* continue processing */
	}

forward:
	switch (p->state) {
	case BR_STATE_FORWARDING:
		rhook = rcu_dereference(br_should_route_hook);
		if (rhook != NULL) {
			if (rhook(skb))
				return skb;
			dest = eth_hdr(skb)->h_dest;
		}
		/* fall through */
	case BR_STATE_LEARNING:
		if (!compare_ether_addr(p->br->dev->dev_addr, dest))
			skb->pkt_type = PACKET_HOST;

		NF_HOOK(PF_BRIDGE, NF_BR_PRE_ROUTING, skb, skb->dev, NULL,
			br_handle_frame_finish);
		break;
	default:
drop:
		kfree_skb(skb);
	}
	return NULL;
}
/* BEGIN: Added by h00169677, 2012/6/8   PN:DTS2012052606695 */
#ifdef CONFIG_IPV6
#define VLAN_HEAD_LEN 4
#define IPV6_HEAD_LEN 40
#define NS_TYPE_OFFSET 40
#define ICMPV6_OFFSET 6
static inline int should_drop_ns_fromwan(struct sk_buff *skb)
{
    struct net_device *dev ;
	struct net *net;
	struct ipv6hdr *ipv6hd = NULL;
	struct nd_msg *msg = NULL;
	struct in6_addr addr_buf;
	
	memset(&addr_buf, 0, sizeof(struct in6_addr));
    if(!IS_WAN_DEVICE(skb->dev->name))
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
/* END:   Added by h00169677, 2012/6/8 */
