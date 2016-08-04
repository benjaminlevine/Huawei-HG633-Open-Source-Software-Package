/******************************************************************************
  文件名称: hi_sfc_mtd.c
  功能描述: spiflash驱动对mtd接口，支撑文系统
  版本描述: V1.0

  创建日期: D2013_08_01
  创建作者: zhouyu 00204772

  修改记录: 
            生成初稿.
******************************************************************************/
#include <linux/sched.h>
#include <linux/mtd/mtd.h>
#include <mach/typedef.h>
#include <linux/mtd/hi_sfc.h>
#include "hi_sfc_drv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


/******************************************************************************
  函数功能:  Erase one ore more blocks
  输入参数:  
                             mtd : MTD device structure
                             instr : erase instruction
  输出参数:  无
  函数返回:  uint       
  函数备注:  
******************************************************************************/
hi_int32 hi_sfc_mtd_erase(struct mtd_info *pst_mtd, struct erase_info *pst_erase_info)
{
    hi_uint32 ui_index          = 0;
    hi_uint32 ui_offset         = (hi_uint32)pst_erase_info->addr;
    hi_uint32 ui_len            = (hi_uint32)pst_erase_info->len;
    hi_uint32 ui_erase_len      = pst_mtd->erasesize;
    struct hi_sfc_chip *pst_chip = pst_mtd->priv;
    struct hi_sfc_host *pst_host = pst_chip->pv_priv;

    HI_SFC_PRINTF("sfc_erase: start = 0x%08x, len = 0x%08x\n", ui_offset, ui_len);
    
    if (pst_erase_info->addr + pst_erase_info->len > pst_mtd->size)
    {
        HI_SFC_PRINTF("erase area out of range of mtd.\n");
        return -1;
    }

    if ((hi_uint32)pst_erase_info->addr & (pst_mtd->erasesize-1))
    {
        HI_SFC_PRINTF("erase start address is not alignment.\n");
        return -1;
    }

    if ((hi_uint32)pst_erase_info->len & (pst_mtd->erasesize-1))
    {
        HI_SFC_PRINTF("erase length is not alignment.\n");
        return -1;
    }

    if (ATP_check_fix_part_protect_addr(ui_offset) < 0)
    {
        return -1;
    }

	/*BEGIN: added by j00199467 for spiflash, 20131119*/
    mutex_lock(&pst_host->st_lock); //kernel only
	/*END: added by j00199467 for spiflash, 20131119*/
    
    while(ui_len)
    {
        if(ui_offset >= pst_chip->st_spec.ui_chip_size)
        {
            ui_offset -= pst_chip->st_spec.ui_chip_size;
            ui_index   = 1;
        }


        if ((0 == ui_offset % BLOCK_SIZE_64K) && (ui_len >= BLOCK_SIZE_64K))
        {
            ui_erase_len      = BLOCK_SIZE_64K;
        }
        else
        {
            ui_erase_len      = SECTOR_SIZE_4K;
        }


        if(hi_sfc_drv_erase_sector(pst_host, ui_index, ui_offset, ui_erase_len))
        {
            pst_erase_info->state = MTD_ERASE_FAILED;
			/*BEGIN: added by j00199467 for spiflash, 20131119*/
            mutex_unlock(&pst_host->st_lock);  //kernel only
			/*END: added by j00199467 for spiflash, 20131119*/
            return -1;
        }

        HI_SFC_PRINTF("\rErasing at 0x%08x complete.", ui_offset+(ui_index*pst_chip->st_spec.ui_chip_size));
        
        ui_offset += ui_erase_len;
        ui_len    -= ui_erase_len;
    }

    HI_SFC_PRINTF("\n");
    
    pst_erase_info->state = MTD_ERASE_DONE;
	/*START MODIFY:t00176367 20130127 for DTS2014012602752 支持coredump诊断*/
    if (pst_erase_info->callback)
        pst_erase_info->callback(pst_erase_info);
	/*END MODIFY:t00176367 20130127 for DTS2014012602752 支持coredump诊断*/
	/*BEGIN: added by j00199467 for spiflash, 20131119*/
    mutex_unlock(&pst_host->st_lock);
	/*END: added by j00199467 for spiflash, 20131119*/

    return 0;
}

