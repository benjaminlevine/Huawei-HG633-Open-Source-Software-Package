/******************************************************************************
  文件名称: hi_sfc_drv.c
  功能描述: spiflash核心驱动-- 控制器(sfc)驱动
  版本描述: V1.0

  创建日期: D2013_08_01
  创建作者: zhouyu 00204772

  修改记录: 
            生成初稿.
******************************************************************************/
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/dma-mapping.h>
#include <linux/mtd/mtd.h>
#include <mach/kexport.h>
#include <mach/typedef.h>
#include <linux/mtd/hi_sfc.h>
#include "hi_sfc_drv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define HI_SPI_CMD_SE_4K            0x20   /* 4KB sector Erase */ 
#define HI_SPI_CMD_SE               0xD8   /* 64KB Sector Erase */

/* global sfc reg define */
static hi_sfc_reg_s *g_pst_sfc_reg;
hi_uchar8 g_chip_mk_id;

/******************************************************************************
  函数功能:  wait for spi status ready or time out
  输入参数:  
                             ui_chip_sel : chip select
  输出参数:  无
  函数返回:  ui_status       
  函数备注:  
******************************************************************************/
static hi_int32 hi_sfc_drv_wait_ready(hi_uint32 ui_chip_sel)
{
    hi_uint32    ui_count;
    hi_uint32    ui_status;
    hi_sfc_cmd_config_u un_reg_sfc_cmd_config;

    ui_count = jiffies + HI_SFC_MAX_READY_WAIT_JIFFIES;
    
    do
    {
        hi_memset(&un_reg_sfc_cmd_config, 0, sizeof(hi_sfc_cmd_config_u));

        /* 发送读状态指令 */
        HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_ins, HI_SPI_CMD_RDSR);

        /* 设置命令寄存器 */
        un_reg_sfc_cmd_config.st_bits.sel_cs    = ~(ui_chip_sel&0x1);
        un_reg_sfc_cmd_config.st_bits.data_cnt  = 0;              /* 实际读/写字节个数= 寄存器值+1(请参考寄存器手册) */
        un_reg_sfc_cmd_config.st_bits.data_en   = HI_TRUE;
        un_reg_sfc_cmd_config.st_bits.start     = HI_TRUE;
        un_reg_sfc_cmd_config.st_bits.rw        = 1;
        HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);

        /* 等待操作完成 */
        do
        {
            HI_REG_READ(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);
        }while(HI_TRUE == un_reg_sfc_cmd_config.st_bits.start);

        /* 读取器件返回的状态值 */
        HI_REG_READ(&g_pst_sfc_reg->ui_sfc_cmd_databuf1, ui_status);
        if(!(ui_status & HI_SPI_CMD_SR_WIP))
        {
            return 0;
        }
       cond_resched(); 
    }while (!time_after_eq(jiffies, ui_count));

    hi_printf("!!! Wait spi flash ready timeout.\n");
    
    return -1;
}

/******************************************************************************
  函数功能:  enable spi write
  输入参数:  
                             ui_chip_sel : chip select
  输出参数:  无
  函数返回:         
  函数备注:  
******************************************************************************/
static hi_int32 hi_sfc_drv_write_enable(hi_uint32 ui_chip_sel)
{
    hi_sfc_cmd_config_u un_reg_sfc_cmd_config;
        
    hi_memset(&un_reg_sfc_cmd_config, 0, sizeof(hi_sfc_cmd_config_u));

    /* 发送写使能指令 */
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_ins, HI_SPI_CMD_WREN);

    /* 设置命令寄存器 */
    un_reg_sfc_cmd_config.st_bits.sel_cs    = ~(ui_chip_sel&0x1);
    un_reg_sfc_cmd_config.st_bits.start     = HI_TRUE;
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);

    /* 等待操作完成 */
    do
    {
        HI_REG_READ(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);
    }while(HI_TRUE == un_reg_sfc_cmd_config.st_bits.start);

    return 0;
}

