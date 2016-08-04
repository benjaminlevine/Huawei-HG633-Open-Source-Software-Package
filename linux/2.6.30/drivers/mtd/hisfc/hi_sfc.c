/******************************************************************************
  文件名称: hi_sfc.c
  功能描述: spiflash初始化入口及对外接口定义
  版本描述: V1.0

  创建日期: D2013_08_01
  创建作者: zhouyu 00204772

  修改记录: 
            生成初稿.
******************************************************************************/
#include <asm/setup.h>
#include <linux/mtd/mtd.h>
#include <linux/platform_device.h>
#include <linux/mtd/partitions.h>
#include <linux/dma-mapping.h>
#include <mach/typedef.h>
#include <linux/mtd/hi_sfc.h>
#include "atpnandparts.h"
#include "kerneltag.h"
#include "hi_sfc_drv.h"
#include "atpconfig.h"
#include "bhal.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


extern hi_int32 hi_sfc_mtd_contact(struct mtd_info *pst_mtd);
extern hi_int32 hi_sfc_mtd_erase(struct mtd_info *pst_mtd, struct erase_info *pst_erase_info);
extern hi_int32 hi_sfc_mtd_write(struct mtd_info *pst_mtd, hi_uint64 ull_dst_addr, 
                              hi_uint32 ui_len, hi_uint32 *pui_retlen, const hi_uchar8 *puc_buf);
extern hi_int32 hi_sfc_mtd_read(struct mtd_info *pst_mtd, hi_uint64 ull_src_addr, 
                                    hi_uint32 ui_len, hi_uint32 *pui_retlen, hi_uchar8 *puc_buf);

/*BEGIN: added by j00199467 for spiflash, 20131119*/
extern void ATP_Set_FlashParam(hi_uint32 flash_total_size, hi_uint32 sector_size);
/*END: added by j00199467 for spiflash, 20131119*/

static struct hi_sfc_host *g_pst_sfc_host = HI_NULL;

/* boot parameters flash tag struct */
extern char *g_pc_flash_info;

#define EQUIP_PART_NAME     "equip"
#define WIFI_PART_NAME      "wlanrf"

extern int g_lBhalFlashWriteFlag;
static hi_uint32 s_sw_protect_addr_start = 0;
static hi_uint32 s_sw_protect_addr_end = 0;


#ifdef CONFIG_SUPPORT_SPIFLASH_PROTECT
#include "spiflash_protect.h" 

static hi_uchar8 hi_sfc_get_status_reg(void)
{
    return hi_sfc_drv_get_status_reg(0);
}

static hi_uchar8 hi_sfc_get_config_reg(void)
{
    return hi_sfc_drv_get_config_reg(0);
}

static hi_int32 hi_sfc_set_status_reg(hi_uchar8 reg_value[], int reg_len)
{
    return hi_sfc_drv_set_status_reg(0, reg_value, reg_len);
}

hi_int32 spiflash_protect_init(hi_uint32 flashSize)
{
     T_SPIFLASH_PROTECT_INIT sfInit;
     
     sfInit.flash_size = flashSize;
     sfInit.flash_area = CONFIG_SPIFLASH_PROTECT_RATIO;
     sfInit.get_config_reg = hi_sfc_get_config_reg;
     sfInit.get_status_reg = hi_sfc_get_status_reg;
     sfInit.set_status_reg = hi_sfc_set_status_reg;
     
     switch(g_chip_mk_id) {
        case HI_SPI_TYPE_WB:
            sfInit.flash_type = FLASH_TYPE_WINBOND;
            break; 
        case HI_SPI_TYPE_SPAN:
            sfInit.flash_type = FLASH_TYPE_SPANSION;
            break;    
        case HI_SPI_TYPE_MXIC:
            if (FLASH_SIZE_16M == flashSize) //16M flash
            {
                sfInit.flash_type = FLASH_TYPE_MXIC_16M;
            }
            else
            {
                sfInit.flash_type = FLASH_TYPE_MXIC;
            }
            break;    
        case HI_SPI_TYPE_MICRON:
            if (FLASH_SIZE_16M == flashSize) //16M flash
            {
                sfInit.flash_type = FLASH_TYPE_MICRON_16M;
            }
            else if (FLASH_SIZE_8M == flashSize) //8M flash
            {
                sfInit.flash_type = FLASH_TYPE_MICRON_8M;
            }
            else
            {
                sfInit.flash_type = FLASH_TYPE_MICRON_4M;
            }
            break;  
        default:
            sfInit.flash_type = FLASH_TYPE_UNKNOWN;
            break;  
    }        
		  
    if (0 == spiflash_protect_param_init(&sfInit))
    {
        return spiflash_set_normalprotect();
    }
    else
    {
        return -1;
    } 
}