/******************************************************************************
  函数功能:  write one ore more pages
  输入参数:  
                             mtd : MTD device structure
                             from : offset to read from
                             len : number of bytes to read
                             retlen : pointer to variable to store the number of written bytes
                             buf : the databuffer to put data
  输出参数:  无
  函数返回:  uint       
  函数备注:  
******************************************************************************/
hi_int32 hi_sfc_mtd_write(struct mtd_info *pst_mtd, hi_uint64 ull_dst_addr, 
                              hi_uint32 ui_len, hi_uint32 *pui_retlen, const hi_uchar8 *puc_buf)
{
    hi_uint32 ui_count = 0, ui_index = 0;
    hi_uint32 ui_write_size;
    hi_uint32 ui_write_len = ui_len;
    hi_uint32 ui_offset    = (hi_uint32)ull_dst_addr;
    hi_uchar8 *puc_write   = puc_buf;
    struct hi_sfc_chip *pst_chip = pst_mtd->priv;
    struct hi_sfc_host *pst_host = pst_chip->pv_priv;

    HI_SFC_PRINTF("\nsfc_write: offset[0x%08x] size[0x%08x]\n",ui_offset, ui_write_len);
    
    if((ui_offset + ui_write_len) > pst_mtd->size)
    {
        HI_SFC_PRINTF("write data out of range.\n");
        return -1;
    }

    *pui_retlen = 0;
    if(!ui_write_len)
    {
        HI_SFC_PRINTF("write length is 0.\n");
        return 0;
    }

    if (ATP_check_fix_part_protect_addr(ui_offset) < 0)
    {
        return -1;
    }

	/*BEGIN: added by j00199467 for spiflash, 20131119*/
    mutex_lock(&pst_host->st_lock);
	/*END: added by j00199467 for spiflash, 20131119*/

    if(ui_offset >= pst_chip->st_spec.ui_chip_size)
    {
        /* 写起始地址为片选1地址 */
        hi_sfc_drv_reg_write(pst_host, 1, puc_write, (ui_offset-pst_chip->st_spec.ui_chip_size), ui_write_len);
        //hi_sfc_drv_bus_write(pst_host, 1, puc_write, (ui_offset-pst_chip->st_spec.ui_chip_size), ui_write_len);
    }
    else if((ui_offset + ui_write_len) > pst_chip->st_spec.ui_chip_size)
    {
        /* 写起始地址为片选0地址，但写长度跨片 */
        hi_sfc_drv_reg_write(pst_host, 0, puc_write, ui_offset, (pst_chip->st_spec.ui_chip_size - ui_offset));
        //hi_sfc_drv_bus_write(pst_host, 0, puc_write, ui_offset, (pst_chip->st_spec.ui_chip_size - ui_offset));
        
        ui_write_len = ui_write_len - (pst_chip->st_spec.ui_chip_size - ui_offset);
        puc_write   += (pst_chip->st_spec.ui_chip_size - ui_offset);
        
        hi_sfc_drv_reg_write(pst_host, 1, puc_write, 0, ui_write_len);
        //hi_sfc_drv_bus_write(pst_host, 1, puc_write, 0, ui_write_len);
    }
    else
    {
        /* 只写片选0 */
        hi_sfc_drv_reg_write(pst_host, 0, puc_write, ui_offset, ui_write_len);
        //hi_sfc_drv_bus_write(pst_host, 0, puc_write, ui_offset, ui_write_len);
    }

    
	/*BEGIN: added by j00199467 for spiflash, 20131119*/
    mutex_unlock(&pst_host->st_lock);
	/*END: added by j00199467 for spiflash, 20131119*/
	
    *pui_retlen = ui_len;
    
    return 0;
}


