/******************************************************************************
  文件名称: smp.c
  功能描述: SMP多核管理接口
  版本描述: V1.0

  创建日期: D2013_10_25
  创建作者: zhouyu 00204772

  修改记录: 
            生成初稿.
******************************************************************************/
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/smp.h>
#include <linux/io.h>

#include <asm/cacheflush.h>
#include <mach/hardware.h>
#include <asm/hardware/gic.h>
#include <asm/mach-types.h>
#include <mach/typedef.h>
#include <mach/early-debug.h>
#include <mach/scu.h>


extern hi_void sd56xx_secondary_startup(hi_void);
extern hi_void __iomem *gic_cpu_base_addr;

static DEFINE_SPINLOCK(boot_lock);

/*
  * control for which core is the next to come out of the secondary
  * boot "holding pen"
  */
hi_int32 __cpuinitdata pen_release = -1;

/******************************************************************************
  函数功能:  获取A9 peripheral的总线映射地址,用于配置SCU
  输入参数:  无
  输出参数:  无
  函数返回:  SCU基地址
  函数备注:  
******************************************************************************/
static hi_void __iomem *smp_get_scu_base_addr(hi_void)
{
    return __io_address(REG_BASE_A9_PERI + REG_A9_PERI_SCU);
}

/******************************************************************************
  函数功能:  获取当前核的个数
  输入参数:  无
  输出参数:  无
  函数返回:  
  函数备注:  
******************************************************************************/
static hi_uint32 __init smp_get_core_num(hi_void)
{
    hi_uint32 ui_core_num;
    
    hi_void __iomem *pv_scu_base = smp_get_scu_base_addr();

    if (pv_scu_base)
    {
        ui_core_num = __raw_readl(pv_scu_base + SCU_CONFIG);
        ui_core_num = (ui_core_num & 0x03) + 1;
    } 
    else
    {
        ui_core_num = 1;
    }

    return ui_core_num;
}

/******************************************************************************
  函数功能:  SCU使能
  输入参数:  无
  输出参数:  无
  函数返回:  
  函数备注:  
******************************************************************************/
static hi_void scu_enable(hi_void)
{
    hi_uint32 ui_scu_ctrl;
    
    void __iomem *pv_scu_base = smp_get_scu_base_addr();

    ui_scu_ctrl = __raw_readl(pv_scu_base + SCU_CTRL);
    ui_scu_ctrl |= 1;
    
    __raw_writel(ui_scu_ctrl, pv_scu_base + SCU_CTRL);
}

/******************************************************************************
  函数功能:  Perform platform specific initialisation of the specified CPU.
  输入参数:  ui_cpu : CPU号
  输出参数:  无
  函数返回:  
  函数备注:  
******************************************************************************/
hi_void platform_secondary_init(hi_uint32 ui_cpu)
{
    trace_hardirqs_off();
    
    edb_trace(1);

    /*
         * if any interrupts are already enabled for the primary
         * core (e.g. timer irq), then they will not have been enabled
         * for us: do so
         */
    gic_cpu_init(0, gic_cpu_base_addr);

    /*
         * let the primary processor know we're out of the
         * pen, then head off into the C entry point
         */
    pen_release = -1;
    smp_wmb();

    /*
         * Synchronise with the boot thread.
         */
    spin_lock(&boot_lock);
    spin_unlock(&boot_lock);
}

