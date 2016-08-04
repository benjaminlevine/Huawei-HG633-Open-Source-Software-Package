/*
    hw_hspa.h - struct for support huawei datacard devices

    * Initial work by:    
     *   (c) 20090508 David Lv (l00135113@huawei.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __LINUX_HW_HSPA_H
#define __LINUX_HW_HSPA_H

#include <linux/kref.h>
#include <linux/mutex.h>

#define HUAWEI_VENDOR_ID	0x12D1
#define HUAWEI_VENDOR_LITTLE_ENDIAN_ID	0xD112
#define ZTE_VENDOR_ID	0xd219
#define ZTE_VENDOR_LITTLE_ENDIAN_ID	0x19d2
#define BANDLUXE_C270_VENDOR_ID	0x1A8D
#define BANDLUXE_C270_LITTLE_ENDIAN_VENDOR_ID	0x8D1A
#define HUAWEI_HSPA_CARD	"Huawei"
#define BANDLUXE_HSPA_CARD	"Bandluxe"
#define ZTE_HSPA_CARD	"ZTE"
/*hw_datacard struct,to match the pid and proc_users_umts string*/
 struct hw_datacard_id 
{
    __u16 pid;

    const char * portString;
};


#define HUAWEI_DATACARD_PID_GT1001	0x1001
#define HUAWEI_DATACARD_PID_GT1003	0x1003
#define HUAWEI_DATACARD_PID_GT1004	0x1004
#define HUAWEI_DATACARD_PID_GT1401	0x1401
#define HUAWEI_DATACARD_PID_GT1402	0x1402
#define HUAWEI_DATACARD_PID_GT1403	0x1403
#define HUAWEI_DATACARD_PID_GT1404	0x1404
#define HUAWEI_DATACARD_PID_GT1405	0x1405
#define HUAWEI_DATACARD_PID_GT1406	0x1406
#define HUAWEI_DATACARD_PID_GT1407	0x1407
#define HUAWEI_DATACARD_PID_GT1408	0x1408
#define HUAWEI_DATACARD_PID_GT1409	0x1409
#define HUAWEI_DATACARD_PID_GT1410	0x1410
#define HUAWEI_DATACARD_PID_GT1411	0x1411
#define HUAWEI_DATACARD_PID_GT1412	0x1412
#define HUAWEI_DATACARD_PID_GT1413	0x1413
#define HUAWEI_DATACARD_PID_GT1414	0x1414
#define HUAWEI_DATACARD_PID_GT1415	0x1415
#define HUAWEI_DATACARD_PID_GT1416	0x1416
#define HUAWEI_DATACARD_PID_GT1417	0x1417
#define HUAWEI_DATACARD_PID_GT1418	0x1418
#define HUAWEI_DATACARD_PID_GT1419	0x1419
#define HUAWEI_DATACARD_PID_GT1420	0x1420
#define HUAWEI_DATACARD_PID_GT1421	0x1421
#define HUAWEI_DATACARD_PID_GT1422	0x1422
#define HUAWEI_DATACARD_PID_GT1426	0x1426
#define HUAWEI_DATACARD_PID_GT1427	0x1427
#define HUAWEI_DATACARD_PID_GT1428	0x1428
#define HUAWEI_DATACARD_PID_GT1429	0x1429
#define HUAWEI_DATACARD_PID_GT1430	0x1430
#define HUAWEI_DATACARD_PID_GT1431	0x1431
#define HUAWEI_DATACARD_PID_GT1432	0x1432
#define HUAWEI_DATACARD_PID_GT1433	0x1433
#define HUAWEI_DATACARD_PID_GT1434	0x1434
#define HUAWEI_DATACARD_PID_GT1435	0x1435
#define HUAWEI_DATACARD_PID_GT1436	0x1436
#define HUAWEI_DATACARD_PID_GT1437	0x1437
#define HUAWEI_DATACARD_PID_GT1438	0x1438
#define HUAWEI_DATACARD_PID_GT1439	0x1439
#define HUAWEI_DATACARD_PID_GT1443	0x1443
#define HUAWEI_DATACARD_PID_GT1446	0x1446
#define HUAWEI_DATACARD_PID_GT1448	0x1448
#define HUAWEI_DATACARD_PID_GT140A	0x140A
#define HUAWEI_DATACARD_PID_GT140B	0x140B
#define HUAWEI_DATACARD_PID_GT140C	0x140C
#define HUAWEI_DATACARD_PID_GT140E	0x140E
#define HUAWEI_DATACARD_PID_GT141A	0x141A
#define HUAWEI_DATACARD_PID_GT141B	0x141B
#define HUAWEI_DATACARD_PID_GT141C	0x141C
#define HUAWEI_DATACARD_PID_GT141D	0x141D
#define HUAWEI_DATACARD_PID_GT141E	0x141E
#define HUAWEI_DATACARD_PID_GT141F	0x141F
#define HUAWEI_DATACARD_PID_GT142A	0x142A
#define HUAWEI_DATACARD_PID_GT142B	0x142B
#define HUAWEI_DATACARD_PID_GT142C	0x142C
#define HUAWEI_DATACARD_PID_GT142D	0x142D
#define HUAWEI_DATACARD_PID_GT142E	0x142E
#define HUAWEI_DATACARD_PID_GT142F	0x142F
#define HUAWEI_DATACARD_PID_GT143A	0x143A
#define HUAWEI_DATACARD_PID_GT143B	0x143B
#define HUAWEI_DATACARD_PID_GT143E	0x143E
#define HUAWEI_DATACARD_PID_GT143F	0x143F
#define HUAWEI_DATACARD_PID_GT144A	0x144A
#define HUAWEI_DATACARD_PID_GT144B	0x144B
#define HUAWEI_DATACARD_PID_GT144C	0x144C
#define HUAWEI_DATACARD_PID_GT144D	0x144D
#define HUAWEI_DATACARD_PID_GT14AC	0x14AC   //E153u,E1820,E367u
/* BEGIN: Added by c106292, 2010/1/13   PN:支持Vodafone的K3765数据卡*/
#define HUAWEI_DATACARD_PID_GT1520	0x1520
/* END:   Added by c106292, 2010/1/13 */

