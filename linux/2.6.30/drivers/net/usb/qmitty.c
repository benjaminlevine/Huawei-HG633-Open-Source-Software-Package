/***********************************************************************
  版权信息: 版权所有(C) 1988-2008, 华为技术有限公司.
  文件名: QMITTY.c
  作者: houxiaohe 00118013
  版本: 001
  创建日期: 2008-5-21
  完成日期: 2008-5-25
  功能描述: QMITTY模块是实现高通Rm Net的重要组成部分，模块在成功加载并被触
            发以后，注册了一组设备文件结点。用户进程通过打开设备文件结点来
            完成QMI消息的读写，完成和无线模块的QMI消息交互。同时在目前的实
            框架中，QMITTY模块为USBNET模块构造出发送和接收QOS消息的调用接口
            ，QMITTY模块和USBNET模块合作完成对于数据包发送的QOS流控。为了有
            效利用资源，模块内部用链表管理一组消息缓冲区。QMITTY模块在建立
            过程中必须要获取1个WDS Client ID和1个QOS Client ID,WDS Client ID
            保留给WDS用户进程使用,QOS Client ID保留给USBNET使用。

  主要函数列表: trigger_tty, exit_tty, create_qmi_interface_process,
                compose_internal_request, send_msg_to_user_space,
                process_inbound_ctl_msg, interrupt_timer_proc,
                handle_reponse_available, get_node, insert_node_to_list,
                remove_list_node, return_node_to_free, qmi_tty_open,
                qmi_tty_close, qmi_tty_write,

                write_control_qmi_msg, write_qmi_completion, read_control_qmi_msg,
                read_qmi_completion, handler_resolve_available_msg,

  修改历史记录列表:
    <作  者>    <修改时间>  <版本>  <修改描述>
    houxiaohe    2008-06-28  001    在QMITTY模块中填加write_control_qmi_msg,
                        read_control_qmi_msg,read_interrupt_qmi_msg,
                        以及和3个函数的回调函数的实现。同时修改了相应的
                        QMI_NODE结构，增加了urb类型的指针和计时量。
    houxiaohe	2008-06-29   001    在QMITTY模块中填加tasklet，用于在轮询中断端点的
                        函数的回调例程中，被触发执行读取QMI消息返馈的任务
    houxiaohe	2008-07-02   001    修改QMI状态机初始化过程中，要进行收发处理的QMI消息
                        为Get_QMI_Version,Get_WDS_Client_ID,Set_QoS_DataFormat,
                        Get_QoS_Client_ID,同时修改收到QMI消息返馈以后的处理
                        过程。QMITTY模块只对CTL类型服务的QMI消息返馈进行处理,
                        WDS相关的消息将送入用户空间进行处理，QoS业务相关的消息
                        将由usbnet进行处理
    houxiaohe	2008-07-02   001    删除了原来设计中的reset_tty函数和相应的消息装配部分
    houxiaohe	2008-07-02   001    添加悬挂的urb处理函数，定时器以2秒的频率检查已提交的urb
                        是否没有响应，如果没有响应的就从底层解链
    houxiaohe   2008-07-18  B000    由于原来在中断完成例程中进行 tasklet_schule触发从
                                   控制端点的QMI消息读写，会造成单板的反复重启，所以修改
                                   read_control_qmi_msg,write_control_qmi_msg的实现，以及
                                   完成例程的实现，同时增加了SET_QMI_INSTANCE_ID的处理，删
                                   除了原来增加的解除悬挂urb的定时器。保留解除悬挂的处理。
    houxiaohe   2008-07-23  B000   修改中断定时器运行打印信息为10次，修改中断定时器处理函数打印信息为10次
    houxiaohe   2008-07-23  B000   增加新的标志量标识USB连接是否正常，修改所有USB读写请求首先要判定
                                   连接是否存在，如果不存在连接就不能发生底层提交。同时在连接
                                   断开，模块被退出时要对中断请求也解链操作。
    houxiaohe   2008-07-25  B000   修改对于 qmi_tty_write中对于用户进程下发的PDU的写操作，新的实现中
                                   对于用户进程下发的PDU只装配QMI头后进行发送，不再对PDU的内容进行改写
    houxiaohe   2008-07-25  B000   修改对于 qmi_tty_ioctl中对于WDS用户进程查询START_NETWORK_INTERFACE_REQ业务ID
                                   查询的支持，新的实现中模块内部维护的TransactionId都是unsigned char，
                                   用户进程自行维护SDU中的Transaction ID
    mayang      2010-10-16  B101   移植到realtek平台产品B683, 无线使用MDM8200A,不支持QoS
  备注:
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
//调试开关
#ifdef _DEBUG_
#define NDIS_DEBUG_MSG printk
#else
#define NDIS_DEBUG_MSG
#endif



/********************* 外部变量 - 定义在usbnet模块中 *********************/

