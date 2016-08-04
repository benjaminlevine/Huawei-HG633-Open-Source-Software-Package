/*
 * Author   : Ryan Shen ( s00152899 )
 * Contract : shentao@huawei.com
 * File     : core.c
 * Describe : This is core code for Linux Porting.           
 * Platform : ARM Cortex-A9 & Linux 2.6.34.10
 * Log      : 2011-09-25 : init version completed.
 */
#include <linux/init.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/sysdev.h>
#include <linux/interrupt.h>
#include <linux/amba/bus.h>
#include <linux/amba/clcd.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/cnt32_to_63.h>
#include <linux/io.h>

#include <asm/clkdev.h>
#include <asm/system.h>
#include <asm/irq.h>
#include <asm/leds.h>
#include <asm/hardware/arm_timer.h>
#include <asm/hardware/gic.h>
#include <asm/hardware/vic.h>
#include <asm/mach-types.h>

#include <asm/mach/arch.h>
#include <asm/mach/flash.h>
#include <asm/mach/irq.h>
#include <asm/mach/time.h>
#include <asm/mach/map.h>

#include <mach/typedef.h>
#include <mach/time.h>
#include <mach/hardware.h>
#include <mach/early-debug.h>
#include <mach/irqs.h>
#include "clock.h"
#include <linux/bootmem.h>
#include <mach/kexport.h>
#include <mach/watchdog.h>

/* global variable */
void __iomem *gic_cpu_base_addr;
static unsigned long sd56xx_timer_reload,timer0_clk_hz,timer1_clk_hz,timer0_clk_khz,timer1_clk_khz;

hi_void __iomem *g_pv_twd_base;

extern void l2cache_init(void);
extern void l2cache_exit(void);
extern void l2cache_clean(void);
extern void hi_acp_init(void);
static HI_CHIP_ID_E get_chip_id(void);
static HI_BOOT_DEV_E get_boot_sel(void);

HI_CHIP_ID_E sd56xx_chip_id=HI_CHIP_ID_NONE_E;
HI_BOOT_DEV_E sd56xx_boot_sel=HI_BOOT_ROM;

#define HI_CHIP_ID_H           0x56100100
#define HI_CHIP_ID_T           0x56102110
#define HI_CHIP_ID_MASK        0xFFFFF000

/*
 * get apb clock and set sys_timer value.
 */
static void early_init(void)
{
    unsigned long busclk;
    
    edb_trace(1);
    busclk = get_apbclk_hw();
    printk("sd56xx apb bus clk is %lu\n",busclk);

    sd56xx_timer_reload = BUSCLK_TO_TIMER_RELOAD(busclk);
    timer0_clk_hz       = BUSCLK_TO_TIMER0_CLK_HZ(busclk);
    timer0_clk_khz      = BUSCLK_TO_TIMER0_CLK_KHZ(busclk);
    timer1_clk_hz       = BUSCLK_TO_TIMER1_CLK_HZ(busclk);
    timer1_clk_khz      = BUSCLK_TO_TIMER1_CLK_KHZ(busclk);
}

/*
 * init gic.
 */
void __init sd56xx_gic_init_irq(void)
{
    edb_trace(1);
    gic_cpu_base_addr = (void __iomem *)CFG_GIC_CPU_BASE;
    gic_dist_init(0,(void __iomem *)CFG_GIC_DIST_BASE, IRQ_LOCALTIMER);
    gic_cpu_init(0,(void __iomem *)CFG_GIC_CPU_BASE);
}

/*
 * cpu idle.
 */
void arch_idle(void)
{
    /*
     * This should do all the clock switching
     * and wait for interrupt tricks
     */
    cpu_do_idle();
}

typedef void (*sd56xx_reboot_callback)(void);

sd56xx_reboot_callback g_pv_sd56xx_reboot_callback = NULL;

void sd56xx_reboot_reg(sd56xx_reboot_callback pv_callback)
{
    printk("REGISTER g_pv_sd56xx_reboot_callback.\n");
    
    g_pv_sd56xx_reboot_callback = pv_callback;
}
EXPORT_SYMBOL(sd56xx_reboot_reg);

/*  
 * cpu reset.
 */
