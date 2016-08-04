/******************************************************************************
  �ļ�����: hi_sfc_drv.h
  ��������: sfc��������ͷ�ļ�
  �汾����: V1.0

  ��������: D2013_08_01
  ��������: zhouyu 00204772

  �޸ļ�¼: 
            ���ɳ���.
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
    hi_uint32 ui_sfc_con;               /* (0x00) -SFCģ�����üĴ��� */
    hi_uint32 ui_sfc_cmd;               /* (0x04) -�������üĴ��� */
    hi_uint32 ui_sfc_ins;               /* (0x08) -ָ��Ĵ��� */
    hi_uint32 ui_sfc_addr;              /* (0x0c) -��ַ�Ĵ��� */
    hi_uint32 ui_sfc_databuffer1;       /* (0x10) -����buffer1�Ĵ��� */
    hi_uint32 ui_sfc_databuffer2;       /* (0x14) -����buffer2�Ĵ��� */
    hi_uint32 ui_sfc_databuffer3;       /* (0x18) -����buffer3�Ĵ��� */
    hi_uint32 ui_sfc_databuffer4;       /* (0x1c) -����buffer4�Ĵ��� */
    hi_uint32 ui_sfc_databuffer5;       /* (0x20) -����buffer5�Ĵ��� */
    hi_uint32 ui_sfc_databuffer6;       /* (0x24) -����buffer6�Ĵ��� */
    hi_uint32 ui_sfc_databuffer7;       /* (0x28) -����buffer7�Ĵ��� */
    hi_uint32 ui_sfc_databuffer8;       /* (0x2c) -����buffer8�Ĵ��� */
    hi_uint32 ui_sfc_cs0config;         /* (0x30) -Ƭѡ0���üĴ��� */
    hi_uint32 ui_sfc_cs1config;         /* (0x34) -Ƭѡ1���üĴ��� */
    hi_uint32 ui_sfc_cs0baseaddr;       /* (0x38) -Ƭѡ0��ַ�Ĵ��� */
    hi_uint32 ui_sfc_cs1baseaddr;       /* (0x3c) -Ƭѡ1��ַ�Ĵ��� */
    hi_uint32 ui_sfc_cs1aliasaddr;      /* (0x40) -Ƭѡ1�ı�������ַ�Ĵ��� */
    hi_uint32 ui_sfc_timing;            /* (0x44) -����ʱ������Ĵ��� */
    hi_uint32 ui_reserve1[2];           /* 0x58-0x4c -���� */
    hi_uint32 ui_sfc_intrawstatus;      /* (0x50) -�ж�ԭʼ״̬�Ĵ��� */
    hi_uint32 ui_sfc_intstatus;         /* (0x54) -�������ε��ж�״̬�Ĵ��� */
    hi_uint32 ui_sfc_intmask;           /* (0x58) -�ж����μĴ��� */
    hi_uint32 ui_sfc_intclear;          /* 0x5c - �ж����������*/
    hi_uint32 ui_reserve2[40];          /* 0x60-0xfc - ���� */
    hi_uint32 ui_sfc_global_config;     /* 0x100 -ȫ�����üĴ��� */
    hi_uint32 ui_reserve3[3];           /* 0x104-0x10c - ���� */
    hi_uint32 ui_sfc_timing2;           /* 0x110 - Timing���üĴ���*/
    hi_uint32 ui_reserve4[3];           /* 0x114-0x11c - ���� */
    hi_uint32 ui_sfc_int_raw_status;    /* 0x120 - �ж�ԭʼ״̬�Ĵ��� */
    hi_uint32 ui_sfc_int_status;        /* 0x124 - �������δ�����ж�״̬�Ĵ��� */
    hi_uint32 ui_sfc_int_mask;          /* 0x128 - �ж����μĴ��� */
    hi_uint32 ui_sfc_int_clear;         /* 0x12c - �ж�����Ĵ���*/
    hi_uint32 ui_reserve5[50];          /* 0x114-0x11c - ����  */
    hi_uint32 ui_sfc_version;           /* 0x1f8 - �汾�Ĵ��� */
    hi_uint32 ui_sfc_version_sel;       /* 0x1fc - �汾ѡ��Ĵ��� */
    hi_uint32 ui_sfc_bus_config1;       /* 0x200 - ���߲�����ʽ���üĴ��� */
    hi_uint32 ui_sfc_bus_config2;       /* 0x204 - ���߲�����ʽ���üĴ���*/
    hi_uint32 ui_reserve6[2];           /* 0x208-0x20c - ����  */
    hi_uint32 ui_sfc_bus_flash_size;    /* 0x210 - ���߲�����ʽӳ��ߴ�Ĵ��� */
    hi_uint32 ui_sfc_bus_base_addr_cs0; /* 0x214 - ���߲�����ʽƬѡ0ӳ�����ַ�Ĵ���*/
    hi_uint32 ui_sfc_bus_base_addr_cs1; /* 0x218 - ���߲�����ʽƬѡ1ӳ�����ַ�Ĵ���*/
    hi_uint32 ui_sfc_bus_alias_addr;    /* 0x21c - ���߲�����ʽAliasӳ�����ַ�Ĵ���*/
    hi_uint32 ui_sfc_bus_alias_cs;      /* 0x220 -���߲�����ʽAliasƬѡָʾ�Ĵ��� */
    hi_uint32 ui_reserve7[55];          /* 0x220-0x2fc - ����  */
    hi_uint32 ui_sfc_cmd_config;        /* 0x300 -���������ʽ���üĴ��� */
    hi_uint32 ui_reserve8[1];           /* 0x304 - ����  */
    hi_uint32 ui_sfc_cmd_ins;           /* 0x308 -���������ʽָ��Ĵ��� */
    hi_uint32 ui_sfc_cmd_addr;          /* 0x30c -���������ʽ��ַ�Ĵ��� */
    hi_uint32 ui_reserve9[60];          /* 0x310-0x3fc - ����  */
    hi_uint32 ui_sfc_cmd_databuf1;      /* 0x400 - ���������ʽ����Buffer�Ĵ���1*/
    hi_uint32 ui_reserve10[62];         /* 0x404-0x4f8 - ���������ʽ����Buffer�Ĵ���2~63 */
    hi_uint32 ui_sfc_cmd_databuf64;     /* 0x4fc - ���������ʽ����Buffer�Ĵ���64*/
}hi_sfc_reg_s;

