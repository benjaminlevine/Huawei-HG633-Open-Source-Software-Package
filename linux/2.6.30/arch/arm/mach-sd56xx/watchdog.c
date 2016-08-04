/******************************************************************************
  文件名称: watchdog.c
  功能描述: 硬件看门狗驱动
  版本描述: V1.0

  创建日期: D2011_09_30
  创建作者: zhouyu 00204772

  修改记录: 
            生成初稿.
******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mach/irq.h>

#include <mach/typedef.h>
#include <mach/watchdog.h>
#include <mach/early-debug.h>
#include <mach/time.h>
#include "clock.h"


/* global crg reg define */
static hi_crg_reg_s *g_pst_crg_reg = HI_NULL;
static unsigned int *g_pst_reset_reason_reg = HI_NULL;

/* software wdg timer */
static struct timer_list g_st_wdg_timer0;
static struct timer_list g_st_wdg_timer1;

/* 最近一次喂狗时间 */
static hi_uint32 g_ui_last_sw_feed_time_cpu0 = 0;
static hi_uint32 g_ui_last_sw_feed_time_cpu1 = 0;

static hi_uint32 g_ui_last_feed_cpu = 1;


/* 记录启动清狗标志*/
static hi_uint32 g_ui_state = 0;


static spinlock_t wdg_lock = SPIN_LOCK_UNLOCKED;
static irqreturn_t  hi_kernel_wdg_cpu0_interrupt(int irq, void *dev_id);
static irqreturn_t  hi_kernel_wdg_cpu1_interrupt(int irq, void *dev_id);
static hi_void hi_kernel_sw_wdg_timer0_expire(hi_void);
static hi_void hi_kernel_sw_wdg_timer1_expire(hi_void);
hi_uint32 hi_kernel_sw_wdg_check_cpu0(hi_void);
hi_uint32 hi_kernel_sw_wdg_check_cpu1(hi_void);



/* 绑定硬中断到指定CPU */
static hi_int32 hi_kernel_irq_bind_cpu(hi_uint32 ui_irq, hi_uint32 ui_cpu)
{
   struct cpumask mask;

   cpumask_clear(&mask);
   cpumask_set_cpu(ui_cpu, &mask);
   
   return irq_set_affinity(ui_irq, &mask);
}

typedef void (*watchdog_reboot_callback)(void);

watchdog_reboot_callback g_pv_watchdog_reboot_callback = NULL;

void watchdog_reboot_reg(watchdog_reboot_callback pv_callback)
{
    g_pv_watchdog_reboot_callback = pv_callback;

    printk("REGISTER g_pv_watchdog_reboot_callback.\n");
}
EXPORT_SYMBOL(watchdog_reboot_reg); 

static struct irqaction sd56xx_watchdog_cpu0_irq =
{
    .name       = "watchdog_0",
    .flags      = IRQF_SHARED | IRQF_TIMER,
    .handler    = hi_kernel_wdg_cpu0_interrupt,
};

static struct irqaction sd56xx_watchdog_cpu1_irq =
{
    .name       = "watchdog_1",
    .flags      = IRQF_SHARED | IRQF_TIMER,
    .handler    = hi_kernel_wdg_cpu1_interrupt,
};


/******************************************************************************
  函数功能:  硬件看门狗开关
  输入参数:  ui_en : 1 : enable  0: disable
  输出参数:  无
  函数返回:  无
  函数备注:  依次写入0xABCD5610---->0xED574447，则关闭看门狗
                             依次写入0x4F4E5452---->0x12345610，则使能看门狗
******************************************************************************/
hi_void hi_kernel_wdg_en(hi_uint32 ui_en)
{
    if(ui_en)
    {
        g_ui_state = 1;
        
        HI_REG_WRITE(&g_pst_crg_reg->ui_crg_sc_perctrl8, 0x4F4E5452);
        HI_REG_WRITE(&g_pst_crg_reg->ui_crg_sc_perctrl8, 0x12345610);
    }
    else
    {
        g_ui_state = 0;
        
        HI_REG_WRITE(&g_pst_crg_reg->ui_crg_sc_perctrl8, 0xABCD5610);
        HI_REG_WRITE(&g_pst_crg_reg->ui_crg_sc_perctrl8, 0xED574447);
    }
}

