/******************************************************************************
  �ļ�����: hi_nand_util.c
  ��������: nandֱ�Ӳ����ӿڣ�֧��nandcmd/nandutil��
  �汾����: V1.0

  ��������: D2013_04_20
  ��������: zhouyu 00204772

  �޸ļ�¼: 
            ���ɳ���.
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


/******************************************************************************
  ��������:  nand erase
  �������:  
                             host : nand driver host structure
                             ui_dst_addr : srase start block addrs
                             ui_len : erase len
  �������:  ��
  ��������:         
  ������ע:  
******************************************************************************/
hi_int32 hi_nand_util_erase(struct hi_nand_host *pst_host, hi_uint32 ui_dst_addr, hi_uint32 ui_len)
{
    hi_int32  i_ret;
    hi_uint32 ui_page_addr, ui_pages_per_block;
    hi_uint32 ui_erase_addr       = ui_dst_addr;
    hi_uint32 ui_erase_len        = ui_len;
    hi_uint32 ui_retry_time;
    
    struct hi_nand_chip *pst_chip = pst_host->pst_chip;

    HI_NAND_PRINTF("nand_erase: start = 0x%08x, len = 0x%08x\n", ui_dst_addr, ui_len);

    /* Start address must align on block boundary */
    if (ui_dst_addr & ((1 << pst_chip->st_spec.ui_erase_shift) - 1))
    {
        hi_printf("nand_erase: Unaligned address\n");
        return -1;
    }

    /* Length must align on block boundary */
    if (ui_len & ((1 << pst_chip->st_spec.ui_erase_shift) - 1))
    {
        hi_printf("nand_erase: Length not block aligned\n");
        return -1;
    }

    /* Do not allow erase past end of device */
    if ((ui_len + ui_dst_addr) > pst_chip->st_spec.ui_chip_size)
    {
        hi_printf("nand_erase: Erase past end of device\n");
        return -1;
    }

    /* Shift to get first page */
    ui_page_addr = ui_dst_addr >> pst_chip->st_spec.ui_page_shift;

    /* Calculate pages in each block */
    ui_pages_per_block = 1 << (pst_chip->st_spec.ui_erase_shift - pst_chip->st_spec.ui_page_shift);

    /* Check, if it is write protected */
    if (hi_nand_drv_wp_check()) 
    {
        hi_printf("nand_erase: Device is write protected!!!\n");
        return -1;
    }

    while(ui_erase_len) 
    {
        /* heck if we have a bad block, we do not erase bad blocks ! */
        if(hi_nand_block_isbad(ui_erase_addr))
        {
            hi_printf("\r\n skipping bad block at 0x%08x\n", ui_erase_addr);

            ui_page_addr  += ui_pages_per_block;
            ui_erase_addr += pst_chip->st_spec.ui_block_size;
            ui_erase_len  -= pst_chip->st_spec.ui_block_size;                   
            continue;
        }
        
        /*Due to voltage spikes and other hardware reasons, the erase operation may fail in this case.
        in order to prevent misuse marked bad blocks, we need to erase failed retries*/
        for ( ui_retry_time = 0 ;ui_retry_time < HI_NAND_ERASE_WRITE_RETRY_TIME;ui_retry_time++)
        {
            i_ret = hi_nand_drv_erase(ui_page_addr);
            if(0 == i_ret)
            {
                break;
            }
        }

                
        if (i_ret)
        {
            hi_printf("nand block erase failure at 0x%08x\n", ui_erase_addr);
            
            if(hi_nand_block_markbad(ui_erase_addr))
            {
                hi_printf("nand block markbad at 0x%08x failed,nand erase abort!\n", ui_erase_addr);

                return -1;
            }
        }
        else
        {
            HI_NAND_PRINTF("\rErasing at 0x%08x complete.", ui_erase_addr);
        }

        /* format for JFFS2 ? */
        //......
        
        /* Increment page address and decrement length */
        ui_page_addr  += ui_pages_per_block;
        ui_erase_addr += pst_chip->st_spec.ui_block_size;
        ui_erase_len  -= pst_chip->st_spec.ui_block_size;
    }

    return 0;
}

