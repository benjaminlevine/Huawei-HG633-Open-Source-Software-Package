/******************************************************************************
  �ļ�����: hi_nand.h
  ��������: nand��������ͷ�ļ�
  �汾����: V1.0

  ��������: D2013_04_20
  ��������: zhouyu 00204772

  �޸ļ�¼: 
            ���ɳ���.
******************************************************************************/
#ifndef __HI_NAND_DRV_H__
#define __HI_NAND_DRV_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/* base bus addr define */
#define HI_NFC_BUFFER_BASE              0x20000000
#define HI_NFC_REG_BASE                 0x10A30000

#if 1    //nfc v500
#define HI_NFC_NANDC_BUF_LEN            (0x2000+0x400)  //9KB
#else    //nfc v300
#define HI_NFC_NANDC_BUF_LEN            (0x800+0x140)   //2048+320B
#endif

/* ��ECC0ģʽ��ȱʡOOB���� */
#define HI_NFC_ECC_DEFAULT_OOB_SIZE     32

#if 1    //nfc v500
/* page0 ֧�������Ч���ݳ��� */
#define HI_NFC_MAX_PAGE0_LEN            0x410   //1040B
#endif

/* ��д������ */
#define HI_NFC_PWIDTH_W_LCNT            0x5
#define HI_NFC_PWIDTH_R_LCNT            0x5
#define HI_NFC_PWIDTH_RW_HCNT           0x3

/* �ж�ʹ������ */
#define HI_NFC_INTERRUPT_MASK           0x7FF

/* ����NAND Flash�ĵ�ַ������ */
#define HI_NFC_RW_ADDR_CYCLE            0x5
#define HI_NFC_ERASE_ADDR_CYCLE         0x3
#define HI_NFC_READID_ADDR_CYCLE        0x1

/* nandc֧�����DMA buffer���� */
#define HI_NFC_MAX_DMA_BUF_LEN          0x1000


/*************************
  * enum
  *************************/
typedef enum
{
    HI_NAND_ADDR_NUM_5CYCLE_E   = 0x0,
    HI_NAND_ADDR_NUM_4CYCLE_E   = 0x1,
}HI_NAND_ADDR_NUM_E;

typedef enum
{
    HI_NAND_OP_MODE_BOOT_E      = 0x0,
    HI_NAND_OP_MODE_NORMAL_E    = 0x1,
}HI_NAND_OP_MODE_E;

typedef enum
{
    HI_NAND_BUS_WIDTH_8BIT_E    = 0x0,
    HI_NAND_BUS_WIDTH_16BIT_E   = 0x1,
}HI_NAND_BUS_WIDTH_E;

typedef enum
{
    HI_NAND_PAGE_2KB_E     = 0x1,
    HI_NAND_PAGE_4KB_E     = 0x2,
    HI_NAND_PAGE_8KB_E     = 0x3
}HI_PAGE_SIZE_E;

/*************************
  * struct
  *************************/
