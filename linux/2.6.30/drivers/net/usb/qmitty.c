/***********************************************************************
  ��Ȩ��Ϣ: ��Ȩ����(C) 1988-2008, ��Ϊ�������޹�˾.
  �ļ���: QMITTY.c
  ����: houxiaohe 00118013
  �汾: 001
  ��������: 2008-5-21
  �������: 2008-5-25
  ��������: QMITTYģ����ʵ�ָ�ͨRm Net����Ҫ��ɲ��֣�ģ���ڳɹ����ز�����
            ���Ժ�ע����һ���豸�ļ���㡣�û�����ͨ�����豸�ļ������
            ���QMI��Ϣ�Ķ�д����ɺ�����ģ���QMI��Ϣ������ͬʱ��Ŀǰ��ʵ
            ����У�QMITTYģ��ΪUSBNETģ�鹹������ͺͽ���QOS��Ϣ�ĵ��ýӿ�
            ��QMITTYģ���USBNETģ�������ɶ������ݰ����͵�QOS���ء�Ϊ����
            Ч������Դ��ģ���ڲ����������һ����Ϣ��������QMITTYģ���ڽ���
            �����б���Ҫ��ȡ1��WDS Client ID��1��QOS Client ID,WDS Client ID
            ������WDS�û�����ʹ��,QOS Client ID������USBNETʹ�á�

  ��Ҫ�����б�: trigger_tty, exit_tty, create_qmi_interface_process,
                compose_internal_request, send_msg_to_user_space,
                process_inbound_ctl_msg, interrupt_timer_proc,
                handle_reponse_available, get_node, insert_node_to_list,
                remove_list_node, return_node_to_free, qmi_tty_open,
                qmi_tty_close, qmi_tty_write,

                write_control_qmi_msg, write_qmi_completion, read_control_qmi_msg,
                read_qmi_completion, handler_resolve_available_msg,

  �޸���ʷ��¼�б�:
    <��  ��>    <�޸�ʱ��>  <�汾>  <�޸�����>
    houxiaohe    2008-06-28  001    ��QMITTYģ�������write_control_qmi_msg,
                        read_control_qmi_msg,read_interrupt_qmi_msg,
                        �Լ���3�������Ļص�������ʵ�֡�ͬʱ�޸�����Ӧ��
                        QMI_NODE�ṹ��������urb���͵�ָ��ͼ�ʱ����
    houxiaohe	2008-06-29   001    ��QMITTYģ�������tasklet����������ѯ�ж϶˵��
                        �����Ļص������У�������ִ�ж�ȡQMI��Ϣ����������
    houxiaohe	2008-07-02   001    �޸�QMI״̬����ʼ�������У�Ҫ�����շ������QMI��Ϣ
                        ΪGet_QMI_Version,Get_WDS_Client_ID,Set_QoS_DataFormat,
                        Get_QoS_Client_ID,ͬʱ�޸��յ�QMI��Ϣ�����Ժ�Ĵ���
                        ���̡�QMITTYģ��ֻ��CTL���ͷ����QMI��Ϣ�������д���,
                        WDS��ص���Ϣ�������û��ռ���д���QoSҵ����ص���Ϣ
                        ����usbnet���д���
    houxiaohe	2008-07-02   001    ɾ����ԭ������е�reset_tty��������Ӧ����Ϣװ�䲿��
    houxiaohe	2008-07-02   001    ������ҵ�urb����������ʱ����2���Ƶ�ʼ�����ύ��urb
                        �Ƿ�û����Ӧ�����û����Ӧ�ľʹӵײ����
    houxiaohe   2008-07-18  B000    ����ԭ�����ж���������н��� tasklet_schule������
                                   ���ƶ˵��QMI��Ϣ��д������ɵ���ķ��������������޸�
                                   read_control_qmi_msg,write_control_qmi_msg��ʵ�֣��Լ�
                                   ������̵�ʵ�֣�ͬʱ������SET_QMI_INSTANCE_ID�Ĵ���ɾ
                                   ����ԭ�����ӵĽ������urb�Ķ�ʱ��������������ҵĴ���
    houxiaohe   2008-07-23  B000   �޸��ж϶�ʱ�����д�ӡ��ϢΪ10�Σ��޸��ж϶�ʱ����������ӡ��ϢΪ10��
    houxiaohe   2008-07-23  B000   �����µı�־����ʶUSB�����Ƿ��������޸�����USB��д��������Ҫ�ж�
                                   �����Ƿ���ڣ�������������ӾͲ��ܷ����ײ��ύ��ͬʱ������
                                   �Ͽ���ģ�鱻�˳�ʱҪ���ж�����Ҳ����������
    houxiaohe   2008-07-25  B000   �޸Ķ��� qmi_tty_write�ж����û������·���PDU��д�������µ�ʵ����
                                   �����û������·���PDUֻװ��QMIͷ����з��ͣ����ٶ�PDU�����ݽ��и�д
    houxiaohe   2008-07-25  B000   �޸Ķ��� qmi_tty_ioctl�ж���WDS�û����̲�ѯSTART_NETWORK_INTERFACE_REQҵ��ID
                                   ��ѯ��֧�֣��µ�ʵ����ģ���ڲ�ά����TransactionId����unsigned char��
                                   �û���������ά��SDU�е�Transaction ID
    mayang      2010-10-16  B101   ��ֲ��realtekƽ̨��ƷB683, ����ʹ��MDM8200A,��֧��QoS
  ��ע:
 ************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/usb.h>
#include <linux/smp_lock.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>
#include "qmitty.h"
#include "qmihdr.h"

#include <linux/netdevice.h>
#include <linux/mii.h>
#include <linux/usb/usbnet.h>

#ifdef  _lint
#undef  copy_from_user
#define copy_from_user strncpy
#endif

#define _DEBUG_
//���Կ���
#ifdef _DEBUG_
#define NDIS_DEBUG_MSG printk
#else
#define NDIS_DEBUG_MSG
#endif



/********************* �ⲿ���� - ������usbnetģ���� *********************/

/* usbnet��probe������̽�鵽��RmNetָ����ж϶˵�Ķ˵�������*/
static struct usb_device * rmnet = NULL;
static struct usb_endpoint_descriptor * ep_int_rmnet = NULL;

//balong LTE���ݿ�����by w00190448@20130509 start
static struct hw_cdc_net	 *hwcdc_net;
//balong LTE���ݿ�����by w00190448@20130509 end
/* <DTS2010112503937.QMITTY: mayang 2010-11-25 ADD BEGIN */
//#define HUAWEI_PID_140C
//#if defined(HUAWEI_PID_140C)
//#define NDIS_IF_IDX 0x01
//#else
//#define NDIS_IF_IDX 0x04
//#endif
/* DTS2010112503937.QMITTY: mayang 2010-11-25 ADD END> */

/********************* �ⲿ�ӿں��� - �ṩ��usbnetģ��ʹ�� *********************/

/*������������usbnet��probe�����ڳɹ�������usb�ײ�ͨ�������*/
//void trigger_tty(void);

/*�˳�����Ŀǰ���ǵ��˳�ʱ������usbnet��disconnect�����е���*/
//void exit_tty(void);

/********************* ��ģ��ʹ�õ�ȫ�ֱ��� *********************/
/*  USB�����Ƿ���������ʶ�� */
volatile unsigned int connect_stat = 0;

/*  �ж϶˵�urb */
static struct urb * p_int_urb = NULL;

static struct work_struct intr_work;

/*  ��¼��Ӧ����tty�ṹ��Ϣ */
static tty_memo external_access_name[QMI_TTY_MINOR];

/* QMI��Ϣ�ڵ� */
QMI_NODE message_node[MSG_ROW];

/* QMI��Ϣ������ */
//unsigned char message_area[MSG_ROW][MSG_COLUMN];
unsigned char (*message_area)[MSG_ROW][MSG_COLUMN];

#define MEG_ROW(row) ((unsigned char *)message_area + (row * MSG_COLUMN))

/* QMI��Ϣ��ȡ�ڵ� */
PQMI_NODE pqmi_read_node;
/* QMI��Ϣд��ڵ� */
PQMI_NODE pqmi_write_node;

/* TTY�ڲ���ȡ����Ϣ��Ӧ�ı�־λ */
unsigned char signal_start_process[TRANSACTION_TYPE_MAX];

/*���ж�ͨ���Ļ������Ĵ�С*/
unsigned char interrupt_query_buf[INTERRUPT_PACKET_SIZE * MSG_ROW];

QMI_SERVICE_VERSION qmuxversion[QMUX_TYPE_ALL + 1];

/* wds client id */
static unsigned char internal_used_wds_clientid = QMUX_BROADCAST_CID - 1;/*���ڲ�ʹ�õ�WDS Client ID��һ��û����ĳ�ֵ*/

/* TTYģ��ʹ�õ�transaction_id */
static unsigned char internal_transaction_id = 0;

/* QMIͨѶ�Ƿ������������, ͨ�����ͻ�ȡqmi�汾��Ϣ�ж� */
static unsigned char qmi_setup_flag   = 0;

static unsigned char intr_setup_flag = 0;
static unsigned char ep0_read_setup_flag = 0;

#if defined(CONFIG_ATP_USB_ADAPTOR) && defined(SUPPORT_ATP_WANUMTS)
extern int g_iBalongDataCard;//0:�����ͨ���ݿ���1:����balong���ݿ�
#else
int g_iBalongDataCard = 0;//0:�����ͨ���ݿ���1:����balong���ݿ�
#endif

static int if_idx;
static int sdu_start_position;


/* <DTS2010112205139.QMITTY: mayang 2010-11-20 ADD BEGIN */
/* ���ڹ����ڴ���Ϣ����������Ϣ�������� */
struct list_head free_list;

/* ��������������������������⾺̬ */
static spinlock_t lock_for_list = SPIN_LOCK_UNLOCKED;
/* DTS2010112205139.QMITTY: mayang 2010-11-20 ADD END> */

/* mayang 2011-9-22 ���sem��֤qmi�״�openʱ��ʼ��������� */
static DECLARE_MUTEX(sema);

/********************* ��ģ��ʹ�õĺ������� *********************/

/* usb�ײ�QMI��Ϣ�Ķ�д�������������*/
static int start_intr_ep_query(void);
static void intr_completion(struct urb * purb);
static int write_control_qmi_msg(PQMI_NODE pnode, int count);
static int read_control_qmi_msg(PQMI_NODE pnode, int count);
//static void write_qmi_completion(struct urb * purb);
//static void read_qmi_completion(struct urb * purb);

static void free_urbs(void);
//static void kill_urbs(void); 

static BOOL create_qmi_interface_process(void);
static BOOL start_internal_transaction(unsigned int flag);
static BOOL internal_process_write(PQMI_NODE ptr, unsigned int flag);
static void compose_internal_request(PQMI_NODE ptr, unsigned int flag);
static void broadcast_msg(PQMI_NODE pnode, int count);
static void send_msg_to_user_space(PQMI_NODE pnode, struct tty_struct * tty, int count);
static void process_inbound_ctl_msg(PQMI_NODE pnode, int ret);
static void handle_get_version_response(PQMI_NODE pnode, int ret);
static void handle_get_clientid_response(PQMI_NODE pnode, int ret);
static void handle_release_clientid_response(PQMI_NODE pnode, int ret);

static void handler_resolve_available_msg(PQMI_NODE pnode, int ret);
/* <DTS2010112205139.QMITTY: mayang 2010-11-20 ADD BEGIN */
static void handle_response_available(void);

static void init_dev_message_node_and_area(void);
static void clean_node_and_area(PQMI_NODE pnode);
static void return_node_to_free(PQMI_NODE pnode);
static PQMI_NODE get_free_node();
/* DTS2010112205139.QMITTY: mayang 2010-11-20 ADD END> */

/* QMITTY����ģ����ں˽ӿں��� */
static int qmi_tty_open(struct tty_struct * tty, struct file * filp);
static void qmi_tty_close(struct tty_struct * tty, struct file * filp);
static int qmi_tty_write(struct tty_struct * tty, const unsigned char * buf, int count);
static int qmi_tty_write_room(struct tty_struct * tty);
static int qmi_tty_ioctl(struct tty_struct * tty, struct file * filp, unsigned int cmd, unsigned long arg);
static void	qmi_tty_throttle(struct tty_struct * tty);
static void qmi_tty_unthrottle(struct tty_struct * tty);

