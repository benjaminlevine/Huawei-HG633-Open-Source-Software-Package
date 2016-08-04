/******************************************************************************
  文件名称: hi_sfc_drv.h
  功能描述: sfc核心驱动头文件
  版本描述: V1.0

  创建日期: D2013_08_01
  创建作者: zhouyu 00204772

  修改记录: 
            生成初稿.
******************************************************************************/
#ifndef __HI_SFC_DRV_H__
#define __HI_SFC_DRV_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define SECTOR_SIZE_4K      0x1000
#define BLOCK_SIZE_64K      0x10000
#define FLASH_SIZE_8M       0x800000
#define FLASH_SIZE_16M      0x1000000

#define HI_SFC_MAX_READY_WAIT_JIFFIES   (40 * HZ)

//#define HI_SFC_ADDR_MASK              0xFFFFFF    /* V200 - 23:0 */
//#define HI_SFC_REG_BUF_SIZE           32          /* V200 - 8databuffer*4B = 32B */   
#define HI_SFC_ADDR_MASK                0x3FFFFFFF  /* V300 - 29:0 */
#define HI_SFC_REG_BUF_SIZE             256         /* V300 - 64databuffer*4 = 256B */


/*************************
  * struct
  *************************/
typedef struct 
{
    hi_uint32 ui_sfc_con;               /* (0x00) -SFC模块配置寄存器 */
    hi_uint32 ui_sfc_cmd;               /* (0x04) -命令设置寄存器 */
    hi_uint32 ui_sfc_ins;               /* (0x08) -指令寄存器 */
    hi_uint32 ui_sfc_addr;              /* (0x0c) -地址寄存器 */
    hi_uint32 ui_sfc_databuffer1;       /* (0x10) -数据buffer1寄存器 */
    hi_uint32 ui_sfc_databuffer2;       /* (0x14) -数据buffer2寄存器 */
    hi_uint32 ui_sfc_databuffer3;       /* (0x18) -数据buffer3寄存器 */
    hi_uint32 ui_sfc_databuffer4;       /* (0x1c) -数据buffer4寄存器 */
    hi_uint32 ui_sfc_databuffer5;       /* (0x20) -数据buffer5寄存器 */
    hi_uint32 ui_sfc_databuffer6;       /* (0x24) -数据buffer6寄存器 */
    hi_uint32 ui_sfc_databuffer7;       /* (0x28) -数据buffer7寄存器 */
    hi_uint32 ui_sfc_databuffer8;       /* (0x2c) -数据buffer8寄存器 */
    hi_uint32 ui_sfc_cs0config;         /* (0x30) -片选0配置寄存器 */
    hi_uint32 ui_sfc_cs1config;         /* (0x34) -片选1配置寄存器 */
    hi_uint32 ui_sfc_cs0baseaddr;       /* (0x38) -片选0基址寄存器 */
    hi_uint32 ui_sfc_cs1baseaddr;       /* (0x3c) -片选1基址寄存器 */
    hi_uint32 ui_sfc_cs1aliasaddr;      /* (0x40) -片选1的别名基地址寄存器 */
    hi_uint32 ui_sfc_timing;            /* (0x44) -设置时序参数寄存器 */
    hi_uint32 ui_reserve1[2];           /* 0x58-0x4c -保留 */
    hi_uint32 ui_sfc_intrawstatus;      /* (0x50) -中断原始状态寄存器 */
    hi_uint32 ui_sfc_intstatus;         /* (0x54) -经过屏蔽的中断状态寄存器 */
    hi_uint32 ui_sfc_intmask;           /* (0x58) -中断屏蔽寄存器 */
    hi_uint32 ui_sfc_intclear;          /* 0x5c - 中断屏清除存器*/
    hi_uint32 ui_reserve2[40];          /* 0x60-0xfc - 保留 */
    hi_uint32 ui_sfc_global_config;     /* 0x100 -全局配置寄存器 */
    hi_uint32 ui_reserve3[3];           /* 0x104-0x10c - 保留 */
    hi_uint32 ui_sfc_timing2;           /* 0x110 - Timing配置寄存器*/
    hi_uint32 ui_reserve4[3];           /* 0x114-0x11c - 保留 */
    hi_uint32 ui_sfc_int_raw_status;    /* 0x120 - 中断原始状态寄存器 */
    hi_uint32 ui_sfc_int_status;        /* 0x124 - 经过屏蔽处理的中断状态寄存器 */
    hi_uint32 ui_sfc_int_mask;          /* 0x128 - 中断屏蔽寄存器 */
    hi_uint32 ui_sfc_int_clear;         /* 0x12c - 中断清除寄存器*/
    hi_uint32 ui_reserve5[50];          /* 0x114-0x11c - 保留  */
    hi_uint32 ui_sfc_version;           /* 0x1f8 - 版本寄存器 */
    hi_uint32 ui_sfc_version_sel;       /* 0x1fc - 版本选择寄存器 */
    hi_uint32 ui_sfc_bus_config1;       /* 0x200 - 总线操作方式配置寄存器 */
    hi_uint32 ui_sfc_bus_config2;       /* 0x204 - 总线操作方式配置寄存器*/
    hi_uint32 ui_reserve6[2];           /* 0x208-0x20c - 保留  */
    hi_uint32 ui_sfc_bus_flash_size;    /* 0x210 - 总线操作方式映射尺寸寄存器 */
    hi_uint32 ui_sfc_bus_base_addr_cs0; /* 0x214 - 总线操作方式片选0映射基地址寄存器*/
    hi_uint32 ui_sfc_bus_base_addr_cs1; /* 0x218 - 总线操作方式片选1映射基地址寄存器*/
    hi_uint32 ui_sfc_bus_alias_addr;    /* 0x21c - 总线操作方式Alias映射基地址寄存器*/
    hi_uint32 ui_sfc_bus_alias_cs;      /* 0x220 -总线操作方式Alias片选指示寄存器 */
    hi_uint32 ui_reserve7[55];          /* 0x220-0x2fc - 保留  */
    hi_uint32 ui_sfc_cmd_config;        /* 0x300 -命令操作方式配置寄存器 */
    hi_uint32 ui_reserve8[1];           /* 0x304 - 保留  */
    hi_uint32 ui_sfc_cmd_ins;           /* 0x308 -命令操作方式指令寄存器 */
    hi_uint32 ui_sfc_cmd_addr;          /* 0x30c -命令操作方式地址寄存器 */
    hi_uint32 ui_reserve9[60];          /* 0x310-0x3fc - 保留  */
    hi_uint32 ui_sfc_cmd_databuf1;      /* 0x400 - 命令操作方式数据Buffer寄存器1*/
    hi_uint32 ui_reserve10[62];         /* 0x404-0x4f8 - 命令操作方式数据Buffer寄存器2~63 */
    hi_uint32 ui_sfc_cmd_databuf64;     /* 0x4fc - 命令操作方式数据Buffer寄存器64*/
}hi_sfc_reg_s;