/******************************************************************************
  ��������:  nand read & write & read/write whole page entry (do not support read/write oob only!)
  �������:  
                             host : nand driver host structure
                             pst_nand_ops : nand operation structure
  �������:  ��
  ��������:         
  ������ע:  
******************************************************************************/
static hi_int32 hi_nand_util_rw(struct hi_nand_host *pst_host, hi_nand_ops *pst_nand_ops)
{
    hi_int32 i_ret;
    hi_uint32 ui_page_addr=0;
    hi_uint32 ui_block_addr=0;
    hi_uint32 ui_rw_len=0;
    hi_uint32 ui_count = 0;
    hi_uint32 ui_pre_block_addr=0;
    hi_uint32 ui_page_used=0;
    hi_uint32 ui_left_to_rw       = pst_nand_ops->ui_data_len;
    hi_uchar8 *puc_data_buf       = pst_nand_ops->puc_data_buf;
    struct hi_nand_chip *pst_chip = pst_host->pst_chip;
    hi_uchar8 *puc_page_buf       = pst_chip->pst_buffers->uac_databuf;
    hi_uint32 ui_retry_time;

    if((HI_NULL == pst_nand_ops) || (0 == pst_nand_ops->ui_data_len) || (HI_NULL == puc_data_buf)
        || ((HI_NAND_READ_E != pst_nand_ops->en_op_type) && (HI_NAND_WRITE_E != pst_nand_ops->en_op_type)))
    {
        return -1;
    }

    /* start address must align on page boundary */
    if(pst_nand_ops->ui_addr & (pst_chip->st_spec.ui_page_size-1)) 
    {
        hi_printf("error! addr[0x%08x] not page size aligned!\n",  pst_nand_ops->ui_addr);
        return -1;
    }

    /* do not allow read past end of device */
    if((pst_nand_ops->ui_addr+pst_nand_ops->ui_data_len) > pst_chip->st_spec.ui_chip_size) 
    {
        hi_printf("error! operate size[0x%08x] past end of device!\n",  pst_nand_ops->ui_addr+pst_nand_ops->ui_data_len);
        return -1;
    }

    /* get page addr */
    ui_page_addr  = pst_nand_ops->ui_addr >> pst_chip->st_spec.ui_page_shift;

    while(ui_left_to_rw) 
    {
        /* get the block addr of this page */
        ui_block_addr = (ui_page_addr << pst_chip->st_spec.ui_page_shift) & (~(pst_chip->st_spec.ui_block_size - 1));

		if(ui_pre_block_addr !=ui_block_addr)
		{
		     ui_page_used = 0;
		     ui_pre_block_addr = ui_block_addr;
		}

        if(HI_TRUE == pst_nand_ops->ui_check_bad)
        {
            if(hi_nand_block_isbad(ui_block_addr))
            {
                hi_printf("\r\nskipping bad block at 0x%08x\n", ui_block_addr);

                /* jump to read/write next block */
                ui_block_addr += pst_chip->st_spec.ui_block_size;
                ui_page_addr   = ui_block_addr >> pst_chip->st_spec.ui_page_shift;
                continue;
            }
        }

        ui_rw_len = (ui_left_to_rw < pst_chip->st_spec.ui_page_size)? ui_left_to_rw : pst_chip->st_spec.ui_page_size;

        hi_memset(puc_page_buf, 0xff, pst_chip->st_spec.ui_page_size + pst_chip->st_spec.ui_oob_size);

        if(HI_NAND_WRITE_E == pst_nand_ops->en_op_type)
        {
            hi_memcpy(puc_page_buf, puc_data_buf, ui_rw_len);

            if(HI_NULL != pst_nand_ops->puc_oob_buf)
            {
                hi_memcpy(puc_page_buf + pst_chip->st_spec.ui_page_size, pst_nand_ops->puc_oob_buf, pst_chip->st_spec.ui_oob_size);
            }

            ui_count++;
            if(64 == ui_count)
            {
                ui_count = 0;
                hi_printf(".");
            }

            /*Due to voltage spikes and other hardware reasons, the write operation may fail in this case.
            in order to prevent misuse marked bad blocks, we need to write failed retries*/
            for ( ui_retry_time = 0 ;ui_retry_time < HI_NAND_ERASE_WRITE_RETRY_TIME;ui_retry_time++)
            {
                i_ret = hi_nand_drv_write(pst_host, puc_page_buf, ui_page_addr);
                if(0 == i_ret)
                {
                    break;
                }
            }
        }
        else
        {
            i_ret = hi_nand_drv_read(pst_host, puc_page_buf, ui_page_addr);
        }
        
        if(i_ret)
        {
            hi_printf("nand %s page failure at 0x%08x\n", (pst_nand_ops->en_op_type == HI_NAND_WRITE_E)?"write":"read", ui_page_addr << pst_chip->st_spec.ui_page_shift);

            if(HI_TRUE == pst_nand_ops->ui_check_bad)
            {
                if(hi_nand_block_markbad(ui_block_addr))
                {
                    hi_printf("nand block markbad at 0x%08x failed,nand read/write abort!\n", ui_block_addr);
                    return -1;
                }

                hi_printf("\r\n skipping bad block at 0x%08x\n", ui_block_addr & (~(pst_chip->st_spec.ui_block_size - 1)));
                
                /* junp to read/write next block */
                ui_block_addr += pst_chip->st_spec.ui_block_size;                
                ui_block_addr  = ui_block_addr & (~(pst_chip->st_spec.ui_block_size - 1));
                puc_data_buf  -= (ui_page_used << pst_chip->st_spec.ui_page_shift) ;
                ui_left_to_rw +=  (ui_page_used << pst_chip->st_spec.ui_page_shift) ;
                ui_page_addr   = ui_block_addr >> pst_chip->st_spec.ui_page_shift;
				
                hi_printf("\r\n block addr:0x%08x data buf addr:0x%08x page addr:0x%08x left to rw:0x%08x\n", ui_block_addr,puc_data_buf,ui_page_addr<< pst_chip->st_spec.ui_page_shift,ui_left_to_rw);
                continue;
            }

            return -1;
        }

        if(HI_NAND_READ_E == pst_nand_ops->en_op_type)
        {
            hi_memcpy(puc_data_buf, puc_page_buf, ui_rw_len);
            
            if(HI_NULL != pst_nand_ops->puc_oob_buf)
            {
                hi_memcpy(pst_nand_ops->puc_oob_buf, puc_page_buf+pst_chip->st_spec.ui_page_size, pst_chip->st_spec.ui_oob_size);
            }
        }
        
        puc_data_buf    += ui_rw_len;
        ui_block_addr   += ui_rw_len;
        ui_left_to_rw   -= ui_rw_len;
        ui_page_addr++;
	 	ui_page_used++;	
    }

    return 0;

}

