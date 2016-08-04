#include <linux/init.h>
#include <linux/timer.h>
#include <linux/ktime.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/spinlock.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/unaligned.h>
#include <mach/kexport.h>

#define   HI_GPIO_SEL_D2_ADDR		     0x149001A8   		
#define   HI_GPIO_26_EN                (1<<10)      			
#define   HI_GPIO_27_EN                (1<<11)   

#define   HI_IO_SEL_D2_ADDR            0x14900194
#define   HI_D2_UART0_EN               (1<<0)     			
#define   HI_D2_DFE_UART_EN            (1<<5)     			
#define   HI_D2_USB_CTRL_EN            (1<<6) 


#define   HI_SC_PEREN0_ADDR		     (0x14880000 + 0x14)   
#define   HI_USBAHB_CLK_EN             (1<<16)      	
#define   HI_USBAHB3_CLK_EN            (1<<16)      		    
#define   HI_USB3_PHY1_CLK_EN          (1<<11)      		    
#define   HI_USB3_PHY0_CLK_EN          (1<<10) 

 
#define   HI_SC_PEREN1_ADDR	         (0x14880000 + 0x20)   
#define   HI_USB_CLK_EN                (1<<22)      			

#define   HI_SC_RST_CTRL0_ADDR         (0x14880000 + 0x2C)  
#define   HI_USB_PHY_SRST_N        	(1<<18)    		     
#define   HI_USB_PORT_SRST_N           (1<<17)
#define   HI_USB_CTRL_SRST_N           (1<<16)
#define   HI_USB_SRST_N                (1<<15)    		
#define   HI_USB3_PHY1_PORT_SRST_N     (1<<19)    		     
#define   HI_USB3_PHY0_PORT_SRST_N     (1<<18)
#define   HI_USB3_PHY1_SRST_N          (1<<17)
#define   HI_USB3_PHY0_SRST_N          (1<<16)    
#define   HI_USB3_CTRL_SRST_N          (1<<15)      



typedef volatile unsigned int       hi_v_u32;

typedef struct
{
    hi_v_u32 *GPIO_SEL_D2;
    hi_v_u32 *IO_SEL_D2;
    hi_v_u32 *SC_PEREN0;
    hi_v_u32 *SC_PEREN1;
    hi_v_u32 *SC_RST_CTRL0;
} hi_usb_ctrl_reg_s;

static hi_usb_ctrl_reg_s  g_usb_reg_ctrl;


static atomic_t dev_open_cnt = {
	.counter = 0,
};

/*
SD5610H:
GPIO 26,GPIO27复用关系设置
GPIO26_en gpio_sel_d2[10]
GPIO27_en	gpio_sel_d2[11]
d2_uart0_en	io_sel_d2[0]
d2_dfe_uart_en	io_sel_d2[5]
d2_usb_ctrl_en	io_sel_d2[6]

GPIO26~27_en=0, d2_uart0_en=0, d2_dfe_uart_en=0, d2_usb_ctrl_en=1
reg_bits_clr 0x149001A8 10 11
reg_bits_clr 0x14900194 0 5
reg_bits_set 0x14900194 6

USB时钟 
reg_bits_set 0x14880014 16 
reg_bits_set 0x14880020 22 
等一下 delay(100us)
USB复位步骤
reg_bits_clr 0x1488002c 15 16 17 18 19                                                                                    
reg_bits_set 0x1488002c 15 
reg_bits_set 0x1488002c 18 
等一下 delay(100us)
reg_bits_set 0x1010012c 17  
等一下 delay(100us)
reg_bits_set 0x1010012c 16 



SD5610T:
GPIO 26,GPIO27复用关系设置
GPIO26_en gpio_sel_d2[10]
GPIO27_en	gpio_sel_d2[11]
d2_uart0_en	io_sel_d2[0]
d2_dfe_uart_en	io_sel_d2[5]
d2_usb_ctrl_en	io_sel_d2[6]

GPIO26~27_en=0, d2_uart0_en=0, d2_dfe_uart_en=0, d2_usb_ctrl_en=1
reg_bits_clr 0x149001A8 10 11
reg_bits_clr 0x14900194 0 5
reg_bits_set 0x14900194 6

USB时钟 
reg_bits_set 0x14880014 16 
reg_bits_set 0x14880020 22 
等一下 delay(100us)
USB复位步骤
reg_bits_clr 0x1488002c 15 16 17 18                                                                                     
reg_bits_set 0x1488002c 18 
等一下 delay(100us)
reg_bits_set 0x1010012c 17  
等一下 delay(100us)
reg_bits_set 0x1010012c 16 

*/