/* usbnet的probe函数中探查到的RmNet指针和中断端点的端点描述符*/
static struct usb_device * rmnet = NULL;
static struct usb_endpoint_descriptor * ep_int_rmnet = NULL;

//balong LTE数据卡驱动by w00190448@20130509 start
static struct hw_cdc_net	 *hwcdc_net;
//balong LTE数据卡驱动by w00190448@20130509 end
/* <DTS2010112503937.QMITTY: mayang 2010-11-25 ADD BEGIN */
//#define HUAWEI_PID_140C
//#if defined(HUAWEI_PID_140C)
//#define NDIS_IF_IDX 0x01
//#else
//#define NDIS_IF_IDX 0x04
//#endif
/* DTS2010112503937.QMITTY: mayang 2010-11-25 ADD END> */

/********************* 外部接口函数 - 提供给usbnet模块使用 *********************/

/*触发器函数由usbnet的probe函数在成功配置完usb底层通道后调用*/
//void trigger_tty(void);

/*退出函数目前考虑的退出时机是在usbnet的disconnect方法中调用*/
//void exit_tty(void);

/********************* 本模块使用的全局变量 *********************/
/*  USB网卡是否正常连接识别 */
volatile unsigned int connect_stat = 0;

/*  中断端点urb */
static struct urb * p_int_urb = NULL;

static struct work_struct intr_work;

/*  记录对应接入tty结构信息 */
static tty_memo external_access_name[QMI_TTY_MINOR];

/* QMI消息节点 */
QMI_NODE message_node[MSG_ROW];

/* QMI消息缓存区 */
//unsigned char message_area[MSG_ROW][MSG_COLUMN];
unsigned char (*message_area)[MSG_ROW][MSG_COLUMN];

#define MEG_ROW(row) ((unsigned char *)message_area + (row * MSG_COLUMN))

/* QMI消息读取节点 */
PQMI_NODE pqmi_read_node;
/* QMI消息写入节点 */
PQMI_NODE pqmi_write_node;

/* TTY内部获取的消息对应的标志位 */
unsigned char signal_start_process[TRANSACTION_TYPE_MAX];

/*读中断通道的缓冲区的大小*/
unsigned char interrupt_query_buf[INTERRUPT_PACKET_SIZE * MSG_ROW];

QMI_SERVICE_VERSION qmuxversion[QMUX_TYPE_ALL + 1];

/* wds client id */
static unsigned char internal_used_wds_clientid = QMUX_BROADCAST_CID - 1;/*给内部使用的WDS Client ID赋一个没意义的初值*/

/* TTY模块使用的transaction_id */
static unsigned char internal_transaction_id = 0;

/* QMI通讯是否可以正常建立, 通过发送获取qmi版本信息判断 */
static unsigned char qmi_setup_flag   = 0;

static unsigned char intr_setup_flag = 0;
static unsigned char ep0_read_setup_flag = 0;

#if defined(CONFIG_ATP_USB_ADAPTOR) && defined(SUPPORT_ATP_WANUMTS)
extern int g_iBalongDataCard;//0:代表高通数据卡，1:代表balong数据卡
#else
int g_iBalongDataCard = 0;//0:代表高通数据卡，1:代表balong数据卡
#endif

static int if_idx;
static int sdu_start_position;


/* <DTS2010112205139.QMITTY: mayang 2010-11-20 ADD BEGIN */
/* 用于管理内存消息缓冲区和消息结点的链表 */
struct list_head free_list;

/* 链表操作函数，加入自旋锁避免竞态 */
static spinlock_t lock_for_list = SPIN_LOCK_UNLOCKED;
/* DTS2010112205139.QMITTY: mayang 2010-11-20 ADD END> */

/* mayang 2011-9-22 添加sem保证qmi首次open时初始化正常完成 */
static DECLARE_MUTEX(sema);

/********************* 本模块使用的函数声明 *********************/

/* usb底层QMI消息的读写函数和完成例程*/
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

/* QMITTY驱动模块的内核接口函数 */
static int qmi_tty_open(struct tty_struct * tty, struct file * filp);
static void qmi_tty_close(struct tty_struct * tty, struct file * filp);
static int qmi_tty_write(struct tty_struct * tty, const unsigned char * buf, int count);
static int qmi_tty_write_room(struct tty_struct * tty);
static int qmi_tty_ioctl(struct tty_struct * tty, struct file * filp, unsigned int cmd, unsigned long arg);
static void	qmi_tty_throttle(struct tty_struct * tty);
static void qmi_tty_unthrottle(struct tty_struct * tty);