/* Ϊ��ҵ���ŵ���ȷ��������ĺ�*/
static spinlock_t lock_for_transactionid;
unsigned long flag_for_transactionid;
#define     GET_TRANSACTIONID(num) \
    do \
    {\
        spin_lock_irqsave(&lock_for_transactionid, flag_for_transactionid); \
        num = ++internal_transaction_id; \
        spin_unlock_irqrestore(&lock_for_transactionid, flag_for_transactionid); \
    } while (0)

/*��ӡǰ100���ж϶˵���ѯ����Ĵ�ӡ��*/
#ifdef PRT_TIMER_INFO
static int count_prn = 0;
 #define PRT_INFO_TIMES 10
 #define VIEW_TIMER(str) \
    do \
    {\
        if (count_prn++ < PRT_INFO_TIMES)\
        {printk(KERN_ALERT "The response message is %s\n", str);}\
    } while (0)
#else
 #define VIEW_TIMER(s)
#endif

/* ��������unsigned long ֵ�Ĳ�*/
#define differ_ret(i, j) (i > j ? i - j : (j ^ (~0)) + i)

/*lint -save -e* */
static struct tty_driver* qmi_tty_driver;
/*lint -restore*/

int set_rmnet_device(struct usb_device * rmnet_device, struct usb_host_interface * rmnet_intf)
{
    rmnet = rmnet_device;
    if (NULL != rmnet_intf)
    {
        if_idx = rmnet_intf->desc.bInterfaceNumber;
        printk("\n get remnet device and get interface No is [%d]\n", if_idx);
    }
    return 0;
}
EXPORT_SYMBOL_GPL(set_rmnet_device);

//balong LTE���ݿ�����by w00190448@20130509 start
int set_hwcdc_device(struct hw_cdc_net *dev, struct usb_host_interface * hw_cdc_net_intf)
{
    hwcdc_net = dev;

    if (NULL != hw_cdc_net_intf)
    {
        if_idx = hw_cdc_net_intf->desc.bInterfaceNumber;
        printk("\n get hw_cdc device and get interface No is [%d]\n", if_idx);
    }
    return 0;
}
EXPORT_SYMBOL_GPL(set_hwcdc_device);
//balong LTE���ݿ�����by w00190448@20130509 end

int set_rmnet_intr_ep(struct usb_endpoint_descriptor * ep_int)
{
    ep_int_rmnet = ep_int;
    return 0;
}
EXPORT_SYMBOL_GPL(set_rmnet_intr_ep);


static bool need_qmitty()
{
    if (rmnet){
        printk("QMITTY: quacomm device\r\n");
        return true;
    }
    else if (hwcdc_net){
        printk("QMITTY: balong device\r\n");
        return true;
    }
    printk("QMITTY: null device, do not support\r\n");
    return false;

}


/*------------------------------------------------------------
  ����ԭ��: void trigger_tty()
  ����: ����QMITTY״̬���Ĵ���������,��usbnet��probe̽�⺯���ڲ�ѯ
        ��Rm Net�ӿں󣬲��ɹ��������ж�ͨ������������ͨ���Ժ���ã�
        trigger_tty�����������ڲ���ʼ��ʼ������Ҫ�õ��ڲ��������ṹ��
        �ڲ�ƥ�����ݽṹ�����ý���QMI��Ϣ�ӿڵ��ڲ����̣����ú�����
        ѯ�����Ƿ�֧��QOS�����ڽ����ɹ��Ժ�ע���豸�ļ���㡣
  ����: ��
  ���: �ɹ�������Ϣ�ӿں󣬻�ע���豸�ļ���㣻����ע�ᣬ����ʧ��
  ����ֵ: ��
-------------------------------------------------------------*/
int trigger_tty(void)
{
	int i = 0;
    p_int_urb = NULL;
    connect_stat = 1;

//LTEģ��ĵ��岻��Ҫ�л�����,���������USB�豸����,�������LTEģ���޷�ʶ��
//����ǿ�����ó�LTEģ��Ϊbalongƽ̨.
#ifdef SUPPORT_ATP_INTERNAL_LTE_MODULE
    printk("\n\nThis is BALONG Internal LTE module\n\n");
    g_iBalongDataCard = 1;
#endif
    if(!need_qmitty())
        return -ENODEV;

    NDIS_DEBUG_MSG("trigger_tty is running!\n");
    //printk("bEpAddress=0x%0x, bInterval=0x%0x\n", ep_int_rmnet->bEndpointAddress, ep_int_rmnet->bInterval);

	/* ������Ϣ������ */
    message_area = kmalloc(sizeof(*message_area), GFP_KERNEL);
    if (NULL == message_area)
    {
    	printk("%s: Kmalloc the buffer failed.\n", __FUNCTION__);
    	return -ENOMEM;
    }

    if(g_iBalongDataCard == 1)//balong
    {
        printk("%s: Balong DataCard.\n", __FUNCTION__);    
        //if_idx = 0x04;//balong
        sdu_start_position = 0;
    }
    else
    {
        printk("%s: NOT Balong DataCard.\n", __FUNCTION__);  
        //if_idx = 0x01;
        sdu_start_position = 6;
    }

    /* ��ʼ���ڲ���������Ϣ�ڵ� */
    init_dev_message_node_and_area();

    /*��ʼ�����õ�����ͷ*/
    INIT_LIST_HEAD(&free_list);

    /*ÿһ����Ϣ���Ķ�д������ָ��ָ����Ӧ��������ָ��������*
     *SDU����ʼλ�ã���Ϣ���������Ա��Ϊ��Ӧ�������ꡣ�ڳ�ʼ��*
    �����е���Ϣ��㶼���������Ϣ�����С�*/
    for (i = 0; i < MSG_ROW; ++i)
    {
        message_node[i].p_msg_array = MEG_ROW(i) + sdu_start_position;
        message_node[i].row_num = (unsigned int)i;
        list_add_tail((plisthead) & message_node[i].ptr_qmi_node, &free_list);
    }

    /* alloc urbs */
    for (i = 0; i < MSG_ROW; ++i)
    {
        message_node[i].purb = usb_alloc_urb (0, GFP_KERNEL);
        if (!message_node[i].purb)
        {
        	printk("%s - No free urbs available, i = %d\n", __FUNCTION__, i);
        	/* free all non-null urb */
        	free_urbs();
            //return -ENOMEM;
        }
    }

    /* ��ʼ����д�ڵ� */
    pqmi_write_node = (PQMI_NODE)&(message_node[0]);
    pqmi_read_node = (PQMI_NODE)&(message_node[1]);
    
    printk("trigger_tty ok\n");
    return 0;
}
EXPORT_SYMBOL_GPL(trigger_tty);

/*------------------------------------------------------------
  ����ԭ��: void exit_tty()
  ����: �˳�QMITTY״̬���ĺ�����Ŀǰ�����˳�ʱ���Ŀ�����Ҫ������
        ��ģ�鸴λ��USB���ӱ���λ��Usbnet����disconnect��������
        usbnet��disconnect�����е��� exit_tty ��ʹQMITTY״̬����
        ����
  ����: ��
  ���: �豸�ļ���㱻ע��
  ����ֵ: ��
-------------------------------------------------------------*/
int exit_tty(void)
{
    if(!need_qmitty())
        return -ENODEV;

    free_urbs();
    connect_stat = 0;

    intr_setup_flag = 0; /* <DTS2010120301841.QMI: gKF34687 2010-12-06 ADD> */

	if (NULL != message_area)
	{
		kfree(message_area);
	}	
	rmnet = NULL;
    hwcdc_net = NULL;
    NDIS_DEBUG_MSG("exit_tty\n");
    return 0;
}
EXPORT_SYMBOL_GPL(exit_tty);
static void kill_urbs()
{
    int i;
    /* free urbs */
    for (i = 0; i < MSG_ROW; ++i)
    {
		if (message_node[i].purb)
        {
			/* kill in_progress urb */
        	if (message_node[i].purb->status == -EINPROGRESS)
            {   
                	usb_kill_urb(message_node[i].purb);
            }
	}
    }

    NDIS_DEBUG_MSG("kill_urbs\n");
}

static void free_urbs()
{
    int i;
    /* free urbs */
    if(p_int_urb)
    {
    	if (p_int_urb->status == -EINPROGRESS)
        {   
    		usb_kill_urb(p_int_urb);
        }
    	usb_free_urb(p_int_urb);
    }
    
    for (i = 0; i < MSG_ROW; ++i)
    {
        if (message_node[i].purb)
        {
            NDIS_DEBUG_MSG("%s - free urbs, i = %d\n", __FUNCTION__, i);
            /* free all non-null urb */
            if (message_node[i].purb->status == -EINPROGRESS)
            {   
                usb_kill_urb(message_node[i].purb);
            }
            usb_free_urb(message_node[i].purb);
        }
    }

    NDIS_DEBUG_MSG("free_urbs\n");
}