/******************************************************************************
  ��������:  nand read (data only!)
  �������:  
                             host : nand driver host structure
                             puc_dst_buf : the databuffer to put data
                             ui_src_addr : read start page addrs
                             ui_len : read len
  �������:  ��
  ��������:         
  ������ע:  
******************************************************************************/
hi_int32 hi_nand_util_read(struct hi_nand_host *pst_host, hi_uchar8 *puc_dst_buf, hi_uint32 ui_src_addr, hi_uint32 ui_len)
{
    hi_nand_ops st_nand_ops;

    st_nand_ops.ui_check_bad = HI_TRUE;
    st_nand_ops.ui_addr      = ui_src_addr;
    st_nand_ops.en_op_type   = HI_NAND_READ_E;
    st_nand_ops.puc_data_buf = puc_dst_buf;
    st_nand_ops.ui_data_len  = ui_len;
    st_nand_ops.puc_oob_buf  = HI_NULL;

    return hi_nand_util_rw(pst_host, &st_nand_ops);
}

/******************************************************************************
  ��������:  nand read one whole page (include data & oob)
  �������:  
                             host : nand driver host structure
                             puc_dst_buf : the databuffer to put data
                             ui_src_addr : read start page addrs
                             ui_len : read len
  �������:  ��
  ��������:         
  ������ע:  
******************************************************************************/
hi_int32 hi_nand_util_read_page(struct hi_nand_host *pst_host, hi_uchar8 *puc_dst_buf, hi_uint32 ui_src_addr)
{
    hi_nand_ops st_nand_ops;
    struct hi_nand_chip *pst_chip = pst_host->pst_chip;

    st_nand_ops.ui_check_bad = HI_FALSE;
    st_nand_ops.ui_addr      = ui_src_addr;
    st_nand_ops.en_op_type   = HI_NAND_READ_E;
    st_nand_ops.puc_data_buf = puc_dst_buf;
    st_nand_ops.ui_data_len  = pst_chip->st_spec.ui_page_size;
    st_nand_ops.puc_oob_buf  = puc_dst_buf+pst_chip->st_spec.ui_page_size;

    return hi_nand_util_rw(pst_host, &st_nand_ops);
}

