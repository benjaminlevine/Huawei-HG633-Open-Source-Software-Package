#include <linux/kernel.h>
#include <linux/times.h>
#include <linux/if_vlan.h>
#include <asm/atomic.h>
#include <linux/module.h>
#include <linux/ip.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include <linux/netfilter_bridge.h>

#include "br_private.h"
#include "br_vlan.h"
#include "atp_interface.h"

#define MAX_VLANMACNUM 128

#if 0
#define VLAN_DEBUGP(format, args...) printk("LAN_VLAN: file[%s] line[%d] " format "\r\n", __FILE__, __LINE__, ## args)
#else
#define VLAN_DEBUGP(format, args...)
#endif

struct list_head		vlan_mac_list;

static void br_vlanMacUpdateList(const char *pMac, const char *pLanDev, int iVlan)
{
    VLAN_MAC_INFO *p = NULL;
    VLAN_MAC_INFO *pMin = NULL;
    int iExist = 0;
    int iNum = 0;
    
    list_for_each_entry(p, &vlan_mac_list, list){
        if(memcmp(pMac, p->cLanMac, ETH_ALEN) == 0)
        {
            memcpy(p->ifLanDev, pLanDev, IFNAMSIZ);
            p->iVlanId = iVlan;
            p->iexpire = jiffies;
            iExist = 1;
            break;
        }
        /*�������δ���µı���*/
        if(!pMin || p->iexpire < pMin->iexpire)
        {
            pMin = p;
        }
        iNum++;
    }

    /*������ʱ��Ҫ�¼��뵽list*/
    if(!iExist)
    {
        /*������������½�����*/
        if(iNum < MAX_VLANMACNUM)
        {
            p = kzalloc(sizeof(VLAN_MAC_INFO), GFP_ATOMIC);
            if(!p)
            {
                return;
            }

            memset(p, 0, sizeof(VLAN_MAC_INFO));
            memcpy(p->cLanMac, pMac, ETH_ALEN);
            memcpy(p->ifLanDev, pLanDev, IFNAMSIZ);
            p->iVlanId = iVlan;
            p->iexpire = jiffies;
            list_add(&p->list, &vlan_mac_list);
        }
        else
        {
            /*���������ʱ�滻�����û���µı���*/
            memcpy(pMin->cLanMac, pMac, ETH_ALEN);
            memcpy(pMin->ifLanDev, pLanDev, IFNAMSIZ);
            pMin->iVlanId = iVlan;
            pMin->iexpire = jiffies;
        }
    }
}

/*ɾ������VLAN�ļ�¼*/
static inline void br_vlanMacDeleteNode(const char *pMac)
{
    VLAN_MAC_INFO *p, *n;

    list_for_each_entry_safe(p, n, &vlan_mac_list, list){
        if(memcmp(pMac, p->cLanMac, ETH_ALEN) == 0)
        {
            list_del(&p->list);
            kfree(p);
            break;
        }
    }
}

/*���ұ����ж�Ӧ��VLAN*/
static inline int br_getVlanByMac(const char *pMac)
{
    VLAN_MAC_INFO *p;
    
    list_for_each_entry(p, &vlan_mac_list, list){
        if(memcmp(pMac, p->cLanMac, ETH_ALEN) == 0)
        {
            return p->iVlanId;
        }
    }

    return NO_VLAN;
}


/* ���������vlan tag */
static inline struct sk_buff *br_vlan_put_tag(struct sk_buff *skb, unsigned short tag)
{
    struct vlan_ethhdr *veth;

    VLAN_DEBUGP("br put vlan %d.", tag);
	if (skb_headroom(skb) < VLAN_HLEN) 
	{
		struct sk_buff *sk_tmp = skb;
		skb = skb_realloc_headroom(sk_tmp, VLAN_HLEN);
		kfree_skb(sk_tmp);
		if (!skb) {
			printk(KERN_ERR "vlan: failed to realloc headroom\n");
			return NULL;
		}
	} 
    
	veth = (struct vlan_ethhdr *)skb_push(skb, VLAN_HLEN);

	/* Move the mac addresses to the beginning of the new header. */
	memmove(skb->data, skb->data + VLAN_HLEN, 2 * VLAN_ETH_ALEN);
    skb->mac_header -= VLAN_HLEN;

	/* first, the ethernet type */
	veth->h_vlan_proto = __constant_htons(ETH_P_8021Q);
    
	/* now, the tag */
	veth->h_vlan_TCI = htons(tag);

	skb->protocol = __constant_htons(ETH_P_8021Q);
    
	return skb;
}

