/* -*- linux-c -*-
 * sysctl_net_core.c: sysctl interface to net core subsystem.
 *
 * Begun April 1, 1996, Mike Shaver.
 * Added /proc/sys/net/core directory entry (empty =) ). [MS]
 */

#include <linux/mm.h>
#include <linux/sysctl.h>
#include <linux/module.h>
#include <linux/socket.h>
#include <linux/netdevice.h>
#include <linux/init.h>
#include <net/ip.h>
#include <net/sock.h>

extern int net_msg_dns;
extern char acdefaultdnsdomain[512];
#ifdef CONFIG_ATP_SKB_LIMIT
extern atomic_t skb_buff_alloc_num;
extern volatile unsigned long skb_max_alloc_num;
#endif
/*start add by z00228368 for DTS2013101800046 630 ttnet add grefon fun @20131017 */
/*从隧道走时，ssid2和ssid3配置的vlan*/
#ifdef CONFIG_ATP_GRE_FON
int g_ssid2_vid = 0;
int g_ssid3_vid = 0;
#endif
/*end add by z00228368 for DTS2013101800046 630 ttnet add grefon fun @20131017 */
static struct ctl_table net_core_table[] = {
#ifdef CONFIG_NET
	{
		.ctl_name	= NET_CORE_WMEM_MAX,
		.procname	= "wmem_max",
		.data		= &sysctl_wmem_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_RMEM_MAX,
		.procname	= "rmem_max",
		.data		= &sysctl_rmem_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_WMEM_DEFAULT,
		.procname	= "wmem_default",
		.data		= &sysctl_wmem_default,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_RMEM_DEFAULT,
		.procname	= "rmem_default",
		.data		= &sysctl_rmem_default,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_DEV_WEIGHT,
		.procname	= "dev_weight",
		.data		= &weight_p,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_MAX_BACKLOG,
		.procname	= "netdev_max_backlog",
		.data		= &netdev_max_backlog,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_MSG_COST,
		.procname	= "message_cost",
		.data		= &net_ratelimit_state.interval,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_jiffies,
		.strategy	= sysctl_jiffies,
	},
	{
		.ctl_name	= NET_CORE_MSG_BURST,
		.procname	= "message_burst",
		.data		= &net_ratelimit_state.burst,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec,
	},
	{
		.ctl_name	= NET_CORE_OPTMEM_MAX,
		.procname	= "optmem_max",
		.data		= &sysctl_optmem_max,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
#ifdef CONFIG_VLAN0
	{
		.ctl_name	= NET_CORE_VLAN0,
		.procname	= "vlan0_untag",
		.data		= &vlan0_untag,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
#endif
#endif /* CONFIG_NET */
	{
		.ctl_name	= NET_CORE_BUDGET,
		.procname	= "netdev_budget",
		.data		= &netdev_budget,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_WARNINGS,
		.procname	= "warnings",
		.data		= &net_msg_warn,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
    {
		.ctl_name	= NET_CORE_WLAN_DNS,
		.procname	= "wlanctl_dnsstart",
		.data		= &net_msg_dns,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
    },
    /*add by h00190880 */
    {
		.ctl_name	= NET_CORE_DEFAULTDOMAIN,
		.procname	= "defaultdomain",
		.data		= acdefaultdnsdomain,
		.maxlen		= 512,
		.mode		= 0644,
		.proc_handler	= proc_dostring
    },
#ifdef CONFIG_ATP_HYBRID_GREACCEL
	{
	    .ctl_name	= NET_CORE_HISI_SW_FLG,
		.procname	= "hisi_sw_accel_flag",
		.data		= &hisi_sw_accel_flag,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
#endif
#ifdef CONFIG_ATP_SKB_LIMIT
	{
	    .ctl_name	= NET_CORE_SKB_MAX_NUM,
		.procname	= "skb_max_alloc_num",
		.data		= &skb_max_alloc_num,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
	{
	    .ctl_name	= NET_CORE_SKB_NUM,
		.procname	= "skb_alloc_num",
		.data		= &skb_buff_alloc_num,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
#endif
    /*start add by z00228368 for DTS2013101800046 630 ttnet add grefon fun @20131017 */
#ifdef CONFIG_ATP_GRE_FON
	{
		.ctl_name	= NET_CORE_GRE_FON_SSID2_VID,
		.procname	= "ssid2_vlan_id",
		.data		= &g_ssid2_vid,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
	{
		.ctl_name	= NET_CORE_GRE_FON_SSID3_VID,
		.procname	= "ssid3_vlan_id",
		.data		= &g_ssid3_vid,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec
	},
#endif
    /*end add by z00228368 for DTS2013101800046 630 ttnet add grefon fun @20131017 */
	{ .ctl_name = 0 }
};
/*start add by z00228368 for DTS2013101800046 630 ttnet add grefon fun @20131017 */
#ifdef CONFIG_ATP_GRE_FON
EXPORT_SYMBOL(g_ssid2_vid);
EXPORT_SYMBOL(g_ssid3_vid);
#endif
/*end add by z00228368 for DTS2013101800046 630 ttnet add grefon fun @20131017 */

static struct ctl_table netns_core_table[] = {
	{
		.ctl_name	= NET_CORE_SOMAXCONN,
		.procname	= "somaxconn",
		.data		= &init_net.core.sysctl_somaxconn,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec
	},
	{ .ctl_name = 0 }
};

__net_initdata struct ctl_path net_core_path[] = {
	{ .procname = "net", .ctl_name = CTL_NET, },
	{ .procname = "core", .ctl_name = NET_CORE, },
	{ },
};

static __net_init int sysctl_core_net_init(struct net *net)
{
	struct ctl_table *tbl;

	net->core.sysctl_somaxconn = SOMAXCONN;

	tbl = netns_core_table;
	if (net != &init_net) {
		tbl = kmemdup(tbl, sizeof(netns_core_table), GFP_KERNEL);
		if (tbl == NULL)
			goto err_dup;

		tbl[0].data = &net->core.sysctl_somaxconn;
	}

	net->core.sysctl_hdr = register_net_sysctl_table(net,
			net_core_path, tbl);
	if (net->core.sysctl_hdr == NULL)
		goto err_reg;

	return 0;

err_reg:
	if (tbl != netns_core_table)
		kfree(tbl);
err_dup:
	return -ENOMEM;
}

static __net_exit void sysctl_core_net_exit(struct net *net)
{
	struct ctl_table *tbl;

	tbl = net->core.sysctl_hdr->ctl_table_arg;
	unregister_net_sysctl_table(net->core.sysctl_hdr);
	BUG_ON(tbl == netns_core_table);
	kfree(tbl);
}

static __net_initdata struct pernet_operations sysctl_core_ops = {
	.init = sysctl_core_net_init,
	.exit = sysctl_core_net_exit,
};

static __init int sysctl_core_init(void)
{
	static struct ctl_table empty[1];

	register_sysctl_paths(net_core_path, empty);
	register_net_sysctl_rotable(net_core_path, net_core_table);
	return register_pernet_subsys(&sysctl_core_ops);
}

fs_initcall(sysctl_core_init);
