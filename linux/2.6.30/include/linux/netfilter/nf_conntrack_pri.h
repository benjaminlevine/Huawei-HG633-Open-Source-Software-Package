/******************************************************************************
  ��Ȩ����  : 2007-2020����Ϊ�������޹�˾
  �� �� ��  : nf_conntrack_pri.h
  ��    ��  : l00180987
  ��    ��  : v1.0
  ��������  : 2013-7-19
  ��    ��  : ���Ӹ������ȼ�ģ������ͷ�ļ�
  �����б�  :

  ��ʷ��¼      :
   1.��    ��   : 2013-7-19
     ��    ��   : l00180987
     �޸�����   : ��ɳ���

*********************************************************************************/


#ifndef _NF_CONNTRACK_PRI_H
#define _NF_CONNTRACK_PRI_H

struct nf_conntrack_proto_pri
{
	struct hlist_node hnode;	/* Internal use. */

	char *name;

	/* ����tuple �ж�Э������*/
	int (*check_by_tuple)(struct net *net, const struct nf_conntrack_tuple *orig);

	/* ����ct �ж�Э������*/
	int (*check_by_ct)(struct nf_conn *ct);
};

extern int nf_conntrack_pri_register(struct nf_conntrack_proto_pri *me);

extern int nf_conntrack_pri_unregister(struct nf_conntrack_proto_pri *me);

extern int nf_pri_is_alg_skb(struct sk_buff *skb);

extern void nf_pri_set_alg_mark(struct sk_buff *skb);

#endif /*_NF_CONNTRACK_PRI_H*/