/******************************************************************************
  函数功能:  看门狗溢出时间配置
  输入参数:  ui_time : 溢出时间
  输出参数:  无
  函数返回:  无
  函数备注:  看门狗复位时间配置，配置值为n，则表示看门狗
                             的溢出周期为n+1秒。
******************************************************************************/
hi_void hi_kernel_wdg_set_reset_time(hi_uint32 ui_time)
{
    hi_crg_sc_perctrl4_u un_reg_crg_sc_perctrl4;

    /*检查ui_time范围*/
    if ((ui_time < HI_WDG_MIN_INTERVAL) || (ui_time > HI_WDG_MAX_INTERVAL))
    {
        return;
    }

    HI_REG_READ (&g_pst_crg_reg->ui_crg_sc_perctrl4, un_reg_crg_sc_perctrl4.ui_value);
    un_reg_crg_sc_perctrl4.st_bits.ui_wdg_rst_time_cfg = ui_time-1;
    
    HI_REG_WRITE(&g_pst_crg_reg->ui_crg_sc_perctrl4, un_reg_crg_sc_perctrl4.ui_value);
}

/******************************************************************************
  函数功能:  硬件看门狗清零(喂狗)
  输入参数:  无
  输出参数:  无
  函数返回:  无
  函数备注:  写入0x55AA_5A5A之后再写入0xAA55_A5A5，即完成对硬件
                             看门狗的一次喂狗操作
******************************************************************************/
hi_void hi_kernel_wdg_clear(hi_void)
{
    HI_REG_WRITE(&g_pst_crg_reg->ui_crg_wdg_init_cfg, 0x55AA5A5A);
    HI_REG_WRITE(&g_pst_crg_reg->ui_crg_wdg_init_cfg, 0xAA55A5A5);
}

static irqreturn_t  hi_kernel_wdg_cpu0_interrupt(int irq, void *dev_id)
{
    static unsigned char uc_print0 = 0;
    static unsigned char uc_print0_ = 0;
    static unsigned char stimer0Init = 0;
    static unsigned char uc_dog_not_feed_times = 0;//6次没有喂狗系统重启。
    if (0 == stimer0Init)    
    {
        init_timer(&g_st_wdg_timer0);
        g_st_wdg_timer0.data     = 0x0;
        g_st_wdg_timer0.function = hi_kernel_sw_wdg_timer0_expire;
        g_st_wdg_timer0.expires  = jiffies + (HI_WDG_DFT_FEED_TIME * HZ);
        add_timer(&g_st_wdg_timer0);      
        g_ui_last_sw_feed_time_cpu0 = jiffies;
        stimer0Init++;
    }

    if ( ( readl(CFG_TIMER_VABASE+REG_TIMER1_RIS) ) & 0x1 )
    {
        //printk("hi_kernel_wdg_cpu0_interrupt\n");
        
        spin_lock(&wdg_lock);

        if ( ( 1 != g_ui_last_feed_cpu ) && ( 0 == uc_print0 ) )    
        {
            if(uc_dog_not_feed_times >= 6)
            {
                printk("CPU1 dying...\n");
                if (g_pv_watchdog_reboot_callback != NULL)
                {
                    g_pv_watchdog_reboot_callback();
                }			
                uc_print0++;
                dump_stack();
            }
            else
            {
                uc_dog_not_feed_times ++;    
                printk("CPU1 dog %d not feed...\n", uc_dog_not_feed_times);
            }
        }
        else
        {
            uc_dog_not_feed_times = 0;
        }
        
        HI_REG_WRITE(&g_pst_crg_reg->ui_crg_wdg_init_cfg, 0x55AA5A5A);

        g_ui_last_feed_cpu = 0;

        if ((0 !=  hi_kernel_sw_wdg_check_cpu0())  && ( 0 == uc_print0_ ) )
        {
            printk("CPU0 Softirq Dying...\n");
            if (g_pv_watchdog_reboot_callback != NULL)
            {
                g_pv_watchdog_reboot_callback();
            }			
            uc_print0_++;
            dump_stack();
        }
                
        spin_unlock(&wdg_lock);


        writel(~0, CFG_TIMER_VABASE + REG_TIMER1_INTCLR);

    }

    return IRQ_HANDLED;
    
}

