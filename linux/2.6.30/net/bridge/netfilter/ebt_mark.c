/*
 *  ebt_mark
 *
 *	Authors:
 *	Bart De Schuymer <bdschuym@pandora.be>
 *
 *  July, 2002
 *
 */

/* The mark target can be used in any chain,
 * I believe adding a mangle table just for marking is total overkill.
 * Marking a frame doesn't really change anything in the frame anyway.
 */

#include <linux/module.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_bridge/ebtables.h>
#include <linux/netfilter_bridge/ebt_mark_t.h>
#ifdef CONFIG_NF_CONNTRACK_PRI
#include <linux/netfilter/nf_conntrack_pri.h>
#endif
#if defined(CONFIG_ARCH_SD56XX) && defined(CONFIG_DOWN_RATE_CONTROL)
#include <net/netfilter/nf_conntrack.h>
#endif


#ifdef CONFIG_NF_CONNTRACK_PRI
static int ebt_action_modify_mark(int action)
{
	if (MARK_SET_VALUE == action)
	{
		return 1;
	}

	if (MARK_OR_VALUE == action)
	{
		return 1;
	}

	if (MARK_AND_VALUE == action)
	{
		return 1;
	}

	if (MARK_XOR_VALUE == action)
	{
		return 1;
	}

	return 0;
}
#endif

static unsigned int
ebt_mark_tg(struct sk_buff *skb, const struct xt_target_param *par)
{
	const struct ebt_mark_t_info *info = par->targinfo;
	int action = info->target & -16;
    
#if defined(CONFIG_ARCH_SD56XX) && defined(CONFIG_DOWN_RATE_CONTROL)
       struct nf_conn *ct=NULL;
       unsigned int	mark=0;
       unsigned int ulwmmQueue = 0;
#endif

#ifdef CONFIG_NF_CONNTRACK_PRI
	if (ebt_action_modify_mark(action) && nf_pri_is_alg_skb(skb))
	{
		nf_pri_set_alg_mark(skb);
		return info->target | ~EBT_VERDICT_BITS;
	}
#endif

	if (action == MARK_SET_VALUE)
		//skb->mark = info->mark;
    {
        if (info->mark & 0x000ff000)
        {
    		skb->mark = (skb->mark & (~0x000ff000)) | info->mark;
               
#if defined(CONFIG_ARCH_SD56XX) && defined(CONFIG_DOWN_RATE_CONTROL)
            if(downqos_enable)
            {
               ct=skb->nfct;
               if(ct)
               {
                   //如果ct的mark没有打上上行QOS的MARK,则需要将info->mark中的下行QOS部分赋值到ct- >mark的上行qos部分
                   //这是为了保证ct->mark相关位保存的永远是上行qos的mark
                   if(!(ct->mark&QOS_UPLINK_CONTROL_MASK))
                   {
                        ulwmmQueue = QOS_WMM_QUEUE_NUM -((info->mark&QOS_WMM_MASK)>>16);

                        if(QOS_WMM_QUEUE_NUM == ulwmmQueue)
                        {   
                            //没有匹配上任何规则时必须为0
                            ulwmmQueue = 0;
                        }
                        
                        mark=(info->mark&QOS_POLICER_MASK)|(ulwmmQueue>>16);
                        ct->mark|=mark;
                        
                        //printk("the mark is %0x.\n",ct->mark);
                   }
               }
            }
#endif
        }
        else
        {
    		skb->mark = info->mark;
        }
    }
	else if (action == MARK_OR_VALUE)
		skb->mark |= info->mark;
	else if (action == MARK_AND_VALUE)
		skb->mark &= info->mark;
	else if (action == MARK_XOR_VALUE)
		skb->mark ^= info->mark;
#ifdef CONFIG_MIPS_BRCM    
	else  //brcm -- begin
   {
		skb->vtag = (unsigned short)(info->mark);

      /* if this is an 8021Q frame and skb->vtag is not zero, we need to do 8021p priority
       * or vlan id marking.
       */
      if (skb->vtag & 0xffff)
      {
         unsigned short TCI = 0;

	      if ((skb->protocol == __constant_htons(ETH_P_8021Q)))
	      {
   	      struct vlan_hdr *frame = (struct vlan_hdr *)(skb->network_header);

		      TCI = ntohs(frame->h_vlan_TCI);

            /* if the 8021p priority field (bits 0-3) of skb->vtag is not zero, remark
             * 8021p priority of the frame.  Since the 8021p priority value in vtag had
             * been incremented by 1, we need to minus 1 from it to get the exact value.
             */
            if (skb->vtag & 0xf)
               TCI = (TCI & 0x1fff) | (((skb->vtag & 0xf) - 1) << 13);

            /* if the vlan id field (bits 4-15) of skb->vtag is not zero, remark vlan id of
             * the frame.  Since the vlan id value in vtag had been incremented by 1, we need
             * to minus 1 from it to get the exact value.
             */
            if (skb->vtag & 0xfff0)
               TCI = (TCI & 0xf000) | (((skb->vtag >> 4) & 0xfff) - 1);

		      frame->h_vlan_TCI = htons(TCI);
   	   }
         else
         {
            if ((skb->mac_header - skb->head) < VLAN_HLEN)
            {
               printk("ebt_mark_tg: No headroom for VLAN tag. Marking is not done.\n");
            }
            else
            {
   	         struct vlan_ethhdr *ethHeader;

               skb->protocol = __constant_htons(ETH_P_8021Q);
               skb->mac_header -= VLAN_HLEN;
               skb->network_header -= VLAN_HLEN;
               skb->data -= VLAN_HLEN;
	            skb->len  += VLAN_HLEN;

               /* Move the mac addresses to the beginning of the new header. */
               memmove(skb->mac_header, skb->mac_header + VLAN_HLEN, 2 * ETH_ALEN);

               ethHeader = (struct vlan_ethhdr *)(skb->mac_header);

               ethHeader->h_vlan_proto = __constant_htons(ETH_P_8021Q);

               /* if the 8021p priority field (bits 0-3) of skb->vtag is not zero, remark
                * 8021p priority of the frame.  Since the 8021p priority value in vtag had
                * been incremented by 1, we need to minus 1 from it to get the exact value.
                */
               if (skb->vtag & 0xf)
                  TCI = (TCI & 0x1fff) | (((skb->vtag & 0xf) - 1) << 13);

               /* if the vlan id field (bits 4-15) of skb->vtag is not zero, remark vlan id of
                * the frame.  Since the vlan id value in vtag had been incremented by 1, we need
                * to minus 1 from it to get the exact value.
                */
               if (skb->vtag & 0xfff0)
                  TCI = (TCI & 0xf000) | (((skb->vtag >> 4) & 0xfff) - 1);

               ethHeader->h_vlan_TCI = htons(TCI);
            }
         }
         skb->vtag = 0;
      }
   }  // brcm -- end
#endif
	return info->target | ~EBT_VERDICT_BITS;
}

