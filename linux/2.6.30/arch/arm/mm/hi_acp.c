#include <linux/module.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/interrupt.h>

#include <mach/platform.h>
#include <mach/kexport.h>

int acp_en=1;
int acp_clk=175;

#define SC_PERCTRL1_REG     ( REG_BASE_SCTL + 0xa0  )
#define SC_PERCTRL6         ( REG_BASE_CRG  + 0x5c )

#define PCIE1_AWADDR33      (1 << 8 )
#define PCIE1_ARADDR33      (1 << 9 )

#define PCIE0_AWADDR33      (1 << 12)
#define PCIE0_ARADDR33      (1 << 13)

#define PIE_AWADDR33        (1 << 16)
#define PIE_ARADDR33        (1 << 17)

#define USB_AWADDR33        (1 << 20)
#define USB_ARADDR33        (1 << 21)

#define IPSEC_AWADDR33      (1 << 24)
#define IPSEC_ARADDR33      (1 << 25)
    
#define ACP_FREQ_MASK       (0x7 << 27) //bit 27~bit29

void hi_acp_init(void)
{
    unsigned long flags, ui_value;

    if(0==acp_en)
    {
        /*  选择PIE、PCIE0、PCIE1读写DDR的通路为直通通路访问. */
        local_irq_save(flags);
        
        ui_value = readl(IO_ADDRESS(SC_PERCTRL1_REG));
        ui_value &= ~(PIE_ARADDR33   | PIE_AWADDR33         //PIE read/write
                    | PCIE0_ARADDR33 | PCIE0_AWADDR33       //PCIE0 read/write
                    | PCIE1_ARADDR33 | PCIE1_AWADDR33       //PCIE1 read/write
                    | IPSEC_ARADDR33 | IPSEC_AWADDR33       //IPSEC read/write
                    | USB_ARADDR33   | USB_AWADDR33);       //USB read/write
        writel(ui_value, IO_ADDRESS(SC_PERCTRL1_REG));
        printk(KERN_INFO "ACP OFF!\n");

        local_irq_restore(flags);
        return;
    }

    /* 选择PIE、PCIE0、PCIE1读写DDR的通路为ACP访问. */
    local_irq_save(flags);
    
    ui_value = readl(IO_ADDRESS(SC_PERCTRL1_REG));
    ui_value |= PIE_ARADDR33 | PIE_AWADDR33         //PIE read/write
            | PCIE0_ARADDR33 | PCIE0_AWADDR33       //PCIE0 read/write
            | PCIE1_ARADDR33 | PCIE1_AWADDR33       //PCIE1 read/write
            | IPSEC_ARADDR33 | IPSEC_AWADDR33       //IPSEC read/write
            | USB_ARADDR33   | USB_AWADDR33;        //USB read/write
    writel(ui_value, IO_ADDRESS(SC_PERCTRL1_REG));
    printk(KERN_INFO "ACP ON!\n");

    /*  选择AXI_ACP总线的工作频率:*/
    ui_value = readl(IO_ADDRESS(SC_PERCTRL6));
    ui_value&= ~ACP_FREQ_MASK;
    if(233==acp_clk)
    {
        printk("ACP bus = 233M!\n");
    }
    else if(140==acp_clk)
    {
        ui_value|= (0x2 << 27);
        printk("ACP bus = 140M!\n");
    }
    else if(116==acp_clk)
    {
        ui_value|= (0x3 << 27);
        printk("ACP bus = 116M!\n");
    }
    else if(70==acp_clk)
    {
        ui_value|= (0x4 << 27);
        printk("ACP bus = 70M!\n");
    }
    else
    {
        ui_value|= (0x1 << 27);
        printk("ACP bus = 175M!\n");
    }
    writel(ui_value, IO_ADDRESS(SC_PERCTRL6));
    
    local_irq_restore(flags);

    return;
}


static int __init acp_en_option(char *str)
{
    get_option(&str, &acp_en);
    return 1;
}
static int __init acp_clk_option(char *str)
{
    get_option(&str, &acp_clk);
    if( (acp_clk!=300)
        && (acp_clk!=200)
        && (acp_clk!=150)
        && (acp_clk!=50) )
    {
        printk("acp_clk option:%s invalid, using:%d(M)\n", str, acp_clk);
    }
    return 1;
}

early_param("acp_en", acp_en_option);
early_param("acp_clk", acp_clk_option);


int hi_kernel_acp_on()
{
    return acp_en;
}
EXPORT_SYMBOL(hi_kernel_acp_on);