/******************************************************************************
  函数功能:  Boot a secondary CPU, and assign it the specified idle task.
                             This also gives us the initial stack to use for this CPU
  输入参数:  ui_cpu : CPU号,pst_task : task (cpu idle wait for sched)
  输出参数:  无
  函数返回:  
  函数备注:  
******************************************************************************/
hi_int32 boot_secondary(hi_uint32 ui_cpu, struct task_struct *pst_task)
{
    hi_uint32 ui_timeout;
    
    edb_trace(1);

    /*
         * set synchronisation state between this boot processor
         * and the secondary one
         */
    spin_lock(&boot_lock);

    /*
         * The secondary processor is waiting to be released from
         * the holding pen - release it, then wait for it to flag
         * that it has been released by resetting pen_release.
         *
         * Note that "pen_release" is the hardware CPU ID, whereas
         * "cpu" is Linux's internal ID.
         */
    pen_release = ui_cpu;
    flush_cache_all();

    /*
         * XXX
         *
         * This is a later addition to the booting protocol: the
         * bootMonitor now puts secondary cores into WFI, so
         * poke_milo() no longer gets the cores moving; we need
         * to send a soft interrupt to wake the secondary core.
         * Use smp_cross_call() for this, since there's little
         * point duplicating the code here
         */
    smp_cross_call(cpumask_of(ui_cpu));

    /* wait for platform_secondary_init to be called, and set pen_release to -1 */
    ui_timeout = jiffies + (1 * HZ);
    while (time_before(jiffies, ui_timeout))
    {
        smp_rmb();
        if (pen_release == -1)
        {
            break;
        }

        udelay(10);
    }

    /*
         * now the secondary core is starting up let it run its
         * calibrations, then wait for it to finish
         */
    spin_unlock(&boot_lock);

    return pen_release != -1 ? -ENOSYS : 0;
}

/******************************************************************************
  函数功能:  write the address of secondary startup into the system-wide flags register
  输入参数:  无
  输出参数:  无
  函数返回:  
  函数备注:  
******************************************************************************/
static hi_void __init poke_milo(hi_void)
{
    extern hi_void secondary_startup(hi_void);
    
    /* nobody is to be released from the pen yet */
    pen_release = -1;
    edb_trace(1);

    /*
         * 将core1的入口地址写入软件测试寄存器1,
         * core1在boot阶段挂住等待该地址值非0后将PC指向该值.
         */
    __raw_writel(virt_to_phys(sd56xx_secondary_startup), __io_address(REG_BASE_SCTL + REG_SC_SC_TESTREG1));

    mb();
}

/******************************************************************************
  函数功能:  Initialise the CPU possible map early - this describes the CPUs
                             which may be present or become present in the system
  输入参数:  无
  输出参数:  无
  函数返回:  
  函数备注:  
******************************************************************************/
hi_void __init smp_init_cpus(hi_void)
{
    hi_uint32 i;
    hi_uint32 ui_ncores = smp_get_core_num();
    
    edb_trace(1);

    for (i = 0; i < ui_ncores; i++)
    {
        cpu_set(i, cpu_possible_map);
    }
}

/******************************************************************************
  函数功能:  Prepare machine for booting other CPUs
  输入参数:  无
  输出参数:  无
  函数返回:  
  函数备注:  
******************************************************************************/
hi_void __init smp_prepare_cpus(hi_uint32 ui_max_cpus)
{
    hi_int32  i;
    hi_uint32 ui_cpu    = smp_processor_id();
    hi_uint32 ui_ncores = smp_get_core_num();
    
    edb_trace(1);

    /* sanity check */
    if (ui_ncores == 0)
    {
        printk(KERN_ERR "Realview: strange CM count of 0? Default to 1\n");

        ui_ncores = 1;
    }

    if (ui_ncores > NR_CPUS)
    {
        printk(KERN_WARNING
               "Realview: no. of cores (%d) greater than configured "
               "maximum of %d - clipping\n",
               ui_ncores, NR_CPUS);

        ui_ncores = NR_CPUS;
    }

    smp_store_cpu_info(ui_cpu);

    /*
         * are we trying to boot more cores than exist?
         */
    if (ui_max_cpus > ui_ncores)
    {
        ui_max_cpus = ui_ncores;
    }

#if defined(CONFIG_LOCAL_TIMERS) || defined(CONFIG_GENERIC_CLOCKEVENTS_BROADCAST)
    /*
         * Enable the local timer or broadcast device for the boot CPU.
         */
    local_timer_setup();
#endif

    /*
         * Initialise the present map, which describes the set of CPUs
         * actually populated at the present time.
         */
	for (i = 0; i < ui_max_cpus; i++)
	{
	    cpu_set(i, cpu_present_map);
	}

    /*
         * Initialise the SCU if there are more than one CPU and let
         * them know where to start. Note that, on modern versions of
         * MILO, the "poke" doesn't actually do anything until each
         * individual core is sent a soft interrupt to get it out of
         * WFI
         */
    if (ui_max_cpus > 1)
    {
        scu_enable();
        poke_milo();
    }
}

