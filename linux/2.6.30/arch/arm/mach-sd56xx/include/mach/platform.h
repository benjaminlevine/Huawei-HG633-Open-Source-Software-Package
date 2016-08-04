

#ifndef    __HI_CHIP_REGS_H__
#define    __HI_CHIP_REGS_H__

#include <mach/io.h>

#define REG_BASE_SCTL                   0x10100000
#define REG_IOSIZE_SCTL                 0x1000

#define REG_BASE_DDRC                   0x10102000
#define REG_IOSIZE_DDRC                 0x1000

#define REG_BASE_TIMER01                0x10104000
#define REG_IOSIZE_TIMER01              0x1000
#define REG_BASE_TIMER23                0x10105000
#define REG_IOSIZE_TIMER23              0x1000


#define REG_BASE_GPIO0                  0x10106000    
#define REG_BASE_GPIO1                  0x10107000    
#define REG_BASE_GPIO2                  0x10108000
#define REG_BASE_GPIO3                  0x10109000
#define REG_BASE_GPIO4                  0x1010A000
#define REG_BASE_GPIO5                  0x1010B000
#define REG_BASE_GPIO6                  0x1010C000    
#define REG_BASE_GPIO7                  0x1010D000

#define REG_BASE_UART0                  0x1010E000
#define REG_IOSIZE_UART0                0x1000
#define REG_BASE_UART1                  0x1010F000
#define REG_IOSIZE_UART1                0x1000

#define REG_BASE_A9_PERI                0x10180000
#define REG_IOSIZE_A9_PERI              0x2000

#define REG_BASE_L2CACHE                0x16800000    
#define REG_IOSIZE_L2CACHE              0x1000    

#define REG_BASE_CRG                    0x14880000
#define REG_IOSIZE_CRG                  0x1000

#define REG_BASE_PCIE0                  0x10A00000    
#define REG_IOSIZE_PCIE0                0x10000    
#define REG_BASE_PCIE1                  0x10A10000
#define REG_IOSIZE_PCIE1                0x10000    

#define REG_BASE_SFC                    0x10A20000            
#define REG_BASE_NANDC                  0x10A30000

#define REG_BASE_EHCI                   0x10A40000
#define REG_IOSIZE_EHCI                 0x1000
#define REG_BASE_OHCI                   0x10A50000
#define REG_IOSIZE_OHCI                 0x1000


#define REG_BASE_MMC_SD_SDIO            0x10A60000


/*CORTTX-A9 MPCORE MEMORY REGION*/
#define REG_A9_PERI_SCU                 0x0000
#define REG_A9_PERI_GIC_CPU             0x0100
#define REG_A9_PERI_GLOBAL_TIMER        0x0200
#define REG_A9_PERI_PRI_TIMER_WDT       0x0600
#define REG_A9_PERI_GIC_DIST            0x1000 
#define REG_SC_LOCAL_TIMER              0x600

#define REG_SC_PLLCTRL4                 0x0074
#define REG_SC_PLLFCTRL4                0x0078
#define REG_SC_PERCTRL44                0x014C
#define REG_SC_SC_TESTREG1              0x0130

#define REG_TIMER_RELOAD                0x000
#define REG_TIMER_VALUE                 0x004
#define REG_TIMER_CONTROL               0x008
#define REG_TIMER_INTCLR                0x00C
#define REG_TIMER_RIS                   0x010
#define REG_TIMER_MIS                   0x014
#define REG_TIMER_BGLOAD                0x018

#define REG_TIMER1_RELOAD               0x020
#define REG_TIMER1_VALUE                0x024
#define REG_TIMER1_CONTROL              0x028
#define REG_TIMER1_INTCLR               0x02C
#define REG_TIMER1_RIS                  0x030
#define REG_TIMER1_MIS                  0x034
#define REG_TIMER1_BGLOAD               0x038

#define CFG_GIC_CPU_BASE                (IO_ADDRESS(REG_BASE_A9_PERI) + REG_A9_PERI_GIC_CPU)
#define CFG_GIC_DIST_BASE               (IO_ADDRESS(REG_BASE_A9_PERI) + REG_A9_PERI_GIC_DIST)

#define DDR_SUSPEND_SIZE                0x1000      /* size 1KB */

#endif /*End of __HI_CHIP_REGS_H__ */