typedef struct 
{
    hi_uint32 ui_nfc_con;                 /* (0x00) -NANDC���üĴ���                                                 */
    hi_uint32 ui_nfc_pwidth;              /* (0x04) -��д���������üĴ���                        */
    hi_uint32 ui_nfc_opidle;              /* (0x08) -����������üĴ���                                    */
    hi_uint32 ui_nfc_cmd;                 /* (0x0c) -���������üĴ���                                          */
    hi_uint32 ui_nfc_addrl;               /* (0x10) -��λ��ַ���üĴ���                                    */
    hi_uint32 ui_nfc_addrh;               /* (0x14) -��λ��ַ���üĴ���                                    */
    hi_uint32 ui_nfc_data_num;            /* (0x18) -��д������Ŀ���üĴ���                        */
    hi_uint32 ui_nfc_op;                  /* (0x1c) -�����Ĵ���                                                            */
    hi_uint32 ui_nfc_status;              /* (0x20) -״̬�Ĵ���                                                            */
    hi_uint32 ui_nfc_inten;               /* (0x24) -�ж�ʹ�ܼĴ���                                                */
    hi_uint32 ui_nfc_ints;                /* (0x28) -�ж�״̬�Ĵ���                                                */
    hi_uint32 ui_nfc_intcrl;              /* (0x2c) -�ж�����Ĵ���                                                */
    hi_uint32 ui_nfc_lock;                /* (0x30) -����ַ���üĴ���                                          */
    hi_uint32 ui_nfc_lock_sa0;            /* (0x34) -����ʼ��ַ0���üĴ���                            */
    hi_uint32 ui_nfc_lock_sa1;            /* (0x38) -����ʼ��ַ1���üĴ���                            */
    hi_uint32 ui_nfc_lock_sa2;            /* (0x3c) -����ʼ��ַ2���üĴ���                            */
    hi_uint32 ui_nfc_lock_sa3;            /* (0x40) -����ʼ��ַ3���üĴ���                            */
    hi_uint32 ui_nfc_lock_ea0;            /* (0x44) -��������ַ0���üĴ���                            */
    hi_uint32 ui_nfc_lock_ea1;            /* (0x48) -��������ַ1���üĴ���                            */
    hi_uint32 ui_nfc_lock_ea2;            /* (0x4c) -��������ַ2���üĴ���                            */
    hi_uint32 ui_nfc_lock_ea3;            /* (0x50) -��������ַ3���üĴ���                            */
#if 1    //nfc v500 
    hi_uint32 ui_nfc_expcmd;              /* (0x54) -��չҳ����Ĵ���                                          */
    hi_uint32 ui_nfc_exbcmd;              /* (0x58) -��չ������Ĵ���                                          */
#else    //nfc v300 
    hi_uint32 ui_nfc_reserved[2];         /* (0x54-0x58) -����                                                                    */
#endif
    hi_uint32 ui_nfc_ecc_test;            /* (0x5c) -ECC���ԼĴ���                                                      */
    hi_uint32 ui_nfc_dma_ctrl;            /* (0x60) -DMA���ƼĴ���                                                     */
    hi_uint32 ui_nfc_baddr_d;             /* (0x64) -�����������Ļ���ַ�Ĵ���                  */
    hi_uint32 ui_nfc_baddr_oob;           /* (0x68) -����OOB���Ļ���ַ�Ĵ���                       */
    hi_uint32 ui_nfc_dma_len;             /* (0x6c) -���ͳ��ȼĴ���                                                */
    hi_uint32 ui_nfc_op_para;             /* (0x70) -���������Ĵ���                                                */
    hi_uint32 ui_nfc_version;             /* (0x74) -nandc�汾�Ĵ���                                                   */
#if 1    //nfc v500 
    hi_uint32 ui_nfc_reserved1[3];        /* (0x78-0x80) -����                                                                    */
#else    //nfc v300 
    hi_uint32 ui_nfc_buf_baddr;           /* (0x78) -BUFFER����ַ�Ĵ���                                          */
    hi_uint32 ui_nfc_rd_logic_addr;       /* (0x7c) -DMA��nandflash���߼���ַ�Ĵ���               */
    hi_uint32 ui_nfc_rd_logic_len;        /* (0x80) -DMA��nandflash���߼����ȼĴ���               */
#endif
    hi_uint32 ui_nfc_segment_id;          /* (0x84) -DMA��дnandflash�����ݶ�ID�Ĵ���            */
    hi_uint32 ui_nfc_reserved2[2];        /* (0x88-0x8c) -����                                                                    */
    hi_uint32 ui_nfc_fifo_empty;          /* (0x90) -�ڲ�fifo״̬�Ĵ���                                           */
#if 1    //nfc v500 
    hi_uint32 ui_nfc_boot_set;            /* (0x94) -boot�����趨�Ĵ���                                         */
    hi_uint32 ui_nfc_reserved3;           /* (0x98) -����                                                                              */
#else    //nfc v300 
    hi_uint32 ui_nfc_reserved3;           /* (0x94) -����                                                                             */
    hi_uint32 ui_nf_status;               /* (0x98) -nandflash��sattus�Ĵ���                                          */
#endif
    hi_uint32 ui_nfc_lp_ctrl;             /* (0x9c) -nandc �ĵ͹��Ŀ��ƼĴ���                          */
    hi_uint32 ui_nfc_err_num0_buf0;       /* (0xa0) -nandflash ��һ��buffer����ͳ�ƼĴ���0    */
    hi_uint32 ui_nfc_err_num1_buf0;       /* (0xa4) -nandflash ��һ��buffer����ͳ�ƼĴ���1    */
    hi_uint32 ui_nfc_err_num0_buf1;       /* (0xa8) -nandflash �ڶ���buffer����ͳ�ƼĴ���0    */
    hi_uint32 ui_nfc_err_num1_buf1;       /* (0xac) -nandflash �ڶ���buffer����ͳ�ƼĴ���1    */
    hi_uint32 ui_nfc_rb_mode;             /* (0xb0) -nandflash ��ready_busyģʽ�Ĵ���                     */
    hi_uint32 ui_nfc_baddr_d1;            /* (0xb4) -�����������Ļ���ַ�Ĵ���1               */
    hi_uint32 ui_nfc_baddr_d2;            /* (0xb8) -�����������Ļ���ַ�Ĵ���2               */
    hi_uint32 ui_nfc_baddr_d3;            /* (0xbc) -�����������Ļ���ַ�Ĵ���3                */
#if 1    //nfc v500 
    hi_uint32 ui_nfc_reserved4;           /* (0xc0) -����                                                                             */
#else    //nfc v300 
    hi_uint32 ui_nfc_randomizer;          /* (0xc0) -nandflash ��randomizerʹ�ܼĴ���                      */
#endif
    hi_uint32 ui_nfc_boot_cfg;            /* (0xc4) -nandflash ��boot���üĴ���                               */
#if 1    //nfc v500 
    hi_uint32 ui_nfc_oob_sel;             /* (0xc8) -16bit ECC��OOB������ѡ��Ĵ���              */
    hi_uint32 ui_nfc_mem_ctrl;            /* (0xcc) -����mem����                                                          */
    hi_uint32 ui_nfc_sync_timing;         /* (0xd0) -ͬ��NAND��дʱ�����                                 */
    hi_uint32 ui_nfc_dll_ctrl;            /* (0xd4) -DLLģ�������Ϣ                                                */
    hi_uint32 ui_nfc_dll_status;          /* (0xd8) -DLLģ��״̬��Ϣ                                                */
#endif
}hi_nfc_reg_s;