/******************************************************************************
  函数功能:  get spi status reg value
  输入参数:  
                             ui_chip_sel : chip select
  输出参数:  无
  函数返回:  ui_status       
  函数备注:  
******************************************************************************/
hi_uchar8 hi_sfc_drv_get_status_reg(hi_uint32 ui_chip_sel)
{
    hi_uint32    ui_count;
    hi_uint32    ui_status;
    hi_sfc_cmd_config_u un_reg_sfc_cmd_config;

    ui_count = jiffies + HI_SFC_MAX_READY_WAIT_JIFFIES;
    
    do
    {
        hi_memset(&un_reg_sfc_cmd_config, 0, sizeof(hi_sfc_cmd_config_u));

        /* 发送读状态指令 */
        HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_ins, HI_SPI_CMD_RDSR);

        /* 设置命令寄存器 */
        un_reg_sfc_cmd_config.st_bits.sel_cs    = ~(ui_chip_sel&0x1);
        un_reg_sfc_cmd_config.st_bits.data_cnt  = 0;              /* 实际读/写字节个数= 寄存器值+1(请参考寄存器手册) */
        un_reg_sfc_cmd_config.st_bits.data_en   = HI_TRUE;
        un_reg_sfc_cmd_config.st_bits.start     = HI_TRUE;
        un_reg_sfc_cmd_config.st_bits.rw        = 1;
        HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);

        /* 等待操作完成 */
        do
        {
            HI_REG_READ(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);
        }while(HI_TRUE == un_reg_sfc_cmd_config.st_bits.start);

        /* 读取器件返回的状态值 */
        HI_REG_READ(&g_pst_sfc_reg->ui_sfc_cmd_databuf1, ui_status);
        if(!(ui_status & HI_SPI_CMD_SR_WIP))
        {
            return (hi_uchar8)ui_status;
        }
    }while (!time_after_eq(jiffies, ui_count));

    return 0;

}

/******************************************************************************
  函数功能:  get spi config reg value
  输入参数:  
                             ui_chip_sel : chip select
  输出参数:  无
  函数返回:  ui_config       
  函数备注:  
******************************************************************************/
hi_uchar8 hi_sfc_drv_get_config_reg(hi_uint32 ui_chip_sel)
{
    hi_uint32    ui_count;
    hi_uint32    ui_status;
    hi_sfc_cmd_config_u un_reg_sfc_cmd_config;

    hi_memset(&un_reg_sfc_cmd_config, 0, sizeof(hi_sfc_cmd_config_u));

    /* 发送读状态指令 */
    if (HI_SPI_TYPE_MXIC == g_chip_mk_id)
    {
        HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_ins, HI_SPI_CMD_RDCF_MXIC);
    }
    else
    {
        HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_ins, HI_SPI_CMD_RDCF);
    }

    /* 设置命令寄存器 */
    un_reg_sfc_cmd_config.st_bits.sel_cs    = ~(ui_chip_sel&0x1);
    un_reg_sfc_cmd_config.st_bits.data_cnt  = 0;              /* 实际读/写字节个数= 寄存器值+1(请参考寄存器手册) */
    un_reg_sfc_cmd_config.st_bits.data_en   = HI_TRUE;
    un_reg_sfc_cmd_config.st_bits.start     = HI_TRUE;
    un_reg_sfc_cmd_config.st_bits.rw        = 1;
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);

    /* 等待操作完成 */
    do
    {
        HI_REG_READ(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);
    }while(HI_TRUE == un_reg_sfc_cmd_config.st_bits.start);

    /* 读取器件返回的状态值 */
    HI_REG_READ(&g_pst_sfc_reg->ui_sfc_cmd_databuf1, ui_status);

    return (hi_uchar8)ui_status;
}