unsigned int ATP_sf_protect_info(void)
{
    unsigned int status_reg;
    int ret = 0;
    int res = 0;

    preempt_enable();
    mutex_lock(&g_pst_sfc_host->st_lock);
    
    res = spiflash_get_protect_info(&status_reg);

    mutex_unlock(&g_pst_sfc_host->st_lock);
    preempt_disable();

    ret |= status_reg & 0xFFFF;

    if (res != SPIFLASH_PROTECT_MAX)
    {
        ret |= res << 16;
    }
    else
    {
        ret |= 1 << 24;
    }    

    return ret;

}

int ATP_sf_set_unprotect(void)
{
    preempt_enable();
    mutex_lock(&g_pst_sfc_host->st_lock);

    spiflash_set_unprotect();
    
    mutex_unlock(&g_pst_sfc_host->st_lock);
    preempt_disable();

    return 0;
}

int ATP_sf_set_normalprotect(void)
{
    preempt_enable();
    mutex_lock(&g_pst_sfc_host->st_lock);
 
    spiflash_set_normalprotect();

    mutex_unlock(&g_pst_sfc_host->st_lock);
    preempt_disable();

    return 0;
}

int ATP_sf_set_forceprotect(void)
{
    preempt_enable();
    mutex_lock(&g_pst_sfc_host->st_lock);

    spiflash_set_forceprotect();

    mutex_unlock(&g_pst_sfc_host->st_lock);
    preempt_disable();

    return 0;
}
#endif

void ATP_get_fix_part_protect_addr(struct mtd_partition *pst_parts, hi_int32 i_nparts)
{
    hi_int32 i;

    for (i=0; i<i_nparts; i++)
    {
        if (0 == strcmp(pst_parts[i].name, EQUIP_PART_NAME))
            s_sw_protect_addr_start = (hi_uint32)(pst_parts[i].offset);

        if (0 == strcmp(pst_parts[i].name, WIFI_PART_NAME))
            s_sw_protect_addr_end = (hi_uint32)(pst_parts[i].offset) + (hi_uint32)(pst_parts[i].size);
    }

    printk("euip part start addr:%x, wifi part end addr:%x\n", s_sw_protect_addr_start, s_sw_protect_addr_end);
}

hi_int32 ATP_check_fix_part_protect_addr(hi_uint32 ui_addr)
{   
    if ((BHAL_FLASH_EQUIP != g_lBhalFlashWriteFlag) &&
        ((ui_addr >= s_sw_protect_addr_start) && (ui_addr < s_sw_protect_addr_end)))
    {
        return -1;
    }
    else
    {
        return 0;
    }    
}

/******************************************************************************
  函数功能:  nand get flash spec info from boot parameter
  输入参数:  
                             pst_chip : nand chip structure
  输出参数:  无
  函数返回:  uint       
  函数备注:  
******************************************************************************/
static hi_void hi_sfc_kernel_get_spec(struct hi_sfc_chip *pst_chip)
{
    struct hi_sfc_spec *pst_sfc_spec;
    
    pst_sfc_spec = g_pc_flash_info + 2 + FLASH_INFO_SIZE;

    hi_memcpy(&pst_chip->st_spec, pst_sfc_spec, sizeof(struct hi_sfc_spec));
    
    return;
}