/*
  * NFC_CON reg
  */
typedef union
{
    hi_uint32 ui_value;
    struct
    {
        hi_uint32 ui_op_mode:1;           /* [0]-nandc����ģʽ                            */
        hi_uint32 ui_page_type:3;         /* [1:3]-nandflashҳ��С                         */
        hi_uint32 ui_bus_width:1;         /* [4]-nandflash�����߿��                */
        hi_uint32 ui_reserved:2;          /* [5:6]-����                                             */
        hi_uint32 ui_cs_ctrl:1;           /* [7]-Ƭѡ����                                    */
        hi_uint32 ui_rb_sel:1;            /* [8]-ready/busy�źſ���                   */
        hi_uint32 ui_ecc_type:3;          /* [9:11]-eccģʽ                                      */
#if 1    //nfc v500 
        hi_uint32 ui_reserved1:1;         /* [12]-����                                              */
        hi_uint32 ui_randomizer_en:1;     /* [13]-randomizerʹ��                             */
        hi_uint32 ui_nf_mode:2;           /* [14:15]-nandflash�ӿ�����ѡ��   */
        hi_uint32 ui_reserved2:16;        /* [16:31]-����                                         */
#else    //nfc v300 
        hi_uint32 ui_reserved1:20;        /* [12:31]-����                                         */
#endif
    }st_bits;
} hi_nfc_con_u;

/*
  * NFC_PWIDTH reg
  */
typedef union
{
    hi_uint32 ui_value;
    struct
    {
        hi_uint32 ui_w_lcnt:4;            /* [0:3] -nandflashд�źŵ͵�ƽ���         */
        hi_uint32 ui_r_lcnt:4;            /* [4:7] -nandflash���źŵ͵�ƽ���         */
        hi_uint32 ui_rw_hcnt:4;           /* [8:11] -nandflash��д�źŸߵ�ƽ��� */
        hi_uint32 ui_reserved:20;         /* [12:31] -����                                                       */
    }st_bits;
} hi_nfc_pwidth_u;