void hiusb_start_hcd(void)
{
	unsigned long flags;

	local_irq_save(flags);
	if(atomic_add_return(1, &dev_open_cnt) == 1){

		int reg;

   		g_usb_reg_ctrl.GPIO_SEL_D2 = ioremap(HI_GPIO_SEL_D2_ADDR, sizeof(hi_v_u32));
		if(NULL == g_usb_reg_ctrl.GPIO_SEL_D2)
		{
		   printk("ioremap error: g_usb_reg_ctrl.GPIO_SEL_D2 = NULL\r\n");
		   return;
		}
		g_usb_reg_ctrl.IO_SEL_D2 = ioremap(HI_IO_SEL_D2_ADDR, sizeof(hi_v_u32));
		if(NULL == g_usb_reg_ctrl.IO_SEL_D2)
		{
		   printk("ioremap error: g_usb_reg_ctrl.PERI_PEREN = NULL\r\n");
		   iounmap(g_usb_reg_ctrl.GPIO_SEL_D2);
		   return;
		}
		g_usb_reg_ctrl.SC_PEREN0 = ioremap(HI_SC_PEREN0_ADDR, sizeof(hi_v_u32));
		if(NULL == g_usb_reg_ctrl.SC_PEREN0)
		{
		   printk("ioremap error: g_usb_reg_ctrl.PERI_PEREN1 = NULL\r\n");
		   iounmap(g_usb_reg_ctrl.GPIO_SEL_D2);
		   iounmap(g_usb_reg_ctrl.IO_SEL_D2);
		   return;
		}
		g_usb_reg_ctrl.SC_PEREN1 = ioremap(HI_SC_PEREN1_ADDR, sizeof(hi_v_u32));
		if(NULL == g_usb_reg_ctrl.SC_PEREN1)
		{
		   printk("ioremap error: g_usb_reg_ctrl.SC_PEREN1 = NULL\r\n");
		   iounmap(g_usb_reg_ctrl.SC_PEREN0);
		   iounmap(g_usb_reg_ctrl.GPIO_SEL_D2);
		   iounmap(g_usb_reg_ctrl.IO_SEL_D2);
		   return;
		}
		g_usb_reg_ctrl.SC_RST_CTRL0 = ioremap(HI_SC_RST_CTRL0_ADDR, sizeof(hi_v_u32));
		if(NULL == g_usb_reg_ctrl.SC_RST_CTRL0)
		{
		   printk("ioremap error: g_usb_reg_ctrl.PERI_PERCTRL39 = NULL\r\n");
		   iounmap(g_usb_reg_ctrl.SC_PEREN1);
		   iounmap(g_usb_reg_ctrl.SC_PEREN0);
		   iounmap(g_usb_reg_ctrl.GPIO_SEL_D2);
		   iounmap(g_usb_reg_ctrl.IO_SEL_D2);
		   return;
		}

    if(HI_CHIP_ID_H_E == hi_kernel_get_chip_id())
    {
        reg = *(g_usb_reg_ctrl.GPIO_SEL_D2);
        reg &= ~HI_GPIO_26_EN;
        reg &= ~HI_GPIO_27_EN;
        *(g_usb_reg_ctrl.GPIO_SEL_D2) = reg;


        reg = *(g_usb_reg_ctrl.IO_SEL_D2);
        reg &= ~HI_D2_UART0_EN;
        reg &= ~HI_D2_DFE_UART_EN;
        reg |=  HI_D2_USB_CTRL_EN;
        *(g_usb_reg_ctrl.IO_SEL_D2) = reg;

        reg = *(g_usb_reg_ctrl.SC_PEREN0);
        reg |= HI_USBAHB_CLK_EN;
        *(g_usb_reg_ctrl.SC_PEREN0) = HI_USBAHB_CLK_EN;

        reg = *(g_usb_reg_ctrl.SC_PEREN1);
        reg |= HI_USB_CLK_EN;
        *(g_usb_reg_ctrl.SC_PEREN1) = HI_USB_CLK_EN;

        udelay(100);

        reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
        reg &= ~HI_USB_PHY_SRST_N;
        reg &= ~HI_USB_PORT_SRST_N;
        reg &= ~HI_USB_CTRL_SRST_N;
        reg &= ~HI_USB_SRST_N;
        *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;


        reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
        reg |= HI_USB_SRST_N;
        *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

        reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
        reg |= HI_USB_PHY_SRST_N;
        *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

        udelay(100);

        reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
        reg |= HI_USB_PORT_SRST_N;
        *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

        udelay(100);

        reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
        reg |= HI_USB_CTRL_SRST_N;
        *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

        udelay(100);    
     }
     else if(HI_CHIP_ID_T_E == hi_kernel_get_chip_id())
     {
        reg = *(g_usb_reg_ctrl.GPIO_SEL_D2);
        reg &= ~HI_GPIO_26_EN;
        reg &= ~HI_GPIO_27_EN;
        *(g_usb_reg_ctrl.GPIO_SEL_D2) = reg;


        reg = *(g_usb_reg_ctrl.IO_SEL_D2);
        reg &= ~HI_D2_UART0_EN;
        reg &= ~HI_D2_DFE_UART_EN;
        reg |=  HI_D2_USB_CTRL_EN;
        *(g_usb_reg_ctrl.IO_SEL_D2) = reg;

        reg = *(g_usb_reg_ctrl.SC_PEREN0);
        reg |= HI_USBAHB3_CLK_EN;
        reg |= HI_USB3_PHY1_CLK_EN;
        reg |= HI_USB3_PHY0_CLK_EN;
        *(g_usb_reg_ctrl.SC_PEREN0) = HI_USBAHB3_CLK_EN;

        reg = *(g_usb_reg_ctrl.SC_PEREN1);
        reg |= HI_USB_CLK_EN;
        *(g_usb_reg_ctrl.SC_PEREN1) = HI_USB_CLK_EN;

        udelay(100);

        reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
        reg &= ~HI_USB3_PHY1_PORT_SRST_N;
        reg &= ~HI_USB3_PHY0_PORT_SRST_N;
        reg &= ~HI_USB3_PHY1_SRST_N;
        reg &= ~HI_USB3_PHY0_SRST_N;
        reg &= ~HI_USB3_CTRL_SRST_N;
        *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;



        reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
        reg |= HI_USB3_PHY1_SRST_N;
        reg |= HI_USB3_PHY0_SRST_N;
        *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

        udelay(100);

        reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
        reg |= HI_USB3_PHY1_PORT_SRST_N;
        reg |= HI_USB3_PHY0_PORT_SRST_N;
        *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

        udelay(100);

        reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
        reg |= HI_USB3_CTRL_SRST_N;
        *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

        udelay(100);        

     }
		printk("hiusb_start_hcd sucessful!\r\n");
	}
	local_irq_restore(flags);
}
EXPORT_SYMBOL(hiusb_start_hcd);

