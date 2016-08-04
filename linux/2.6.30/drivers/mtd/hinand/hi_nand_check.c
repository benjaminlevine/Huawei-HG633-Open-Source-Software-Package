/******************************************************************************

          ��Ȩ���� (C), 2008-2018, ��˼�뵼�����޹�˾

******************************************************************************
  �ļ�����: hi_nand_check.c
  ��������: nand ����дУ��
  �汾����: V1.0

  ��������: D2014_02_29
  ��������: linqing 00204224
******************************************************************************/
#include <linux/mtd/mtd.h>
#include <mach/typedef.h>
#include <linux/mtd/hi_nand.h>
#include "hi_nand_drv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
static hi_uchar8  g_uc_page_buf[HI_NAND_MAX_PAGE_SIZE + HI_NAND_MAX_OOB_SIZE];
#define HI_RET_SUCC 0

hi_int32 hi_nand_drv_check_erase(hi_uint32 ui_dst_addr)
{
    hi_int32      i_ret         = HI_RET_SUCC;
    hi_uint32     ui_block_size = hi_nand_get_block_size();
    hi_uint32     ui_page_size  = hi_nand_get_page_size();
    hi_uint32     ui_page_addr  = ui_dst_addr;    
    struct hi_nand_host *pst_nand_host = hi_nand_get_nand_host();
    hi_uint32 ui_cnt = 0;
    
    while(ui_block_size)
    {
        hi_memset( g_uc_page_buf, 0, ui_page_size );
        // ��һ��page����,Ԥ�ڽ��Ϊȫ0xff
        i_ret = hi_nand_drv_read( pst_nand_host, g_uc_page_buf, ui_page_addr);
        if( HI_RET_SUCC != i_ret )
        {
            hi_printf("[check erase] read page error\n");
            return -1;
        }
        
        for ( ui_cnt=0; ui_cnt<ui_page_size; ui_cnt++ )
        {
            if ( 0xff != g_uc_page_buf[ui_cnt] )
            {
                hi_printf("[check erase]data error at page:%x - page offset:%x - data:%x\n",ui_page_addr,ui_cnt,g_uc_page_buf[ui_cnt]);
                return -1;
            }
        }
        
        // ��ǰƫ��һ��page
        ui_block_size   -= ui_page_size;
        ui_page_addr ++;
    }
    return 0;
}

hi_int32 hi_nand_drv_check_write(hi_uchar8 *puc_buf,hi_uint32 ui_page_addr)
{
    hi_int32      i_ret         = HI_RET_SUCC;
    hi_uint32     ui_page_size  = hi_nand_get_page_size();
    hi_uint32     ui_cnt =0;
    struct hi_nand_host *pst_nand_host = hi_nand_get_nand_host();

    hi_memset( g_uc_page_buf, 0x0, ui_page_size );
    
    // ��һ��page���ݣ�Ԥ�ڽ��Ϊ
    i_ret = hi_nand_drv_read( pst_nand_host, g_uc_page_buf, ui_page_addr);
    if( HI_RET_SUCC != i_ret )
    {
        hi_printf("[check write] read page error\n");
        return -1;
    }
    for ( ui_cnt = 0; ui_cnt < ui_page_size; ui_cnt++ )
    {
        if ( puc_buf[ui_cnt] != g_uc_page_buf[ui_cnt] )
        {
            hi_printf("[check write] data error at page:%x - page offset:%x - [write data]:%x:[read check data]:%x\n",ui_page_addr,ui_cnt,puc_buf[ui_cnt],g_uc_page_buf[ui_cnt]);
            return -1;
        }
    }
    
    return 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