/******************************************************************************
  ��������:  nand write (data only)
  �������:  
                             host : nand driver host structure
                             puc_src_buf : the databuffer to put data
                             ui_dst_addr : write start page addrs
  �������:  ��
  ��������:         
  ������ע:  
******************************************************************************/
hi_int32 hi_nand_util_write(struct hi_nand_host *pst_host, hi_uchar8 *puc_src_buf, hi_uint32 ui_dst_addr, hi_uint32 ui_len)
{
    hi_nand_ops st_nand_ops;

    HI_NAND_PRINTF("\n");
    HI_NAND_PRINTF("\nNAND write: offset[0x%08x] size[0x%08x]\n",ui_dst_addr, ui_len);

    st_nand_ops.ui_check_bad = HI_TRUE;
    st_nand_ops.ui_addr      = ui_dst_addr;
    st_nand_ops.en_op_type   = HI_NAND_WRITE_E;
    st_nand_ops.puc_data_buf = puc_src_buf;
    st_nand_ops.ui_data_len  = ui_len;
    st_nand_ops.puc_oob_buf  = HI_NULL;

    return hi_nand_util_rw(pst_host, &st_nand_ops);
}

/******************************************************************************
  ��������:  nand write one whole page (include data & oob)
  �������:  
                             host : nand driver host structure
                             puc_src_buf : the databuffer to put data
                             ui_dst_addr : write start page addrs
  �������:  ��
  ��������:         
  ������ע:  
******************************************************************************/
hi_int32 hi_nand_util_write_page(struct hi_nand_host *pst_host, hi_uchar8 *puc_src_buf, hi_uint32 ui_dst_addr)
{
    hi_nand_ops st_nand_ops;
    struct hi_nand_chip *pst_chip = pst_host->pst_chip;

    hi_printf("\n");
    hi_printf("\nNAND write: offset[0x%08x] size[0x%08x]\n",ui_dst_addr, pst_chip->st_spec.ui_page_size);

    st_nand_ops.ui_check_bad = HI_FALSE;
    st_nand_ops.ui_addr      = ui_dst_addr;
    st_nand_ops.en_op_type   = HI_NAND_WRITE_E;
    st_nand_ops.puc_data_buf = puc_src_buf;
    st_nand_ops.ui_data_len  = pst_chip->st_spec.ui_page_size;
    st_nand_ops.puc_oob_buf  = puc_src_buf+pst_chip->st_spec.ui_page_size;

    return hi_nand_util_rw(pst_host, &st_nand_ops);
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