/*BEDIN:Add by l00193995, 2011/11/22   PN:  HG532c  埃及和南非K系列数据卡定制需求*/
#define HUAWEI_DATACARD_PID_GT1464	0x1464  //K4505
#define HUAWEI_DATACARD_PID_GT1465	0x1465  //K3765
#define HUAWEI_DATACARD_PID_GT14BB	0x14BB  //K3770
#define HUAWEI_DATACARD_PID_GT14C6	0x14C6  //K4605
#define HUAWEI_DATACARD_PID_GT14C9	0x14C9  //K3770
/*END: Add by l00193995,  2011/11/22 */

/*BEDIN:Add by l00193995, 2011/11/22   PN:  HG532c  南非E系列巴龙数据卡定制需求*/

#define HUAWEI_DATACARD_PID_E173S	0x1C05  //E173S

/*END: Add by l00193995,  2011/11/22 */

/*BEDIN:Add by l00193995, 2011/11/22   PN:  HG532c  南非E系列高通数据卡定制需求*/
#define HUAWEI_DATACARD_PID_GT14A5	0x14A5  //E173U
#define HUAWEI_DATACARD_PID_GT14A8	0x14A8  //E173U
#define HUAWEI_DATACARD_PID_GT1C23	0x1C23  //E173U
#define HUAWEI_DATACARD_PID_GT1506	0x1506  //E367U
/*END: Add by l00193995,  2011/11/22 */

/*分给TD方案的PID*/
#define HUAWEI_DATACARD_PID_TD1D02	0x1D02
#define HUAWEI_DATACARD_PID_TD1D03	0x1D03
#define HUAWEI_DATACARD_PID_TD1D04	0x1D04
#define HUAWEI_DATACARD_PID_TD1D05	0x1D05
#define HUAWEI_DATACARD_PID_TD1D06	0x1D06
#define HUAWEI_DATACARD_PID_TD1D07	0x1D07
#define HUAWEI_DATACARD_PID_TD1D08	0x1D08
#define HUAWEI_DATACARD_PID_TD1D09	0x1D09
#define HUAWEI_DATACARD_PID_TD1D0A	0x1D0A
#define HUAWEI_DATACARD_PID_TD1D0B	0x1D0B
#define HUAWEI_DATACARD_PID_TD1D0C	0x1D0C

#define HUAWEI_DATACARD_PID_GT1009    0x1009
#define HUAWEI_DATACARD_PID_GT9002    0x9002
#define HUAWEI_DATACARD_PID_GT1C05    0x1C05
#define HUAWEI_DATACARD_PID_GT9401    0x9401

#define HUAWEI_VENDOR_ID	0x12D1
#define HUAWEI_VENDOR_LITTLE_ENDIAN_ID	0xD112

//j00202470 for zte mf serial support
#define ZTE_DATACARD_PID_X220S  0x0017
#define ZTE_DATACARD_PID_X080S  0x0000
#define ZTE_DATACARD_VID_MF112  0x19d2
#define ZTE_DATACARD_PID_MF112  0x0031

//MF626+ : PID 0x0031, VID 0x19d2
#define ZTE_DATACARD_VID_MF626  0x19d2
#define ZTE_DATACARD_PID_MF626  0x0031 
#define ZTE_DATACARD_PID_MF626A  0x3100
#define ZTE_DATACARD_PID_MF626B  0x5001
#define ZTE_DATACARD_PID_MF626C  0x0150

//MF668:    PID 0x0124, VID 0x19d2
#define ZTE_DATACARD_VID_MF668  0x19d2
#define ZTE_DATACARD_PID_MF668  0x0124 

#define ZTE_DATACARD_VID_MF112  0x19d2
#define ZTE_DATACARD_PID_MF112  0x0031


#define ZTE_DATACARD_VID_MF193  0x19d2
#define ZTE_DATACARD_PID_MF193  0x0117

