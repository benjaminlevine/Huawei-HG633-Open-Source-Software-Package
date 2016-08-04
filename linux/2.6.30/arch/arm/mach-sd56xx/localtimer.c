/******************************************************************************
  文件名称: localtimer.c
  功能描述: 
  版本描述: V1.0

  创建日期: D2013_10_25
  创建作者: zhouyu 00204772

  修改记录: 
            生成初稿.
******************************************************************************/
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/smp.h>
#include <linux/jiffies.h>
#include <linux/percpu.h>
#include <linux/clockchips.h>
#include <linux/irq.h>
#include <linux/io.h>
    
#include <asm/hardware/arm_twd.h>
#include <asm/hardware/gic.h>
#include <mach/hardware.h>
#include <asm/irq.h>

#include <mach/typedef.h>


/* set up by the platform code */
extern hi_void __iomem *g_pv_twd_base;

static hi_uint32 g_ui_mpcore_timer_rate;

static DEFINE_PER_CPU(struct clock_event_device, local_clockevent);

/******************************************************************************
  函数功能:  local_timer_set_mode
  输入参数:  无
  输出参数:  无
  函数返回:  
  函数备注:  
******************************************************************************/
static hi_void local_timer_set_mode(enum clock_event_mode em_mode, struct clock_event_device *pst_clk)
{
    hi_uint32 ui_ctrl;

    switch(em_mode)
    {
        case CLOCK_EVT_MODE_PERIODIC:
            /* timer load already set up */
            ui_ctrl = TWD_TIMER_CONTROL_ENABLE | TWD_TIMER_CONTROL_IT_ENABLE | TWD_TIMER_CONTROL_PERIODIC;
            break;
            
        case CLOCK_EVT_MODE_ONESHOT:
            /* period set, and timer enabled in 'next_event' hook */
            ui_ctrl = TWD_TIMER_CONTROL_IT_ENABLE | TWD_TIMER_CONTROL_ONESHOT;
            break;
            
        case CLOCK_EVT_MODE_UNUSED:
        case CLOCK_EVT_MODE_SHUTDOWN:
        default:
            ui_ctrl = 0;
    }

    __raw_writel(ui_ctrl, g_pv_twd_base + TWD_TIMER_CONTROL);
}

/******************************************************************************
  函数功能:  local_timer_set_next_event
  输入参数:  无
  输出参数:  无
  函数返回:  
  函数备注:  
******************************************************************************/
static hi_int32 local_timer_set_next_event(hi_uint32 ui_evt, struct clock_event_device *pst_unused)
{
    hi_uint32 ui_ctrl = __raw_readl(g_pv_twd_base + TWD_TIMER_CONTROL);

    __raw_writel(ui_evt, g_pv_twd_base + TWD_TIMER_COUNTER);
    __raw_writel(ui_ctrl | TWD_TIMER_CONTROL_ENABLE, g_pv_twd_base + TWD_TIMER_CONTROL);

    return 0;
}

/******************************************************************************
  函数功能:  checks for a local timer interrupt
  输入参数:  无
  输出参数:  无
  函数返回:  
  函数备注:  If a local timer interrupt has occurred, acknowledge and return 1. Otherwise, return 0
******************************************************************************/
static hi_void __cpuinit twd_calibrate_rate(hi_void)
{
    hi_uint32 ui_load;
    hi_uint32 ui_count;
    hi_uint64 ull_waitjiffies;

    /*
         * If this is the first time round, we need to work out how fast
         * the timer ticks
         */
    if (g_ui_mpcore_timer_rate == 0)
    {
        printk("Calibrating local timer... ");

        /* Wait for a tick to start */
        ull_waitjiffies = get_jiffies_64() + 1;

        while(get_jiffies_64() < ull_waitjiffies)
        {
            udelay(10);
        }

        /* OK, now the tick has started, let's get the timer going */
        ull_waitjiffies += 5;

        /* enable, no interrupt or reload */
        __raw_writel(0x1, g_pv_twd_base + TWD_TIMER_CONTROL);

        /* maximum value */
        __raw_writel(0xFFFFFFFFU, g_pv_twd_base + TWD_TIMER_COUNTER);

        while (get_jiffies_64() < ull_waitjiffies)
        {
            udelay(10);
        }

        ui_count = __raw_readl(g_pv_twd_base + TWD_TIMER_COUNTER);

        g_ui_mpcore_timer_rate = (0xFFFFFFFFU - ui_count) * (HZ / 5);

        printk("%lu.%02luMHz.\n", g_ui_mpcore_timer_rate / 1000000, (g_ui_mpcore_timer_rate / 100000) % 100);
    }

    ui_load = g_ui_mpcore_timer_rate / HZ;

    __raw_writel(ui_load, g_pv_twd_base + TWD_TIMER_LOAD);
}

/******************************************************************************
  函数功能:  Used on SMP for either the local timer or IPI_TIMER
  输入参数:  无
  输出参数:  无
  函数返回:  
  函数备注:  
******************************************************************************/
hi_void local_timer_interrupt(hi_void)
{
    struct clock_event_device *pst_clk = &__get_cpu_var(local_clockevent);

    pst_clk->event_handler(pst_clk);
}

/******************************************************************************
  函数功能:  local_timer_ack
  输入参数:  无
  输出参数:  无
  函数返回:  
  函数备注:  
******************************************************************************/
hi_int32 local_timer_ack(hi_void)
{
    if (__raw_readl(g_pv_twd_base + TWD_TIMER_INTSTAT))
    {
        __raw_writel(1, g_pv_twd_base + TWD_TIMER_INTSTAT);
        return 1;
    }

    return 0;
}

/******************************************************************************
  函数功能:  Setup the local clock events for a CPU
  输入参数:  无
  输出参数:  无
  函数返回:  
  函数备注:  
******************************************************************************/
hi_void __cpuinit local_timer_setup(hi_void)
{
    hi_uint32 ui_flags;
    hi_uint32 ui_cpu = smp_processor_id();
    struct clock_event_device *pst_clk = &per_cpu(local_clockevent, ui_cpu);

    twd_calibrate_rate();

    pst_clk->name           = "local_timer";
    pst_clk->features       = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT;
    pst_clk->rating         = 350;
    pst_clk->set_mode       = local_timer_set_mode;
    pst_clk->set_next_event = local_timer_set_next_event;
    pst_clk->irq            = IRQ_LOCALTIMER;
    pst_clk->cpumask        = cpumask_of(ui_cpu);
    pst_clk->shift          = 20;
    pst_clk->mult           = div_sc(g_ui_mpcore_timer_rate, NSEC_PER_SEC, pst_clk->shift);
    pst_clk->max_delta_ns   = clockevent_delta2ns(0xffffffff, pst_clk);
    pst_clk->min_delta_ns   = clockevent_delta2ns(0xf, pst_clk);

    /* Make sure our local interrupt controller has this enabled */
    local_irq_save(ui_flags);
    get_irq_chip(IRQ_LOCALTIMER)->unmask(IRQ_LOCALTIMER);
    local_irq_restore(ui_flags);

    clockevents_register_device(pst_clk);
}

/******************************************************************************
  函数功能:  take a local timer down
  输入参数:  无
  输出参数:  无
  函数返回:  
  函数备注:  
******************************************************************************/
hi_void __cpuexit local_timer_stop(hi_void)
{
    __raw_writel(0, g_pv_twd_base + TWD_TIMER_CONTROL);
}