void arch_reset(char mode,const char *cmd)
{   
    printk("RESET!\n");

    if (g_pv_sd56xx_reboot_callback != NULL)
    {
        g_pv_sd56xx_reboot_callback();
    }

    /* 在系统重启前，复位sfc，关闭sfc时钟 */
    writel(0x0,       IO_ADDRESS(REG_BASE_CRG + 30));
    writel(0xfffffff6, IO_ADDRESS(REG_BASE_CRG + 18));

    if(HI_CHIP_ID_H_E == get_chip_id())
    {
        /* 0x14880000先写入0x56100100，然后写入0XA9EF_FEFF。触发软复位 */
        writel(0x56100100, IO_ADDRESS(REG_BASE_CRG + 0));
        writel(0XA9EFFEFF, IO_ADDRESS(REG_BASE_CRG + 0));
    }
    else
    {
        /* 0x14880000先写入0x56102100，然后写入0XA9EF_DEFF。触发软复位 */
        writel(0x56102100, IO_ADDRESS(REG_BASE_CRG + 0));
        writel(0XA9EFDEFF, IO_ADDRESS(REG_BASE_CRG + 0));
    }
}

/* 静态地址映射，可以直接通过.virtual定义的地址访问物理地址 */
static struct map_desc sd56xx_io_desc[] __initdata = {
    { /* 系统控制器 */
       .virtual     =  IO_ADDRESS(REG_BASE_SCTL),
        .pfn        = __phys_to_pfn(REG_BASE_SCTL),
        .length     = REG_IOSIZE_SCTL,
        .type       = MT_DEVICE
    },{/* 串口0 */
       .virtual     =  IO_ADDRESS(REG_BASE_UART0),
        .pfn        = __phys_to_pfn(REG_BASE_UART0),
        .length     = REG_IOSIZE_UART0,
        .type       = MT_DEVICE
    },{/* 串口1 */
       .virtual     =  IO_ADDRESS(REG_BASE_UART1),
        .pfn        = __phys_to_pfn(REG_BASE_UART1),
        .length     = REG_IOSIZE_UART1,
        .type       = MT_DEVICE
    },{/* 定时器0和1，作为系统时钟和看门狗使用*/
       .virtual     =  IO_ADDRESS(REG_BASE_TIMER01),
        .pfn        = __phys_to_pfn(REG_BASE_TIMER01),
        .length     =  REG_IOSIZE_TIMER01,
        .type       = MT_DEVICE
    },{/* 定时器2和3，作为系统时钟和看门狗使用*/
       .virtual     =  IO_ADDRESS(REG_BASE_TIMER23),
        .pfn        = __phys_to_pfn(REG_BASE_TIMER23),
        .length     =  REG_IOSIZE_TIMER23,
        .type       = MT_DEVICE
    },{/* A9 PERI */
       .virtual     =  IO_ADDRESS(REG_BASE_A9_PERI),
        .pfn        = __phys_to_pfn(REG_BASE_A9_PERI),
        .length     =  REG_IOSIZE_A9_PERI,
        .type       = MT_DEVICE
    },{/* L2 Cache */
        .virtual    =  IO_ADDRESS(REG_BASE_L2CACHE),
        .pfn        = __phys_to_pfn(REG_BASE_L2CACHE),
        .length     =  REG_IOSIZE_L2CACHE,
        .type       = MT_DEVICE_STRONGLY_ORDERED
    },{/* CRG */
        .virtual    =  IO_ADDRESS(REG_BASE_CRG),
        .pfn        = __phys_to_pfn(REG_BASE_CRG),
        .length     =  REG_IOSIZE_CRG,
        .type       = MT_DEVICE
    },     
    
};

/* 静态地址映射 */
void __init sd56xx_map_io(void)
{
    int i;
    
    iotable_init(sd56xx_io_desc, ARRAY_SIZE(sd56xx_io_desc));

    /* 初始化early debug */
    edb_init();
    
    for(i=0; i<ARRAY_SIZE(sd56xx_io_desc); i++)
    {
        edb_putstr(" V: ");    edb_puthex(sd56xx_io_desc[i].virtual);
        edb_putstr(" P: ");    edb_puthex(sd56xx_io_desc[i].pfn);
        edb_putstr(" S: ");    edb_puthex(sd56xx_io_desc[i].length);
        edb_putstr(" T: ");    edb_putul(sd56xx_io_desc[i].type);
        edb_putstr("\n");
    }
    
    early_init();

    edb_trace(1);
}

