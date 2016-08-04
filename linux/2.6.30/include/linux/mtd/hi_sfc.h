/******************************************************************************
  文件名称: hi_sfc.h
  功能描述: spiflash驱动接口公用头文件
  版本描述: V1.0

  创建日期: D2013_08_01
  创建作者: zhouyu 00204772

  修改记录: 
            生成初稿.
******************************************************************************/
#ifndef __HI_SFC_H__
#define __HI_SFC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#ifndef CONFIG_MTD_HSAN_SFC_CHIPNUM
#define CONFIG_MTD_HSAN_SFC_CHIPNUM 2
#endif

#define HI_SFC_MAX_CHIP_CS          2

/* 最大SPI ID长度 */
#define HI_SPI_ID_MAX_LEN           8

/*
  * HSAN spi flash commands
  */
#define HI_SPI_CMD_WREN             0x06    /* Write Enable */
//-----------------------------------------------------------------------------
#define HI_SPI_CMD_SE_4K            0x20   /* 4KB sector Erase */ 
#define HI_SPI_CMD_SE_32K           0x52   /* 32KB sector Erase */ 
#define HI_SPI_CMD_SE               0xD8   /* 64KB Sector Erase */
#define HI_SPI_CMD_BE               0xC7   /* chip erase */

//-----------------------------------------------------------------------------
#define HI_SPI_CMD_RDSR             0x05    /* Read Status Register */
#define HI_SPI_CMD_WRSR             0x01    /* Read Status Register */
#define HI_SPI_CMD_RDCF_MXIC        0x15    /* Read mxic config Register */
#define HI_SPI_CMD_RDCF             0x35    /* Read Config Register */
#define HI_SPI_CMD_RDID             0x9F    /* Read Identification */
//-----------------------------------------------------------------------------
#define HI_SPI_CMD_PP               0x02    /* Page Programming */
#define HI_SPI_CMD_WRITE_DUAL       0xA2    /* fast program dual input */
#define HI_SPI_CMD_WRITE_QUAD       0x32    /* fast program quad input */
#define HI_SPI_CMD_WRITE_DUAL_ADDR  0xD2    /* Dual I/O High Performance Write */
#define HI_SPI_CMD_WRITE_QUAD_ADDR  0x12    /* Quad I/O High Performance Write */
//-----------------------------------------------------------------------------
#define HI_SPI_CMD_READ             0x03    /* Read Data bytes */
#define HI_SPI_CMD_FAST_READ        0x0B    /* Read Data Bytes at Higher Speed */
#define HI_SPI_CMD_READ_DUAL        0x3B    /* fast read dual output */
#define HI_SPI_CMD_READ_QUAD        0x6B    /* fast read quad output */
#define HI_SPI_CMD_READ_DUAL_ADDR   0xBB    /* Dual I/O High Performance Read */
#define HI_SPI_CMD_READ_QUAD_ADDR   0xEB    /* Quad I/O High Performance Read */
//-----------------------------------------------------------------------------
#define HI_SPI_CMD_SR_WIP           1       /* Write in Progress */
#define HI_SPI_CMD_SR_WEL           2       /* Write Enable Latch */
//-----------------------------------------------------------------------------
#define HI_SPI_CMD_EN4B             0xB7    /* enter to 4 bytes mode and set 4 byte bit as '1' */
#define HI_SPI_CMD_EX4B             0xE9    /* exit 4 bytes mode and clear 4 byte bit as '0' */


#define HI_SPI_TYPE_SPAN            0x01
#define HI_SPI_TYPE_MICRON          0x20
#define HI_SPI_TYPE_MXIC            0xC2
#define HI_SPI_TYPE_WB              0xEF

/* SFC版本选择*/
typedef enum
{
    HI_SFC_V200_E       = 0x0,
    HI_SFC_V300_E       = 0x1,
}hi_sfc_version_sel_e;

/* SPI FLASH接口类型 */
typedef enum
{
    HI_SFC_IF_STD_E             = 0x0,
    HI_SFC_IF_DUALIN_DUALOUT_E  = 0x1,
    HI_SFC_IF_QUAL_IO_E         = 0x2,
    HI_SFC_IF_FULL_DIO_E        = 0x3,
    //0x4保留
    HI_SFC_IF_QUADIN_DUALOUT_E  = 0x5,
    HI_SFC_IF_QUAD_IO_E         = 0x6,
    HI_SFC_IF_FULL_QIO_E        = 0x7,
}hi_sfc_if_e;

/* 
  * spi读写操作参数
  */
typedef struct
{
    hi_sfc_if_e em_mem_if_type;
    hi_uchar8   uc_dummy_bytes;
    hi_uchar8   uc_cmd;
}hi_sfc_ops;

/*
  * spi chip 器件相关参数
  */
struct hi_sfc_spec
{
    hi_uint32   ui_chip_size;
    hi_uint32   ui_erase_size;
    hi_uint32   ui_page_size;

    hi_sfc_ops  st_wr_ops;
    hi_sfc_ops  st_rd_ops;

    hi_uchar8 uc_erase_cmd;
};

/*
  * spi chip 基本参数
  */
struct hi_sfc_chip
{
    struct hi_sfc_spec st_spec;

    hi_uint32 ui_total_size;        /* 所有片选容量之和 */
    
    hi_void  *pv_priv;
};

/*
  * spi chip 器件列表属性
  */
struct hi_sfc_ids_s
{
    hi_char8    *pc_name;

    hi_uchar8   uac_id[HI_SPI_ID_MAX_LEN];
    hi_uint32   ui_id_len;

    hi_uint32   ui_chip_size;
    hi_uint32   ui_erase_size;
    hi_uint32   ui_page_size;

    hi_sfc_ops  st_wr_ops;
    hi_sfc_ops  st_rd_ops;

    hi_uchar8 uc_erase_cmd;
};


extern hi_int32 hi_sfc_erase( hi_uint32 ui_offset, hi_uint32 ui_len );
extern hi_int32 hi_sfc_read( hi_uchar8 *puc_buf, hi_uint32 ui_offset, hi_uint32 ui_len );
extern hi_int32 hi_sfc_write( hi_uchar8 *puc_buf, hi_uint32 ui_offset, hi_uint32 ui_len );
extern hi_uint32 hi_sfc_get_block_size(hi_void);
extern hi_uint32 hi_sfc_get_total_size(hi_void);

/*************************
  * for debug
  *************************/
//#define HI_SFC_DEBUG   1

#ifdef HI_SFC_DEBUG
#define HI_SFC_PRINTF( fmt,arg... ) { \
        if ( 1 ) \
        { \
            hi_printf( fmt, ##arg );\
        } \
    }
#else
#define HI_SFC_PRINTF( fmt,arg... ) { \
        if ( 0 ) \
        { \
            hi_printf( fmt, ##arg );\
        } \
    }
#endif /* HI_SFC_DEBUG */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_SFC_H__ */