void hiusb_stop_hcd(void)
{
unsigned long flags;

local_irq_save(flags);
if(atomic_sub_return(1, &dev_open_cnt) == 0){

	int reg;
#ifdef CONFIG_USB_HSAN_SD5610H		
	reg = *(g_usb_reg_ctrl.SC_PEREN0);
	reg &=~HI_USBAHB_CLK_EN;
	*(g_usb_reg_ctrl.SC_PEREN0) = reg;

	reg = *(g_usb_reg_ctrl.SC_PEREN1);
	reg &=~ HI_USB_CLK_EN;
	*(g_usb_reg_ctrl.SC_PEREN1) = reg;

    reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
    reg  &=~HI_USB_PHY_SRST_N;
    reg  &=~ HI_USB_PORT_SRST_N;
    reg &=~ HI_USB_CTRL_SRST_N;
    reg  &=~ HI_USB_SRST_N;
	*(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

	reg = *(g_usb_reg_ctrl.GPIO_SEL_D2);
	reg |=HI_GPIO_26_EN;
	reg |=HI_GPIO_27_EN;
	*(g_usb_reg_ctrl.GPIO_SEL_D2) = reg;


    reg = *(g_usb_reg_ctrl.IO_SEL_D2);
    reg &=~HI_D2_UART0_EN;
    reg &=~HI_D2_UART0_EN;
    reg &=~HI_D2_UART0_EN;
    *(g_usb_reg_ctrl.IO_SEL_D2) = reg;

	
#elif CONFIG_USB_HSAN_SD5610T

	reg = *(g_usb_reg_ctrl.SC_PEREN0);
	reg &=~HI_USBAHB3_CLK_EN;
	reg &=~HI_USB3_PHY1_CLK_EN;
	reg &=~HI_USB3_PHY0_CLK_EN;
	*(g_usb_reg_ctrl.SC_PEREN0) = reg;

	reg = *(g_usb_reg_ctrl.SC_PEREN1);
	reg &=~ HI_USB_CLK_EN;
	*(g_usb_reg_ctrl.SC_PEREN1) = reg;

    reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
    reg  &=~HI_USB3_PHY1_PORT_SRST_N;
    reg  &=~HI_USB3_PHY0_PORT_SRST_N;
    reg  &=~HI_USB3_PHY1_SRST_N;
    reg  &=~HI_USB3_PHY0_SRST_N;
    reg  &=~HI_USB3_CTRL_SRST_N;
	*(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;


    reg = *(g_usb_reg_ctrl.GPIO_SEL_D2);
    reg |=HI_GPIO_26_EN;
    reg |=HI_GPIO_27_EN;
    *(g_usb_reg_ctrl.GPIO_SEL_D2) = reg;


    reg = *(g_usb_reg_ctrl.IO_SEL_D2);
    reg &=~HI_D2_UART0_EN;
    reg &=~HI_D2_UART0_EN;
    reg &=~HI_D2_UART0_EN;
    *(g_usb_reg_ctrl.IO_SEL_D2) = reg;


#endif
	iounmap(g_usb_reg_ctrl.GPIO_SEL_D2);
	iounmap(g_usb_reg_ctrl.IO_SEL_D2);
	iounmap(g_usb_reg_ctrl.SC_PEREN0);
	iounmap(g_usb_reg_ctrl.SC_PEREN1);
	iounmap(g_usb_reg_ctrl.SC_RST_CTRL0);
	
	printk("hiusb_stop_hcd sucessful!\r\n");
}
local_irq_restore(flags);
}
EXPORT_SYMBOL(hiusb_stop_hcd);
