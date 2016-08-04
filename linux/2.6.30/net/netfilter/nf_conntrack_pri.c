/******************************************************************************
  ��Ȩ����  : 2007-2020����Ϊ�������޹�˾
  �� �� ��  : nf_conntrack_pri.c
  ��    ��  : l00180987
  ��    ��  : v1.0
  ��������  : 2013-7-19
  ��    ��  : ���Ӹ������ȼ�����ģ��
  �����б�  :
                find_lowest_ct
                nf_conntrack_del_used_by_pri
                nf_conntrack_pri_ct_need_alloc
                nf_conntrack_pri_ct_reserved
                nf_conntrack_pri_fini
                nf_conntrack_pri_init
                nf_conntrack_pri_register
                nf_conntrack_pri_unregister
                nf_pri_is_alg_skb
                nf_pri_set_alg_mark

  ��ʷ��¼      :
   1.��    ��   : 2013-7-19
     ��    ��   : l00180987
     �޸�����   : ��ɳ���

*********************************************************************************/


#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/ctype.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <linux/netfilter/nf_conntrack_pri.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Li Jian");
MODULE_DESCRIPTION("Conntrack priority");

extern int (*nf_ct_reserved_hook)(struct nf_conn *ct);
extern int (*nf_ct_high_pri_hook)(struct net *net, const struct nf_conntrack_tuple *orig);
extern int (*nf_ct_del_used_by_pri_hook)(struct net *net);
int nf_conntrack_pri_ct_reserved(struct nf_conn *ct);

static DEFINE_MUTEX(nf_ct_pri_mutex);
HLIST_HEAD(nf_ct_pri_list);


/*���ȼ��ӵ͵���Ϊ:0x000,0x900,0x800,0x700,0x600,0x500,0x400,0x300,0x200,0x100����QOS����*/
static struct nf_conn * find_lowest_ct(struct net *net)
{
	unsigned int hash = 0;
	struct nf_conntrack_tuple_hash *h = NULL;
	struct hlist_nulls_node *n = NULL;
	struct nf_conn *ct = NULL;
	struct nf_conn *find_ct = NULL;
	u_int32_t ct_qos_mark = 0;

	spin_lock_bh(&nf_conntrack_lock);
	for (hash = 0; hash < nf_conntrack_htable_size; hash++)
	{
		hlist_nulls_for_each_entry_rcu(h, n, &net->ct.hash[hash], hnnode)
		{
			ct = nf_ct_tuplehash_to_ctrack(h);
			if (NULL == ct)
			{
				continue;
			}

			/*��ע����Ҫ������ct��ɾ��*/
			if (nf_conntrack_pri_ct_reserved(ct))
			{
				continue;
			}

			/*��ȷ�ϵ�ct�����ܱ�֤ct mark��ȷ�����ֻɾASSURED CT*/
			if (!test_bit(IPS_ASSURED_BIT, &ct->status))
			{
				continue;
			}

			ct_qos_mark = ct->mark & QOS_PRI_MASK;

			/*Ĭ�����ȼ�ƥ��ɹ��ʺܸߣ����жϴ���*/
			if (QOS_DEFAULT_PRI == ct_qos_mark)
			{
				find_ct = ct;
				break;
			}

			/*�����ȼ����ϼ���ɾ��*/
			if (QOS_HIGH_PRI >= ct_qos_mark)
			{
				continue;
			}

			if (NULL == find_ct)
			{
				find_ct = ct;
				continue;
			}

			/*��ǰct���ȼ�����*/
			if (ct_qos_mark > (find_ct->mark & QOS_PRI_MASK))
			{
				find_ct = ct;
			}

		}
	}

	spin_unlock_bh(&nf_conntrack_lock);

	return find_ct;
}


/*RTSP�½����ϣ�����1��ʾɾ������ct�ɹ��������½�*/
int nf_conntrack_del_used_by_pri(struct net *net)
{
	struct nf_conn *ct = NULL;

	/*ɾctʱ����������ȼ���ʼɾ������ߴθ�(markΪ0x100/0x200) ������ɾ��*/
	ct = find_lowest_ct(net);
	if (NULL == ct)
	{
		return 0;
	}

	/*ct�ܿ�������ʹ�ã�ʹ���ӳ�ɾ���ķ�ʽ����ɾ��*/
	set_bit(IPS_DELETE_BIT, &ct->status);
	nf_conntrack_clean = IPS_DELETE;

	return 1;
}