static unsigned long long sd56xx_cycles_2_ns(unsigned long long cyc)
{
    unsigned long cyc2ns_scale = (1000000 << 10)/timer1_clk_khz;
    return (cyc * cyc2ns_scale) >> 10;
}

static unsigned long free_timer_overflows=0;

unsigned long long sched_clock(void)
{
    unsigned long long ticks64;
    unsigned long ticks2,ticks1;
    
    ticks2 = 0UL - (unsigned long)readl(CFG_TIMER2_VABASE + REG_TIMER_VALUE);
    
    do{
        ticks1  = ticks2;
        ticks64 = free_timer_overflows;
        ticks2  = 0UL - (unsigned long)readl(CFG_TIMER2_VABASE + REG_TIMER_VALUE);
    }while(ticks1 > ticks2);

    /* 
         * If INT is not cleaned, means the function is called with irq_save.
         * The ticks has overflow but 'free_timer_overflows' is not be update.
         */
    if(readl(CFG_TIMER2_VABASE + REG_TIMER_MIS))
    {
        ticks64 += 1;
        ticks2  = 0UL - (unsigned long)readl(CFG_TIMER2_VABASE + REG_TIMER_VALUE);
    }
    
    return sd56xx_cycles_2_ns( (ticks64 << 32) | ticks2);
}

#define HIL_AMBADEV_NAME(name) hil_ambadevice_##name