/*------------------------------------------------------------
  ����ԭ��: int qmi_tty_open(struct tty_struct * tty, struct file * filp)
  ����: qmi_tty_open������tty�����Ľӿں��������û�����ʹ��open������
        ��Ӧ���豸�ļ����ʱ��tty���Ľ������qmi_tty_open��������ɴ�
        �Ĳ�����Ϊ��֧�ֶ���û����̣��ڴ򿪹�����Ҫ���û�����ʹ�õĸ�
        �豸�Ž����б𣬶��ڲ���WDS���̵��û����̣�QMITTYģ���ڲ������
        1����ȡWDS Client ID��QMI��Ϣ��������ģ�顣�����ڵõ���ӦResponse
        �Ժ󣬼�¼���豸ƥ������֮�С�
        WDSӦ�ý����ڴ�QMITTYģ�������ڲ�״̬����־ֵû����λ������
        ״̬��������
  ����: tty_struct * tty ָ���ں�TTY�ṹ��ָ��
        file * filpָ���ں��ļ��ṹ��ָ��
  ���: ��
  ����ֵ: int ret ͨ��������0��ʾ�򿪲����Ѿ���ɡ�
-------------------------------------------------------------*/
static int qmi_tty_open(struct tty_struct * tty, struct file * filp)
{
    int ret = 0;
    unsigned short num = 0;
    int nMsgLen = 0;
    struct usb_device_descriptor desc;
    u16 status;

    PQMI_NODE pNode = NULL;

    if(tty == NULL)
    {
        return -ENODEV;
    }

    num = tty->index;

    if (0 != external_access_name[num].ptr_tty)
    {
        printk("%s - qmitty%d is already opened by someone!\n", __FUNCTION__, num);
        return -EBUSY;
    }

    tty->low_latency = 1;		
    tty->raw = 1;
    tty->real_raw = 1;

    //balong���ݿ����߸÷�֧
    if(g_iBalongDataCard != 1)
    {
#if defined(CONFIG_HSAN)
        ret = usb_get_descriptor(rmnet, USB_DT_DEVICE, 0, &desc, sizeof(desc));
        if (ret < 0) 
        {   
            printk("%s - qmitty%d : usb_get_descriptor failed!\n", __FUNCTION__, num);
        }
#else
        ret = usb_get_descriptor(rmnet, USB_DT_DEVICE, 0, &desc, sizeof(desc));
#endif
    }
    ret = usb_get_status(rmnet, USB_RECIP_ENDPOINT, 0, &status);
   // printk("usb_get_ep_status, ret=%d [0x%04x]\n", ret, status);

    ret = usb_get_status(rmnet, USB_RECIP_INTERFACE, 3, &status);
    //printk("usb_get_intf_status, ret=%d[0x%04x]\n", ret, status);

    if (connect_stat)
    {
        //balong���ݿ���֧
        if(g_iBalongDataCard == 1)
        {
            /* ������֤��һ�γ�ʼ��������� */
            down_interruptible(&sema);
    		if (1 == intr_setup_flag)
    		{
    			if((NULL != p_int_urb) && (-EINPROGRESS == p_int_urb->status))
                {
                    usb_kill_urb(p_int_urb);
                }
    		}
			/* �����ж϶˵���ѯ */
            ret = start_intr_ep_query();

            if (ret < 0)
            {
                printk(KERN_ALERT "%s- start intr query failed.\n", __FUNCTION__);
                if((NULL != p_int_urb) && (-EINPROGRESS == p_int_urb->status))
                {
                    usb_kill_urb(p_int_urb);
                }
                up(&sema);
                return ret;
            }
            intr_setup_flag = 1;
            /* ��ʼ��ndis��Ϣ����ȡndis version����Ӧ�ϲ�tty��client id
               ��ǰֻ֧��/dev/ndistty0, �����Ȳ�֧��
             */
            up(&sema);
            /* ��¼�ϲ����client��Ӧ��tty�ṹ */
            external_access_name[num].ptr_tty = tty;
        }
        else //��ͨ���ݿ�
        {
            {
#ifdef QMI_TTY_EP0_TEST
                if (0 == ep0_read_setup_flag)
                {
                    /* ��ȡ0�ſ��ƶ˵����� */
                    read_control_qmi_msg(pqmi_read_node, MSG_COLUMN);
                    ep0_read_setup_flag = 1;
                }
#endif
#ifdef QMI_TTY_EP0_TEST
                compose_internal_request(pqmi_write_node, GET_QMI_VERSION_INFO);
                //nMsgLen = le16_to_cpu(((PQCQMI)message_area[pqmi_write_node->row_num])->Length) + 1;
                nMsgLen = le16_to_cpu(((PQCQMI)MEG_ROW(pqmi_write_node->row_num))->Length) + 1;
                //printk("msg_len = 0x%0x\n", nMsgLen);
                write_control_qmi_msg(pqmi_write_node, nMsgLen);

                wait_ms(500);
                //compose_internal_request(pqmi_write_node, GET_INTERNAL_WDS_CLIENTID);
                //nMsgLen = le16_to_cpu(((PQCQMI)message_area[pqmi_write_node->row_num])->Length) + 1;
                //nMsgLen = le16_to_cpu(((PQCQMI)MEG_ROW(pqmi_write_node->row_num))->Length) + 1;
                //printk("msg_len = 0x%0x\n", nMsgLen);
                //write_control_qmi_msg(pqmi_write_node, nMsgLen);
#endif
#ifdef QMI_TTY_EP0_TEST
                int i = 0;
                for(i=0; i<10; i++)
                {
                    wait_ms(500);
                    read_control_qmi_msg(pqmi_read_node, MSG_COLUMN);
                }
#endif

                /* ������֤��һ�γ�ʼ��������� */
                down_interruptible(&sema);
#ifdef QMI_TTY_INTR_SUPPORT
                if (0 == intr_setup_flag)
                {
                    /* �����ж϶˵���ѯ */
                    ret = start_intr_ep_query();
                    if (ret < 0)
                    {
                        printk(KERN_ALERT "%s- start intr query failed.\n", __FUNCTION__);
                        if((NULL != p_int_urb) && (-EINPROGRESS == p_int_urb->status))
                        {
                            usb_kill_urb(p_int_urb);
                        }
                        up(&sema);
                        return ret;
                    }
                    intr_setup_flag = 1;
                }
#endif
                /* ��ʼ��qmi��Ϣ����ȡqmi version����Ӧ�ϲ�tty��client id
                   ��ǰֻ֧��/dev/qmitty0, �����Ȳ�֧��
                   mayang 2011-9-22 modify ��ʼ���У�ɾ����ȡ�ϲ�clientid��֧�ֶ��client
                 */
                qmi_setup_flag = 1; 
                if (0 == qmi_setup_flag)
                {
                    if (FALSE == create_qmi_interface_process())
                    {
                        printk("%s-init QMI failed!\n", __FUNCTION__);
                        kill_urbs();/* <DTS2010120301841.QMI: gKF34687 2010-12-06 ADD> */
                        up(&sema);
                        return -ENOSYS;
                    }
                    qmi_setup_flag = 1;
                }
                up(&sema);
/* <DTS2010112205120.QMITTY: mayang 2010-11-20 ADD BEGIN */
                while ((num < QMI_TTY_MINOR) && (0 == external_access_name[num].client_id))
                {
/* <DTS2010112205139.QMITTY: mayang 2010-11-20 Mod BEGIN */
                    /* get tty%d's corresponding clientid */
                    pNode = get_free_node();
                    if (NULL == pNode)
                    {
                        printk("%s-can not get free node, no mem here !!\n", __FUNCTION__);
                        return -ENOMEM;
                    }
                    compose_internal_request(pNode, GET_INTERNAL_WDS_CLIENTID);
                    //nMsgLen = le16_to_cpu(((PQCQMI)message_area[pNode->row_num])->Length) + 1;
                    nMsgLen = le16_to_cpu(((PQCQMI)MEG_ROW(pNode->row_num))->Length) + 1;
                    /*��¼�»�ȡClient ID��ҵ���ţ������յ�����ʱ�ж����ĸ��û����̻����Client ID*/
                    external_access_name[num].transaction_id = ((PQCQMICTL_MSG_HDR)
                    											(MEG_ROW(pNode->row_num)+ sdu_start_position))->TransactionId;
                                                                //&message_area[pNode->row_num][sdu_start_position])->TransactionId;
                    write_control_qmi_msg(pNode, nMsgLen);
        														
        			wait_ms(500);
                    read_control_qmi_msg(pqmi_read_node, MSG_COLUMN);											
/* DTS2010112205139.QMITTY: mayang 2010-11-20 Mod END> */
                }
/* DTS2010112205120.QMITTY: mayang 2010-11-20 ADD END> */
/* <DTS2010112205116.QMITTY: mayang 2010-11-20 ADD BEGIN */
        /* ��¼�ϲ����client��Ӧ��tty�ṹ */
                external_access_name[num].ptr_tty = tty;
            }

            
        }  //if(g_iBalongDataCard == 1)
    }
    else
    {
        printk("NDISTTY not actived.\n");
        return -ENODEV;
    }  //if (connect_stat)

    NDIS_DEBUG_MSG("NDISTTY is opened by %d th devfs node\n", num);
    return 0;
}

/*------------------------------------------------------------
  ����ԭ��: void qmi_tty_close(struct tty_struct * tty, struct file * filp)
  ����: qmi_tty_close������tty�����Ľӿں��������û�����ʹ��close�����ر�
        ��Ӧ���豸�ļ����ʱ��tty���Ľ������qmi_tty_close��������ɹر�
        �Ĳ�����Ϊ��֧�ֶ���û����̣��ڹرչ�����Ҫ���û�����ʹ�õĸ�
        �豸�Ž����б𣬶��ڲ���WDS���̵��û����̣�QMITTYģ���ڲ������
        1���ͷ�WDS Client ID��QMI��Ϣ��������ģ�顣�����ڵõ���ӦResponse
        �Ժ�����豸ƥ������֮����Ӧ�ļ�¼�Ĭ��WDS���̲����˳������
        WDS�����˳���QMITTY����ģ���˳���
  ����: tty_struct * tty ָ���ں�TTY�ṹ��ָ��
        file * filpָ���ں��ļ��ṹ��ָ��
  ���: ��
  ����ֵ: ��
-------------------------------------------------------------*/
static void qmi_tty_close(struct tty_struct * tty, struct file * filp)
{
/* <DTS2010112205139.QMITTY: mayang 2010-11-20 Mod BEGIN */
    unsigned short num = 0;
    PQMI_NODE pNode = NULL;

    num = tty->index;
       //balong���ݿ���֧
    if(g_iBalongDataCard == 1)
    {
        if(p_int_urb!=NULL)
        {
            usb_free_urb(p_int_urb);
            p_int_urb =NULL;
        }
        if (NULL == external_access_name[num].ptr_tty)
        {
            printk("%s - ndistty%d is already closed!\n", __FUNCTION__, num);
            return;
        }

        NDIS_DEBUG_MSG("%s - close ndistty%d successfully! \n", __FUNCTION__, num);            
        memset((void *)&external_access_name[num], 0, sizeof(tty_memo));
    }
    else
    {
    pNode = get_free_node();
    if (NULL == pNode)
    {
        printk("%s-can not get free node, no mem here !!\n", __FUNCTION__);
        return;
    }
    /* release corresponding client id and tty */
    /* �ڹرղ���ʱ��Ҫ�Ƚ����ļ�����ж���0#���ΪWDS����ʹ��*/
   // if (num == 0)
    {
        //if (0 != signal_start_process[GET_INTERNAL_WDS_CLIENTID])
        {
            int nMsgLen = 0;
            compose_internal_request(pNode, RELEASE_CLIENT_ID_TRANSACTION);

            /*������Ϣ�е�Client IDΪ�ڲ�ƥ���м�¼��Client ID*/
            ((PQMICTL_RELEASE_CLIENT_ID_REQ_MSG)
             //&message_area[pNode->row_num][sdu_start_position])->ClientId =
             (MEG_ROW(pNode->row_num)+ sdu_start_position))->ClientId =
                external_access_name[num].client_id;

            /*ͬʱ��¼�·����ͷ�Client Id��ҵ��ID���ڲ�ƥ���¼�ṹ�У�����*
            *�յ�����ʱ�����бȽ�����һ���û����̵�Client ID���ͷ�        */
            external_access_name[num].transaction_id = ((PQCQMICTL_MSG_HDR)
            											(MEG_ROW(pNode->row_num)+ sdu_start_position))->TransactionId;
                                                        //&message_area[pNode->row_num][sdu_start_position])->TransactionId;

            nMsgLen = le16_to_cpu(((PQCQMI)MEG_ROW(pNode->row_num))->Length) + 1;
            write_control_qmi_msg(pNode, nMsgLen);
            }
        }
    }

    NDIS_DEBUG_MSG("NDISTTY is closed by %d th devfs node\n", num);
}

/*------------------------------------------------------------
  ����ԭ��: int qmi_tty_write(struct tty_struct * tty, int from_user, const unsigned char * buf, int count)
  ����: qmi_tty_write������qmitty�����������tty����������Ҫ�Ĳ���
        ����֮һ�����û�������open�������豸�ļ����ʱ��tty����
        ���������ע���tty�����ķ������е� qmi_tty_write ��������
        ��ɰ��û��ռ�����ݴ����ںˡ���qmi_tty_write�л���û�����
        ����Ϣ��װ��Ϊ��һ��QMI��Ϣ��Ȼ����USB�ײ㷢�͡�
  ����: struct tty_struct * tty  ϵͳ�����ָ��tty_struct��ָ��
        int  from_user  ָʾ���������û��ռ仹�����ں˿ռ䡣
        const unsigned char * buf ָ���û��ռ���������ݻ�������
        int  count  Ҫ����������ֽ�����
  ���: ��
  ����ֵ: ��USB�ײ�ɹ�������ֽ�����ȥQMI��Ϣͷ�ĳ��ȡ�
-------------------------------------------------------------*/
static int qmi_tty_write(struct tty_struct * tty, const unsigned char * buf, int count)
{
    int ret = 0;
/* <DTS2010112205092.QMITTY: mayang 2010-11-20 Mod BEGIN */
    unsigned short num = 0;
    PQCQMI_HDR pqmihdr = NULL;
    int nMsgLen = 0;
/* <DTS2010112205139.QMITTY: mayang 2010-11-20 Mod BEGIN */
    PQMI_NODE pNode = NULL;

    /*printk(KERN_ALERT "qmi_tty_write is running\n");*/    /* < DTS2011011305021: gkf34687 2011-03-02 DEL>*/
    num = tty->index;
    pNode = get_free_node();
    if (NULL == pNode)
    {
        printk("%s-can not get free node, no mem here !!\n", __FUNCTION__);
        return -ENOMEM;
    }

    //balong���ݿ���֧
    if(g_iBalongDataCard == 1)
    {
        if (0 == connect_stat)
        {
            return_node_to_free(pNode);
            return -ENODEV;
        }
        memcpy((MEG_ROW(pNode->row_num)), buf, count);
        nMsgLen = ((unsigned short)((unsigned int)count - 1));
        ret = write_control_qmi_msg(pNode, nMsgLen + 1);

        return ret;
    }
    else
    {
        
        /*Ҫ����QMI��Ϣ���û����̱����Ѿ����Client ID */
        if (connect_stat && external_access_name[num].client_id == 0)
        {
            return -EFAULT;
        }

        /*�õ��û����̷��͵���Ϣ*/
        //memcpy(&message_area[pNode->row_num][sdu_start_position], buf, count);
        memcpy((MEG_ROW(pNode->row_num)+ sdu_start_position), buf, count);
        nMsgLen = ((unsigned short)(sizeof(QCQMI_HDR) + (unsigned int)count - 1));
        //pqmihdr = (PQCQMI_HDR)message_area[pNode->row_num];
        pqmihdr = (PQCQMI_HDR)MEG_ROW(pNode->row_num);

        pqmihdr->IFType   = USB_CTL_MSG_TYPE_QMI;
        pqmihdr->Length   = cpu_to_le16(nMsgLen);
        pqmihdr->CtlFlags = QMICTL_CTL_FLAG_CMD;

        pqmihdr->ClientId = external_access_name[num].client_id;
        pqmihdr->QMIType = external_access_name[num].type;

        //printk("%s - count = %d\n", __FUNCTION__, nMsgLen + 1);
        ret = write_control_qmi_msg(pNode, nMsgLen + 1);
        /* DTS2010112205139.QMITTY: mayang 2010-11-20 Mod END> */

        /*printk("%s - ret = %d, count = %d\n", __FUNCTION__, ret, count);*/    /* < DTS2011011305021: gkf34687 2011-03-02 DEL>*/
        /* DTS2010112205092.QMITTY: mayang 2010-11-20 Mod END> */
        if (ret > 0)
        {
            return (ret - (int)sizeof(QCQMI_HDR));/*���ظ��û��ռ����SDU�����Լ�ȥ��QMI��Ϣ��ͷ*/
        }
        else
        {
            return ret;
        }
    }
}