/*
  * NFC_CMD reg
  */
typedef union
{
    hi_uint32 ui_value;
    struct
    {
        hi_uint32 ui_cmd1:8;              /* [0:7]   -controller����nandflash�ĵ�һ������*/
        hi_uint32 ui_cmd2:8;              /* [8:15] -controller����nandflash�ĵڶ�������*/
        hi_uint32 ui_read_status_cmd:8;   /* [16:23] -read status������                                       */
        hi_uint32 ui_reserved:8;          /* [24:31] -����                                                              */
    }st_bits;
} hi_nfc_cmd_u;

/*
  * NFC_ADDRL reg
  */
typedef union
{
    hi_uint32 ui_value;
    struct
    {
        hi_uint32 ui_page_inner_addr:16;  /* [0:15]-ҳ�ڵ�ַ��ֻ��bus��ʽֻ��OOBʱʹ��    */
        hi_uint32 ui_page_addr:16;        /* [16:31]-ҳ��ַ                                                                                */
    }st_rw_bits;
    struct
    {
        hi_uint32 ui_erase_addr:24;       /* [0:23]-��������ʱ�Ŀ��ַ    */
        hi_uint32 ui_reserved:8;          /* [24:31]-����                                            */
    }st_erase_bits;
} hi_nfc_addrl_u;

/*
  * NFC_ADDRH reg, read/write may use, erase never use!!
  */
typedef union
{
    hi_uint32 ui_value;
    struct
    {
        hi_uint32 ui_addr_h:8;             /* [0:7]-��8bit��ַ�ռ�                       */
        hi_uint32 ui_reserved:24;          /* [8:31]-����                                            */
    }st_rw_bits;
} hi_nfc_addrh_u;

/*
  * NFC_OP reg
  */
typedef union
{
    hi_uint32 ui_value;
    struct
    {
        hi_uint32 ui_read_status_en:1;    /* [0]-��״̬ʹ��                                                                          */
        hi_uint32 ui_read_data_en:1;      /* [1]-������ʹ��                                                                          */
        hi_uint32 ui_wait_ready_en:1;     /* [2]-�ȴ�ready/busy�źű��ʹ��                                       */
        hi_uint32 ui_cmd2_en:1;           /* [3]-��cmd2����ʹ��                                                                  */
        hi_uint32 ui_write_data_en:1;     /* [4]-��nandflashд����ʹ��                                                      */
        hi_uint32 ui_addr_en:1;           /* [5]-��nandд������ַʹ��                                                 */
        hi_uint32 ui_cmd1_en:1;           /* [6]-��cms1����ʹ��                                                                  */
        hi_uint32 ui_nf_cs:2;             /* [7:8]-ѡ�������nandflash��������Ƭѡ                     */
        hi_uint32 ui_address_cycle:3;     /* [9:11]-����nandflash�ĵ�ַ������                                     */
#if 1    //nfc v500 
        hi_uint32 ui_read_id_en:1;        /* [12]-����toggle nand��0:�Ƕ�ID������1:��ID����      */
        hi_uint32 ui_rw_reg_en:1;         /* [13]-��ͬ��nand��0:��д���ݣ�1:��д�Ĵ���     */
        hi_uint32 ui_reserved:18;         /* [14:31]-����                                                                                      */
#else    //nfc v300 
        hi_uint32 ui_reserved:20;         /* [12:31]-����                                                                                      */
#endif
    }st_bits;
} hi_nfc_op_u;

/*
  * NFC_STATUS reg
  */