#define ZTE_DATACARD_VID_MF190  0x19d2
#define ZTE_DATACARD_PID_MF190  0x0124

#define ZTE_DATACARD_VID_MF190_UNI       0x19D2
#define ZTE_DATACARD_PID_MF190_UNI       0x2000

#define ZTE_DATACARD_PID_MF190_UNI_R       0x0020

#define ZTE_DATACARD_VID_K5006_Z 0x19d2
#define ZTE_DATACARD_PID_K5006_Z 0x1018

#define HA35_LTE_MODEL_VID 0x12d1
#define HA35_LTE_MODEL_PID 0x1506
#define HA35_MODEM_VOICE_PCUI_NDIS "modem:ttyUSB1\nndis:qmitty0\nvoice:ttyUSB3\npcui:ttyUSB2\n"

#define ZTE_HSPA_CARD	"ZTE"
#define ZTE_UNKONWN2_PCUI_MODEM     "modem:ttyUSB4\nunknown0:ttyUSB2\nunknown1:ttyUSB3\ndiag:ttyUSB0\npcui:ttyUSB1\n"
#define ZTE_UNKONWN3_PCUI_MODEM     "modem:ttyUSB4\nunknown0:ttyUSB0\nunknown1:ttyUSB1\nunknown2:ttyUSB2\npcui:ttyUSB3\n"
#define ZTE_DIAG_PCUI_MODEM         "modem:ttyUSB2\ndiag:ttyUSB0\npcui:ttyUSB1\n"

//Diag, Puerto AT, Modem y T card //forMF626+
#define ZTE_4POINT_DIAG_PCUI_MODEM_CARD   "modem:ttyUSB2\ndiag:ttyUSB0\npcui:ttyUSB1\n"

//Diag, Puerto AT, Puerto Expansion AT, Modem, T card, Simreader y NDIS //forMF668
#define ZTE_7POINT_DIAG_PCUI_EXPANSION_MODEM_CARD_READER_NDIS   "modem:ttyUSB3\ndiag:ttyUSB0\npcui:ttyUSB1\nunknown1:ttyUSB2\n"



/*the interfaces' order: modem+diag+pcui*/
#define MODEM_DIAG_PCUI         "modem:ttyUSB0\nvoice:ttyUSB1\npcui:ttyUSB2\n"

/*the interfaces' order: modem+pcui*/
#define MODEM_PCUI                  "modem:ttyUSB0\npcui:ttyUSB1\n"

/*the interfaces' order: MDM+NDIS+DIAG+PCUI*/
#define MODEM_NDIS_DIAG_PCUI  "modem:ttyUSB0\nndis:ttyUSB1\nvoice:ttyUSB2\npcui:ttyUSB3\n"

#define MODEM_NULL_DIAG_PCUI  "modem:ttyUSB0\nvoice:ttyUSB2\npcui:ttyUSB3\n"

/*the interfaces' order:MDM+HID+DIAG+PCUI*/
#define MODEM_HID_DIAG_PCUI     "modem:ttyUSB0\nhid:ttyUSB1\nvoice:ttyUSB2\npcui:ttyUSB3\n"

/*the interfaces' order:MDM+NDIS+PCUI*/
#define MODEM_NDIS_PCUI         "modem:ttyUSB0\nndis:ttyUSB1\npcui:ttyUSB2\n"

#define ZTE_K5006_Z_NDIS_MODEM_DIAG_PCUI    "modem:ttyUSB2\nndis:qmitty0\nvoice:ttyUSB0\npcui:ttyUSB1\n"

#define NDIS_PCUI            "modem:ttyUSB0\nndis:qmitty0\npcui:ttyUSB1\ndiag:ttyUSB2\n"
/*the interface's order:MDM+PCUI*/

/* RNDIS MF667 DEVICE, NOTICE: THERE IS A COMMOM MF667 DEIVCEI */
#define ZTE_MF667_MODEM_MODE       "modem:ttyUSB2\ndiag:ttyUSB0\npcui:ttyUSB1\nrestorefactory\nnotreboot\n"


/*为什么有个相同的定义?注释by w00190448*/
//#define MODEM_PCUI         "modem:ttyUSB0\npcui:ttyUSB2\n"
/*the interfaces' order: modem+diag+pcui*/
#define MODEM_DIAG_PCUI_NDIS         "modem:ttyUSB0\nvoice:ttyUSB1\npcui:ttyUSB2\nndis:ttyUSB3\n"
/*the interfaces' order: modem+ndis+diag+pcui*/
/*正常情况下语音数据走diag口,所以这里需要把ndiag:ttyUSB1改成nvoice:ttyUSB1,否则导致进行语音业务时找不到语音输出端口使语音业务异常*/
#define MODEM_QMI_DIAG_PCUI         "modem:ttyUSB0\nndis:qmitty0\nvoice:ttyUSB1\npcui:ttyUSB2\n"
#endif	/* ifdef __LINUX_HW_HSPA_H */