/*------------------------------------------------------------
  ����ԭ��: int qmi_tty_write_room(struct tty_struct * tty)
  ����: qmi_tty_write_room��tty�������ĵ��õģ���tty����ִ��write
       ����ʱ��tty�������Ļ����qmi_tty_write_room��������ѯ����
       �������Ĵ�С
  ����: ϵͳ�����ָ��tty_struct��ָ��
  ���: �ޡ�
  ����ֵ: �ڱ������з�����һ���̶��Ĵ�С��
-------------------------------------------------------------*/
static int qmi_tty_write_room(struct tty_struct * tty)
{
    return QMI_TTY_ROOM_SIZE;
}

/*------------------------------------------------------------
  ����ԭ��: void qmi_tty_throttle(struct tty_struct * tty)
  ����: qmi_tty_throttle���ڵ���������ʱ��֪ͨ�豸��Ҫ��������
  ����: ϵͳ�����ָ��tty_struct��ָ�롣
  ���: �ޡ�
  ����ֵ: �ޡ�
-------------------------------------------------------------*/
static void qmi_tty_throttle(struct tty_struct * tty)
{
    return;
}

/*------------------------------------------------------------
  ����ԭ��: void qmi_tty_unthrottle(struct tty_struct * tty)
  ����: qmi_tty_unthrottle���ڵ����������������֪ͨ�豸��������
  ����: ϵͳ�����ָ��tty_struct��ָ�롣
  ���: �ޡ�
  ����ֵ: ��
-------------------------------------------------------------*/
static void qmi_tty_unthrottle(struct tty_struct * tty)
{
    return;
}

/*------------------------------------------------------------
  ����ԭ��: int qmi_tty_ioctl(struct tty_struct * tty, struct file * filp, unsigned int cmd, unsigned long arg)
  ����: qmi_tty_ioctl����Ҫ���û����̵Ŀ��Ʒ����ӿڣ����û�����
        ������Ӧ��tty�豸�ļ����ʱ������ʹ��ioctl��������tty
        ����ִ��һЩ�ض��Ĳ���������ִ��ʲô�����ɹ��������֡�
  ����: struct tty_struct * tty  ϵͳ�����ָ��tty_struct��ָ��
        struct file * filp   ϵͳά�����ڲ����ļ�ָ��
        unsigned int cmd  ����Ĺ�����
        unsigned long arg  һ����һ�������ָ��
  ���: �ɹ���������
  ����ֵ: ִ���ض������Ľ��ֵ��
-------------------------------------------------------------*/
static int qmi_tty_ioctl(struct tty_struct * tty, struct file * filp, unsigned int cmd, unsigned long arg)
{
    short num;

    num = tty->index;
    switch (cmd)
    {
        case QMI_TTY_IOCQNUM:
        {
            return 0;
        }

        case QMI_TTY_IOCQNUM_CID:
        {
            /*�û������ڴ�QMITTY�豸�ļ�����Ժ󣬶�Ҫ���� ioctl(,QMI_TTY_IOCQNUM_CID)����ѯ�Ƿ��ѻ��Client ID*/
            NDIS_DEBUG_MSG("Query tty%d's CID=%d.\n", num, external_access_name[num].client_id);
            if (external_access_name[num].client_id)
            {
                QMI_IOCTL_PARAMS_ST stParams;
                if (0 == copy_from_user((void*)&stParams, (void*)arg, sizeof(stParams)))
                {
                    stParams.client_id = external_access_name[num].client_id;
                    __copy_to_user((QMI_IOCTL_PARAMS_ST*)arg, &stParams, sizeof(QMI_IOCTL_PARAMS_ST));
                }
            }
            return 0;
            /* DTS2010121503955.QMITTY: gKF34687 2011-01-19 ADD END> */
        }
        case QMI_TTY_IOCQNUM_TEST02:
        {
            printk(KERN_ALERT "Just do it in the third QMI_Tty_IOCQNUM_TEST02## ");
            return (2);
        }
        case QMI_TTY_IOCQNUM_TEST03:
        {
            printk(KERN_ALERT "Just do it in the fourth QMI_Tty_IOCQNUM_TEST03## ");

            return (3);
        }
        case QMI_TTY_IOCQNUM_TEST04:
        {
            printk(KERN_ALERT "Just do it in the fifth QMI_Tty_IOCQNUM_TEST04## ");

            return (1);
        }
        case QMI_TTY_IOCQNUM_TEST05:
        {
            printk(KERN_ALERT "Just do it in the sixth QMI_Tty_IOCQNUM_TEST05## \n");
            return (ERR_COMMAND_NOT_IMPLEMENT);
        }
        case QMI_TTY_IOCQNUM_TEST06:
        {
            return (ERR_COMMAND_NOT_IMPLEMENT);
        }
        case QMI_TTY_IOCQNUM_TEST07:
        {
            printk(KERN_ALERT "Just do it in the eighth QMI_Tty_IOCQNUM_TEST07## ");
            return (ERR_COMMAND_NOT_IMPLEMENT);
        }
        case QMI_TTY_IOCQNUM_TEST08:
        {
            printk(KERN_ALERT "Just do it in the ninth QMI_Tty_IOCQNUM_TEST08## ");
            return (ERR_COMMAND_NOT_IMPLEMENT);
        }

        default:
            return (ERR_COMMAND_NOT_EXIST);
    }
}

/*------------------------------------------------------------
  ����ԭ��: int write_control_qmi_msg(PQMI_NODE pnode, int count)
  ����: write_control_qmi_msg��ͨ���ײ��usb��Զ˵�����ģ�鷢��QMI��Ϣ�ĺ�����
        QMI��Ϣͨ��0�˵㷢�͡�
  ����: PQMI_NODE pnode �ǵ����߻�õĿ��е���Ϣ��㣬
        int count ��Ҫ���͵�SDU����
  ���: urb�ύ�ɹ����������������ڵײ�����Ŷ�
  ����ֵ: ���ͳɹ����ط�����Ϣ�ĳ��ȣ�ʧ����ѽ�������ͬʱ����usb���صĴ�����
-------------------------------------------------------------*/
static int write_control_qmi_msg(PQMI_NODE pnode, int count)
{
    int ret;

    //balong���ݿ���֧
    if(g_iBalongDataCard == 1)
    {
        if(connect_stat)
        {
        	char *pbuf = NULL;
        	pbuf = MEG_ROW(pnode->row_num);
            ret = usb_control_msg(rmnet,
                                  usb_sndctrlpipe(rmnet, 0),
                                  CDC_SEND_ENCAPSULATED_CMD, 
                                  CLASS_HOST_TO_INTERFACE,
                                  0x00,
                                  if_idx, /* <DTS2010112503937.NDISTTY: mayang 2010-11-25 MOD> */
                                  pbuf,
                                  count,
                                  5000);
        	if (ret < 0)
        	{
        		printk("%s - error, write < %s > ret = %d\n", __FUNCTION__, pbuf, ret);
        	}

            return_node_to_free(pnode);
        	return ret;
        }
        else
        {
            return_node_to_free(pnode);
            return 0;
        }
    }
    else //��ͨ���ݿ�
    {
#ifdef QMI_TTY_USB_CNTL_MSG
    	char *pbuf = NULL;
    	if (0 < count)
        {
    		pbuf = (char *)kmalloc(count + 1, GFP_KERNEL);
    		if (NULL == pbuf)
            {
    			printk("%s: Kmalloc the buffer failed.\n", __FUNCTION__);
    			return -1;
    		}
    		memset(pbuf, 0, count+1);
    	}
        //memcpy(pbuf, message_area[pnode->row_num], count);
    	memcpy(pbuf, MEG_ROW(pnode->row_num), count);
        ret = usb_control_msg(rmnet,
                              usb_sndctrlpipe(rmnet, 0),
                              CDC_SEND_ENCAPSULATED_CMD, 
                              CLASS_HOST_TO_INTERFACE,
                              0x00,
                              if_idx, /* <DTS2010112503937.QMITTY: mayang 2010-11-25 MOD> */
                              pbuf,
                              count,
                              5000);
        //printk("%s - ret = %d\n", __FUNCTION__, ret);
    	if (ret < 0)
    	{
    		printk("%s - error, ret = %d\n", __FUNCTION__, ret);
    	}
    	
        if (NULL != pbuf)
        {
            kfree(pbuf);
            pbuf = NULL;
        }
    	return ret;

#else
        if(connect_stat)
        {
            int nValue = 0x00;
            int nIdx   = if_idx;/* <DTS2010112503937.QMITTY: mayang 2010-11-25 MOD> */
            pnode->dr.bRequestType = CLASS_HOST_TO_INTERFACE;
            pnode->dr.bRequest = CDC_SEND_ENCAPSULATED_CMD;
            pnode->dr.wValue  = cpu_to_le16(nValue);
            pnode->dr.wIndex  = cpu_to_le16(nIdx);
            pnode->dr.wLength = cpu_to_le16(count);

            pnode->trans_start = jiffies;

            usb_fill_control_urb (pnode->purb,
                                    rmnet,
                                    usb_sndctrlpipe(rmnet, 0),
                                    (unsigned char*)&(pnode->dr),
                                    //(void *)&(message_area[pnode->row_num]),
                                    (void *)MEG_ROW(pnode->row_num),
                                    count,/* build urb */
                                    (usb_complete_t)write_qmi_completion,
                                    (void *)pnode);/*lint !e648*/

            ret = usb_submit_urb(pnode->purb, GFP_KERNEL);
            if(ret < 0)
            {
                printk("%s - submit_urb failed, ret = %d\n", __FUNCTION__, ret);
                return_node_to_free(pnode);/* DTS2010112205139.QMITTY: mayang 2010-11-20 Mod */
                return ret;
            }
            else
            {
                /*printk("%s - submit_urb successful, ret = %d\n", __FUNCTION__, ret); */   /* < DTS2011011305021: gkf34687 2011-03-02 DEL>*/
                return count;
            }
        }
        else
        {
            return_node_to_free(pnode);/* DTS2010112205139.QMITTY: mayang 2010-11-20 Mod */
            return 0;
        }
#endif       
    }
}

#if 0
/*------------------------------------------------------------
  ����ԭ��: void write_qmi_completion(struct urb * purb)
  ����: write_qmi_completion��write_control_qmi_msg��������̣����д���
        ���ύ��urb�ɹ�ִ�����Ժ�ĺ���������
  ����: struct urb * purb�ύ�˵�urb
  ���: ��ʹ�õ���Ϣ�������ӣ��ͷ�urbʹ�õ��ڴ�
  ����ֵ: ��
-------------------------------------------------------------*/
static void write_qmi_completion(struct urb * purb)
{
    /*printk("%s - urb->status=%d\n", __FUNCTION__, purb->status);*/    /* < DTS2011011305021: gkf34687 2011-03-02 DEL>*/
    if (purb->status)
    {
        printk("%s - error, urb->status=%d\n", __FUNCTION__, purb->status);
    }
    return_node_to_free((PQMI_NODE )(purb->context));/* DTS2010112205139.QMITTY: mayang 2010-11-20 Add */
}
#endif

