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



#define   HI_SC_PEREN0_ADDR		     (0x14880000 + 0x14)   
#define   HI_USBAHB_CLK_EN             (1<<16)
#define   HI_USBAHB3_CLK_EN            (1<<16)
#define   HI_USB3_PHY1_CLK_EN          (1<<11)    
#define   HI_USB3_PHY0_CLK_EN          (1<<10)

 
#define   HI_SC_PEREN1_ADDR	         (0x14880000 + 0x20)   
#define   HI_USB_CLK_EN                (1<<22)

#define   HI_SC_RST_CTRL0_ADDR         (0x14880000 + 0x2C)  
#define   HI_USB_PHY_SRST_N            (1<<18)     
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
    hi_v_u32 *SC_PEREN0;
    hi_v_u32 *SC_PEREN1;
    hi_v_u32 *SC_RST_CTRL0;
} hi_usb_ctrl_reg_s;

static hi_usb_ctrl_reg_s  g_usb_reg_ctrl;


static atomic_t dev_open_cnt = {
	.counter = 0,
};


void hiusb_start_hcd(void)
{
	unsigned long flags;
	int reg;

	local_irq_save(flags);
	if(atomic_add_return(1, &dev_open_cnt) == 1)
    {
        g_usb_reg_ctrl.SC_PEREN0 = ioremap(HI_SC_PEREN0_ADDR, sizeof(hi_v_u32));
        if(NULL == g_usb_reg_ctrl.SC_PEREN0)
        {
            printk("ioremap error: g_usb_reg_ctrl.SC_PEREN0 = NULL\r\n");
            return;
        }
        g_usb_reg_ctrl.SC_PEREN1 = ioremap(HI_SC_PEREN1_ADDR, sizeof(hi_v_u32));
        if(NULL == g_usb_reg_ctrl.SC_PEREN1)
        {
            printk("ioremap error: g_usb_reg_ctrl.SC_PEREN1 = NULL\r\n");
            iounmap(g_usb_reg_ctrl.SC_PEREN0);
            return;
        }
        g_usb_reg_ctrl.SC_RST_CTRL0 = ioremap(HI_SC_RST_CTRL0_ADDR, sizeof(hi_v_u32));
        if(NULL == g_usb_reg_ctrl.SC_RST_CTRL0)
        {
            printk("ioremap error: g_usb_reg_ctrl.SC_RST_CTRL0 = NULL\r\n");
            iounmap(g_usb_reg_ctrl.SC_PEREN1);
            iounmap(g_usb_reg_ctrl.SC_PEREN0);
            return;
        }

        if(HI_CHIP_ID_H_E == hi_kernel_get_chip_id())
        {

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

            udelay(10);
            
            reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
            reg |= HI_USB_SRST_N;
            *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

            reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
            reg |= HI_USB_PHY_SRST_N;
            *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

            udelay(50);

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
            reg = *(g_usb_reg_ctrl.SC_PEREN0);
            reg |= HI_USBAHB3_CLK_EN;
            reg |= HI_USB3_PHY1_CLK_EN;
            reg |= HI_USB3_PHY0_CLK_EN;
            *(g_usb_reg_ctrl.SC_PEREN0) = reg;

            reg = *(g_usb_reg_ctrl.SC_PEREN1);
            reg |= HI_USB_CLK_EN;
            *(g_usb_reg_ctrl.SC_PEREN1) = reg;

            udelay(10);

            reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
            reg &= ~HI_USB3_PHY1_PORT_SRST_N;
            reg &= ~HI_USB3_PHY0_PORT_SRST_N;
            reg &= ~HI_USB3_PHY1_SRST_N;
            reg &= ~HI_USB3_PHY0_SRST_N;
            reg &= ~HI_USB3_CTRL_SRST_N;
            *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

            udelay(20);

            reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
            reg |= HI_USB3_PHY1_PORT_SRST_N;
            reg |= HI_USB3_PHY0_PORT_SRST_N;
            *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;
      
            udelay(250);

            reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
            reg |= HI_USB3_PHY1_SRST_N;
            reg |= HI_USB3_PHY0_SRST_N;
            *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

            udelay(10);

            reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
            reg |= HI_USB3_CTRL_SRST_N;
            *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

            udelay(10);        

    }
		printk("hiusb_start_hcd sucessful!\r\n");
	}
	local_irq_restore(flags);
}
EXPORT_SYMBOL(hiusb_start_hcd);

void hiusb_stop_hcd(void)
{
    unsigned long flags;
    int reg;

    local_irq_save(flags);
    if(atomic_sub_return(1, &dev_open_cnt) == 0)
    {
        if(HI_CHIP_ID_H_E == hi_kernel_get_chip_id())
        {
            reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
            reg  &=~HI_USB_PHY_SRST_N;
            reg  &=~HI_USB_PORT_SRST_N;
            reg  &=~HI_USB_CTRL_SRST_N;
            reg  &=~HI_USB_SRST_N;
            *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

            reg = *(g_usb_reg_ctrl.SC_PEREN0);
            reg &=~HI_USBAHB_CLK_EN;
            *(g_usb_reg_ctrl.SC_PEREN0) = reg;

            reg = *(g_usb_reg_ctrl.SC_PEREN1);
            reg &=~HI_USB_CLK_EN;
            *(g_usb_reg_ctrl.SC_PEREN1) = reg;
        }
        else
        {
            reg = *(g_usb_reg_ctrl.SC_RST_CTRL0);
            reg  &=~HI_USB3_PHY1_PORT_SRST_N;
            reg  &=~HI_USB3_PHY0_PORT_SRST_N;
            reg  &=~HI_USB3_PHY1_SRST_N;
            reg  &=~HI_USB3_PHY0_SRST_N;
            reg  &=~HI_USB3_CTRL_SRST_N;
            *(g_usb_reg_ctrl.SC_RST_CTRL0) = reg;

            reg = *(g_usb_reg_ctrl.SC_PEREN0);
            reg &=~HI_USBAHB3_CLK_EN;
            reg &=~HI_USB3_PHY1_CLK_EN;
            reg &=~HI_USB3_PHY0_CLK_EN;
            *(g_usb_reg_ctrl.SC_PEREN0) = reg;

            reg = *(g_usb_reg_ctrl.SC_PEREN1);
            reg &=~ HI_USB_CLK_EN;
            *(g_usb_reg_ctrl.SC_PEREN1) = reg;
        }

	iounmap(g_usb_reg_ctrl.SC_PEREN0);
	iounmap(g_usb_reg_ctrl.SC_PEREN1);
	iounmap(g_usb_reg_ctrl.SC_RST_CTRL0);
	
	printk("hiusb_stop_hcd sucessful!\r\n");
    }
local_irq_restore(flags);
}
EXPORT_SYMBOL(hiusb_stop_hcd);