/******************************************************************************
  函数功能:  sfc get total size
  输入参数:  无
  输出参数:  无
  函数返回:  chip size
  函数备注:  
******************************************************************************/
hi_uint32 hi_sfc_get_total_size(hi_void)
{
    struct hi_sfc_chip *pst_chip = g_pst_sfc_host->pst_chip;
    
    return pst_chip->ui_total_size;
}

/******************************************************************************
  函数功能:  sfc get block size
  输入参数:  无
  输出参数:  无
  函数返回:  block size
  函数备注:  
******************************************************************************/
hi_uint32 hi_sfc_get_block_size(hi_void)
{
    struct hi_sfc_chip *pst_chip = g_pst_sfc_host->pst_chip;

    return pst_chip->st_spec.ui_erase_size;
}

/******************************************************************************
  函数功能:  sfc erase all page
  输入参数:  
                             ui_offset : start erase address
                             ui_len : erase len
  输出参数:  无
  函数返回:  0:OK;   -1:NOK
  函数备注:  
******************************************************************************/
hi_int32 hi_sfc_erase( hi_uint32 ui_offset, hi_uint32 ui_len )
{
    struct erase_info st_erase;
    struct mtd_info *pst_mtd = g_pst_sfc_host->pst_mtd;

    hi_memset(&st_erase, 0, sizeof(struct erase_info));

    st_erase.addr = ui_offset;
    st_erase.len  = ui_len;
    
    return hi_sfc_mtd_erase(pst_mtd, &st_erase);
}

/******************************************************************************
  函数功能:  sfc write data
  输入参数:  
                             puc_buf : data buffer
                             ui_offset : start address
                             ui_len : read len
  输出参数:  无
  函数返回: 0:OK;   -1:NOK
  函数备注:  
******************************************************************************/
hi_int32 hi_sfc_read( hi_uchar8 *puc_buf, hi_uint32 ui_offset, hi_uint32 ui_len )
{
    hi_uint32 ui_retlen;
    struct mtd_info *pst_mtd = g_pst_sfc_host->pst_mtd;
    
    return hi_sfc_mtd_read(pst_mtd, ui_offset, ui_len, &ui_retlen, puc_buf);
}

/******************************************************************************
  函数功能:  sfc write data
  输入参数:  
                             puc_buf : data buffer
                             ui_offset : start address
                             ui_len : write len
  输出参数:  无
  函数返回:  0:OK;   -1:NOK
  函数备注:  
******************************************************************************/
hi_int32 hi_sfc_write( hi_uchar8 *puc_buf, hi_uint32 ui_offset, hi_uint32 ui_len )
{
    hi_uint32 ui_retlen;
    struct mtd_info *pst_mtd = g_pst_sfc_host->pst_mtd;

    return hi_sfc_mtd_write(pst_mtd, ui_offset, ui_len, &ui_retlen, puc_buf);
}

/******************************************************************************
  函数功能:  spi setup mtd partitions
  输入参数:  
                             mtd  : lmtd device structure
                             parts: mtd patition structure
                             i_nparts : partition numbers
  输出参数:  无
  函数返回:  uint       
  函数备注:  
******************************************************************************/
static hi_void hi_sfc_setup_mtd_partitions(struct mtd_info *pst_mtd,
                            struct mtd_partition **ppst_parts, hi_int32 *i_nparts)
{
    hi_uint32 ui_active_sys;
    hi_uint32 ui_upg_flag = 0;
    hi_uint32 ui_sys_type = 0;

    /* get sys type & upgflag & partition info from boot parameters */
    ui_sys_type = g_pc_flash_info[0];
    ui_upg_flag = g_pc_flash_info[1];

    if((HI_TYPE_DOUBLE==ui_sys_type) && (SLAVE_ACTIVE==ui_upg_flag))
    { 
        ui_active_sys = 1;
    }
    else
    {
        ui_active_sys = 0;
    }

    atp_setup_mtd_partitions(pst_mtd, g_pc_flash_info+2, ui_active_sys, ppst_parts, i_nparts);
}