/*ȥ��VLANͷ*/
static inline unsigned short br_vlan_remove_tag(struct sk_buff *skb)
{
    struct vlan_hdr   *vlhdr = NULL;
    unsigned short vlan_TCI;
    unsigned short vlan_id;
    
    vlhdr = (struct vlan_hdr*)skb->data;
              
    vlan_TCI = ntohs(vlhdr->h_vlan_TCI);
    vlan_id = (vlan_TCI & VLAN_VID_MASK);

    memmove(skb->data - ETH_HLEN + VLAN_HLEN, skb->data - ETH_HLEN, VLAN_ETH_ALEN * 2);
    skb_pull(skb, VLAN_HLEN);
    skb->mac_header += VLAN_HLEN;
        
    skb->protocol = vlhdr->h_vlan_encapsulated_proto;
    
    return vlan_id;
}

/*�ж���ͬ��VLAN��¼�Ƿ��Ѵ���*/
static inline unsigned short br_vlan_tagged_before(const char *pMac, const char *pDev, int iVlan)
{
    VLAN_MAC_INFO *p;
    unsigned short ret = 0;

    list_for_each_entry(p, &vlan_mac_list, list){
        if(memcmp(pMac, p->cLanMac, ETH_ALEN) == 0)
        {
            break;
        }

        if(p->iVlanId == iVlan && 0 == strcmp(p->ifLanDev, pDev))
        {
            ret = 1;
            break;
        }
    }

    return ret;
}

void br_vlan_lan_xmit(struct sk_buff *skb)
{
    struct sk_buff *skb2;
    VLAN_MAC_INFO *p;
    unsigned char *dest;
    int vlan;
    unsigned short untag = 0;

    dest = eth_hdr(skb)->h_dest;
    /*�㲥ʱ�Ĵ���*/
    if(is_multicast_ether_addr(dest))
    {
        list_for_each_entry(p, &vlan_mac_list, list){
            if(0 == strcmp(skb->dev->name, p->ifLanDev))
            {
                /*����VLAN���������*/
                if(p->iVlanId < 0)
                {
                    untag = 1;
                    continue;
                }
                /*��ͬVLAN���ظ�����*/
                if(br_vlan_tagged_before(p->cLanMac, p->ifLanDev, p->iVlanId))
                {
                    continue;
                }
                
                if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
                    kfree_skb(skb);
                    return;
                }

#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BLOG)
                blog_clone(skb, blog_ptr(skb2));
#endif
                skb2 = br_vlan_put_tag(skb2, p->iVlanId);
                if(skb2)
                {
                    dev_queue_xmit(skb2);
                }
            }
        }
        
        /*����VLAN��Ҳ��һ��*/
        if(untag)
        {
            dev_queue_xmit(skb);
        }
        else
        {
            kfree_skb(skb);
        }
    }
    else
    {
        vlan = br_getVlanByMac(dest);
        if(vlan > 0)
        {
            skb = br_vlan_put_tag(skb, vlan);
            VLAN_DEBUGP("xmit alone packet tag vlan %d.", vlan);
        }
        if(skb)
        {
            dev_queue_xmit(skb);
        }
    }
}

/*�ú�����br_handle_frame_finish֮ǰ���ã������ڹ��ӵ�NF_BR_PRE_ROUTING*/
static unsigned int br_vlan_pre_routing(unsigned int hook, struct sk_buff *skb,
   const struct net_device *indev, const struct net_device *outdev,
   int (*okfn)(struct sk_buff *))
{
    unsigned int ret = NF_ACCEPT;
    unsigned short vlan_id;

    /*�ж��Ƿ�����LAN��*/
    if(!IS_LAN_DEV(indev->name))
    {
        return ret;
    }

    /*�ж��Ƿ����VLAN*/
    if (__constant_htons(ETH_P_8021Q) == skb->protocol)
    {        
        vlan_id = br_vlan_remove_tag(skb);
        VLAN_DEBUGP("receive VLAN %d packet.", vlan_id);

        br_vlanMacUpdateList(eth_hdr(skb)->h_source, indev->name, vlan_id);
    }
    else
    {       
        br_vlanMacUpdateList(eth_hdr(skb)->h_source, indev->name, NO_VLAN);
    }

    return ret;
}


static struct nf_hook_ops br_vlan_ops[] = {
	{ .hook = br_vlan_pre_routing, 
	  .owner = THIS_MODULE, 
	  .pf = PF_BRIDGE, 
	  .hooknum = NF_BR_PRE_ROUTING, 
	  .priority = NF_BR_PRI_BRNF - 100, },   //���ȼ���� -100
};

void br_vlan_init(void)
{
	int i;

    INIT_LIST_HEAD(&vlan_mac_list);
    
	for (i = 0; i < ARRAY_SIZE(br_vlan_ops); i++) {
		int ret;

		if ((ret = nf_register_hook(&br_vlan_ops[i])) >= 0)
			continue;

		while (i--)
			nf_unregister_hook(&br_vlan_ops[i]);

		return ;
	}

	printk(KERN_NOTICE "Bridge LAN vlan registered\n");

	return ;
}

void br_vlan_fini(void)
{
	int i;

	for (i = ARRAY_SIZE(br_vlan_ops) - 1; i >= 0; i--)
		nf_unregister_hook(&br_vlan_ops[i]);

	return ;
}


