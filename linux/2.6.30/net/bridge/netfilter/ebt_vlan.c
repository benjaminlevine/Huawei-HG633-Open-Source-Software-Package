/*
 * Description: EBTables 802.1Q match extension kernelspace module.
 * Authors: Nick Fedchik <nick@fedchik.org.ua>
 *          Bart De Schuymer <bdschuym@pandora.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
/*********************************************************************
 Edit History of Huawei: 
 <修改人>   <修改时间>   <问题单号>          <修改描述>
t00176367          2015/02/06            DTS2015020606189        海思方案vlan 层非硬加速问题qos不生效问题修改
 ***********************************************************************/

#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_bridge/ebtables.h>
#include <linux/netfilter_bridge/ebt_vlan.h>

static int debug;
#define MODULE_VERS "0.6"

module_param(debug, int, 0);
MODULE_PARM_DESC(debug, "debug=1 is turn on debug messages");
MODULE_AUTHOR("Nick Fedchik <nick@fedchik.org.ua>");
MODULE_DESCRIPTION("Ebtables: 802.1Q VLAN tag match");
MODULE_LICENSE("GPL");


#define DEBUG_MSG(args...) if (debug) printk (KERN_DEBUG "ebt_vlan: " args)
#define GET_BITMASK(_BIT_MASK_) info->bitmask & _BIT_MASK_
#define EXIT_ON_MISMATCH(_MATCH_,_MASK_) {if (!((info->_MATCH_ == _MATCH_)^!!(info->invflags & _MASK_))) return false; }

/*<DTS2015020606189 tangjianxue 20150206 begin*/
#ifdef CONFIG_HSAN
/* 
  * 海思方案中，内核收到的数据包都已经剥离了VLAN，如果需要得到VLAN信息
  *     VLAN信息从hi_cb中获得
  * return value: 
  *  0 -- 参数vlanid为vlan值；
  *  1 -- 没有vlan  。
  */
/* hi_cb域结构定义，从上到下依次为外层vlan到内层vlan */
typedef struct 
{
    unsigned int        ui_chipinfo[4];
    void                    *pst_rxdev;
    
    unsigned short    us_svlan;
    unsigned short    us_cvlan;
    unsigned short    us_dvlan;
    unsigned short    us_evlan;
    unsigned char      uc_tagcnt;
    unsigned char      uc_resv[3];
}hi_eth_cb_s; 

int hisi_get_svlan(struct sk_buff *skb, unsigned short *vlanid)
{
    if (0 == ((hi_eth_cb_s*)(skb->hi_cb))->uc_tagcnt)
    {
        return 1;
    }    
    else
    {
        *vlanid = ((hi_eth_cb_s*)(skb->hi_cb))->us_svlan;
        return 0;
    }
}
#endif
/*DTS2015020606189 tangjianxue 20150206 end>*/

static bool
ebt_vlan_mt(const struct sk_buff *skb, const struct xt_match_param *par)
{
	const struct ebt_vlan_info *info = par->matchinfo;
	const struct vlan_hdr *fp;
	struct vlan_hdr _frame;

	unsigned short TCI;	/* Whole TCI, given from parsed frame */
	unsigned short id;	/* VLAN ID, given from frame TCI */
	unsigned char prio;	/* user_priority, given from frame TCI */
	/* VLAN encapsulated Type/Length field, given from orig frame */
/*<DTS2015020606189 tangjianxue 20150206 begin*/
#ifndef CONFIG_HSAN
	__be16 encap;
#endif

#ifdef CONFIG_HSAN
    if(hisi_get_svlan(skb,&TCI))
    {
        return false;
    }
#else
	fp = skb_header_pointer(skb, 0, sizeof(_frame), &_frame);
	if (fp == NULL)
		return false;
#endif

	/* Tag Control Information (TCI) consists of the following elements:
	 * - User_priority. The user_priority field is three bits in length,
	 * interpreted as a binary number.
	 * - Canonical Format Indicator (CFI). The Canonical Format Indicator
	 * (CFI) is a single bit flag value. Currently ignored.
	 * - VLAN Identifier (VID). The VID is encoded as
	 * an unsigned binary number. */
#ifndef CONFIG_HSAN
	TCI = ntohs(fp->h_vlan_TCI);
#endif
	id = TCI & VLAN_VID_MASK;
	prio = (TCI >> 13) & 0x7;
#ifndef CONFIG_HSAN
	encap = fp->h_vlan_encapsulated_proto;
#endif

	/* Checking VLAN Identifier (VID) */
	if (GET_BITMASK(EBT_VLAN_ID))
		EXIT_ON_MISMATCH(id, EBT_VLAN_ID);

	/* Checking user_priority */
	if (GET_BITMASK(EBT_VLAN_PRIO))
		EXIT_ON_MISMATCH(prio, EBT_VLAN_PRIO);

#ifndef CONFIG_HSAN
	/* Checking Encapsulated Proto (Length/Type) field */
	if (GET_BITMASK(EBT_VLAN_ENCAP))
		EXIT_ON_MISMATCH(encap, EBT_VLAN_ENCAP);
#endif
/*DTS2015020606189 tangjianxue 20150206 end>*/
	return true;
}