/* 为了业务编号的正确，而定义的宏*/
static spinlock_t lock_for_transactionid;
unsigned long flag_for_transactionid;
#define     GET_TRANSACTIONID(num) \
    do \
    {\
        spin_lock_irqsave(&lock_for_transactionid, flag_for_transactionid); \
        num = ++internal_transaction_id; \
        spin_unlock_irqrestore(&lock_for_transactionid, flag_for_transactionid); \
    } while (0)

/*打印前100次中断端点轮询结果的打印宏*/
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

/* 计算两个unsigned long 值的差*/
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

//balong LTE数据卡驱动by w00190448@20130509 start
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
//balong LTE数据卡驱动by w00190448@20130509 end

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
  函数原型: void trigger_tty()
  描述: 建立QMITTY状态机的触发器函数,由usbnet的probe探测函数在查询
        到Rm Net接口后，并成功配置完中断通道、批量数据通道以后调用，
        trigger_tty触发器函数内部开始初始化所有要用的内部缓冲区结构、
        内部匹配数据结构、调用建立QMI消息接口的内部过程，调用函数查
        询网络是否支持QOS，并在建立成功以后注册设备文件结点。
  输入: 无
  输出: 成功建立消息接口后，会注册设备文件结点；否则不注册，建立失败
  返回值: 无
-------------------------------------------------------------*/
int trigger_tty(void)
{
	int i = 0;
    p_int_urb = NULL;
    connect_stat = 1;

//LTE模块的单板不需要切换过程,如果带其他USB设备启动,可能造成LTE模块无法识别
//这里强制设置成LTE模块为balong平台.
#ifdef SUPPORT_ATP_INTERNAL_LTE_MODULE
    printk("\n\nThis is BALONG Internal LTE module\n\n");
    g_iBalongDataCard = 1;
#endif
    if(!need_qmitty())
        return -ENODEV;

    NDIS_DEBUG_MSG("trigger_tty is running!\n");
    //printk("bEpAddress=0x%0x, bInterval=0x%0x\n", ep_int_rmnet->bEndpointAddress, ep_int_rmnet->bInterval);

	/* 分配消息缓冲区 */
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

    /* 初始化内部变量和消息节点 */
    init_dev_message_node_and_area();

    /*初始化所用到链表头*/
    INIT_LIST_HEAD(&free_list);

    /*每一个消息结点的读写缓冲区指针指向相应行坐标所指向的数组的*
     *SDU的起始位置，消息结点的坐标成员赋为对应的行坐标。在初始阶*
    段所有的消息结点都链入空闲消息链表中。*/
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

    /* 初始化读写节点 */
    pqmi_write_node = (PQMI_NODE)&(message_node[0]);
    pqmi_read_node = (PQMI_NODE)&(message_node[1]);
    
    printk("trigger_tty ok\n");
    return 0;
}
EXPORT_SYMBOL_GPL(trigger_tty);