/******************************************************************************
  函数功能:  read datas
  输入参数:  
                             mtd : MTD device structure
                             from : offset to read from
                             len : number of bytes to read
                             retlen : pointer to variable to store the number of read bytes
                             buf : the databuffer to put data
  输出参数:  无
  函数返回:  uint       
  函数备注:  
******************************************************************************/
hi_int32 hi_sfc_mtd_read(struct mtd_info *pst_mtd, hi_uint64 ull_src_addr, 
                                    hi_uint32 ui_len, hi_uint32 *pui_retlen, hi_uchar8 *puc_buf)
{
    hi_uchar8 *puc_read   = puc_buf;
    hi_uint32 ui_offset   = (hi_uint32)ull_src_addr;
    hi_uint32 ui_read_len = ui_len;
    struct hi_sfc_chip *pst_chip = pst_mtd->priv;
    struct hi_sfc_host *pst_host = pst_chip->pv_priv;

    if((ui_offset + ui_read_len) > (hi_uint32)(pst_mtd->size))
    {
        HI_SFC_PRINTF("read area out of range.\n");
        return -1;
    }

    *pui_retlen = 0;
    if(!ui_read_len)
    {
        HI_SFC_PRINTF("read length is 0.\n");
        return 0;
    }

	/*BEGIN: added by j00199467 for spiflash, 20131119*/
    mutex_lock(&pst_host->st_lock);
	/*END: added by j00199467 for spiflash, 20131119*/

    if(ui_offset >= pst_chip->st_spec.ui_chip_size)
    {
        /* 读起始地址为片选1地址 */
        hi_sfc_drv_reg_read(pst_host, 1, puc_read, (ui_offset-pst_chip->st_spec.ui_chip_size), ui_read_len);
        //hi_sfc_drv_bus_read(pst_host, 1, puc_read, (ui_offset-pst_chip->st_spec.ui_chip_size), ui_read_len);
    }
    else if((ui_offset + ui_read_len) > pst_chip->st_spec.ui_chip_size)
    {
        /* 读起始地址为片选0地址，但读长度跨片 */
        hi_sfc_drv_reg_read(pst_host, 0, puc_read, ui_offset, (pst_chip->st_spec.ui_chip_size - ui_offset));
        //hi_sfc_drv_bus_read(pst_host, 0, puc_read, ui_offset, (pst_chip->st_spec.ui_chip_size - ui_offset));
        
        ui_read_len = ui_read_len - (pst_chip->st_spec.ui_chip_size - ui_offset);
        puc_read   += (pst_chip->st_spec.ui_chip_size - ui_offset);
        
        hi_sfc_drv_reg_read(pst_host, 1, puc_read, 0, ui_read_len);
        //hi_sfc_drv_bus_read(pst_host, 1, puc_read, 0, ui_read_len);
    }
    else
    {
        /* 只读片选0 */
        hi_sfc_drv_reg_read(pst_host, 0, puc_read, ui_offset, ui_read_len);
        //hi_sfc_drv_bus_read(pst_host, 0, puc_read, ui_offset, ui_read_len);
    }

	/*BEGIN: added by j00199467 for spiflash, 20131119*/
    mutex_unlock(&pst_host->st_lock);
	/*END: added by j00199467 for spiflash, 20131119*/
	
    *pui_retlen = ui_len;
    
    return 0;
}

/******************************************************************************
  函数功能:  sfc register contact mtd struct
  输入参数:  
                             mtd : MTD device structure
  输出参数:  无
  函数返回:  uint       
  函数备注:  
******************************************************************************/
hi_int32 hi_sfc_mtd_contact(struct mtd_info *pst_mtd)
{
    struct hi_sfc_chip *pst_chip = pst_mtd->priv;

    pst_mtd->type      = MTD_NORFLASH;
    pst_mtd->flags     = MTD_CAP_NORFLASH;
    pst_mtd->owner     = THIS_MODULE;

    pst_mtd->writesize = 256;   //设置为spiflash的页大小以提高mtd写速度
    pst_mtd->erasesize = pst_chip->st_spec.ui_erase_size;
    pst_mtd->size      = pst_chip->ui_total_size;

    pst_mtd->erase     = hi_sfc_mtd_erase;
    pst_mtd->write     = hi_sfc_mtd_write;
    pst_mtd->read      = hi_sfc_mtd_read;

    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