/******************************************************************************
  函数功能:  spi get flash spec info from boot parameter
  输入参数:  无
  输出参数:  无
  函数返回:  0:OK;   -1:NOK
  函数备注:  
******************************************************************************/
static hi_int32 hi_sfc_chip_init(struct hi_sfc_chip *pst_chip)
{
    hi_sfc_kernel_get_spec(pst_chip);

    pst_chip->ui_total_size = pst_chip->st_spec.ui_chip_size * CONFIG_MTD_HSAN_SFC_CHIPNUM;
    
    return 0;
}

/******************************************************************************
  函数功能:  spi flash probe entry
  输入参数:  无
  输出参数:  无
  函数返回:  0:OK;   -1:NOK
  函数备注:  
******************************************************************************/
static hi_int32 hi_sfc_probe(struct platform_device * pst_pltdev)
{
    hi_int32 i;
    hi_int32 i_ret      = 0;
    hi_int32 i_nparts   = 0;
    struct hi_sfc_chip *pst_chip    = HI_NULL;
    struct hi_sfc_host *pst_host    = HI_NULL;
    struct mtd_partition *pst_parts = NULL;

    /* 申请连续的内存空间，方便统一管理*/
    struct mtd_info *pst_mtd        = HI_NULL;
    hi_uint32 ui_host_size = sizeof(struct hi_sfc_host) + sizeof(struct hi_sfc_chip) + sizeof(struct mtd_info);

    g_pst_sfc_host = pst_host = hi_malloc(ui_host_size);
    if (HI_NULL == pst_host)
    {
        return -1;
    }
    
    hi_memset((hi_char8 *)pst_host, 0, ui_host_size);

    platform_set_drvdata(pst_pltdev, pst_host);

    pst_host->pst_dev  = &pst_pltdev->dev;
    pst_host->pst_chip = pst_chip = (struct hi_sfc_chip *)&pst_host[1];     //point to host+sizeof(struct hi_sfc_host)
    pst_chip->pv_priv  = pst_host;

    pst_host->pst_mtd  = pst_mtd  = (struct mtd_info *)&pst_chip[1];        //point to host+sizeof(struct hi_sfc_host)+sizeof(struct sfc_chip)
    pst_mtd->priv      = pst_chip;
    
    pst_mtd->owner     = THIS_MODULE;
    pst_mtd->name      = (hi_char8 *)(pst_pltdev->name);

	/*BEGIN: added by j00199467 for spiflash, 20131119*/
    mutex_init(&(pst_host->st_lock));
	/*END: added by j00199467 for spiflash, 20131119*/

    /* sfc pre_init,init the sfc buffer and reg remap */
    i_ret = hi_sfc_drv_pre_init(pst_host);
    if(i_ret)
    {
        hi_free(pst_host);
        platform_set_drvdata(pst_pltdev, HI_NULL);
        return -1;
    }

    /* Get the flash and manufacturer id and lookup if the type is supported */
    i_ret = hi_sfc_chip_init(pst_chip);
    if(i_ret)
    {
        hi_free(pst_host);
        platform_set_drvdata(pst_pltdev, HI_NULL);
        return i_ret;
    }

    /* spi controller innit */
    if(hi_sfc_drv_init(pst_host))
    {
        hi_free(pst_host);
        platform_set_drvdata(pst_pltdev, HI_NULL);
        return -1;
    }

    hi_sfc_mtd_contact(pst_mtd);
    
        /* setup mtd partitions */
#ifdef CONFIG_MTD_CMDLINE_PARTS
        static const char *part_probes[] = { "cmdlinepart", HI_NULL, };
        i_nparts = parse_mtd_partitions(pst_mtd, part_probes, &pst_parts, 0);
#else
        hi_sfc_setup_mtd_partitions(pst_mtd, &pst_parts, &i_nparts);
#endif
    
        if (!i_nparts)
        {
            printk("ERROR! can not find any mtd partitions!\n");
            return -1;
        }
    
        for (i = 0; i < i_nparts; i++) 
        {
            DEBUG(MTD_DEBUG_LEVEL2, "partitions[%d] = {.name = %s, .offset = 0x%.8llx, .size = 0x%.8llx (%lluKiB) }\n",
                                    i, pst_parts[i].name, pst_parts[i].offset, pst_parts[i].size, pst_parts[i].size / 1024);
        }

        ATP_get_fix_part_protect_addr(pst_parts, i_nparts);
        
        i_ret = add_mtd_partitions(pst_mtd, pst_parts, i_nparts);

	/*BEGIN: added by j00199467 for spiflash, 20131119*/
    ATP_Set_FlashParam(pst_chip->st_spec.ui_chip_size, pst_chip->st_spec.ui_erase_size);
	/*END: added by j00199467 for spiflash, 20131119*/

#ifdef CONFIG_SUPPORT_SPIFLASH_PROTECT
    spiflash_protect_init(pst_chip->st_spec.ui_chip_size);
#endif

    return 0;
}

