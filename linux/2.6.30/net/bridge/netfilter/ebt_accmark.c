/*
 *  ebt_accmark
 *
 *	Authors:
 *	    z00190439 
 *
 *  May, 2014
 *
 */

/* The mark target can be used in any chain,
 * I believe adding a mangle table just for marking is total overkill.
 * Marking a frame doesn't really change anything in the frame anyway.
 */

#include <linux/module.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_bridge/ebtables.h>
#include <linux/netfilter_bridge/ebt_accmark_t.h>

static unsigned int
ebt_accmark_tg(struct sk_buff *skb, const struct xt_target_param *par)
{
	const struct ebt_accmark_t_info *info = par->targinfo;
	int action = info->target & -16;
   
	if (action == MARK_SET_VALUE)
        {
              skb->accelmark = info->mark;
        }
	else if (action == MARK_OR_VALUE)
		skb->accelmark |= info->mark;
	else if (action == MARK_AND_VALUE)
		skb->accelmark &= info->mark;
	else if (action == MARK_XOR_VALUE)
		skb->accelmark ^= info->mark;
	return info->target | ~EBT_VERDICT_BITS;
}

static bool ebt_accmark_tg_check(const struct xt_tgchk_param *par)
{
	const struct ebt_accmark_t_info *info = par->targinfo;
	int tmp;

	tmp = info->target | ~EBT_VERDICT_BITS;
	if (BASE_CHAIN && tmp == EBT_RETURN)
		return false;
	if (tmp < -NUM_STANDARD_TARGETS || tmp >= 0)
		return false;
	tmp = info->target & ~EBT_VERDICT_BITS;
	if (tmp != MARK_SET_VALUE && tmp != MARK_OR_VALUE &&
	    tmp != MARK_AND_VALUE && tmp != MARK_XOR_VALUE 
        )
		return false;
	return true;
}

static struct xt_target ebt_accmark_tg_reg __read_mostly = {
	.name		= "accmark",
	.revision	= 0,
	.family		= NFPROTO_BRIDGE,
	.target		= ebt_accmark_tg,
	.checkentry	= ebt_accmark_tg_check,
	.targetsize	= XT_ALIGN(sizeof(struct ebt_accmark_t_info)),
	.me		= THIS_MODULE,
};

static int __init ebt_accmark_init(void)
{
	return xt_register_target(&ebt_accmark_tg_reg);
}

static void __exit ebt_accmark_fini(void)
{
	xt_unregister_target(&ebt_accmark_tg_reg);
}

module_init(ebt_accmark_init);
module_exit(ebt_accmark_fini);
MODULE_DESCRIPTION("Ebtables: Packet mark modification");
MODULE_LICENSE("GPL");