// Define the union BUS_CONFIG1
typedef union
{
    struct
    {
        hi_uint32    rd_mem_if_type         : 3   ; // [0..2] 总线读操作指定连接的SPI FLASH接口类型。
        hi_uint32    rd_dummy_bytes         : 3   ; // [3..5] 总线写操作DummyByte。
        hi_uint32    rd_prefetch_cnt        : 2   ; // [6..7] 总线访问Flash方式（非定长读）预取周期
        hi_uint32    rd_ins                 : 8   ; // [8..15] 读指令。
        hi_uint32    wr_mem_if_type         : 3   ; // [16..18] 总线写操作指定连接的SPI FLASH接口类型。
        hi_uint32    wr_dummy_bytes         : 3   ; // [19..21] 总线写操作DummyByte
        hi_uint32    wr_ins                 : 8   ; // [22..29] 写指令。
        hi_uint32    wr_enable              : 1   ; // [30] 总线写控制。
        hi_uint32    reserved               : 1   ; //[31] 保留。
    } st_bits;

    hi_uint32 ui_value;
} hi_sfc_bus_config1_u;

// Define the union CMD_CONFIG
typedef union
{
    struct
    {
        hi_uint32    start                 : 1   ; // [0] 指令操作开始标志。
        hi_uint32    sel_cs                : 1   ; // [1] 选择对那个片选进行操作。
        hi_uint32    reserved              : 1   ; // [2] 保留。
        hi_uint32    addr_en               : 1   ; // [3] 此次操作是否有地址。
        hi_uint32    dummy_byte_cnt        : 3   ; // [4..6] 寄存器命令操作方式DummyByte。
        hi_uint32    data_en               : 1   ; // [7] 标识此次操作是否有数据
        hi_uint32    rw                    : 1   ; // [8] 标识此次操作数据读写，需data_en为1,<0：写/1：读>
        hi_uint32    data_cnt              : 8   ; // [16..9] 读写数据长度N+1Bytes
        hi_uint32    mem_if_type           : 3   ; // [19..17] 指定寄存器命令操作方式连接的SPI FLASH接口类型。
        hi_uint32    Reserved              : 12  ; // [31..20] 保留
    } st_bits;

    hi_uint32 ui_value;
} hi_sfc_cmd_config_u;

/*
  * Define the union BUS_FLASH_SIZE
  */