static bool ebt_mark_tg_check(const struct xt_tgchk_param *par)
{
	const struct ebt_mark_t_info *info = par->targinfo;
	int tmp;

	tmp = info->target | ~EBT_VERDICT_BITS;
	if (BASE_CHAIN && tmp == EBT_RETURN)
		return false;
	if (tmp < -NUM_STANDARD_TARGETS || tmp >= 0)
		return false;
	tmp = info->target & ~EBT_VERDICT_BITS;
	if (tmp != MARK_SET_VALUE && tmp != MARK_OR_VALUE &&
	    tmp != MARK_AND_VALUE && tmp != MARK_XOR_VALUE 
#ifdef CONFIG_MIPS_BRCM 
        && tmp != VTAG_SET_VALUE
#endif
        )    /* brcm */
		return false;
	return true;
}

static struct xt_target ebt_mark_tg_reg __read_mostly = {
	.name		= "mark",
	.revision	= 0,
	.family		= NFPROTO_BRIDGE,
	.target		= ebt_mark_tg,
	.checkentry	= ebt_mark_tg_check,
	.targetsize	= XT_ALIGN(sizeof(struct ebt_mark_t_info)),
	.me		= THIS_MODULE,
};

static int __init ebt_mark_init(void)
{
	return xt_register_target(&ebt_mark_tg_reg);
}

static void __exit ebt_mark_fini(void)
{
	xt_unregister_target(&ebt_mark_tg_reg);
}

module_init(ebt_mark_init);
module_exit(ebt_mark_fini);
MODULE_DESCRIPTION("Ebtables: Packet mark modification");
MODULE_LICENSE("GPL");