/******************************************************************************
  函数功能:  set spi status reg value
  输入参数:  
                             ui_chip_sel : chip select
                             reg_value:    reg value buffer
                             reg_len:       reg count
  输出参数:  无
  函数返回:  0/-1       
  函数备注:  
******************************************************************************/
hi_int32 hi_sfc_drv_set_status_reg(hi_uint32 ui_chip_sel, hi_uchar8 reg_value[], hi_uint32 reg_len)
{   
    hi_sfc_cmd_config_u un_reg_sfc_cmd_config;
    hi_uint32 ui_data      = 0;

    if (reg_len > 4)
        return -1;
        
    hi_memset(&un_reg_sfc_cmd_config, 0, sizeof(hi_sfc_cmd_config_u));

    /* spiflash write enable */
    hi_sfc_drv_write_enable(ui_chip_sel);
    
    /* 发送寄存器设置指令 */
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_ins, HI_SPI_CMD_WRSR);

    /* 填充数据 */
    hi_memcpy(&ui_data, reg_value, reg_len);
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_databuf1, ui_data);

    /* 设置命令寄存器 */
    un_reg_sfc_cmd_config.st_bits.sel_cs  = ~(ui_chip_sel&0x1);
    un_reg_sfc_cmd_config.st_bits.data_en        = HI_TRUE;
    un_reg_sfc_cmd_config.st_bits.start          = HI_TRUE;
    un_reg_sfc_cmd_config.st_bits.rw             = HI_SFC_CMD_WRITE_E;
    un_reg_sfc_cmd_config.st_bits.data_cnt       = reg_len-1;
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);

    /* 等待操作完成 */
    do
    {
        HI_REG_READ(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);
    }while(HI_TRUE == un_reg_sfc_cmd_config.st_bits.start);

    /* 等待设备ready*/
    if(hi_sfc_drv_wait_ready(ui_chip_sel))
    {
        return -1;
    }

    return 0;
}

/******************************************************************************
  函数功能:  sfc read device id
  输入参数:  
                             ui_chip_sel : chip select
                             puc_buf : output id buffer
                             uc_id_len : read id len
  输出参数:  无
  函数返回:  无
  函数备注:  
******************************************************************************/
hi_void hi_sfc_drv_read_id(hi_uint32 ui_chip_sel, hi_uchar8 *puc_buf, hi_uchar8 uc_id_len)
{
    hi_uint32    i_reg_index = 0;
    hi_uint32    ui_value;
    hi_uchar8    *puc_ptr    = puc_buf;
    hi_uint32    ui_read_len = uc_id_len;
    
    hi_sfc_cmd_config_u un_reg_sfc_cmd_config;
    hi_memset(&un_reg_sfc_cmd_config, 0, sizeof(hi_sfc_cmd_config_u));

    if(ui_read_len > HI_SPI_ID_MAX_LEN)
    {
        ui_read_len = HI_SPI_ID_MAX_LEN;
    }

    /* 发送读ID指令 */
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_ins, HI_SPI_CMD_RDID);

    /* 设置命令寄存器 */
    un_reg_sfc_cmd_config.st_bits.sel_cs    = ~(ui_chip_sel&0x1);
    un_reg_sfc_cmd_config.st_bits.data_cnt  = ui_read_len - 1;
    un_reg_sfc_cmd_config.st_bits.data_en   = HI_TRUE;
    un_reg_sfc_cmd_config.st_bits.start     = HI_TRUE;
    un_reg_sfc_cmd_config.st_bits.rw        = 1;
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);

    /* 等待操作完成 */
    do
    {
        HI_REG_READ(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);
    }while(HI_TRUE == un_reg_sfc_cmd_config.st_bits.start);

    /* read len aligned to 4BYTES */
    ui_read_len = ((ui_read_len + 0x03) & (~0x03));

    while(ui_read_len)
    {
        HI_REG_READ(&g_pst_sfc_reg->ui_sfc_cmd_databuf1 + i_reg_index, ui_value);
        hi_memcpy(puc_ptr, &ui_value, 4);
        
        puc_ptr      += 4;
        i_reg_index  += 1;
        ui_read_len  -= 4;
    }

    return;
}