typedef union
{
    struct
    {
        hi_uint32    flash_size_cs0         : 4   ; // [0..3] 指定片选0连接的SPI Flash容量。
        hi_uint32    reserved0              : 4   ; // [4..7] 保留。
        hi_uint32    flash_size_cs1         : 4   ; // [8..11] 指定片选1连接的SPI Flash容量。
        hi_uint32    reserved1              : 20  ; // [12..31] 保留。
    } st_bits;

    hi_uint32 ui_value;
} hi_sfc_bus_flash_size_u;

/*
  * spiflash driver host structure
  */
struct hi_sfc_host 
{
    struct hi_sfc_chip *pst_chip;
    struct mtd_info    *pst_mtd;
    struct device      *pst_dev;    /* kernel only */
    struct mutex       st_lock;     /* kernel only */
    
    hi_void __iomem    *pv_sfc_cs0_buffer;     /* sfc cs0 buffer */
    hi_void __iomem    *pv_sfc_cs1_buffer;     /* sfc cs1 buffer */
};

/*
  * BUS_FLASH_SIZE, bit
  */
typedef enum
{
    HI_SFC_CMD_WRITE_E      = 0x0,
    HI_SFC_CMD_READ_E       = 0x1,
}hi_sfc_cmd_rw_e;

/*
  * BUS_FLASH_SIZE, bit
  */
typedef enum
{
    HI_FLASH_SIZE_0_E       = 0x0,
    HI_FLASH_SIZE_512K_E    = 0x1,
    HI_FLASH_SIZE_1MB_E     = 0x2,
    HI_FLASH_SIZE_2MB_E     = 0x3,
    HI_FLASH_SIZE_4MB_E     = 0x4,
    HI_FLASH_SIZE_8MB_E     = 0x5,
    HI_FLASH_SIZE_16MB_E    = 0x6,
    HI_FLASH_SIZE_32MB_E    = 0x7,
    HI_FLASH_SIZE_64MB_E    = 0x8,
    HI_FLASH_SIZE_128MB_E   = 0x9,
    HI_FLASH_SIZE_256MB_E   = 0xa,
    HI_FLASH_SIZE_512MB_E   = 0xb,
    //HI_FLASH_SIZE_1GB_E     = 0xc,
    //HI_FLASH_SIZE_2GB_E     = 0xd,
    //HI_FLASH_SIZE_4GB_E     = 0xe,
    //HI_FLASH_SIZE_8GB_E     = 0xf
}hi_sfc_bus_flash_size_e;


extern hi_int32 hi_sfc_drv_pre_init(struct hi_sfc_host *pst_host);
extern hi_void hi_sfc_drv_read_id(hi_uint32 ui_chip_sel, hi_uchar8 *puc_buf, hi_uchar8 uc_id_len);
extern hi_int32 hi_sfc_drv_init(struct hi_sfc_host *pst_host);
extern hi_int32 hi_sfc_drv_exit(struct hi_sfc_host *pst_host);
#if 0
extern hi_int32 hi_sfc_drv_bus_read(struct hi_sfc_host *pst_host, hi_uint32 ui_chip_sel, 
                                    hi_uchar8 *puc_buf, hi_uint32 ui_src_addr, hi_uint32 ui_len);
extern hi_int32 hi_sfc_drv_bus_write(struct hi_sfc_host *pst_host, hi_uint32 ui_chip_sel, 
                                    hi_uchar8 *puc_buf, hi_uint32 ui_dst_addr, hi_uint32 ui_len);
#endif
extern hi_int32 hi_sfc_drv_reg_read(struct hi_sfc_host *pst_host, hi_uint32 ui_chip_sel, 
                                    hi_uchar8 *puc_buf, hi_uint32 ui_src_addr, hi_uint32 ui_len);
extern hi_int32 hi_sfc_drv_reg_write(struct hi_sfc_host *pst_host, hi_uint32 ui_chip_sel, 
                                    hi_uchar8 *puc_buf, hi_uint32 ui_dst_addr, hi_uint32 ui_len);
extern hi_int32 hi_sfc_drv_erase_sector(struct hi_sfc_host *pst_host, hi_uint32 ui_chip_sel, hi_uint32 ui_dst_addr, hi_uint32 ui_erase_len);
extern hi_int32 hi_sfc_drv_set_status_reg(hi_uint32 ui_chip_sel, hi_uchar8 reg_value[], hi_uint32 reg_len);
extern hi_uchar8 hi_sfc_drv_get_config_reg(hi_uint32 ui_chip_sel);
extern hi_uchar8 hi_sfc_drv_get_status_reg(hi_uint32 ui_chip_sel);
extern hi_int32 ATP_check_fix_part_protect_addr(hi_uint32 ui_addr);
extern hi_uchar8 g_chip_mk_id;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_NAND_DRV_H__ */
