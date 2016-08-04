/******************************************************************************

                  ��Ȩ���� (C), 2009-2019, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : hi_kernel_eth0_qos.h
  �� �� ��   : ����
  ��    ��   : luocheng 00183967
  ��������   : D2012_02_27

******************************************************************************/
#ifndef __HI_KERNEL_SOFT_QOS_H__
#define __HI_KERNEL_SOFT_QOS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


/***************************************************************************/
#ifndef CONFIG_ARCH_SD56XX
extern void hi_kernel_vif_qoshook_reg( void *pv_qos_callback ); 
extern void hi_kernel_vif_vlan_rmk_hook_reg(void* pv_hook);
#endif
/***************************************************************************/

/* �Ƿ���û�����ݵ�ACK���� */
static int is_ack(struct sk_buff* skb);

static void set_highest_priority( struct sk_buff *skb);

/***************************************************************************/

/* skb->mark */
/*----------|----------|--------------|----|-----|-----|------|-----------*/
/* policy route|policy  route|default qos mark|wmm|policer|queue|802.1p|downlinkQos */

#define SKB_MARK_8021P_REMARK_ENABLE    0x00000010          //�Ƿ���Ҫ����802.1p Remark
#define SKB_MARK_8021P_REMARK_MASK      0x000000E0          //802.1p Remarkֵ
#define SKB_MARK_8021P_REMARK_OFFSET    5                   //802.1p Remarkֵ��offset

#define SKB_MARK_QUEUE_MASK             0x00000F00          //queue id MASK
#define SKB_MARK_QUEUE_OFFSET           8                   //queue idֵ��offset

#define SKB_MARK_POLICER_MASK           0x0000F000          //policer id MASK
#define SKB_MARK_POLICER_OFFSET         12                  //policer idֵ��offset


/* skb->accelmark */
/*----------|----------|--------------|----|-----|-----|------|--------------*/
/*                                          reserved                                                 |acceleration tyep */

#define SKB_ACCEL_MARK_ENABLE           0x00000001          //�Ƿ���Ҫ��ע�ñ�־
#define SKB_ACCEL_MARK_MASK             0x00000006          //���ٱ�־��MASK    
#define SKB_ACCEL_MARK_OFFSET           1                   //���ٱ�־��OFFSET
#define SKB_ACCEL_MARK_NONE             0x00000000          //����Ҫ����
#define SKB_ACCEL_MARK_SW               0x00000002          //�����
#define SKB_ACCEL_MARK_HW               0x00000004          //Ӳ����
#define SKB_ACCEL_MARK_CFI_ENABLE       0x00000008          //�Ƿ�Ϊ���Ʊ���

#define SKB_ACCEL_MARK_CFI_BIT          0x1000              //CFI BIT
#define SKB_ACCEL_MARK_PRIORITY_MASK    0xE000              //TCI��PRIORITY��MASK
#define SKB_ACCEL_MARK_PRIORITY_OFFSET  13                  //TCI��PRIORITY��ƫ��




/***************************************************************************/

#define SKB_UPLINK_BASE_VLAN               4000                  //������������car id��ӵĻ�vlan
#define SKB_DOWNLINK_BASE_VLAN         4017                  //������������car id��ӵĻ�vlan

/* ע�� skb->accelmark ��0-2λ��ʶ����״̬�� ��3λ��ʶ�Ƿ���dscp mark */
/* skb->accelmark */
/* reserved | dscp mark(1 bit) |acceleration type(3 bit) */
#define SKB_MARK_DSCP_REMARK_ENABLE    0x8          //�Ƿ���Ҫ����dscp Remark

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HI_KERNEL_SOFT_QOS_H__ */