/******************************************************************************
  函数功能:  read spiflash memory buffer
  输入参数:  
  输出参数:  无
  函数返回:         
  函数备注:  
******************************************************************************/
static hi_int32 hi_sfc_drv_reg_read_buf(struct hi_sfc_host *pst_host, hi_uint32 ui_chip_sel, 
                                    hi_uchar8 *puc_buf, hi_uint32 ui_src_addr, hi_uint32 ui_len)
{
    hi_uint32 ui_num      = 0;
    hi_uint32 ui_data     = 0;
    hi_int32 i_reg_index  = 0;
    hi_uchar8 *puc_buffer = puc_buf;
    hi_uint32 ui_read_len = ui_len;
    hi_sfc_cmd_config_u un_reg_sfc_cmd_config;
    struct hi_sfc_chip *pst_chip = pst_host->pst_chip;

    hi_memset(&un_reg_sfc_cmd_config, 0, sizeof(hi_sfc_cmd_config_u));
    
    /* 设置地址寄存器 */
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_addr, (ui_src_addr & HI_SFC_ADDR_MASK));

    /* 发送写指令 */
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_ins, pst_chip->st_spec.st_rd_ops.uc_cmd);

    /* 配置CMD寄存器启动读操作 */
    un_reg_sfc_cmd_config.st_bits.sel_cs         = ~(ui_chip_sel&0x1);
    un_reg_sfc_cmd_config.st_bits.mem_if_type    = pst_chip->st_spec.st_rd_ops.em_mem_if_type;
    un_reg_sfc_cmd_config.st_bits.dummy_byte_cnt = pst_chip->st_spec.st_rd_ops.uc_dummy_bytes;
    un_reg_sfc_cmd_config.st_bits.addr_en        = HI_TRUE;
    un_reg_sfc_cmd_config.st_bits.data_en        = HI_TRUE;
    un_reg_sfc_cmd_config.st_bits.start          = HI_TRUE;
    un_reg_sfc_cmd_config.st_bits.rw             = HI_SFC_CMD_READ_E;
    un_reg_sfc_cmd_config.st_bits.data_cnt       = ui_len-1;
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);

    /* 等待操作完成 */
    do
    {
        HI_REG_READ(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);
    }while(HI_TRUE == un_reg_sfc_cmd_config.st_bits.start);

    /* 从databuffer1~64拷贝数据到目标buffer */
    do
    {
        ui_num = (ui_read_len > 4) ? 4 : ui_read_len;
        
        HI_REG_READ(&g_pst_sfc_reg->ui_sfc_cmd_databuf1 + i_reg_index, ui_data);
        hi_memcpy(puc_buffer, &ui_data, ui_num);
        
        i_reg_index += 1;    /* point to next databuffer reg address */
        puc_buffer  += ui_num;
        ui_read_len -= ui_num;
    }while(ui_read_len);

    return 0;
}

/******************************************************************************
  函数功能:  read spiflash memory by reg way
  输入参数:  
  输出参数:  无
  函数返回:         
  函数备注:  
******************************************************************************/
hi_int32 hi_sfc_drv_reg_read(struct hi_sfc_host *pst_host, hi_uint32 ui_chip_sel, 
                                    hi_uchar8 *puc_buf, hi_uint32 ui_src_addr, hi_uint32 ui_len)
{
    hi_uint32 ui_read_size= 0;
    hi_uint32 ui_read_len = ui_len;
    hi_uint32 ui_offset   = ui_src_addr;
    hi_uchar8 *puc_read   = puc_buf;

    //mutex_lock(&pst_host->st_lock);
    
    while(ui_read_len)
    {
        ui_read_size = ((ui_read_len > HI_SFC_REG_BUF_SIZE) ? HI_SFC_REG_BUF_SIZE : ui_read_len);
        
        if(hi_sfc_drv_reg_read_buf(pst_host, ui_chip_sel, puc_read, ui_offset, ui_read_size))
        {
            //mutex_unlock(&pst_host->st_lock);
            return -1;
        }

        ui_offset   += ui_read_size;
        puc_read    += ui_read_size;
        ui_read_len -= ui_read_size;
    }
    
    //mutex_unlock(&pst_host->st_lock);

    return 0;
}