#define HIL_AMBA_DEVICE(name,busid,base,platdata,id)        \
static struct amba_device HIL_AMBADEV_NAME(name) = {        \
    .dev = {                                                \
        .coherent_dma_mask  = ~0,                           \
        .init_name          = busid,                        \
        .platform_data      = platdata,                     \
    },                                                      \
    .res = {                                                \
        .start              = REG_BASE_##base,              \
        .end                = REG_BASE_##base + 0x1000 -1,  \
        .flags              = IORESOURCE_IO,                \
    },                                                      \
    .dma_mask = ~0,                                         \
    .periphid = id,                                         \
    .irq      = { INTNR_##base, NO_IRQ }                    \
}

#define UART0_IRQ    { INTNR_UART0, NO_IRQ }
#define UART1_IRQ    { INTNR_UART1, NO_IRQ }

HIL_AMBA_DEVICE(uart0, "uart:0",  UART0,    NULL,   0x00041011);
HIL_AMBA_DEVICE(uart1, "uart:1",  UART1,    NULL,   0x00041011);

static struct amba_device *amba_devs[] __initdata =
{                        
        & HIL_AMBADEV_NAME(uart0),
        & HIL_AMBADEV_NAME(uart1),
};

/*
 * These are fixed clocks.
 */
static struct clk uart_clk =
{
    .rate = 100000000,
};


static struct clk_lookup lookups[] =
{
    {
        .dev_id     = "uart:0",
        .clk        = &uart_clk,
    },/* UART0 */
    {
        .dev_id     = "uart:1",
        .clk        = &uart_clk,
    }/* UART1 */
};

void __init sd56xx_init(void)
{
    unsigned long i;
    
    edb_trace(1);

    for (i = 0; i < ARRAY_SIZE(lookups); i++)
    {
        clkdev_add(&lookups[i]);
    }

    for (i = 0; i < ARRAY_SIZE(amba_devs); i++)
    {
        edb_trace(1);
        amba_device_register(amba_devs[i], &iomem_resource);
    }

    sd56xx_chip_id  =get_chip_id();
    sd56xx_boot_sel =get_boot_sel();

    hi_acp_init();
    edb_trace(1);

    l2cache_init();
    edb_trace(1);
}

static void timer_set_mode(enum clock_event_mode mode, struct clock_event_device *clk)
{
    unsigned long ctrl;
    
    switch(mode)
    {
        case CLOCK_EVT_MODE_PERIODIC:
            writel(0, CFG_TIMER_VABASE + REG_TIMER_CONTROL);
            writel(sd56xx_timer_reload, CFG_TIMER_VABASE + REG_TIMER_RELOAD);
            writel(CFG_TIMER_CONTROL, CFG_TIMER_VABASE + REG_TIMER_CONTROL);
            break;
        case CLOCK_EVT_MODE_ONESHOT:
            writel((CFG_TIMER_32BIT |CFG_TIMER_ONESHOT), CFG_TIMER_VABASE + REG_TIMER_CONTROL);
            break;
        case CLOCK_EVT_MODE_UNUSED:
        case CLOCK_EVT_MODE_SHUTDOWN:
        default:
            ctrl = readl(CFG_TIMER_VABASE + REG_TIMER_CONTROL);
            ctrl &= ~CFG_TIMER_ENABLE;
            writel(ctrl, CFG_TIMER_VABASE + REG_TIMER_CONTROL);
    }
}

static int timer_set_next_event(unsigned long evt, struct clock_event_device *unused)
{
    unsigned long ctrl;

    ctrl = readl(CFG_TIMER_VABASE + REG_TIMER_CONTROL);
    ctrl &=~(CFG_TIMER_ENABLE | CFG_TIMER_INTMASK);
    writel(ctrl, CFG_TIMER_VABASE + REG_TIMER_CONTROL);
    writel(evt, CFG_TIMER_VABASE + REG_TIMER_RELOAD);
    writel(CFG_TIMER_ONE_CONTROL, CFG_TIMER_VABASE + REG_TIMER_CONTROL);

    return 0;
}

static struct clock_event_device timer0_clockevent =
{
    .name           = "timer0",
    .shift          = 32,
    .features       = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
    .set_mode       = timer_set_mode,
    .set_next_event = timer_set_next_event,
};

/*
 * IRQ handler for the timer
 */
static irqreturn_t sd56xx_timer_interrupt(int irq, void *dev_id)
{
    static unsigned char uc_print = 0;
    
    if ( ( readl(CFG_TIMER_VABASE+REG_TIMER_RIS) ) & 0x1 )
    {
        writel(~0, CFG_TIMER_VABASE + REG_TIMER_INTCLR);
        timer0_clockevent.event_handler(&timer0_clockevent);
    }
    
    return IRQ_HANDLED;
}

static irqreturn_t sd56xx_freetimer_interrupt(int irq, void *dev_id)
{
    if((readl(CFG_TIMER2_VABASE+REG_TIMER_RIS))&0x1)
    {
        free_timer_overflows++;
        writel(~0, CFG_TIMER2_VABASE + REG_TIMER_INTCLR);
    }
    
    return IRQ_HANDLED;
}

static struct irqaction sd56xx_timer_irq =
{
    .name       = "sd56xx System Timer",
    .flags      = IRQF_SHARED | IRQF_DISABLED | IRQF_TIMER,
    .handler    = sd56xx_timer_interrupt,
};

static struct irqaction sd56xx_freetimer_irq =
{
    .name       = "Free Timer",
    .flags      = IRQF_SHARED | IRQF_DISABLED | IRQF_TIMER,
    .handler    = sd56xx_freetimer_interrupt,
};

static cycle_t sd56xx_get_cycles(struct clocksource *cs)
{
    return ~readl(CFG_TIMER2_VABASE + REG_TIMER_VALUE);
}

static struct clocksource sd56xx_clocksource =
{
    .name       = "timer2",
    .rating     = 200,
    .read       = sd56xx_get_cycles,
    .mask       = CLOCKSOURCE_MASK(32),
    .shift      = 20,
    .flags      = CLOCK_SOURCE_IS_CONTINUOUS,
};

static int __init sd56xx_clocksource_init(void)
{
#ifdef CONFIG_LOCAL_TIMERS
    g_pv_twd_base = (void *)IO_ADDRESS(REG_BASE_A9_PERI) + REG_SC_LOCAL_TIMER;
    edb_trace(1);
    edb_putstr("g_pv_twd_base :\n");
    edb_puthex((int)g_pv_twd_base);
    edb_putstr("\n");
#endif

    writel(0,                  CFG_TIMER2_VABASE + REG_TIMER_CONTROL);
    writel(0xffffffff,        CFG_TIMER2_VABASE + REG_TIMER_RELOAD );
    writel(CFG_TIMER_CONTROL,  CFG_TIMER2_VABASE + REG_TIMER_CONTROL);



    
     sd56xx_clocksource.mult = clocksource_khz2mult(timer1_clk_khz, sd56xx_clocksource.shift);
     
     clocksource_register(&sd56xx_clocksource);

     return 0;
}

static void __init sd56xx_timer_init(void)
{    
    edb_trace(1);    

    /* enable of timer 0 and timer 1*/
    writel(readl(IO_ADDRESS(REG_BASE_SCTL)) | CFG_TIMER0_CLK_SOURCE, IO_ADDRESS(REG_BASE_SCTL));
    writel(readl(IO_ADDRESS(REG_BASE_SCTL)) | CFG_TIMER1_CLK_SOURCE, IO_ADDRESS(REG_BASE_SCTL));

    writel(0, CFG_TIMER_VABASE + REG_TIMER_CONTROL);
    writel(0, CFG_TIMER_VABASE + REG_TIMER1_CONTROL);
    writel(0, CFG_TIMER2_VABASE + REG_TIMER_CONTROL);
    writel(0, CFG_TIMER2_VABASE + REG_TIMER1_CONTROL);
    
    setup_irq(INTNR_TIMER_0_1, &sd56xx_timer_irq);
    setup_irq(INTNR_TIMER_2_3, &sd56xx_freetimer_irq);

    /* init clock soure and clock event */
    sd56xx_clocksource_init();

    timer0_clockevent.mult         = div_sc(timer0_clk_hz, NSEC_PER_SEC, timer0_clockevent.shift);
    timer0_clockevent.max_delta_ns = clockevent_delta2ns(0xffffffff, &timer0_clockevent);
    timer0_clockevent.min_delta_ns = clockevent_delta2ns(0xf, &timer0_clockevent);
    timer0_clockevent.cpumask      = cpumask_of(0);
 
    clockevents_register_device(&timer0_clockevent);
    
    /*set timer clock source from our bus clock*/
    edb_trace(1);
}

struct sys_timer sd56xx_timer =
{
    .init = sd56xx_timer_init,
};

static HI_BOOT_DEV_E get_boot_sel(void)
{
    unsigned int ui_reg_value;

    ui_reg_value=readl(IO_ADDRESS(REG_BASE_SCTL+0x8));
    printk("Boot_mode:%d\n", (ui_reg_value & 0x3));

    return (HI_BOOT_DEV_E)(ui_reg_value & 3);
}

HI_BOOT_DEV_E hi_kernel_get_boot_sel(void)
{
    return sd56xx_boot_sel;
}
EXPORT_SYMBOL(hi_kernel_get_boot_sel);

static HI_CHIP_ID_E get_chip_id(void)
{
    unsigned int ui_reg_value;

    ui_reg_value=readl(IO_ADDRESS(REG_BASE_SCTL+0x0800));
    printk("Chip_id:0x%x\n", ui_reg_value);

    switch(ui_reg_value & HI_CHIP_ID_MASK)
    {
        case (HI_CHIP_ID_H & HI_CHIP_ID_MASK):
            return HI_CHIP_ID_H_E;
            
        case (HI_CHIP_ID_T & HI_CHIP_ID_MASK):
            return HI_CHIP_ID_T_E;

        default:
            break;
    }

    return HI_CHIP_ID_NONE_E;
}

HI_CHIP_ID_E hi_kernel_get_chip_id()
{
    return sd56xx_chip_id;
}
EXPORT_SYMBOL(hi_kernel_get_chip_id);

MACHINE_START(SD56XX, "sd56xx")
    .phys_io        = 0x1010e000,                   /* 实际没有用处，配置系统中外设IO的起始地址即可 */
    .io_pg_offst    = (0xd010e000 >> 18) & 0xfffc,  /* 实际没有用处 */
    .boot_params    = PHYS_OFFSET+0x100,            /* 存放uboot参数的地址，需要和uboot中设置一致 */                                                  //必须和uboot里面设置的CFG_BOOT_PARAMS一致   
    .map_io         = sd56xx_map_io,
    .init_irq       = sd56xx_gic_init_irq,
    .timer          = &sd56xx_timer,
    .init_machine   = sd56xx_init,
MACHINE_END