int nf_pri_is_alg_skb(struct sk_buff *skb)
{
	struct nf_conn *ct = NULL;

	if (NULL == skb)
	{
		return 0;
	}

	ct = (struct nf_conn *)skb->nfct;
	if (NULL == ct)
	{
		return 0;
	}

	if (ct->master)
	{
		return 1;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(nf_pri_is_alg_skb);


void nf_pri_set_alg_mark(struct sk_buff *skb)
{
	struct nf_conn *ct = NULL;

	if (NULL == skb)
	{
		return;
	}

	ct = (struct nf_conn *)skb->nfct;
	if (NULL == ct)
	{
		return;
	}

	/*��ct��ȡmark*/
	skb->mark = ct->mark;

	return;
}
EXPORT_SYMBOL_GPL(nf_pri_set_alg_mark);



/*���ct�Ƿ�ע��Ϊ���ȱ���ct*/
int nf_conntrack_pri_ct_reserved(struct nf_conn *ct)
{
	struct nf_conntrack_proto_pri *proto_pri = NULL;
	struct hlist_node *n = NULL;
	int ct_reserved = 0;

	if (NULL == ct) {
		return ct_reserved;
	}

	rcu_read_lock();

	hlist_for_each_entry_rcu(proto_pri, n, &nf_ct_pri_list, hnode) {
		if (proto_pri->check_by_ct(ct)) {
			ct_reserved = 1;
			break;
		}
	}

	rcu_read_unlock();

	return ct_reserved;
}


/*���tuple�Ƿ�������ȷ�������ɹ��ĸ����ȼ�����*/
int nf_conntrack_pri_ct_need_alloc(struct net *net, const struct nf_conntrack_tuple *orig)
{
	struct nf_conntrack_proto_pri *proto_pri = NULL;
	struct hlist_node *n = NULL;
	int need_alloc = 0;

	if (NULL == orig) {
		return need_alloc;
	}

	rcu_read_lock();

	hlist_for_each_entry_rcu(proto_pri, n, &nf_ct_pri_list, hnode) {
		if (proto_pri->check_by_tuple(net, orig)) {
			need_alloc = 1;
			break;
		}
	}

	rcu_read_unlock();

	return need_alloc;
}


int nf_conntrack_pri_register(struct nf_conntrack_proto_pri *me)
{
	mutex_lock(&nf_ct_pri_mutex);
	hlist_add_head_rcu(&me->hnode, &nf_ct_pri_list);
	mutex_unlock(&nf_ct_pri_mutex);

	return 0;
}
EXPORT_SYMBOL_GPL(nf_conntrack_pri_register);


int nf_conntrack_pri_unregister(struct nf_conntrack_proto_pri *me)
{
	mutex_lock(&nf_ct_pri_mutex);
	hlist_del_rcu(&me->hnode);
	mutex_unlock(&nf_ct_pri_mutex);

	return 0;
}
EXPORT_SYMBOL_GPL(nf_conntrack_pri_unregister);


static int __init nf_conntrack_pri_init(void)
{
	rcu_assign_pointer(nf_ct_reserved_hook, nf_conntrack_pri_ct_reserved);
	rcu_assign_pointer(nf_ct_high_pri_hook, nf_conntrack_pri_ct_need_alloc);
	rcu_assign_pointer(nf_ct_del_used_by_pri_hook, nf_conntrack_del_used_by_pri);
	return 0;
}

static void nf_conntrack_pri_fini(void)
{
	rcu_assign_pointer(nf_ct_reserved_hook, NULL);
	rcu_assign_pointer(nf_ct_high_pri_hook, NULL);
	rcu_assign_pointer(nf_ct_del_used_by_pri_hook, NULL);
	return;
}

module_init(nf_conntrack_pri_init);
module_exit(nf_conntrack_pri_fini);