/******************************************************************************
  函数功能:  write spiflash memory buffer
  输入参数:  
  输出参数:  无
  函数返回:         
  函数备注:  
******************************************************************************/
static hi_sfc_drv_reg_write_buf(struct hi_sfc_host *pst_host, hi_uint32 ui_chip_sel, 
                                    hi_uchar8 *puc_buf, hi_uint32 ui_dst_addr, hi_uint32 ui_len)
{
    hi_int32  i_reg_index  = 0;
    hi_uint32 ui_num       = 0;
    hi_uint32 ui_data      = 0;
    hi_uint32 ui_write_len = ui_len;
    hi_uchar8 *puc_buffer  = puc_buf;
    hi_sfc_cmd_config_u un_reg_sfc_cmd_config;
    struct hi_sfc_chip *pst_chip = pst_host->pst_chip;

    hi_memset(&un_reg_sfc_cmd_config, 0, sizeof(hi_sfc_cmd_config_u));

    /* spi write enable */
    hi_sfc_drv_write_enable(ui_chip_sel);

    /* 设置地址寄存器 */
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_addr, (ui_dst_addr & HI_SFC_ADDR_MASK));

    /* 发送写指令 */
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_ins, pst_chip->st_spec.st_wr_ops.uc_cmd);

    /* 往databuffer1~64填充数据 */
    do
    {
        ui_num = (ui_write_len > 4) ? 4 : ui_write_len;
        hi_memcpy(&ui_data, puc_buffer, ui_num);
        
        HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_databuf1 + i_reg_index, ui_data);
        
        i_reg_index  += 1;    /* point to next databuffer reg address */
        puc_buffer   += ui_num;
        ui_write_len -= ui_num;
    }while(ui_write_len);

    /* 开始数据传输 */
    un_reg_sfc_cmd_config.st_bits.sel_cs         = ~(ui_chip_sel&0x1);
    un_reg_sfc_cmd_config.st_bits.mem_if_type    = pst_chip->st_spec.st_wr_ops.em_mem_if_type;
    un_reg_sfc_cmd_config.st_bits.dummy_byte_cnt = pst_chip->st_spec.st_wr_ops.uc_dummy_bytes;
    un_reg_sfc_cmd_config.st_bits.addr_en        = HI_TRUE;
    un_reg_sfc_cmd_config.st_bits.data_en        = HI_TRUE;
    un_reg_sfc_cmd_config.st_bits.start          = HI_TRUE;
    un_reg_sfc_cmd_config.st_bits.rw             = HI_SFC_CMD_WRITE_E;
    un_reg_sfc_cmd_config.st_bits.data_cnt       = ui_len-1;
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);

    /* 等待操作完成 */
    do
    {
        HI_REG_READ(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);
    }while(HI_TRUE == un_reg_sfc_cmd_config.st_bits.start);

    /* 等待设备ready*/
    if(hi_sfc_drv_wait_ready(ui_chip_sel))
    {
        return -1;
    }

    return 0;
}