static bool ebt_vlan_mt_check(const struct xt_mtchk_param *par)
{
	struct ebt_vlan_info *info = par->matchinfo;
	const struct ebt_entry *e = par->entryinfo;

	/* Is it 802.1Q frame checked? */
/*<DTS2015020606189 tangjianxue 20150206 begin*/    
#ifndef CONFIG_HSAN
	if (e->ethproto != htons(ETH_P_8021Q)) {
		DEBUG_MSG
		    ("passed entry proto %2.4X is not 802.1Q (8100)\n",
		     (unsigned short) ntohs(e->ethproto));
		return false;
	}
#endif

	/* Check for bitmask range
	 * True if even one bit is out of mask */
	if (info->bitmask & ~EBT_VLAN_MASK) {
		DEBUG_MSG("bitmask %2X is out of mask (%2X)\n",
			  info->bitmask, EBT_VLAN_MASK);
		return false;
	}

	/* Check for inversion flags range */
	if (info->invflags & ~EBT_VLAN_MASK) {
		DEBUG_MSG("inversion flags %2X is out of mask (%2X)\n",
			  info->invflags, EBT_VLAN_MASK);
		return false;
	}

	/* Reserved VLAN ID (VID) values
	 * -----------------------------
	 * 0 - The null VLAN ID.
	 * 1 - The default Port VID (PVID)
	 * 0x0FFF - Reserved for implementation use.
	 * if_vlan.h: VLAN_GROUP_ARRAY_LEN 4096. */
	if (GET_BITMASK(EBT_VLAN_ID)) {
		if (!!info->id) { /* if id!=0 => check vid range */
			if (info->id > VLAN_GROUP_ARRAY_LEN) {
				DEBUG_MSG
				    ("id %d is out of range (1-4096)\n",
				     info->id);
				return false;
			}
			/* Note: This is valid VLAN-tagged frame point.
			 * Any value of user_priority are acceptable,
			 * but should be ignored according to 802.1Q Std.
			 * So we just drop the prio flag. */
			info->bitmask &= ~EBT_VLAN_PRIO;
		}
		/* Else, id=0 (null VLAN ID)  => user_priority range (any?) */
	}

	if (GET_BITMASK(EBT_VLAN_PRIO)) {
		if ((unsigned char) info->prio > 7) {
			DEBUG_MSG("prio %d is out of range (0-7)\n",
			     info->prio);
			return false;
		}
	}
	
#ifndef CONFIG_HSAN
	/* Check for encapsulated proto range - it is possible to be
	 * any value for u_short range.
	 * if_ether.h:  ETH_ZLEN        60   -  Min. octets in frame sans FCS */
	if (GET_BITMASK(EBT_VLAN_ENCAP)) {
		if ((unsigned short) ntohs(info->encap) < ETH_ZLEN) {
			DEBUG_MSG
			    ("encap frame length %d is less than minimal\n",
			     ntohs(info->encap));
			return false;
		}
	}
#endif
/*DTS2015020606189 tangjianxue 20150206 end>*/

	return true;
}

static struct xt_match ebt_vlan_mt_reg __read_mostly = {
	.name		= "vlan",
	.revision	= 0,
	.family		= NFPROTO_BRIDGE,
	.match		= ebt_vlan_mt,
	.checkentry	= ebt_vlan_mt_check,
	.matchsize	= XT_ALIGN(sizeof(struct ebt_vlan_info)),
	.me		= THIS_MODULE,
};

static int __init ebt_vlan_init(void)
{
	DEBUG_MSG("ebtables 802.1Q extension module v"
		  MODULE_VERS "\n");
	DEBUG_MSG("module debug=%d\n", !!debug);
	return xt_register_match(&ebt_vlan_mt_reg);
}

static void __exit ebt_vlan_fini(void)
{
	xt_unregister_match(&ebt_vlan_mt_reg);
}

module_init(ebt_vlan_init);
module_exit(ebt_vlan_fini);