/*------------------------------------------------------------
  ����ԭ��: int read_control_qmi_msg(PQMI_NODE pnode, int count)
  ����: read_control_qmi_msg�Ǵ�usb�ײ��ȡ����ģ�鷢�͵�QMI��Ϣ�ĺ�����
  ����: PQMI_NODE pnode ���÷���õ���Ϣ���
        int count Ҫ���͵���Ϣ����
  ���: �ɹ��ύurb,�����������ڵײ����urb�Ŷ�
  ����ֵ: �ɹ�ִ�з��ط����˵���Ϣ����ֵ��ʧ�ܰ�ʹ�õĽ�������ͬʱ����usb���صĴ�����
-------------------------------------------------------------*/
static int read_control_qmi_msg(PQMI_NODE pnode, int count)
{
    int ret = 0;

    //balong���ݿ���֧
    if(g_iBalongDataCard == 1)
    {    
        if(connect_stat)
        {		

            unsigned char* pbuf = NULL;
            pbuf = MEG_ROW(pnode->row_num);

        	ret = usb_control_msg(rmnet,
        	                        usb_rcvctrlpipe(rmnet, 0),
        	                        CDC_GET_ENCAPSULATED_RSP,
        	                        CLASS_INTERFACE_TO_HOST,
        	                        0x00,
        	                        if_idx,/* <DTS2010112503937.QMITTY: mayang 2010-11-25 MOD> */
        	                        pbuf,
        	                        count,
        	                        5000);
           // printk("%s - ret = %d \n", __FUNCTION__, ret);
            if(0 < ret)
            {		
        		handler_resolve_available_msg(pnode, ret);
            }
        	else if(0 > ret)
        	{
    			NDIS_DEBUG_MSG("%s - error, ret = %d\n", __FUNCTION__, ret);
    			return_node_to_free(pnode);
        	}
    		else
    		{
    			return_node_to_free(pnode);
    		}

    		return ret;
        }
        else
        {
            return_node_to_free(pnode);
    		printk("[%s][%s][%d]\r\n",__FILE__,__FUNCTION__,__LINE__);
            return 0;
        }	
    }
    else
    {
#ifdef QMI_TTY_USB_CNTL_MSG
    unsigned char* pbuf = NULL;
    pbuf = MEG_ROW(pnode->row_num);

    	ret = usb_control_msg(rmnet,
    	                        usb_rcvctrlpipe(rmnet, 0),
    	                        CDC_GET_ENCAPSULATED_RSP,
    	                        CLASS_INTERFACE_TO_HOST,
    	                        0x00,
    	                        if_idx,/* <DTS2010112503937.QMITTY: mayang 2010-11-25 MOD> */
    	                        pbuf,
    	                        count,
    	                        5000);
       // printk("%s - ret = %d \n", __FUNCTION__, ret);
        if(0 < ret)
        {
    		handler_resolve_available_msg(pnode, ret);
        }
    	else if(0 > ret)
    	{
            printk("\nComehere===LINE = %d; FILE = %s; FUNCTION = %s===\n",__LINE__,__FILE__,__FUNCTION__);
			NDIS_DEBUG_MSG("%s - error, ret = %d\n", __FUNCTION__, ret);
    	}
#else
        if(connect_stat)
        {
            pnode->dr.bRequestType = CLASS_INTERFACE_TO_HOST;
            pnode->dr.bRequest = CDC_GET_ENCAPSULATED_RSP;
            pnode->dr.wValue  = (unsigned short)cpu_to_le16(0x00);
            pnode->dr.wIndex  = (unsigned short)cpu_to_le16(NDIS_IF_IDX);/* <DTS2010112503937.QMITTY: mayang 2010-11-25 MOD> */
            pnode->dr.wLength = (unsigned short)cpu_to_le16(count);

            usb_fill_control_urb (pnode->purb,
                                  rmnet,
                                  usb_rcvctrlpipe(rmnet, 0),
                                  (unsigned char*)&(pnode->dr),
                                  //(void *)&(message_area[pnode->row_num]),
                                  (void *)MEG_ROW(pnode->row_num),
                                  count, 
                                  (usb_complete_t)read_qmi_completion,
                                  (void *)pnode);/*lint !e648*/

            ret = usb_submit_urb(pnode->purb, GFP_ATOMIC);
            if(ret < 0)
            {
                //printk("%s-submit read urb failed, ret=%d\n", __FUNCTION__, ret);
                return_node_to_free(pnode);/* DTS2010112205139.QMITTY: mayang 2010-11-20 Add */
                return ret;
            }
            else
            {
                /*printk("%s-submit read urb successful, ret=%d\n", __FUNCTION__, ret);*/    /* < DTS2011011305021: gkf34687 2011-03-02 DEL>*/
                return 0;
            }
        }
        else
        {
            return_node_to_free(pnode);/* DTS2010112205139.QMITTY: mayang 2010-11-20 Add */
            return 0;
        }
#endif
    }
}

/*------------------------------------------------------------
  ����ԭ��: void read_qmi_completion(struct urb * purb)
  ����: read_qmi_completion��read_control_qmi_msg��������̣����
        ���ύ��urb�����Ժ�ĺ�������
  ����: struct urb * purb �ύ��urb��ָ��
  ���: ����Ϣ������Ҫ������Ӧ�Ĵ���ͬʱҪ�ͷ�urbʹ���ڴ�
  ����ֵ: ��
-------------------------------------------------------------*/
static void read_qmi_completion(struct urb * purb)
{
    if (purb->actual_length && purb->status == 0)
    {
#ifdef QMI_MSG_PRINT_DBG
        int i = 0;
        PQMI_NODE tmpnode = purb->context;
        for(i = 0; i < purb->actual_length;++i)
        {
            //printk("%02x ",message_area[tmpnode->row_num][i]);
			printk("%02x ", *(unsigned char *)(MEG_ROW(tmpnode->row_num)+i));
            if ((0 != i) && (0 == (i+1)%16))
            {
                printk("\n");
            }
        }
        printk("\nRead Rsp msg(0x%0x) end\n", le16_to_cpu(*(USHORT*)(MEG_ROW(tmpnode->row_num)+sdu_start_position + 3)));
#endif
        handler_resolve_available_msg((PQMI_NODE)(purb->context), purb->actual_length);
    }
/* <DTS2010112205139.QMITTY: mayang 2010-11-20 ADD BEGIN */
    else
    {
        return_node_to_free((PQMI_NODE)(purb->context));
    }
/* DTS2010112205139.QMITTY: mayang 2010-11-20 ADD END> */
#ifndef QMI_TTY_INTR_SUPPORT
    if (connect_stat)
    {
        /* resubmit urb */
        wait_ms(100);
        if (purb->status != -ESHUTDOWN)
        {
            int ret = 0;
            ret = usb_submit_urb(purb, GFP_ATOMIC);
            if(ret < 0)
            {
                printk("%s-submit read urb failed, ret=%d\n", __FUNCTION__, ret);
            }
        }
    }
#endif
}

/*------------------------------------------------------------
  ����ԭ��: BOOL create_qmi_interface_process(void)
  ����: create_qmi_interface_process�ǽ���QMITTYģ��״̬������Ҫ
       ��ں����������Ĺ��̷�Ϊ���²��裺1�����Ȱ�״��ʱ��������
       ������ѯ�ж�ͨ���ķ���״̬��2����ʼ�ڲ�ҵ����̣���Щ����
       ��һ��ʧ����ɾ����ʱ��,ͬʱ״̬������ʧ�ܣ�ҵ�����Ϊ��a��
       ��ȡQMUXЭ��ջ�İ汾��Ϣ��b����ȡ�ڲ���WDS Client ID��c��
       ����Ҫ��������ݸ�ʽ��d����ȡ�ڲ���QoS Client ID��e������
       QoS�ϱ��¼���
  ����: ��
  ���: �ɹ�����״̬�������߲��ܽ���״̬����
  ����ֵ: BOOL �ɹ�����TRUE,ʧ�ܷ���FALSE.
-------------------------------------------------------------*/
static BOOL create_qmi_interface_process(void)
{
    if (FALSE == start_internal_transaction(GET_QMI_VERSION_INFO))
    {
        printk(KERN_ALERT "start internal transaction with GET_QMI_VERSION_INFO failed!\n");
        return FALSE;
    }
    return TRUE;
}

/*------------------------------------------------------------
  ����ԭ��: BOOL internal_process_write(PQMI_NODE ptr, unsigned int flag)
  ����: internal_process_write�������ڶ��ڣ�QMITTYģ���״̬��
       �ڳ�ʼ���������У����ڱ���Ҫ��ɵ�ҵ����з��Ͳ��ȴ���Ϣ
       ������ȷ��ҵ�񱻴��еĴ���ͬʱΪ�˱�����Ϣһ�η��Ͳ���
       ��������ģ�鲻�������ظ���Ϣ�������ڷ��ͺ͵ȴ������ж���
       ���˵ȴ����ơ�
  ����: PQMI_NODE ptr ���ڷ�����Ϣ����Ϣ��㡣
       unsigned int flag ��ʶ������ҵ��
  ���: �ޡ�
  ����ֵ: BOOL �ɹ�����TRUE��ʧ�ܷ���FALSE��
-------------------------------------------------------------*/
static BOOL internal_process_write(PQMI_NODE ptr, unsigned int flag)
{
    BOOL ret   = FALSE;
    int retnum = 0;
    int i = 0;
    int nMsgLen = 0;

    nMsgLen = le16_to_cpu(((PQCQMI)MEG_ROW(ptr->row_num))->Length) + 1;/* DTS2010112205139.QMITTY: mayang 2010-11-20 Mod */

    /*ѭ��������Ϣ3�Σ���һ�γɹ��򷵻أ�3�ζ����ɹ���ҵ��ʧ��*/
    for (i = 0; i < RETRY_TIMES; ++i)
    {
        retnum = write_control_qmi_msg(ptr, nMsgLen);

        if (retnum > 0)
        {
            ret = TRUE;
            break;
        }
        else
        {
            wait_ms((unsigned int)(WAIT_INTERVALTIME + WAIT_INTERVALTIME * i));
            printk(KERN_ALERT "can't write message\n");
        }
    }

    return ret;
}

/*------------------------------------------------------------
  ����ԭ��: BOOL start_internal_transaction(unsigned int flag)
  ����: start_internal_transaction�����Ĺ������ڳ�ʼ����QMITTY״
        ̬��ʱ�����Ϸ���QMI��Ϣ�ͽ���QMI��Ϣ�������Դ��е�ִ�С�
        ���ڷ���һ����Ϣ�����ڹ涨ʱ���ڵȴ�����������涨ʱ����
        û�з����򷵻�ʧ�ܡ�
  ����: unsigned int flag ��ʶ���Ǹ���ʼ�׶ε��ڲ�ҵ��
  ���: ��
  ����ֵ: BOOL �ɹ�����TRUEʧ�ܣ�����FALSE��
-------------------------------------------------------------*/
static BOOL start_internal_transaction(unsigned int flag)
{
    BOOL retval = FALSE;
    int i = 0;
/* <DTS2010112205139.QMITTY: mayang 2010-11-20 Mod BEGIN */
    PQMI_NODE pNode = NULL;
    
    pNode = get_free_node();
    if (NULL == pNode)
    {
        printk("%s-can not get free node, no mem here !!\n", __FUNCTION__);
        return -ENOMEM;
    }
    compose_internal_request(pNode, flag);

    if (internal_process_write(pNode, flag) == FALSE)
/* DTS2010112205139.QMITTY: mayang 2010-11-20 Mod END> */
    {
        printk(KERN_ALERT "internal_process_write failed\n");
        return FALSE;
    }

    /*ѭ���ȴ�3�Σ��鿴�ź������Ƿ���λ����һ�α���λ��ɹ�������*
     *3�β�ѯ�����ɹ�����ʧ�ܷ���                                   */
    for(i=0; i<RETRY_TIMES; ++i)
    {
        wait_ms((unsigned int)(WAIT_INTERVALTIME + WAIT_INTERVALTIME * i));
        if (1 == signal_start_process[flag])
        {
            retval = TRUE;
            break;
        }
    }
/* <DTS2010120301841.QMI: gKF34687 2010-12-06 ADD BEGIN*/
    // �����ɹ�����ȡ������ҵ���ͷ�pndoe
    if (FALSE == retval)
    {
        if (pNode->purb->status == -EINPROGRESS)
        {
        /* <DTS2010121503955.QMITTY: gKF34687 2011-01-19 ADD BEGIN */
            printk("%s-Failed, flag=%d, URB in Progress, being killed.\n", __FUNCTION__, flag);
            usb_kill_urb(pNode->purb);
            return_node_to_free(pNode);
        }
        /* DTS2010121503955.QMITTY: gKF34687 2011-01-19 ADD END> */
    }
/* DTS2010120301841.QMI: gKF34687 2010-12-06 ADD END>*/
    return retval;
}