static hi_int32 hi_sfc_remove(struct platform_device * pst_pltdev)
{
    struct hi_sfc_host *pst_host = platform_get_drvdata(pst_pltdev);

#ifdef CONFIG_MTD_PARTITIONS
    del_mtd_partitions(pst_host->pst_mtd);
#endif

    del_mtd_device(pst_host->pst_mtd);

    hi_sfc_drv_exit(pst_host);

    hi_free(pst_host);

    platform_set_drvdata(pst_pltdev, HI_NULL);

    return 0;
}

static hi_void hi_sfc_pltdev_release(struct device *pst_dev)
{
}

static struct resource g_ast_sfc_resources[] = 
{
    {
        .start = CONFIG_MTD_HSAN_SFC_REG_BASE_ADDRESS,
        .end   = CONFIG_MTD_HSAN_SFC_REG_BASE_ADDRESS + CONFIG_MTD_HSAN_SFC_REG_BASE_ADDRESS_LEN - 1,
        .flags = IORESOURCE_MEM,
    },
    
    {
        .start = CONFIG_MTD_HSAN_SFC_CS0_BUFFER_BASE_ADDRESS,
        .end   = CONFIG_MTD_HSAN_SFC_CS0_BUFFER_BASE_ADDRESS + CONFIG_MTD_HSAN_SFC_CS0_BUFFER_BASE_ADDRESS_LEN - 1,
        .flags = IORESOURCE_MEM,

    },

    {
        .start = CONFIG_MTD_HSAN_SFC_CS1_BUFFER_BASE_ADDRESS,
        .end   = CONFIG_MTD_HSAN_SFC_CS1_BUFFER_BASE_ADDRESS + CONFIG_MTD_HSAN_SFC_CS1_BUFFER_BASE_ADDRESS_LEN - 1,
        .flags = IORESOURCE_MEM,
    
    },  
};

static struct platform_driver g_st_sfc_pltdrv =
{
    .driver.name    = "hi_sfc",
    .probe          = hi_sfc_probe,
    .remove         = hi_sfc_remove,
    
    .driver.owner   = THIS_MODULE,
    .driver.bus     = &platform_bus_type,
};

static struct platform_device g_st_sfc_pltdev =
{
    .name           = "hi_sfc",
    .id             = -1,

    .dev.release    = hi_sfc_pltdev_release,

    .num_resources  = ARRAY_SIZE(g_ast_sfc_resources),
    .resource       = g_ast_sfc_resources,
};

static hi_int32 __init hi_sfc_init(hi_void)
{
    hi_int32 i_ret = 0;
    
    printk("HSAN Spi Flash Controller v300 Device Driver, Version 1.0\n");

    i_ret = platform_driver_register(&g_st_sfc_pltdrv);
    if (i_ret < 0)
    {
        return i_ret;
    }

    i_ret = platform_device_register(&g_st_sfc_pltdev);
    if (i_ret < 0)
    {
        platform_driver_unregister(&g_st_sfc_pltdrv);
        return i_ret;
    }

    return i_ret;
}

static hi_void __exit hi_sfc_exit(hi_void)
{
    platform_device_unregister(&g_st_sfc_pltdev);
    platform_driver_unregister(&g_st_sfc_pltdrv);
}

module_init(hi_sfc_init);
module_exit(hi_sfc_exit);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