/******************************************************************************
  函数功能:  write spiflash memory by reg way
  输入参数:  
  输出参数:  无
  函数返回:         
  函数备注:  
******************************************************************************/
hi_int32 hi_sfc_drv_reg_write(struct hi_sfc_host *pst_host, hi_uint32 ui_chip_sel, 
                                    hi_uchar8 *puc_buf, hi_uint32 ui_dst_addr, hi_uint32 ui_len)
{
    hi_uint32 ui_count     = 0;
    hi_uint32 ui_write_size= 0;
    hi_uint32 ui_write_len = ui_len;
    hi_uint32 ui_offset    = ui_dst_addr;
    hi_uchar8 *puc_write   = puc_buf;

    //mutex_lock(&pst_host->st_lock);

    /* 处理非HI_SFC_REG_BUF_SIZE对齐的写起始地址的第一个buf数据 */
    if(ui_offset & (HI_SFC_REG_BUF_SIZE-1))
    {
        if((ui_write_size = HI_SFC_REG_BUF_SIZE - (ui_offset & (HI_SFC_REG_BUF_SIZE-1))) > ui_write_len)
        {
            ui_write_size = ui_write_len;
        }

        if(hi_sfc_drv_reg_write_buf(pst_host, ui_chip_sel, puc_write, ui_offset, ui_write_size))
        {
            //mutex_unlock(&pst_host->st_lock);
            return -1;
        }

        ui_offset    += ui_write_size;
        puc_write    += ui_write_size;
        ui_write_len -= ui_write_size;
    }

    while(ui_write_len)
    {
        ui_write_size = ((ui_write_len > HI_SFC_REG_BUF_SIZE) ? HI_SFC_REG_BUF_SIZE : ui_write_len);
        
        if(hi_sfc_drv_reg_write_buf(pst_host, ui_chip_sel, puc_write, ui_offset, ui_write_size))
        {
            //mutex_unlock(&pst_host->st_lock);
            return -1;
        }

        ui_offset    += ui_write_size;
        puc_write    += ui_write_size;
        ui_write_len -= ui_write_size;

        if(0 == ((ui_count++) % 256))
        {
            hi_printf(".");
        }
    }

    //hi_printf("\n");
    
    //mutex_unlock(&pst_host->st_lock);

    return 0;
}

/******************************************************************************
  函数功能:  spi controller erase one sector
  输入参数:  
                             ui_chip_sel : chip select
                             ui_dst_addr : sector addr
  输出参数:  无
  函数返回:         
  函数备注:  
******************************************************************************/
hi_int32 hi_sfc_drv_erase_sector(struct hi_sfc_host *pst_host, hi_uint32 ui_chip_sel, hi_uint32 ui_dst_addr, hi_uint32 ui_erase_len)
{
    hi_sfc_cmd_config_u un_reg_sfc_cmd_config;
    struct hi_sfc_chip *pst_chip = pst_host->pst_chip;
    hi_uchar8 uc_erase_cmd;
        
    hi_memset(&un_reg_sfc_cmd_config, 0, sizeof(hi_sfc_cmd_config_u));

    /*BEGIN: added by j00199467 for spiflash, 20140110*/
    if (BLOCK_SIZE_64K == ui_erase_len)
    {
        uc_erase_cmd = HI_SPI_CMD_SE;
    }
    else if (SECTOR_SIZE_4K == ui_erase_len)
    {
        uc_erase_cmd = HI_SPI_CMD_SE_4K;
    }
    else
    {
        return -1;
    }
    /*END: added by j00199467 for spiflash, 20140110*/
    
    /* spiflash write enable */
    hi_sfc_drv_write_enable(ui_chip_sel);

    /* 发送擦除指令 */
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_ins, uc_erase_cmd);

    /* 设置地址寄存器 */
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_addr, (ui_dst_addr & HI_SFC_ADDR_MASK));

    /* 设置命令寄存器 */
    un_reg_sfc_cmd_config.st_bits.sel_cs  = ~(ui_chip_sel&0x1);
    un_reg_sfc_cmd_config.st_bits.addr_en = HI_TRUE;
    un_reg_sfc_cmd_config.st_bits.start   = HI_TRUE;
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);

    /* 等待操作完成 */
    do
    {
        HI_REG_READ(&g_pst_sfc_reg->ui_sfc_cmd_config, un_reg_sfc_cmd_config.ui_value);
    }while(HI_TRUE == un_reg_sfc_cmd_config.st_bits.start);

    /* 等待设备ready*/
    if(hi_sfc_drv_wait_ready(ui_chip_sel))
    {
        return -1;
    }

    return 0;
}