typedef union
{
    hi_uint32 ui_value;
    struct
    {
        hi_uint32 ui_nfc_ready:1;         /* [0]-nandc��ǰ����״̬                                            */
        hi_uint32 ui_nf0_ready:1;         /* [1]-Ƭѡ0������ready/busy�ź�״̬                */
        hi_uint32 ui_nf1_ready:1;         /* [2]-Ƭѡ1������ready/busy�ź�״̬                */
#if 1    //nfc v500 
        hi_uint32 ui_nf2_ready:1;         /* [3]-Ƭѡ2������ready/busy�ź�״̬                */
        hi_uint32 ui_nf3_ready:1;         /* [4]-Ƭѡ3������ready/busy�ź�״̬                */
        hi_uint32 ui_reserved:3;          /* [5:7]-����                                                                          */
        hi_uint32 ui_nf_status:8;         /* [8:15]-��nandflash���ص�nandflash ��status����    */
        hi_uint32 ui_nf_ecc_type:3;       /* [16:18]-nandc eccģʽ����CON��ͬ��ֻ��       */
        hi_uint32 ui_nf_randomizer_en:1;  /* [19]-randomizer״̬����CON��ͬ����ֻ��    */
        hi_uint32 ui_reserved1:12;        /* [20:31] -����                                                                     */
#else    //nfc v300 
        hi_uint32 ui_reserved:2;          /* [3:4]-����                                                                          */
        hi_uint32 ui_nf_status:8;         /* [5:12]-��nandflash���ص�nandflash ��status����    */
        hi_uint32 ui_reserved1:19;        /* [13:31] -����                                                                     */
#endif
    }st_bits;
} hi_nfc_status_u;

/*
  * NFC_INTS reg
  */
typedef union
{
    hi_uint32 ui_value;
    struct
    {
        hi_uint32 ui_op_done:1;           /* [0]-NANDC���β��������ж�                                            */
        hi_uint32 ui_cs0_done:1;          /* [1]-Ƭѡ0��Ӧ��ready/busy�ź��ɵͱ���ж�     */
        hi_uint32 ui_cs1_done:1;          /* [2]-Ƭѡ1��Ӧ��ready/busy�ź��ɵͱ���ж�     */
        hi_uint32 ui_cs2_done:1;          /* [3]-Ƭѡ2��Ӧ��ready/busy�ź��ɵͱ���ж�     */
        hi_uint32 ui_cs3_done:1;          /* [4]-Ƭѡ3��Ӧ��ready/busy�ź��ɵͱ���ж�     */
        hi_uint32 ui_err_valid:1;         /* [5]-�����ɾ����Ĵ����ж�                                          */
        hi_uint32 ui_err_invalid:1;       /* [6]-�������ɾ����Ĵ����ж�                                    */
        hi_uint32 ui_ahb_op:1;            /* [7]-NANDC��дflash����cpu��дnandc buffer�����ж� */
        hi_uint32 ui_wr_lock:1;           /* [8]-��lock��ַ����д���������ж�                        */
        hi_uint32 ui_dma_done:1;          /* [9]-DMA��������ж�                                                           */
        hi_uint32 ui_dma_err:1;           /* [10]-DMA�������߳��ִ����ж�                                 */
        hi_uint32 ui_reserved:21;         /* [11:31] -����                                                                                 */
    }st_bits;
} hi_nfc_ints_u;

/*
  * NFC_DMA_CTRL reg
  */
typedef union
{
    hi_uint32 ui_value;
    struct
    {
        hi_uint32 ui_dma_start:1;         /* [0]-����dma����              */
        hi_uint32 ui_dma_wr_en:1;         /* [1]-dma��дʹ��              */
        hi_uint32 ui_reserved:2;          /* [2:3]-����                             */
        hi_uint32 ui_burst4_en:1;         /* [4]-burst4ʹ��                       */
        hi_uint32 ui_burst8_en:1;         /* [5]-burst8ʹ��                       */
        hi_uint32 ui_burst16_en:1;        /* [6]-burst16ʹ��                     */
        hi_uint32 ui_dma_addr_num:1;      /* [7]-��ַ��                           */
        hi_uint32 ui_dma_nf_cs:2;         /* [8:9]-ѡ��dmaƬѡ           */
#if 1    //nfc v500 
        hi_uint32 ui_reserved1:1;         /* [10]-����                               */
#else    //nfc v300 
        hi_uint32 ui_rw_nf_disable:1;     /* [10]-�ж�DMA��дʱ���ݰ���ͬ�� */
#endif
        hi_uint32 ui_wr_cmd_disable:1;    /* [11]-�Ƿ񷢳�д���� */
#if 1    //nfc v500 
        hi_uint32 ui_dma_rd_oob:1;        /* [12]-�Ƿ�ֻ��OOB            */
        hi_uint32 ui_reserved2:19;        /* [13:31]-����                          */
#else    //nfc v300 
        hi_uint32 ui_reserved2:20;        /* [12:31]-����                          */
#endif
    }st_bits;
} hi_nfc_dma_ctrl_u;

