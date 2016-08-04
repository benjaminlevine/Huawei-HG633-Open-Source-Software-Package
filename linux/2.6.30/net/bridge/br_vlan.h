#ifndef __BR_VLAN_H
#define __BR_VLAN_H

#define NO_VLAN -1

/* MAC绑定信息表 */
typedef struct 
{
    struct list_head		list;
    
    char ifLanDev[IFNAMSIZ + 1]; /* LAN口名称 */
    unsigned char cLanMac[ETH_ALEN];/* LAN侧MAC*/
    int  iVlanId ;/* LAN侧VLAN ID */
    unsigned long   iexpire;/* 记录时间*/
}VLAN_MAC_INFO;

void br_vlan_init(void);

void br_vlan_fini(void);

void br_vlan_lan_xmit(struct sk_buff *skb);

#endif
