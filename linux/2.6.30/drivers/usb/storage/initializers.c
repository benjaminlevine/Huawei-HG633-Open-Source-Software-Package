/* Special Initializers for certain USB Mass Storage devices
 *
 * Current development and maintenance by:
 *   (c) 1999, 2000 Matthew Dharm (mdharm-usb@one-eyed-alien.net)
 *
 * This driver is based on the 'USB Mass Storage Class' document. This
 * describes in detail the protocol used to communicate with such
 * devices.  Clearly, the designers had SCSI and ATAPI commands in
 * mind when they created this document.  The commands are all very
 * similar to commands in the SCSI-II and ATAPI specifications.
 *
 * It is important to note that in a number of cases this class
 * exhibits class-specific exemptions from the USB specification.
 * Notably the usage of NAK, STALL and ACK differs from the norm, in
 * that they are used to communicate wait, failed and OK on commands.
 *
 * Also, for certain devices, the interrupt endpoint is used to convey
 * status of a command.
 *
 * Please see http://www.one-eyed-alien.net/~mdharm/linux-usb for more
 * information about this driver.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/errno.h>

#include "usb.h"
#include "initializers.h"
#include "debug.h"
#include "transport.h"

/* This places the Shuttle/SCM USB<->SCSI bridge devices in multi-target
 * mode */
int usb_stor_euscsi_init(struct us_data *us)
{
	int result;

	US_DEBUGP("Attempting to init eUSCSI bridge...\n");
	us->iobuf[0] = 0x1;
	result = usb_stor_control_msg(us, us->send_ctrl_pipe,
			0x0C, USB_RECIP_INTERFACE | USB_TYPE_VENDOR,
			0x01, 0x0, us->iobuf, 0x1, 5*HZ);
	US_DEBUGP("-- result is %d\n", result);

	return 0;
}

/* This function is required to activate all four slots on the UCR-61S2B
 * flash reader */
int usb_stor_ucr61s2b_init(struct us_data *us)
{
	struct bulk_cb_wrap *bcb = (struct bulk_cb_wrap*) us->iobuf;
	struct bulk_cs_wrap *bcs = (struct bulk_cs_wrap*) us->iobuf;
	int res;
	unsigned int partial;
	static char init_string[] = "\xec\x0a\x06\x00$PCCHIPS";

	US_DEBUGP("Sending UCR-61S2B initialization packet...\n");

	bcb->Signature = cpu_to_le32(US_BULK_CB_SIGN);
	bcb->Tag = 0;
	bcb->DataTransferLength = cpu_to_le32(0);
	bcb->Flags = bcb->Lun = 0;
	bcb->Length = sizeof(init_string) - 1;
	memset(bcb->CDB, 0, sizeof(bcb->CDB));
	memcpy(bcb->CDB, init_string, sizeof(init_string) - 1);

	res = usb_stor_bulk_transfer_buf(us, us->send_bulk_pipe, bcb,
			US_BULK_CB_WRAP_LEN, &partial);
	if(res)
		return res;

	US_DEBUGP("Getting status packet...\n");
	res = usb_stor_bulk_transfer_buf(us, us->recv_bulk_pipe, bcs,
			US_BULK_CS_WRAP_LEN, &partial);

	return (res ? -1 : 0);
}

/* This places the HUAWEI E220 devices in multi-port mode */
int usb_stor_huawei_e220_init(struct us_data *us)
{
	int result;

	result = usb_stor_control_msg(us, us->send_ctrl_pipe,
				      USB_REQ_SET_FEATURE,
				      USB_TYPE_STANDARD | USB_RECIP_DEVICE,
				      0x01, 0x0, NULL, 0x0, 1000);
	US_DEBUGP("usb_control_msg performing result is %d\n", result);
	return (result ? 0 : -1);
}

#ifdef CONFIG_ATP_USB_ADAPTOR
#define IS_HUAWEI_DONGLES 1
#define NOT_HUAWEI_DONGLES 0

//这个地方是数据卡切换新方案，目前为兼容老方案，balong LTE数据卡切换动作在此完成

static int usb_stor_huawei_dongles_pid(struct us_data *us)
{
    int ret = NOT_HUAWEI_DONGLES;
    struct usb_interface_descriptor *idesc = NULL;
    idesc = &us->pusb_intf->cur_altsetting->desc;
    if(NULL != idesc)
    {
        printk("\nle16_to_cpu(us->pusb_dev->descriptor.idProduct):[0x%x]\n", le16_to_cpu(us->pusb_dev->descriptor.idProduct));
        
        if(0x0000 == le16_to_cpu(idesc->bInterfaceNumber))
        {
            if((0x1401<= le16_to_cpu(us->pusb_dev->descriptor.idProduct) && 0x1600 >= le16_to_cpu(us->pusb_dev->descriptor.idProduct))
                ||(0x1c02<=le16_to_cpu(us->pusb_dev->descriptor.idProduct) && 0x2202 >= le16_to_cpu(us->pusb_dev->descriptor.idProduct))
                ||(0x1001==le16_to_cpu(us->pusb_dev->descriptor.idProduct))
                ||(0x1003==le16_to_cpu(us->pusb_dev->descriptor.idProduct))
                ||(0x1004==le16_to_cpu(us->pusb_dev->descriptor.idProduct)))
            {
                if((0x1501<=le16_to_cpu(us->pusb_dev->descriptor.idProduct)) &&( 0x1504 >= le16_to_cpu(us->pusb_dev->descriptor.idProduct)))
                {
                    ret = NOT_HUAWEI_DONGLES;
                }else{
                    ret = IS_HUAWEI_DONGLES;
                }
            }
        }
    }

    return ret;
}

int usb_stor_huawei_scsi_init(struct us_data *us)
{
    int result =  0;
    int act_len = 0;

    /* 切换为NDIS模式*/
    /* 更新为华为数据卡嵌入式Linux及Android内核驱动集成指导文档 V1.3.8 中的新命令 */
    unsigned char cmd[32] = {   0x55, 0x53, 0x42, 0x43, 
                                0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 
                                0x00, 0x00, 0x00, 0x11,
                                0x06, 0x30, 0x00, 0x00, 
                                0x01, 0x00, 0x01, 0x00,
                                0x00, 0x00, 0x00, 0x00, 
                                0x00, 0x00, 0x00}; 

    
    result = usb_stor_bulk_transfer_buf(us, us->send_bulk_pipe, cmd, 31, &act_len);
    return result;
}

extern atomic_t g_iDataCardIn;

int usb_stor_huawei_init(struct us_data *us)
{
    int result = 0;
    if(usb_stor_huawei_dongles_pid(us))
    {
        printk("-------------This is huawei dongles-------------\n");
        //目前单独适配balong芯片方案数据卡E3276
#if 0
        if(0x1446<= le16_to_cpu(us->pusb_dev->descriptor.idProduct))//目前单独适配balong芯片方案数据卡E3276
#else
        /* 适配E3276S-150的两种不同PID的数据卡 */
        if((0x1f01== le16_to_cpu(us->pusb_dev->descriptor.idProduct)) || (0x14fe== le16_to_cpu(us->pusb_dev->descriptor.idProduct)))
#endif
        {
            printk("\nBalong: begin to send Set_feature_D command\n");
            result = usb_stor_huawei_scsi_init(us);
        }else
        {
            //result = usb_stor_huawei_e220_init(us);
            printk("-------------Balong: NOT begin to send Set_feature_D command-------------\n");
        }
    }
    else
    {
        printk("-------------This is huawei NOT dongles-------------\n");
    }
    return result;
}
#endif