/*------------------------------------------------------------
  函数原型: void exit_tty()
  描述: 退出QMITTY状态机的函数。目前对于退出时机的考虑主要是在无
        线模块复位后，USB连接被复位，Usbnet调用disconnect方法，在
        usbnet的disconnect方法中调用 exit_tty ，使QMITTY状态机撤
        销。
  输入: 无
  输出: 设备文件结点被注销
  返回值: 无
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
  函数原型: int qmi_tty_open(struct tty_struct * tty, struct file * filp)
  描述: qmi_tty_open函数是tty驱动的接口函数，当用户进程使用open函数打开
        相应的设备文件结点时，tty核心将会调用qmi_tty_open方法，完成打开
        的操作，为了支持多个用户进程，在打开过程中要对用户进程使用的副
        设备号进行判别，对于不是WDS进程的用户进程，QMITTY模块内部构造出
        1条获取WDS Client ID的QMI消息发往无线模块。并会在得到相应Response
        以后，记录在设备匹配数组之中。
        WDS应用进程在打开QMITTY模块后，如果内部状态机标志值没有置位，则开启
        状态机创建。
  输入: tty_struct * tty 指向内核TTY结构的指针
        file * filp指向内核文件结构的指针
  输出: 无
  返回值: int ret 通常都返回0表示打开操作已经完成。
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

    //balong数据卡不走该分支
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
        //balong数据卡分支
        if(g_iBalongDataCard == 1)
        {
            /* 加锁保证第一次初始化正常完成 */
            down_interruptible(&sema);
    		if (1 == intr_setup_flag)
    		{
    			if((NULL != p_int_urb) && (-EINPROGRESS == p_int_urb->status))
                {
                    usb_kill_urb(p_int_urb);
                }
    		}
			/* 启动中断端点轮询 */
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
            /* 初始化ndis消息，获取ndis version及对应上层tty的client id
               当前只支持/dev/ndistty0, 其他先不支持
             */
            up(&sema);
            /* 记录上层接入client对应的tty结构 */
            external_access_name[num].ptr_tty = tty;
        }
        else //高通数据卡
        {
            {
#ifdef QMI_TTY_EP0_TEST
                if (0 == ep0_read_setup_flag)
                {
                    /* 读取0号控制端点数据 */
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

                /* 加锁保证第一次初始化正常完成 */
                down_interruptible(&sema);
#ifdef QMI_TTY_INTR_SUPPORT
                if (0 == intr_setup_flag)
                {
                    /* 启动中断端点轮询 */
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
                /* 初始化qmi消息，获取qmi version及对应上层tty的client id
                   当前只支持/dev/qmitty0, 其他先不支持
                   mayang 2011-9-22 modify 初始化中，删除获取上层clientid，支持多个client
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
                    /*记录下获取Client ID的业务编号，用于收到返馈时判断是哪个用户进程获得了Client ID*/
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
        /* 记录上层接入client对应的tty结构 */
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
  函数原型: void qmi_tty_close(struct tty_struct * tty, struct file * filp)
  描述: qmi_tty_close函数是tty驱动的接口函数，当用户进程使用close函数关闭
        相应的设备文件结点时，tty核心将会调用qmi_tty_close方法，完成关闭
        的操作，为了支持多个用户进程，在关闭过程中要对用户进程使用的副
        设备号进行判别，对于不是WDS进程的用户进程，QMITTY模块内部构造出
        1条释放WDS Client ID的QMI消息发往无线模块。并会在得到相应Response
        以后，清除设备匹配数组之中相应的记录项。默认WDS进程不能退出，如果
        WDS进程退出则QMITTY驱动模块退出。
  输入: tty_struct * tty 指向内核TTY结构的指针
        file * filp指向内核文件结构的指针
  输出: 无
  返回值: 无
-------------------------------------------------------------*/
static void qmi_tty_close(struct tty_struct * tty, struct file * filp)
{
/* <DTS2010112205139.QMITTY: mayang 2010-11-20 Mod BEGIN */
    unsigned short num = 0;
    PQMI_NODE pNode = NULL;

    num = tty->index;
       //balong数据卡分支
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
    /* 在关闭操作时，要先进行文件结点判定，0#结点为WDS进程使用*/
   // if (num == 0)
    {
        //if (0 != signal_start_process[GET_INTERNAL_WDS_CLIENTID])
        {
            int nMsgLen = 0;
            compose_internal_request(pNode, RELEASE_CLIENT_ID_TRANSACTION);

            /*发送消息中的Client ID为内部匹配中记录的Client ID*/
            ((PQMICTL_RELEASE_CLIENT_ID_REQ_MSG)
             //&message_area[pNode->row_num][sdu_start_position])->ClientId =
             (MEG_ROW(pNode->row_num)+ sdu_start_position))->ClientId =
                external_access_name[num].client_id;

            /*同时记录下发送释放Client Id的业务ID在内部匹配记录结构中，用于*
            *收到返馈时，进行比较是哪一个用户进程的Client ID被释放        */
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
  函数原型: int qmi_tty_write(struct tty_struct * tty, int from_user, const unsigned char * buf, int count)
  描述: qmi_tty_write函数是qmitty驱动所定义的tty驱动的最重要的操作
        方法之一。当用户进程用open方法打开设备文件结点时，tty核心
        会调用我们注册的tty驱动的方法集中的 qmi_tty_write 函数，来
        完成把用户空间的数据传入内核。在qmi_tty_write中会把用户输入
        的消息包装成为了一个QMI消息，然后向USB底层发送。
  输入: struct tty_struct * tty  系统传入的指向tty_struct的指针
        int  from_user  指示操作是在用户空间还是在内核空间。
        const unsigned char * buf 指向用户空间的输入数据缓冲区。
        int  count  要输入的数据字节数。
  输出: 无
  返回值: 在USB底层成功传输的字节数减去QMI消息头的长度。
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

    //balong数据卡分支
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
        
        /*要发送QMI消息的用户进程必须已经获得Client ID */
        if (connect_stat && external_access_name[num].client_id == 0)
        {
            return -EFAULT;
        }

        /*得到用户进程发送的消息*/
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
            return (ret - (int)sizeof(QCQMI_HDR));/*返回给用户空间的是SDU，所以减去的QMI消息的头*/
        }
        else
        {
            return ret;
        }
    }
}

/*------------------------------------------------------------
  函数原型: int qmi_tty_write_room(struct tty_struct * tty)
  描述: qmi_tty_write_room是tty驱动核心调用的，当tty驱动执行write
       方法时，tty驱动核心会调用qmi_tty_write_room方法来查询输入
       缓冲区的大小
  输入: 系统传入的指向tty_struct的指针
  输出: 无。
  返回值: 在本工程中返回了一个固定的大小。
-------------------------------------------------------------*/
static int qmi_tty_write_room(struct tty_struct * tty)
{
    return QMI_TTY_ROOM_SIZE;
}

/*------------------------------------------------------------
  函数原型: void qmi_tty_throttle(struct tty_struct * tty)
  描述: qmi_tty_throttle用于当缓冲区满时，通知设备不要输入数据
  输入: 系统传入的指向tty_struct的指针。
  输出: 无。
  返回值: 无。
-------------------------------------------------------------*/
static void qmi_tty_throttle(struct tty_struct * tty)
{
    return;
}

/*------------------------------------------------------------
  函数原型: void qmi_tty_unthrottle(struct tty_struct * tty)
  描述: qmi_tty_unthrottle用于当缓冲区被清除进，通知设备输入数据
  输入: 系统传入的指向tty_struct的指针。
  输出: 无。
  返回值: 无
-------------------------------------------------------------*/
static void qmi_tty_unthrottle(struct tty_struct * tty)
{
    return;
}

/*------------------------------------------------------------
  函数原型: int qmi_tty_ioctl(struct tty_struct * tty, struct file * filp, unsigned int cmd, unsigned long arg)
  描述: qmi_tty_ioctl是重要的用户进程的控制方法接口，当用户进程
        打开了相应的tty设备文件结点时，可以使用ioctl方法来让tty
        驱动执行一些特定的操作。具体执行什么操作由功能码区分。
  输入: struct tty_struct * tty  系统传入的指向tty_struct的指针
        struct file * filp   系统维护的内部的文件指针
        unsigned int cmd  定义的功能码
        unsigned long arg  一般是一个传入的指针
  输出: 由功能码区分
  返回值: 执行特定操作的结果值。
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
            /*用户进程在打开QMITTY设备文件结点以后，都要先用 ioctl(,QMI_TTY_IOCQNUM_CID)来查询是否已获得Client ID*/
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
  函数原型: int write_control_qmi_msg(PQMI_NODE pnode, int count)
  描述: write_control_qmi_msg是通过底层的usb向对端的无线模块发送QMI消息的函数，
        QMI消息通过0端点发送。
  输入: PQMI_NODE pnode 是调用者获得的空闲的消息结点，
        int count 是要发送的SDU长度
  输出: urb提交成功，主机控制器会在底层进行排队
  返回值: 发送成功返回发送消息的长度，失败则把结点重入队同时返回usb传回的错误码
-------------------------------------------------------------*/
static int write_control_qmi_msg(PQMI_NODE pnode, int count)
{
    int ret;

    //balong数据卡分支
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
    else //高通数据卡
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
  函数原型: void write_qmi_completion(struct urb * purb)
  描述: write_qmi_completion是write_control_qmi_msg的完成例程，进行处理
        当提交的urb成功执行了以后的后续工作。
  输入: struct urb * purb提交了的urb
  输出: 把使用的消息结点重入队，释放urb使用的内存
  返回值: 无
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
  函数原型: int read_control_qmi_msg(PQMI_NODE pnode, int count)
  描述: read_control_qmi_msg是从usb底层读取无线模块发送的QMI消息的函数，
  输入: PQMI_NODE pnode 调用方获得的消息结点
        int count 要发送的消息长度
  输出: 成功提交urb,主机控制器在底层对于urb排队
  返回值: 成功执行返回发送了的消息长度值，失败把使用的结点重入队同时返回usb传回的错误码
-------------------------------------------------------------*/
static int read_control_qmi_msg(PQMI_NODE pnode, int count)
{
    int ret = 0;

    //balong数据卡分支
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
  函数原型: void read_qmi_completion(struct urb * purb)
  描述: read_qmi_completion是read_control_qmi_msg的完成例程，完成
        当提交的urb返回以后的后续工作
  输入: struct urb * purb 提交的urb的指针
  输出: 有消息读出则要进行相应的处理，同时要释放urb使用内存
  返回值: 无
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
  函数原型: BOOL create_qmi_interface_process(void)
  描述: create_qmi_interface_process是建立QMITTY模块状态机的重要
       入口函数，建立的过程分为如下步骤：1、首先安状定时器函数，
       用于轮询中断通道的返馈状态；2、开始内部业务过程，这些过程
       有一步失败则删除定时器,同时状态机建立失败，业务过程为：a、
       获取QMUX协议栈的版本信息；b、获取内部的WDS Client ID；c、
       设置要传输的数据格式；d、获取内部的QoS Client ID；e、设置
       QoS上报事件；
  输入: 无
  输出: 成功建立状态机，或者不能建立状态机。
  返回值: BOOL 成功返回TRUE,失败返回FALSE.
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
  函数原型: BOOL internal_process_write(PQMI_NODE ptr, unsigned int flag)
  描述: internal_process_write函数用于对于，QMITTY模块的状态机
       在初始建立过程中，对于必须要完成的业务进行发送并等待消息
       返馈，确保业务被串行的处理。同时为了避免消息一次发送不成
       功和无线模块不能立即回复消息返馈，在发送和等待返馈中都启
       用了等待机制。
  输入: PQMI_NODE ptr 用于发送消息的消息结点。
       unsigned int flag 标识是那种业务。
  输出: 无。
  返回值: BOOL 成功返回TRUE，失败返回FALSE。
-------------------------------------------------------------*/
static BOOL internal_process_write(PQMI_NODE ptr, unsigned int flag)
{
    BOOL ret   = FALSE;
    int retnum = 0;
    int i = 0;
    int nMsgLen = 0;

    nMsgLen = le16_to_cpu(((PQCQMI)MEG_ROW(ptr->row_num))->Length) + 1;/* DTS2010112205139.QMITTY: mayang 2010-11-20 Mod */

    /*循环发送消息3次，有一次成功则返回，3次都不成功则业务失败*/
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
  函数原型: BOOL start_internal_transaction(unsigned int flag)
  描述: start_internal_transaction函数的功能是在初始建立QMITTY状
        态机时，保障发送QMI消息和接收QMI消息返馈可以串行的执行。
        对于发出一条消息，会在规定时间内等待返馈，如果规定时间内
        没有返馈则返回失败。
  输入: unsigned int flag 标识是那个初始阶段的内部业务。
  输出: 无
  返回值: BOOL 成功返回TRUE失败；返回FALSE。
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

    /*循环等待3次，查看信号数组是否被置位，有一次被置位则成功并返回*
     *3次查询都不成功，则失败返回                                   */
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
    // 若不成功，则取消本次业务，释放pndoe
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
  函数原型: void compose_internal_request(PQMI_NODE ptr, unsigned int flag)
  描述: compose_internal_request是针对内部组装QMI消息中有很多共同
        过程，所以单独组成一个函数。根据标志量的不同，在消息结构中
        填充不同的字段值。
  输入: PQMI_NODE ptr 是调用方传入的消息结点指针。
        unsigned int 是标识我们要组装的是哪一条QMI消息。
  输出: 一条被赋值了的QMI消息。
  返回值: 无。
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
  函数原型: static int start_intr_ep_query()
  描述: 启动中断端点轮询
  输入: 无
  输出: 无
  返回值: 成功返回0，否则返回errno
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
  函数原型: void intr_completion(struct urb * purb)
  描述: intr_completion是read_interrupt_qmi_msg的完成例程，
  输入: 提交的urb指针
  输出: 有效的命令通告
  返回值: 无
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
  函数原型: void handle_get_version_response(PQMI_NODE pnode, int ret)
  描述: handle_get_version_response是对于建立QMITTY状态机阶段，对于
        获取QMUX协议栈版本信息反馈的处理。处理的过程主要是要解析
        出所有QMUX协议栈的的版本号。并在解析完成后进行比较确定是否
        是可以支持QOS功能的版本。
  输入: PQMI_NODE pnode 用于获取版本信息的消息结点指针。
        int ret 从USB底层读出的消息长度。
  输出: 解析出的版本信息填充在记录版本信息的数组中。
  返回值: 无。
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

    /* 整个QMUXCTL_MSG消息返馈中value的长度 */
    msglen = le16_to_cpu(pqmi_ver_rsp->Length);
    /* 指向了整个value的首字节 */
    pmsg = (unsigned char *)qmictl_msg + sizeof(QCQMICTL_MSG_HDR);
    
    if (QMI_RESULT_SUCCESS == le16_to_cpu(pqmi_ver_rsp->QMIResult))
    {
        unsigned short i;
        /*减去第一个TYPE LENGTH组合的长度*/
        msglen -= (int)sizeof(QMI_TLV_HDR);
        /*减去Result 和 error code的长度*/
        msglen -= le16_to_cpu(pqmi_ver_rsp->TLVLength);
        /*减去第二个TYPE LENGTH组合的长度*/
        msglen -= (int)sizeof(QMI_TLV_HDR);
        /*减去了返回的NumElements的长度和所有的QMUX版本信息结构中长度*/
        msglen -= le16_to_cpu(pqmi_ver_rsp->TLV2Length);

        /*指针越过了第一个TYPE LENGTH组合*/
        pmsg += (int)sizeof(QMI_TLV_HDR);
        /*指针越过了Result和error code*/
        pmsg += le16_to_cpu(pqmi_ver_rsp->TLVLength);
        /*指针越过了第二个TYPE LENGTH组合*/
        pmsg += (int)sizeof(QMI_TLV_HDR);
        /*指针越过了NumElements的长度和所有的QMUX版本信息结构*/
        pmsg += le16_to_cpu(pqmi_ver_rsp->TLV2Length);

        /* 根据消息的返馈解析出有多少QMUX协议栈版本信息被返回，然后按照个数指针依次移动，得到协议栈主版本信息 */
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
  函数原型: void    handle_get_clientid_response(PQMI_NODE pnode, int ret)
  描述: handle_get_clientid_response函数，用于对于获取Client ID
        的消息返馈进行处理。首先要确定是否消息返馈指示请求是否成
        功，如果成功，则要解析出请求的QMUX服务类型和分配的Client
        ID，并记录在相应的数据结构中，用于后续的使用。
  输入: PQMI_NODE pnode 用于获取Client ID的消息结点指针。
        int ret 从USB底层读出的消息长度。
  输出: 记录在内部使用结构中的Client ID
  返回值: 无。
-------------------------------------------------------------*/
static void    handle_get_clientid_response(PQMI_NODE pnode, int ret)
{
    PQMICTL_MSG qmictl_msg = (PQMICTL_MSG)(MEG_ROW(pnode->row_num)+sdu_start_position);
    unsigned char pqmitype;
    unsigned char pclientid;
    unsigned short i;

    /*解析出response中所请求的服务类型和Client ID              */
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
  函数原型: void    handle_release_clientid_response(PQMI_NODE pnode, int ret)
  描述: handle_release_clientid_response是用于处理，当上层用户进
        程放弃使用设备文件结点后，要释放申请到的WDS Client ID。函
        数首先判定释放操作是否成功，如果释放成功则要清理对应内部
        记录结构中的相应的信息。
  输入: PQMI_NODE pnode 用于释放Client ID的消息结点指针。
        int ret 从USB底层读出的消息长度。
  输出: 清理了相应内部记录结构中的信息。
  返回值: 无。
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
                /* mayang 2011-1-13 如果wds则情况clientid标记 */
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
  函数原型: void process_inbound_ctl_msg(PQMI_NODE pnode, int ret)
  描述: process_inbound_ctl_msg函数用于，对从控制通道读出的CTL
       类型的QMI消息返馈进行处理，函数的内部处理主要是折分转入
       具体的消息返馈中进行处理。
  输入: PQMI_NODE pnode 用于处理控制类消息的消息结点指针。
        int ret 从USB底层读出的消息长度。
  输出: 无。
  返回值: 无。
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
  函数原型: void    send_msg_to_user_space(PQMI_NODE pnode, struct tty_struct * tty, int count)
  描述: send_msg_to_user_space函数是在，当从USB底层成功读入QMI消息
        时，并且经过判别是WDS类型的消息，需要交给WDS进程或其他用户
        进程进行处理的情况，调用tty核心的缓冲区流输入和流刷新函数
        把WDS消息送入用户进程空间。
  输入: PQMI_NODE pnode 读入WDS消息的消息结点.
        struct tty_struct * tty 记录下的对应的用户进程打开的指向的
        tty_struct的指针
        int count 要刷新到用户进程空间的字节数。
  输出: 输出到用户空间的WDS消息。
  返回值: 无。
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

    //balong数据卡走该分支
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
        //balong数据卡走该分支
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
  函数原型: void    broadcast_msg(PQMI_NODE pnode, int count)
  描述: broadcast_msg 是对于是从USB底层读入的需要广播处理的消息，
       处理的算法如下：首先针对每一个存在的上层WDS用户进程，执行
       send_msg_to_user_space，把消息送入用户空间。最后，把消息
       结点插入到QoS_used_list链表中，唤醒usbnet进行处理。
  输入: PQMI_NODE pnode 需要进行处理的消息结点。
        int count 需要刷入用户空间的消息的长度。
  输出: 所有存在的用户进程和usbnet都会接收到广播消息。
  返回值: 无。
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
  函数原型: void  handler_resolve_available_msg(PQMI_NODE tmpnode, int ret)
  描述: handler_resolve_available_msg在读QMI消息的完成例程中被调用，用于对读入
        的QMI消息进行处理
  输入: PQMI_NODE tmpnode 完成例程中要处理的消息结点
        int ret 实际读出的消息长度值
  输出: 对读入的QMI消息进行解析操作
  返回值: 无
-------------------------------------------------------------*/
static void handler_resolve_available_msg(PQMI_NODE tmpnode, int ret)
{
    int i;

    //!balong数据卡分支
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
                        /*缺省的情况下，我们对其他的用户进程client_id进行比较，如果相同则送入相应用户进程空间中*/
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
  函数原型: void    handle_response_available(void)
  描述: handle_response_available是进行QMI消息解析处理的关键函数，
        在中断通道返馈有命令通告后，handle_response_available发
        起从USB控制通道读入QMI消息返馈。在成功读取了消息返馈以
        后，根据消息返馈中的Client Id，进行判别消息该进行那种处
        理方式。目前，本工程中采用的算法时：广播消息转入广播处理；
        Client Id为0则转入CTL类消息处理；是WDS Client Id的把消息
        体送入WDS用户进程空间；是其他用户进程的Client Id则把消息
        送入其他用户进程空间。
  输入: 无。
  输出: 根据不同的Client Id转入不同的处理中。
  返回值: 无。
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
  函数原型: void    init_dev_message_node_and_area(void)
  描述: init_dev_message_node_and_area函数完成，对于QMITTY模块中
        所要用到的，内部信号数组，消息缓冲区，消息结点，用户进程
        匹配数组的初始化。并且初始化完成后，每个消息结点的指针成
        员指向SDU起始位置，消息结点的行成员与消息缓冲区的行坐标
        一一对应，所有的消息结点初始完后都被链入到空闲结点链表中。
  输入: 无
  输出: 完成初始阶段的初始化工作和链表构造工作
  返回值: 无
-------------------------------------------------------------*/
static void init_dev_message_node_and_area(void)
{
    int i;
    //printk(KERN_ALERT "init_dev_message_node_and_area is running!\n");

    qmi_setup_flag = 0;

    /*初始化中断轮询缓冲区*/
    memset((void *)interrupt_query_buf, 0, INTERRUPT_PACKET_SIZE * MSG_ROW);

    /*初始化信号数组*/
    memset((void *) signal_start_process, 0, TRANSACTION_TYPE_MAX);

    /*初始化用户进程匹配数组*/
    for (i = 0; i < QMI_TTY_MINOR; ++i)
    {
        memset((void *)&external_access_name[i], 0, sizeof(tty_memo));
    }

    /*初始化消息缓冲区*/
	memset(message_area, 0, sizeof(*message_area));
	

    /*初始化消息结点*/
    for (i = 0; i < MSG_ROW; ++i)
    {
        memset((void *) &message_node[i], 0, sizeof(QMI_NODE));
    }

}

/* <DTS2010112205139.QMITTY: mayang 2010-11-20 ADD BEGIN */
/*------------------------------------------------------------
  函数原型: PQMI_NODE   get_free_node()
  描述: 任何需要读取和发送QMI消息的操作，都必须先获取一个消息结
        点，通过对于消息结点所指向的消息缓冲区，进行QMI消息的装
        配、或者是对消息进行解析。
  输入: 无
  输出: 无
  返回值: 成功获取，则返回成功获取的消息结点的指针
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
  函数原型: void return_node_to_free(PQMI_NODE pnode)
  描述: return_read_node_to_free函数，把已经完成读消息处理的消息
        结点，首先从读链表中删除，然后对相应消息缓冲区清零，最后
        把消息结点归还到空闲链表中。
  输入: PQMI_NODE pnode等待归还的消息结点。
  输出: 成功执行，结点归还到空闲链表中
  返回值: 无
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
  函数原型: void    clean_node_and_area(PQMI_NODE pnode)
  描述: clean_node_and_area函数对于，完成了申请读、写的消息结点
        对于相对应的消息缓冲区进行清零操作。操作的主要步骤为：1、
        初始化每一个QMI_NODE中的list_head；2、对消息缓冲区按行坐
        标清零。
  输入: PQMI_NODE pnode等待执行清零操作消息结点。
  输出: 对于相对应的消息缓冲区进行清零。
  返回值: 无
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
  函数原型: static int tty_module_init(void)
  描述: tty_module_init是QMITTY模块的，模块初始化函数，是模块装
        入的入口点。
  输入: 无
  输出: 无
  返回值: 无
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
  函数原型: static void tty_module_exit(void)
  描述: tty_module_exit是QMITTY模块的，模块的退出函数
  输入: 无
  输出: 无
  返回值: 无
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