// Define the union BUS_CONFIG1
typedef union
{
    struct
    {
        hi_uint32    rd_mem_if_type         : 3   ; // [0..2] ���߶�����ָ�����ӵ�SPI FLASH�ӿ����͡�
        hi_uint32    rd_dummy_bytes         : 3   ; // [3..5] ����д����DummyByte��
        hi_uint32    rd_prefetch_cnt        : 2   ; // [6..7] ���߷���Flash��ʽ���Ƕ�������Ԥȡ����
        hi_uint32    rd_ins                 : 8   ; // [8..15] ��ָ�
        hi_uint32    wr_mem_if_type         : 3   ; // [16..18] ����д����ָ�����ӵ�SPI FLASH�ӿ����͡�
        hi_uint32    wr_dummy_bytes         : 3   ; // [19..21] ����д����DummyByte
        hi_uint32    wr_ins                 : 8   ; // [22..29] дָ�
        hi_uint32    wr_enable              : 1   ; // [30] ����д���ơ�
        hi_uint32    reserved               : 1   ; //[31] ������
    } st_bits;

    hi_uint32 ui_value;
} hi_sfc_bus_config1_u;

// Define the union CMD_CONFIG
typedef union
{
    struct
    {
        hi_uint32    start                 : 1   ; // [0] ָ�������ʼ��־��
        hi_uint32    sel_cs                : 1   ; // [1] ѡ����Ǹ�Ƭѡ���в�����
        hi_uint32    reserved              : 1   ; // [2] ������
        hi_uint32    addr_en               : 1   ; // [3] �˴β����Ƿ��е�ַ��
        hi_uint32    dummy_byte_cnt        : 3   ; // [4..6] �Ĵ������������ʽDummyByte��
        hi_uint32    data_en               : 1   ; // [7] ��ʶ�˴β����Ƿ�������
        hi_uint32    rw                    : 1   ; // [8] ��ʶ�˴β������ݶ�д����data_enΪ1,<0��д/1����>
        hi_uint32    data_cnt              : 8   ; // [16..9] ��д���ݳ���N+1Bytes
        hi_uint32    mem_if_type           : 3   ; // [19..17] ָ���Ĵ������������ʽ���ӵ�SPI FLASH�ӿ����͡�
        hi_uint32    Reserved              : 12  ; // [31..20] ����
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
        hi_uint32    flash_size_cs0         : 4   ; // [0..3] ָ��Ƭѡ0���ӵ�SPI Flash������
        hi_uint32    reserved0              : 4   ; // [4..7] ������
        hi_uint32    flash_size_cs1         : 4   ; // [8..11] ָ��Ƭѡ1���ӵ�SPI Flash������
        hi_uint32    reserved1              : 20  ; // [12..31] ������
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