/*------------------------------------------------------------
  ����ԭ��: void compose_internal_request(PQMI_NODE ptr, unsigned int flag)
  ����: compose_internal_request������ڲ���װQMI��Ϣ���кܶ๲ͬ
        ���̣����Ե������һ�����������ݱ�־���Ĳ�ͬ������Ϣ�ṹ��
        ��䲻ͬ���ֶ�ֵ��
  ����: PQMI_NODE ptr �ǵ��÷��������Ϣ���ָ�롣
        unsigned int �Ǳ�ʶ����Ҫ��װ������һ��QMI��Ϣ��
  ���: һ������ֵ�˵�QMI��Ϣ��
  ����ֵ: �ޡ�
-------------------------------------------------------------*/
static void compose_internal_request(PQMI_NODE ptr, unsigned int flag)
{
    PQCQMI_HDR pqmi;
    PQMICTL_MSG pqmux;
/* <AU3D00183.QMITTY: houxiaohe 2008-7-25 MODIFY BEGIN */
    unsigned char id;
/* AU3D00183.QMITTY: houxiaohe 2008-7-25 MODIFY END> */

    pqmi  = (PQCQMI_HDR)MEG_ROW(ptr->row_num);
    pqmux = (PQMICTL_MSG)ptr->p_msg_array;

    pqmi->IFType   = USB_CTL_MSG_TYPE_QMI;
    pqmi->CtlFlags = QCQMI_CTL_FLAG_CTL_POINT;

    switch (flag)
    {
    case GET_QMI_VERSION_INFO:
    {
        PQMICTL_GET_VERSION_REQ_MSG tmptr;
        tmptr = (PQMICTL_GET_VERSION_REQ_MSG)pqmux;
        tmptr->CtlFlags = QMICTL_FLAG_REQUEST;
        GET_TRANSACTIONID(id);
        tmptr->TransactionId = id;
        tmptr->QMICTLType = cpu_to_le16(QMICTL_GET_VERSION_REQ);
        tmptr->Length    = cpu_to_le16(0);

        pqmi->QMIType = QMUX_TYPE_CTL;
        pqmi->Length   = cpu_to_le16((unsigned short)(sizeof(QCQMI_HDR) + sizeof(QMICTL_GET_VERSION_REQ_MSG) - (unsigned int)1));
        pqmi->ClientId = 0;
        break;
    }
    case GET_INTERNAL_WDS_CLIENTID:
    {
        PQMICTL_GET_CLIENT_ID_REQ_MSG tmptr;
        tmptr = (PQMICTL_GET_CLIENT_ID_REQ_MSG)pqmux;
        tmptr->CtlFlags = QMICTL_FLAG_REQUEST;
        GET_TRANSACTIONID(id);
        tmptr->TransactionId = id;
        tmptr->QMICTLType = cpu_to_le16(QMICTL_GET_CLIENT_ID_REQ);
        tmptr->Length    = cpu_to_le16((unsigned short)(sizeof(QMICTL_GET_CLIENT_ID_REQ_MSG) - sizeof(QCQMICTL_MSG_HDR)));
        tmptr->TLVType   = QCTLV_TYPE_REQUIRED_PARAMETER;
        tmptr->TLVLength = cpu_to_le16(0x01);
        tmptr->QMIType   = QMUX_TYPE_WDS;

        pqmi->QMIType = QMUX_TYPE_CTL;
        pqmi->Length   = cpu_to_le16((unsigned short)(sizeof(QCQMI_HDR) + sizeof(QMICTL_GET_CLIENT_ID_REQ_MSG) - (unsigned int)1));
        pqmi->ClientId = 0;
        break;
    }
    case RELEASE_CLIENT_ID_TRANSACTION:
    {
        PQMICTL_RELEASE_CLIENT_ID_REQ_MSG tmptr;
        tmptr = (PQMICTL_RELEASE_CLIENT_ID_REQ_MSG)pqmux;
        tmptr->CtlFlags = QMICTL_FLAG_REQUEST;
        GET_TRANSACTIONID(id);
        tmptr->TransactionId = id;
        tmptr->QMICTLType = cpu_to_le16(QMICTL_RELEASE_CLIENT_ID_REQ);
        tmptr->Length    = cpu_to_le16((unsigned short)(sizeof(QMICTL_RELEASE_CLIENT_ID_REQ_MSG) - sizeof(QCQMICTL_MSG_HDR)));
        tmptr->TLVType   = QCTLV_TYPE_REQUIRED_PARAMETER;
        tmptr->TLVLength = cpu_to_le16(0x02);
        tmptr->QMIType   = QMUX_TYPE_WDS;

        pqmi->QMIType = QMUX_TYPE_CTL;
        pqmi->Length   = cpu_to_le16((unsigned short)(sizeof(QMICTL_RELEASE_CLIENT_ID_REQ_MSG) + sizeof(QCQMI_HDR) - (unsigned int)1));
        pqmi->ClientId = 0;
        break;
    }
    default:
        break;
    }
}

/*------------------------------------------------------------
  ����ԭ��: static int start_intr_ep_query()
  ����: �����ж϶˵���ѯ
  ����: ��
  ���: ��
  ����ֵ: �ɹ�����0�����򷵻�errno
-------------------------------------------------------------*/
static int start_intr_ep_query()
{
    int ret;
    unsigned int pipe = 0;
    unsigned int maxp = 0;
    unsigned char *pbuf = NULL;
    pipe = usb_rcvintpipe(rmnet, ep_int_rmnet->bEndpointAddress);
    maxp = le16_to_cpu(ep_int_rmnet->wMaxPacketSize);
    p_int_urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!p_int_urb)
    {
        printk("%s - alloc int urb failed\n", __FUNCTION__);
        return -ENOMEM;
    }
    
    pbuf = kmalloc(sizeof(interrupt_query_buf), GFP_KERNEL);
	if (NULL == pbuf)
    {
		printk("%s: Kmalloc the buffer failed.\n", __FUNCTION__);
		return -ENOMEM;
	}	
	memset(pbuf, 0, sizeof(interrupt_query_buf));

	memset(&intr_work, 0, sizeof(struct work_struct));
	INIT_WORK(&intr_work, handle_response_available);

    /* fill interrupt urb */
    usb_fill_int_urb(p_int_urb,
                     rmnet,
                     pipe,
                     pbuf,/*interrupt_query_buf,*/
                     maxp,
                     intr_completion,
                     &intr_work,
                     10);
    /* usb_qmi_clear_halt(rmnet, p_int_urb->pipe); */
    
    ret = usb_submit_urb(p_int_urb, GFP_KERNEL);
    if (ret < 0)
    {
        printk("%s - submit_urb failed, ret=%d\n", __FUNCTION__, ret);
    }
    else
    {
        NDIS_DEBUG_MSG("%s - submit_urb successful, ret=%d, len=%d\n", __FUNCTION__, ret, p_int_urb->actual_length);
        ret = p_int_urb->actual_length;
    }
    return ret;
}


/*------------------------------------------------------------
  ����ԭ��: void intr_completion(struct urb * purb)
  ����: intr_completion��read_interrupt_qmi_msg��������̣�
  ����: �ύ��urbָ��
  ���: ��Ч������ͨ��
  ����ֵ: ��
-------------------------------------------------------------*/
static void intr_completion(struct urb * purb)
{
    int status = 0;
	int urbstatus = purb->status;
    struct usb_ctrlrequest * ctlreq;
    static int i = 0;

	if (-ENOENT == urbstatus || -ESHUTDOWN == urbstatus)
	{
        //if (netif_msg_ifdown (hwcdc_net)){
	//		printk("[%s][%d] intr shutdown, code %d\r\n",__FILE__,__LINE__,urbstatus);
        //}
        return;
	}
	
	if (purb->actual_length == 0)
    {
        if (i++ < 10)
        {
            printk("%s -- i = %d\n", __FUNCTION__, i);
        }
        goto readtimer;/* lint !e801 */
    }
    else
    {
        ctlreq = (struct usb_ctrlrequest *)purb->transfer_buffer;

		if (0xa1 == ctlreq->bRequestType)
		{
	        switch (ctlreq->bRequest)
	        {
	            case CDC_NOTIFICATION_SERIAL_STATE:
	            {
	                VIEW_TIMER("CDC_NOTIFICATION_SERIAL_STATE");
	                break;
	            }
	            case CDC_NOTIFICATION_NETWORK_CONNECTION:
	            {
	                VIEW_TIMER("CDC_NOTIFICATION_NETWORK_CONNECTION");
	                break;
	            }
	            case CDC_NOTIFICATION_RESPONSE_AVAILABLE:
	            {
	                VIEW_TIMER("CDC available message");
	                if(connect_stat)
	                {
	                    //handle_response_available();/* DTS2010112205139.QMITTY: mayang 2010-11-20 Mod */
	                    schedule_work((struct work_struct*)purb->context);
	                }
	                break;
	            }
	            case CDC_NOTIFICATION_CONNECTION_SPD_CHG:
	            {
	                VIEW_TIMER("CDC_NOTIFICATION_CONNECTION_SPD_CHG");
	                break;
	            }
	            default:
	            {
	                VIEW_TIMER("CDC, default case.");
	                break;
	            }
        	}
        }
    }

    /* re-submit intr urb */
    if(connect_stat)
    {
        memset(purb->transfer_buffer, 0, purb->transfer_buffer_length);
        status = usb_submit_urb (purb, GFP_ATOMIC);
        /*printk("%s - resubmit intr urb, status=%d\n", __FUNCTION__, status);*/    /* < DTS2011011305021: gkf34687 2011-03-02 DEL>*/
        if (status != 0)
        {
            printk("%s - resubmit intr urb failed, status=%d\n", __FUNCTION__, status);
        }
    }
    
readtimer:
    VIEW_TIMER("CDC message end");
}