static irqreturn_t  hi_kernel_wdg_cpu1_interrupt(int irq, void *dev_id)
{
    static unsigned char uc_print1 = 0;
    static unsigned char uc_print1_ = 0;
    static unsigned char stimer1Init = 0;
    static unsigned char uc_dog_not_feed_times = 0;//6次没有喂狗系统重启。

    if (0 == stimer1Init)    
    {
        init_timer(&g_st_wdg_timer1);
        g_st_wdg_timer1.data     = 0x0;
        g_st_wdg_timer1.function = hi_kernel_sw_wdg_timer1_expire;
        g_st_wdg_timer1.expires  = jiffies + (HI_WDG_DFT_FEED_TIME * HZ);
        add_timer(&g_st_wdg_timer1);        
        g_ui_last_sw_feed_time_cpu1 = jiffies;
        stimer1Init++;
    }

    if((readl(CFG_TIMER2_VABASE+REG_TIMER1_RIS))&0x1)
    {

        //printk("hi_kernel_wdg_cpu1_interrupt\n");
        
        spin_lock(&wdg_lock);

        if ( ( 0 != g_ui_last_feed_cpu ) && ( 0 == uc_print1 ) )    
        {
            if(uc_dog_not_feed_times >= 6)
            {
                printk("CPU0 dying...\n");
                if (g_pv_watchdog_reboot_callback != NULL)
                {
                    g_pv_watchdog_reboot_callback();
                }					
                uc_print1++;
                dump_stack();
            }
            else
            {
                uc_dog_not_feed_times ++;    
                printk("CPU0 dog %d not feed...\n", uc_dog_not_feed_times);
            }
        }
        else
        {
            uc_dog_not_feed_times = 0;
        }
        
        HI_REG_WRITE(&g_pst_crg_reg->ui_crg_wdg_init_cfg, 0xAA55A5A5);
        
        g_ui_last_feed_cpu = 1;

        if ((0 !=  hi_kernel_sw_wdg_check_cpu1())  && ( 0 == uc_print1_ ) )
        {
            printk("CPU1 Softirq Dying...\n");
            if (g_pv_watchdog_reboot_callback != NULL)
            {
                g_pv_watchdog_reboot_callback();
            }			
            uc_print1_++;
            dump_stack();
        }
        
        spin_unlock(&wdg_lock);

        writel(~0, CFG_TIMER2_VABASE + REG_TIMER1_INTCLR);
    }

    return IRQ_HANDLED;
}


/* 检查是否正常喂狗 */
hi_uint32 hi_kernel_sw_wdg_check_cpu0(hi_void)
{
    hi_uint32 ui_inter = jiffies - g_ui_last_sw_feed_time_cpu0;
    hi_uint32 ui_dev   = (HI_WDG_DFT_RST_TIME - HI_WDG_DFT_FEED_TIME) * HZ;

    return ( ui_inter > ui_dev ) ? 1 : 0;

}

hi_uint32 hi_kernel_sw_wdg_check_cpu1(hi_void)
{
    hi_uint32 ui_inter = jiffies - g_ui_last_sw_feed_time_cpu1;
    hi_uint32 ui_dev   = (HI_WDG_DFT_RST_TIME - HI_WDG_DFT_FEED_TIME) * HZ;

    return ( ui_inter > ui_dev ) ? 1 : 0;
}