/******************************************************************************
  函数功能:  spi drv first init(to init sfc buffer & reg ioremap)
  输入参数:  
                             host : sfc driver host structure
  输出参数:  无
  函数返回:  0:OK;  -1:ioremap error(kernel only)       
  函数备注:  
******************************************************************************/
hi_int32 hi_sfc_drv_pre_init(struct hi_sfc_host *pst_host)
{    
    /* regbase ioremap */
    g_pst_sfc_reg = (hi_sfc_reg_s *)ioremap_nocache(CONFIG_MTD_HSAN_SFC_REG_BASE_ADDRESS, sizeof(hi_sfc_reg_s));
    if (HI_NULL == g_pst_sfc_reg)
    {
        return -1;
    }

    /* 选择新版本寄存器组，即V300 */
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_version_sel, HI_SFC_V300_E);
    
    return 0;
}

static hi_uint32 hi_sfc_drv_get_bus_flash_size(hi_uint32 ui_chip_size)
{
    switch (ui_chip_size)
    {
        case 0x0:
            return HI_FLASH_SIZE_0_E;
        case 0x10000:
            return HI_FLASH_SIZE_512K_E;
        case 0x20000:
            return HI_FLASH_SIZE_1MB_E;
        case 0x40000:
            return HI_FLASH_SIZE_2MB_E;
        case 0x80000:
            return HI_FLASH_SIZE_4MB_E;
        case 0x100000:
            return HI_FLASH_SIZE_8MB_E;
        case 0x200000:
            return HI_FLASH_SIZE_16MB_E;
        case 0x400000:
            return HI_FLASH_SIZE_32MB_E;
        case 0x800000:
            return HI_FLASH_SIZE_64MB_E;
        case 0x1000000:
            return HI_FLASH_SIZE_128MB_E;
        case 0x2000000:
            return HI_FLASH_SIZE_256MB_E;
        case 0x4000000:
            return HI_FLASH_SIZE_512MB_E;
        default:
            return HI_FLASH_SIZE_128MB_E;
    }
}

/******************************************************************************
  函数功能:  spi drv init
  输入参数:  
                             host : spi driver host structure
  输出参数:  无
  函数返回:      
  函数备注:  
******************************************************************************/
hi_int32 hi_sfc_drv_init(struct hi_sfc_host *pst_host)
{
    hi_sfc_bus_config1_u    un_reg_sfc_bus_config1;
    hi_sfc_bus_flash_size_u un_reg_sfc_bus_flash_size;
    struct hi_sfc_chip *pst_chip = pst_host->pst_chip;

    hi_memset(&un_reg_sfc_bus_config1,    0, sizeof(hi_sfc_bus_config1_u)   );
    hi_memset(&un_reg_sfc_bus_flash_size, 0, sizeof(hi_sfc_bus_flash_size_u));
    
    /* 根据器件参数配置SFC con寄存器，未赋值的bit按默认值0 */
    un_reg_sfc_bus_config1.st_bits.rd_ins           = pst_chip->st_spec.st_rd_ops.uc_cmd;
    un_reg_sfc_bus_config1.st_bits.rd_mem_if_type   = pst_chip->st_spec.st_rd_ops.em_mem_if_type;
    un_reg_sfc_bus_config1.st_bits.rd_dummy_bytes   = pst_chip->st_spec.st_rd_ops.uc_dummy_bytes;
    un_reg_sfc_bus_config1.st_bits.wr_ins           = pst_chip->st_spec.st_wr_ops.uc_cmd;
    un_reg_sfc_bus_config1.st_bits.wr_mem_if_type   = pst_chip->st_spec.st_wr_ops.em_mem_if_type;
    un_reg_sfc_bus_config1.st_bits.wr_dummy_bytes   = pst_chip->st_spec.st_wr_ops.uc_dummy_bytes;
    HI_REG_WRITE(&g_pst_sfc_reg->ui_sfc_bus_config1, un_reg_sfc_bus_config1.ui_value);

    
    hi_sfc_drv_read_id(0, &g_chip_mk_id, 1); 

    return 0;
}
/******************************************************************************
  函数功能:  spi drv exit
  输入参数:  
                             host : spi driver host structure
  输出参数:  无
  函数返回:    
  函数备注:  
******************************************************************************/
hi_int32 hi_sfc_drv_exit(struct hi_sfc_host *pst_host)
{
    iounmap(g_pst_sfc_reg);

    return 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

