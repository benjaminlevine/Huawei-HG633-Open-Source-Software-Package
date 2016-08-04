#ifndef __BR_VLAN_H
#define __BR_VLAN_H

#define NO_VLAN -1

/* MAC����Ϣ�� */
typedef struct 
{
    struct list_head		list;
    
    char ifLanDev[IFNAMSIZ + 1]; /* LAN������ */
    unsigned char cLanMac[ETH_ALEN];/* LAN��MAC*/
    int  iVlanId ;/* LAN��VLAN ID */
    unsigned long   iexpire;/* ��¼ʱ��*/
}VLAN_MAC_INFO;

void br_vlan_init(void);

void br_vlan_fini(void);

void br_vlan_lan_xmit(struct sk_buff *skb);

#endif