static hi_void hi_kernel_sw_wdg_timer0_expire(hi_void)
{
    g_st_wdg_timer0.expires  = jiffies + ( HI_WDG_DFT_FEED_TIME * HZ );
    add_timer(&g_st_wdg_timer0);
    g_ui_last_sw_feed_time_cpu0 = jiffies;
}

static hi_void hi_kernel_sw_wdg_timer1_expire(hi_void)
{
 
    g_st_wdg_timer1.expires  = jiffies + ( HI_WDG_DFT_FEED_TIME * HZ );
    add_timer(&g_st_wdg_timer1);
    g_ui_last_sw_feed_time_cpu1 = jiffies;
}

hi_void hi_kernel_wdg_init(hi_void)
{
	unsigned int reboot_reason = 0;
    unsigned long busclk;
    
    busclk = get_apbclk_hw();
    writel(0,                                      CFG_TIMER_VABASE + REG_TIMER1_CONTROL);
    writel((150*BUSCLK_TO_TIMER_RELOAD(busclk)),         CFG_TIMER_VABASE + REG_TIMER1_RELOAD );
    writel((300*BUSCLK_TO_TIMER_RELOAD(busclk)),         CFG_TIMER_VABASE + REG_TIMER1_BGLOAD  );
    writel(CFG_TIMER_CONTROL,                      CFG_TIMER_VABASE + REG_TIMER1_CONTROL);

    writel(0,                                      CFG_TIMER2_VABASE + REG_TIMER1_CONTROL);
    writel((300*BUSCLK_TO_TIMER_RELOAD(busclk)),         CFG_TIMER2_VABASE + REG_TIMER1_RELOAD );
    writel((300*BUSCLK_TO_TIMER_RELOAD(busclk)),         CFG_TIMER2_VABASE + REG_TIMER1_BGLOAD  ); //这里延后1秒，防止冲突
    writel(CFG_TIMER_CONTROL,                      CFG_TIMER2_VABASE + REG_TIMER1_CONTROL);

    setup_irq(INTNR_TIMER_0_1, &sd56xx_watchdog_cpu0_irq);

    hi_kernel_irq_bind_cpu(INTNR_TIMER_2_3, 1);
    setup_irq(INTNR_TIMER_2_3, &sd56xx_watchdog_cpu1_irq);    
    /*物理地址映射到虚拟内存*/
    g_pst_crg_reg = ioremap(HI_REG_BASE_CRG, sizeof(hi_crg_reg_s));
    if ( NULL == g_pst_crg_reg )
    {
        printk("ioremap error: g_pst_crg_reg = NULL");
        return;
    }
    g_ui_state = 1;    
    hi_kernel_wdg_set_reset_time(HI_WDG_DFT_RST_TIME);
    hi_kernel_wdg_clear();
 

    printk("kernel wdtchdog init sucessful!\n");

	g_pst_reset_reason_reg = ioremap(0x14880090, 4);
    if ( NULL == g_pst_reset_reason_reg )
    {
        printk("ioremap error: g_pst_reset_reason_reg = NULL");
        return -1;
    }
	
	reboot_reason = ((*g_pst_reset_reason_reg)>>14) & 0x3;
	if ( 0x0 == reboot_reason)
	{
		printk("Last reboot is power up boot [%d]\n", reboot_reason);
	}
	else if ( 0x1 == reboot_reason)
	{
		printk("Last reboot is soft reboot [%d]\n", reboot_reason);
	}
	else if ( 0x2 == reboot_reason)
	{
		printk("Last reboot is watchdog reboot [%d]\n", reboot_reason);
	}	
	else
	{
		printk("Last reboot is unknown reason [%d]\n", reboot_reason);
	}

}

EXPORT_SYMBOL(hi_kernel_wdg_en);
EXPORT_SYMBOL(hi_kernel_wdg_set_reset_time);
EXPORT_SYMBOL(hi_kernel_wdg_clear);

pure_initcall(hi_kernel_wdg_init);