/*
  * NFC_OP_PARA reg
  */
typedef union
{
    hi_uint32 ui_value;
    struct
    {
        hi_uint32 ui_data_rw_en:1;        /* [0] -nandflash��������д             */
        hi_uint32 ui_oob_rw_en:1;         /* [1] -nandflash��������дʹ�� */
        hi_uint32 ui_data_edc_en:1;       /* [2] -data��У��ʹ��                    */
        hi_uint32 ui_oob_edc_en:1;        /* [3] -oob��У��ʹ��                     */
#if 1    //nfc v500 
        hi_uint32 ui_reserved:28;         /* [4:31] -����                                       */
#else    //nfc v300 
        hi_uint32 ui_data_ecc_en:1;       /* [4] -data��ECC����ʹ��              */
        hi_uint32 ui_oob_ecc_en:1;        /* [5] -oob��ECC����ʹ��              */
        hi_uint32 ui_reserved:26;         /* [6:31] -����                                       */
#endif
    }st_bits;
} hi_nfc_op_para_u;

/*
  * nand driver host structure
  */
struct hi_nand_host 
{
    struct hi_nand_chip *pst_chip;
    struct mtd_info     *pst_mtd;
    struct device       *pst_dev;       /* kernel only */

    HI_NAND_ECC_TYPE_E en_ecc_type;     /* nand ecc type, support 1bit/4bit/8bit/24bit */
    HI_PAGE_SIZE_E     en_page_type;    /* nand page type, support 2k/4k/8k */

    hi_uint32 ui_dma;                   /* dma buffer physics addr */
    hi_uint32 ui_dma_extern;            /* 8k pagesize need an extern dma buffer */
    hi_uint32 ui_dma_oob;               /* oob dma physics addr */
    
    hi_uchar8 *puc_buffer;              /* dma buffer virtual addr */
    hi_uchar8 *puc_buffer_extern;        /* 8k pagesize need an extern dma buffer */
    hi_uchar8 *puc_oob_buffer;

    hi_void __iomem *pv_nand_buffer;          /* nandc buffer */
};

#if 0    //nfc v300 
/* ��ecc0��OOB���ȼ��� */
struct hi_nand_oob_size
{
    hi_uint32 ui_ecc_type;
    hi_uint32 ui_page_type;
    hi_uint32 ui_oob_size;
};
#endif

#if 1    //nfc v500 
/* bus��ʽֻ��OOB������£�����OOB��Ϣ��sector��ҳ����ʼ��ַ */
struct hi_nand_oob_offset
{
    hi_uint32 ui_ecc_type;
    hi_uint32 ui_page_type;
    hi_uint32 ui_oob_sel;
    hi_uint32 ui_oob_offset;
};
#endif

/*************************
  * declare
  *************************/
hi_void  hi_nand_drv_reset(hi_void);
hi_int32 hi_nand_drv_init(struct hi_nand_host *host);
hi_int32 hi_nand_drv_exit(struct hi_nand_host *pst_host);
hi_int32 hi_nand_drv_pre_init(struct hi_nand_host *pst_host);
hi_int32 hi_nand_drv_erase(hi_uint32 ui_dst_addr);
hi_int32 hi_nand_drv_read(struct hi_nand_host *pst_host, hi_uchar8 *puc_buf, hi_uint32 ui_dst_addr);
hi_int32 hi_nand_drv_read_oob(struct hi_nand_host *pst_host, hi_uchar8 *puc_buf, hi_uint32 ui_dst_addr);
hi_int32 hi_nand_drv_write(struct hi_nand_host *pst_host, hi_uchar8 *puc_buf, hi_uint32 ui_dst_addr);
hi_int32 hi_nand_drv_write_oob(struct hi_nand_host *pst_host, hi_uchar8 *puc_buf, hi_uint32 ui_dst_addr);
hi_int32 hi_nand_drv_wp_check(hi_void);

#if 0    //nfc v300 
hi_uint32 hi_nand_drv_get_oob_size(struct hi_nand_host *pst_host);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_NAND_DRV_H__ */