/*------------------------------------------------------------
  ����ԭ��: void handle_get_version_response(PQMI_NODE pnode, int ret)
  ����: handle_get_version_response�Ƕ��ڽ���QMITTY״̬���׶Σ�����
        ��ȡQMUXЭ��ջ�汾��Ϣ�����Ĵ�������Ĺ�����Ҫ��Ҫ����
        ������QMUXЭ��ջ�ĵİ汾�š����ڽ�����ɺ���бȽ�ȷ���Ƿ�
        �ǿ���֧��QOS���ܵİ汾��
  ����: PQMI_NODE pnode ���ڻ�ȡ�汾��Ϣ����Ϣ���ָ�롣
        int ret ��USB�ײ��������Ϣ���ȡ�
  ���: �������İ汾��Ϣ����ڼ�¼�汾��Ϣ�������С�
  ����ֵ: �ޡ�
-------------------------------------------------------------*/
static void handle_get_version_response(  PQMI_NODE  pnode, int ret)
{
    PQMICTL_MSG qmictl_msg;
    PQMICTL_GET_VERSION_RESP_MSG pqmi_ver_rsp;
    PQMUX_TYPE_VERSION_STRUCT pqmuxtypeversion;
    int msglen;
    unsigned char * pmsg;

    qmictl_msg = (PQMICTL_MSG)(MEG_ROW(pnode->row_num)+sdu_start_position);
    pqmi_ver_rsp = &(qmictl_msg->uGetVersionRsp);

    /* ����QMUXCTL_MSG��Ϣ������value�ĳ��� */
    msglen = le16_to_cpu(pqmi_ver_rsp->Length);
    /* ָ��������value�����ֽ� */
    pmsg = (unsigned char *)qmictl_msg + sizeof(QCQMICTL_MSG_HDR);
    
    if (QMI_RESULT_SUCCESS == le16_to_cpu(pqmi_ver_rsp->QMIResult))
    {
        unsigned short i;
        /*��ȥ��һ��TYPE LENGTH��ϵĳ���*/
        msglen -= (int)sizeof(QMI_TLV_HDR);
        /*��ȥResult �� error code�ĳ���*/
        msglen -= le16_to_cpu(pqmi_ver_rsp->TLVLength);
        /*��ȥ�ڶ���TYPE LENGTH��ϵĳ���*/
        msglen -= (int)sizeof(QMI_TLV_HDR);
        /*��ȥ�˷��ص�NumElements�ĳ��Ⱥ����е�QMUX�汾��Ϣ�ṹ�г���*/
        msglen -= le16_to_cpu(pqmi_ver_rsp->TLV2Length);

        /*ָ��Խ���˵�һ��TYPE LENGTH���*/
        pmsg += (int)sizeof(QMI_TLV_HDR);
        /*ָ��Խ����Result��error code*/
        pmsg += le16_to_cpu(pqmi_ver_rsp->TLVLength);
        /*ָ��Խ���˵ڶ���TYPE LENGTH���*/
        pmsg += (int)sizeof(QMI_TLV_HDR);
        /*ָ��Խ����NumElements�ĳ��Ⱥ����е�QMUX�汾��Ϣ�ṹ*/
        pmsg += le16_to_cpu(pqmi_ver_rsp->TLV2Length);

        /* ������Ϣ�ķ����������ж���QMUXЭ��ջ�汾��Ϣ�����أ�Ȼ���ո���ָ�������ƶ����õ�Э��ջ���汾��Ϣ */
        //printk("%s - Total 0x%0x set of Rsp.\n", __FUNCTION__, pqmi_ver_rsp->NumElements);
        pqmuxtypeversion = (PQMUX_TYPE_VERSION_STRUCT)&(pqmi_ver_rsp->TypeVersion);
        for (i = 0; i < pqmi_ver_rsp->NumElements; ++i)
        {
            qmuxversion[pqmuxtypeversion->QMUXType].Major = le16_to_cpu(pqmuxtypeversion->MajorVersion);
            qmuxversion[pqmuxtypeversion->QMUXType].Minor = le16_to_cpu(pqmuxtypeversion->MinorVersion);
            pqmuxtypeversion += 1;
        }

    }
    else
    {
        return;
    }

    if (qmuxversion[QMUX_TYPE_CTL].Major >= QMICTL_SUPPORTED_MAJOR_VERSION)
    {
        signal_start_process[GET_QMI_VERSION_INFO] = 1;
    }
}

/*------------------------------------------------------------
  ����ԭ��: void    handle_get_clientid_response(PQMI_NODE pnode, int ret)
  ����: handle_get_clientid_response���������ڶ��ڻ�ȡClient ID
        ����Ϣ�������д�������Ҫȷ���Ƿ���Ϣ����ָʾ�����Ƿ��
        ��������ɹ�����Ҫ�����������QMUX�������ͺͷ����Client
        ID������¼����Ӧ�����ݽṹ�У����ں�����ʹ�á�
  ����: PQMI_NODE pnode ���ڻ�ȡClient ID����Ϣ���ָ�롣
        int ret ��USB�ײ��������Ϣ���ȡ�
  ���: ��¼���ڲ�ʹ�ýṹ�е�Client ID
  ����ֵ: �ޡ�
-------------------------------------------------------------*/
static void    handle_get_clientid_response(PQMI_NODE pnode, int ret)
{
    PQMICTL_MSG qmictl_msg = (PQMICTL_MSG)(MEG_ROW(pnode->row_num)+sdu_start_position);
    unsigned char pqmitype;
    unsigned char pclientid;
    unsigned short i;

    /*������response��������ķ������ͺ�Client ID              */
    if (QMI_RESULT_SUCCESS == le16_to_cpu(qmictl_msg->uGetClientIdRsp.QMIResult))
    {
        pclientid = qmictl_msg->uGetClientIdRsp.ClientId;
        pqmitype = qmictl_msg->uGetClientIdRsp.QMIType;
    }
    else
    {
        printk(KERN_ALERT "%s - Get ClientId failed\n", __FUNCTION__);
        return;
    }
    if (pqmitype == QMUX_TYPE_WDS)
    {
        {
            for (i = 0; i < QMI_TTY_MINOR; ++i)
            {
                if (external_access_name[i].transaction_id == qmictl_msg->uGetClientIdRsp.TransactionId)
                {
                    external_access_name[i].type = pqmitype;
                    external_access_name[i].client_id = pclientid;
                    if(0 == i)
                    {
                        internal_used_wds_clientid = pclientid;
                        signal_start_process[GET_INTERNAL_WDS_CLIENTID] = 1;
                    }
                    break;
                }
            }
        }
    }
    else
    {
        printk(KERN_ALERT "%s - Unknown QMUX type rsp, SrvType=0x%0x\n", __FUNCTION__, pqmitype);
    }
}

/*------------------------------------------------------------
  ����ԭ��: void    handle_release_clientid_response(PQMI_NODE pnode, int ret)
  ����: handle_release_clientid_response�����ڴ������ϲ��û���
        �̷���ʹ���豸�ļ�����Ҫ�ͷ����뵽��WDS Client ID����
        �������ж��ͷŲ����Ƿ�ɹ�������ͷųɹ���Ҫ�����Ӧ�ڲ�
        ��¼�ṹ�е���Ӧ����Ϣ��
  ����: PQMI_NODE pnode �����ͷ�Client ID����Ϣ���ָ�롣
        int ret ��USB�ײ��������Ϣ���ȡ�
  ���: ��������Ӧ�ڲ���¼�ṹ�е���Ϣ��
  ����ֵ: �ޡ�
-------------------------------------------------------------*/
static void handle_release_clientid_response(PQMI_NODE pnode, int ret)
{
    int i;
    PQMICTL_MSG qmictl_msg;
    qmictl_msg = (PQMICTL_MSG)(MEG_ROW(pnode->row_num)+sdu_start_position);
    if (QMI_RESULT_SUCCESS == le16_to_cpu(qmictl_msg->uReleaseClientIdRsp.QMIResult))
    {
        for (i = 0; i < QMI_TTY_MINOR; ++i)
        {
            if (qmictl_msg->uReleaseClientIdRsp.TransactionId == external_access_name[i].transaction_id)
            {
                /* <DTS2010121503955.QMITTY: gKF34687 2011-01-19 ADD BEGIN */
                /* mayang 2011-1-13 ���wds�����clientid��� */
                if (0 == i)
                {
                    signal_start_process[GET_INTERNAL_WDS_CLIENTID] = 0;
                }
                memset((void *)&external_access_name[i], 0, sizeof(tty_memo));
                printk(KERN_ALERT "release the %d th devfs node.\n", i);
                break;
                /* DTS2010121503955.QMITTY: gKF34687 2011-01-19 ADD END> */
            }
        }
    }
}

/*------------------------------------------------------------
  ����ԭ��: void process_inbound_ctl_msg(PQMI_NODE pnode, int ret)
  ����: process_inbound_ctl_msg�������ڣ��Դӿ���ͨ��������CTL
       ���͵�QMI��Ϣ�������д����������ڲ�������Ҫ���۷�ת��
       �������Ϣ�����н��д���
  ����: PQMI_NODE pnode ���ڴ����������Ϣ����Ϣ���ָ�롣
        int ret ��USB�ײ��������Ϣ���ȡ�
  ���: �ޡ�
  ����ֵ: �ޡ�
-------------------------------------------------------------*/
static void process_inbound_ctl_msg(PQMI_NODE pnode, int ret)
{
    PQCQMICTL_MSG_HDR pmuxhdr;

    pmuxhdr = (PQCQMICTL_MSG_HDR)(MEG_ROW(pnode->row_num)+sdu_start_position);
    pmuxhdr->QMICTLType = le16_to_cpu(pmuxhdr->QMICTLType);
    pmuxhdr->Length = le16_to_cpu(pmuxhdr->Length);
    switch (pmuxhdr->QMICTLType)
    {
        case QMICTL_GET_VERSION_RESP:
        {
            //printk("%s -- Get Version Resp.\n", __FUNCTION__);
            handle_get_version_response(pnode, ret);
            break;
        }
        case QMICTL_GET_CLIENT_ID_RESP:
        {
            //printk("%s -- Get ClientID Resp.\n", __FUNCTION__);
            handle_get_clientid_response(pnode, ret);
            break;
        }
        case QMICTL_RELEASE_CLIENT_ID_RESP:
        {
            //printk("%s -- Release ClientID Resp.\n", __FUNCTION__);
            handle_release_clientid_response(pnode, ret);
            break;
        }
        default:
        {
            //printk("%s-Unkonw inbound Msg.\n", __FUNCTION__);
            break;
        }
    }
}


/*------------------------------------------------------------
  ����ԭ��: void    send_msg_to_user_space(PQMI_NODE pnode, struct tty_struct * tty, int count)
  ����: send_msg_to_user_space�������ڣ�����USB�ײ�ɹ�����QMI��Ϣ
        ʱ�����Ҿ����б���WDS���͵���Ϣ����Ҫ����WDS���̻������û�
        ���̽��д�������������tty���ĵĻ��������������ˢ�º���
        ��WDS��Ϣ�����û����̿ռ䡣
  ����: PQMI_NODE pnode ����WDS��Ϣ����Ϣ���.
        struct tty_struct * tty ��¼�µĶ�Ӧ���û����̴򿪵�ָ���
        tty_struct��ָ��
        int count Ҫˢ�µ��û����̿ռ���ֽ�����
  ���: ������û��ռ��WDS��Ϣ��
  ����ֵ: �ޡ�
-------------------------------------------------------------*/
static void send_msg_to_user_space(PQMI_NODE pnode, struct tty_struct * tty, int count)
{
    int i = 0;
    if (NULL == tty)
    {
        printk("%s - tty maybe null.\n", __FUNCTION__);
        return;
    }
    /*printk("%s - send msg_len(0x%0x) to user space.\n", __FUNCTION__, count-sdu_start_position);*/    /* < DTS2011011305021: gkf34687 2011-03-02 DEL>*/

    //balong���ݿ��߸÷�֧
    if(g_iBalongDataCard == 1)
    {
        tty_buffer_request_room(tty, count);
    }
    else
    {
        tty_buffer_request_room(tty, count-sdu_start_position);
    }
    for (i = 0; i < (count - sdu_start_position); ++i)
    {
        //balong���ݿ��߸÷�֧
        if(g_iBalongDataCard == 1)
        {
            tty_insert_flip_char(tty, *(unsigned char *)(MEG_ROW(pnode->row_num)+i),
             TTY_NORMAL);
        }
        else
        {
            tty_insert_flip_char(tty, *(unsigned char *)(MEG_ROW(pnode->row_num)+sdu_start_position +i),
            TTY_NORMAL);
        }
            
    }
    tty_flip_buffer_push(tty);
}


/*------------------------------------------------------------
  ����ԭ��: void    broadcast_msg(PQMI_NODE pnode, int count)
  ����: broadcast_msg �Ƕ����Ǵ�USB�ײ�������Ҫ�㲥�������Ϣ��
       ������㷨���£��������ÿһ�����ڵ��ϲ�WDS�û����̣�ִ��
       send_msg_to_user_space������Ϣ�����û��ռ䡣��󣬰���Ϣ
       �����뵽QoS_used_list�����У�����usbnet���д���
  ����: PQMI_NODE pnode ��Ҫ���д������Ϣ��㡣
        int count ��Ҫˢ���û��ռ����Ϣ�ĳ��ȡ�
  ���: ���д��ڵ��û����̺�usbnet������յ��㲥��Ϣ��
  ����ֵ: �ޡ�
-------------------------------------------------------------*/
static void broadcast_msg(PQMI_NODE pnode, int count)
{
    int i = 0;

    for (i = 0; i < QMI_TTY_MINOR; ++i)
    {
        if (external_access_name[i].ptr_tty != NULL)
        {
            send_msg_to_user_space(pnode, external_access_name[i].ptr_tty, count);
        }
    }
}

/*------------------------------------------------------------
  ����ԭ��: void  handler_resolve_available_msg(PQMI_NODE tmpnode, int ret)
  ����: handler_resolve_available_msg�ڶ�QMI��Ϣ����������б����ã����ڶԶ���
        ��QMI��Ϣ���д���
  ����: PQMI_NODE tmpnode ���������Ҫ�������Ϣ���
        int ret ʵ�ʶ�������Ϣ����ֵ
  ���: �Զ����QMI��Ϣ���н�������
  ����ֵ: ��
-------------------------------------------------------------*/
static void handler_resolve_available_msg(PQMI_NODE tmpnode, int ret)
{
    int i;

    //!balong���ݿ���֧
    if(g_iBalongDataCard == 1)
    {
        if (ret)
        {
    		for(i = 0; i < QMI_TTY_MINOR; i++)
    		{
    	        if (external_access_name[i].ptr_tty)
    	        {
    		        //printk("%s: send_msg_to_user_space [%d]%s\r\n",__FUNCTION__,i,external_access_name[i].ptr_tty->name); 
    	            send_msg_to_user_space(tmpnode, external_access_name[i].ptr_tty, ret);
    	        }
    		}
            return_node_to_free(tmpnode);
        }
        else
        {
            return_node_to_free(tmpnode);
        }           
    }
    else
    {
        if (ret)
        {
            PQCQMI_HDR pqmihdr = (PQCQMI_HDR)MEG_ROW(tmpnode->row_num);
            switch (pqmihdr->ClientId)
            {
                //printk("%s, QMIType=0x%0x, ClientId=0x%0x\n", __FUNCTION__, pqmihdr->QMIType, pqmihdr->ClientId);
                case QMUX_BROADCAST_CID:
                {
                    broadcast_msg(tmpnode, ret);
                    break;
                }
                case QCQMI_CTL_FLAG_CTL_POINT:
                {
                    process_inbound_ctl_msg(tmpnode, ret);
                    break;
                }
                default:
                {
                    if ((pqmihdr->QMIType == QMUX_TYPE_WDS) && (pqmihdr->ClientId == internal_used_wds_clientid))
                    {
                        send_msg_to_user_space(tmpnode, external_access_name[0].ptr_tty, ret);
                    }
                    else
                    {
                        /*ȱʡ������£����Ƕ��������û�����client_id���бȽϣ������ͬ��������Ӧ�û����̿ռ���*/
                        for (i = 1; i < QMI_TTY_MINOR; ++i)
                        {
                            if (pqmihdr->ClientId == external_access_name[i].client_id)
                            {
                                send_msg_to_user_space(tmpnode, external_access_name[i].ptr_tty, ret);
                            }
                        }
                    }
                }
                break;
            }
            return_node_to_free(tmpnode);/* DTS2010112205139.QMITTY: mayang 2010-11-20 Add */
        }
    /* <DTS2010112205139.QMITTY: mayang 2010-11-20 Mod BEGIN */
        else
        {
            return_node_to_free(tmpnode);
        }
        /* DTS2010112205139.QMITTY: mayang 2010-11-20 Mod END> */        
    }
}

/* <DTS2010112205139.QMITTY: mayang 2010-11-20 ADD BEGIN */
/*------------------------------------------------------------
  ����ԭ��: void    handle_response_available(void)
  ����: handle_response_available�ǽ���QMI��Ϣ��������Ĺؼ�������
        ���ж�ͨ������������ͨ���handle_response_available��
        ���USB����ͨ������QMI��Ϣ�������ڳɹ���ȡ����Ϣ������
        �󣬸�����Ϣ�����е�Client Id�������б���Ϣ�ý������ִ�
        ��ʽ��Ŀǰ���������в��õ��㷨ʱ���㲥��Ϣת��㲥����
        Client IdΪ0��ת��CTL����Ϣ������WDS Client Id�İ���Ϣ
        ������WDS�û����̿ռ䣻�������û����̵�Client Id�����Ϣ
        ���������û����̿ռ䡣
  ����: �ޡ�
  ���: ���ݲ�ͬ��Client Idת�벻ͬ�Ĵ����С�
  ����ֵ: �ޡ�
-------------------------------------------------------------*/
static void handle_response_available(void)
{
    PQMI_NODE pNode = NULL;

    pNode = get_free_node();
    if (NULL == pNode)
    {
        printk("%s-can not get free node, no mem here !!\n", __FUNCTION__);
        return;
    }

    read_control_qmi_msg(pNode, MSG_COLUMN);
}
/* DTS2010112205139.QMITTY: mayang 2010-11-20 ADD END> */

/*------------------------------------------------------------
  ����ԭ��: void    init_dev_message_node_and_area(void)
  ����: init_dev_message_node_and_area������ɣ�����QMITTYģ����
        ��Ҫ�õ��ģ��ڲ��ź����飬��Ϣ����������Ϣ��㣬�û�����
        ƥ������ĳ�ʼ�������ҳ�ʼ����ɺ�ÿ����Ϣ����ָ���
        Աָ��SDU��ʼλ�ã���Ϣ�����г�Ա����Ϣ��������������
        һһ��Ӧ�����е���Ϣ����ʼ��󶼱����뵽���н�������С�
  ����: ��
  ���: ��ɳ�ʼ�׶εĳ�ʼ�������������칤��
  ����ֵ: ��
-------------------------------------------------------------*/
static void init_dev_message_node_and_area(void)
{
    int i;
    //printk(KERN_ALERT "init_dev_message_node_and_area is running!\n");

    qmi_setup_flag = 0;

    /*��ʼ���ж���ѯ������*/
    memset((void *)interrupt_query_buf, 0, INTERRUPT_PACKET_SIZE * MSG_ROW);

    /*��ʼ���ź�����*/
    memset((void *) signal_start_process, 0, TRANSACTION_TYPE_MAX);

    /*��ʼ���û�����ƥ������*/
    for (i = 0; i < QMI_TTY_MINOR; ++i)
    {
        memset((void *)&external_access_name[i], 0, sizeof(tty_memo));
    }

    /*��ʼ����Ϣ������*/
	memset(message_area, 0, sizeof(*message_area));
	

    /*��ʼ����Ϣ���*/
    for (i = 0; i < MSG_ROW; ++i)
    {
        memset((void *) &message_node[i], 0, sizeof(QMI_NODE));
    }

}

/* <DTS2010112205139.QMITTY: mayang 2010-11-20 ADD BEGIN */
/*------------------------------------------------------------
  ����ԭ��: PQMI_NODE   get_free_node()
  ����: �κ���Ҫ��ȡ�ͷ���QMI��Ϣ�Ĳ������������Ȼ�ȡһ����Ϣ��
        �㣬ͨ��������Ϣ�����ָ�����Ϣ������������QMI��Ϣ��װ
        �䡢�����Ƕ���Ϣ���н�����
  ����: ��
  ���: ��
  ����ֵ: �ɹ���ȡ���򷵻سɹ���ȡ����Ϣ����ָ��
-------------------------------------------------------------*/
static PQMI_NODE get_free_node()
{
    PQMI_NODE tmpnode = NULL;
    unsigned long flags;
    struct list_head * tmplist = &free_list;

    spin_lock_irqsave(&lock_for_list, flags);

    if (!list_empty(tmplist))
    {
        tmpnode = (PQMI_NODE)tmplist->next;//lint !e40 !e115
        list_del_init((plisthead) & tmpnode->ptr_qmi_node);
    }

    spin_unlock_irqrestore(&lock_for_list, flags);

    return tmpnode;
}

/*------------------------------------------------------------
  ����ԭ��: void return_node_to_free(PQMI_NODE pnode)
  ����: return_read_node_to_free���������Ѿ���ɶ���Ϣ�������Ϣ
        ��㣬���ȴӶ�������ɾ����Ȼ�����Ӧ��Ϣ���������㣬���
        ����Ϣ���黹�����������С�
  ����: PQMI_NODE pnode�ȴ��黹����Ϣ��㡣
  ���: �ɹ�ִ�У����黹������������
  ����ֵ: ��
-------------------------------------------------------------*/
static void return_node_to_free(PQMI_NODE pnode)
{
    unsigned long flags;
    clean_node_and_area(pnode);
    spin_lock_irqsave(&lock_for_list, flags);
    list_add_tail(((plisthead) & pnode->ptr_qmi_node), &free_list);
    spin_unlock_irqrestore(&lock_for_list, flags);
}
/* DTS2010112205139.QMITTY: mayang 2010-11-20 ADD END> */


/*------------------------------------------------------------
  ����ԭ��: void    clean_node_and_area(PQMI_NODE pnode)
  ����: clean_node_and_area�������ڣ�������������д����Ϣ���
        �������Ӧ����Ϣ���������������������������Ҫ����Ϊ��1��
        ��ʼ��ÿһ��QMI_NODE�е�list_head��2������Ϣ������������
        �����㡣
  ����: PQMI_NODE pnode�ȴ�ִ�����������Ϣ��㡣
  ���: �������Ӧ����Ϣ�������������㡣
  ����ֵ: ��
-------------------------------------------------------------*/
static void    clean_node_and_area(PQMI_NODE pnode)
{
    INIT_LIST_HEAD(&pnode->ptr_qmi_node);
    memset((void * )MEG_ROW(pnode->row_num), 0, MSG_COLUMN);
}

static const struct tty_operations qmi_tty_ops = {
	.open       = qmi_tty_open,
	.close      = qmi_tty_close,
	.write      = qmi_tty_write,
	.write_room = qmi_tty_write_room,
	.ioctl      = qmi_tty_ioctl,
	.throttle   = qmi_tty_throttle,
	.unthrottle = qmi_tty_unthrottle,
};

/*------------------------------------------------------------
  ����ԭ��: static int tty_module_init(void)
  ����: tty_module_init��QMITTYģ��ģ�ģ���ʼ����������ģ��װ
        �����ڵ㡣
  ����: ��
  ���: ��
  ����ֵ: ��
-------------------------------------------------------------*/
static int tty_module_init(void)
{
    int result = 0;
    qmi_tty_driver = alloc_tty_driver(QMI_TTY_MINOR);
	if (!qmi_tty_driver)
    {
        printk("%s - alloc_tty_driver failed!\n", __FUNCTION__);
	    return -ENOMEM;
    }

    qmi_tty_driver->owner = THIS_MODULE;
	qmi_tty_driver->driver_name = "QMITTY";
	qmi_tty_driver->name = 	"qmitty";
	qmi_tty_driver->major = QMI_TTY_MAJOR;
	qmi_tty_driver->minor_start = 0;
	qmi_tty_driver->type = TTY_DRIVER_TYPE_SERIAL;
	qmi_tty_driver->subtype = SERIAL_TYPE_NORMAL;
	qmi_tty_driver->flags = TTY_DRIVER_REAL_RAW /*| TTY_DRIVER_DYNAMIC_DEV*/;
	qmi_tty_driver->init_termios = tty_std_termios;
    qmi_tty_driver->init_termios.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
    qmi_tty_driver->init_termios.c_lflag = ISIG;
    qmi_tty_driver->init_termios.c_oflag = 0;
    tty_set_operations(qmi_tty_driver, &qmi_tty_ops);
    
    result = tty_register_driver(qmi_tty_driver);
    if (result)
    {
        printk(KERN_ALERT "%s - tty_register_driver failed!\n", __FUNCTION__);
        return (-1);
    }

    return 0;
}

/*------------------------------------------------------------
  ����ԭ��: static void tty_module_exit(void)
  ����: tty_module_exit��QMITTYģ��ģ�ģ����˳�����
  ����: ��
  ���: ��
  ����ֵ: ��
-------------------------------------------------------------*/
static void tty_module_exit(void)
{
    tty_unregister_driver(qmi_tty_driver);
    /* <DTS2010121503955.QMITTY: gKF34687 2011-01-19 ADD BEGIN */
    /* free tty driver */
    put_tty_driver(qmi_tty_driver);

    if (NULL != message_area)
    {
    	kfree(message_area);
    }	
    /* DTS2010121503955.QMITTY: gKF34687 2011-01-19 ADD END> */
}

MODULE_LICENSE("GPL");
module_init(tty_module_init);
module_exit(tty_module_exit);


