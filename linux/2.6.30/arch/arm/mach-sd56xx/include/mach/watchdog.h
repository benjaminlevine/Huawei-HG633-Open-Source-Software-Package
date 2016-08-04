/******************************************************************************
  �ļ�����: watchdog.h
  ��������: ͷ�ļ�
  �汾����: V1.0

  ��������: D2011_09_30
  ��������: zhouyu 00204772

  �޸ļ�¼: 
            ���ɳ���.
******************************************************************************/
#ifndef __HI_WATCHDOG_H__
#define __HI_WATCHDOG_H__

// REG define
#define HI_REG_BASE_CRG                 0x14880000

//����λʱ�伫ֵ
#define HI_WDG_MIN_INTERVAL             1
#define HI_WDG_MAX_INTERVAL             512

/* Ĭ��ι��ʱ���Լ�����λʱ�� */
#define HI_WDG_DFT_FEED_TIME            3
#define HI_WDG_DFT_RST_TIME             30

/*crg ---- crg����Ĵ����ṹ*/
typedef struct
{
    volatile hi_uint32 ui_crg_sc_sysstat;               /* 0x0 - ��λ���ƼĴ��� */
    volatile hi_uint32 ui_crg_ecsplll_ctrl0;            /* 0x4 - ECS PLL���ƼĴ��� */
    volatile hi_uint32 ui_crg_ecsplll_ctrl1;            /* 0x8 - ECS PLLƵ�ʿ��ƼĴ��� */
    volatile hi_uint32 ui_crg_ethplll_ctrl0;            /* 0xc - ETH PLL���ƼĴ��� */
    volatile hi_uint32 ui_crg_ethplll_ctrl1;            /* 0x10 - ETH PLLƵ�ʿ��ƼĴ��� */
    volatile hi_uint32 ui_crg_sc_peren0;                /* 0x14 - ����ʱ��ʹ�ܼĴ���0 */
    volatile hi_uint32 ui_crg_sc_perdis0;               /* 0x18 - ����ʱ�ӽ�ֹ�Ĵ���0 */
    volatile hi_uint32 ui_crg_sc_perclkst0;             /* 0x1c - ����ʱ��ʹ��״̬�Ĵ��� */
    volatile hi_uint32 ui_crg_sc_peren1;                /* 0x20 - ����ʱ��ʹ�ܼĴ���1 */
    volatile hi_uint32 ui_crg_sc_perdis1;               /* 0x24 - ����ʱ�ӽ�ֹ�Ĵ���1 */
    volatile hi_uint32 ui_crg_sc_perclkst1;             /* 0x28 - ����ʱ��ʹ��״̬�Ĵ���1 */
    volatile hi_uint32 ui_crg_sc_rst_ctrl0;             /* 0x2c - ���踴λ���ƼĴ���0 */
    volatile hi_uint32 ui_crg_sc_rst_ctrl1;             /* 0x30 - ���踴λ���ƼĴ���1 */
    volatile hi_uint32 ui_crg_sc_rst_ctrl2;             /* 0x34 - ���踴λ���ƼĴ���2 */
    volatile hi_uint32 ui_crg_cpu_cfg;                  /* 0x38 - CPU���ƼĴ��� */
    volatile hi_uint32 ui_crg_hw_cfg;                   /* 0x3c - HW���ƼĴ��� */
    volatile hi_uint32 ui_crg_sc_perctrl0;              /* 0x40 - ������ƼĴ���0 */
    volatile hi_uint32 ui_crg_sc_perctrl1;              /* 0x44 - ������ƼĴ���1 */
    volatile hi_uint32 ui_crg_sc_perctrl2;              /* 0x48 - ������ƼĴ���2 */
    volatile hi_uint32 ui_crg_reserved_1[1];            /* 0x4c - ���� */
    volatile hi_uint32 ui_crg_sc_perctrl3;              /* 0x50 - ������ƼĴ���3 */
    volatile hi_uint32 ui_crg_sc_perctrl4;              /* 0x54 - ������ƼĴ���4 */
    volatile hi_uint32 ui_crg_sc_perctrl5;              /* 0x58 - ������ƼĴ���5 */
    volatile hi_uint32 ui_crg_sc_perctrl6;              /* 0x5c - ������ƼĴ���6 */
    volatile hi_uint32 ui_crg_sc_perctrl7;              /* 0x60 - ������ƼĴ���7 */
    volatile hi_uint32 ui_crg_sc_perctrl8;              /* 0x64 - ������ƼĴ���8 (Ӳ�����Ź�ʹ�ܿ���)*/
    volatile hi_uint32 ui_crg_ram_ctrl_bus;             /* 0x68 - RAM���ƼĴ��� */
    volatile hi_uint32 ui_crg_wdg_init_cfg;             /* 0x6c - ���Ź���ʼ�������ź� */
    volatile hi_uint32 ui_crg_osc_cfg;                  /* 0x70 - ���������ź� */
    volatile hi_uint32 ui_crg_reserved_2[7];            /* 0x74:8c - ���� */
    volatile hi_uint32 ui_crg_crg_stat0;                /* 0x90 - CRG״̬�Ĵ���0 */
    volatile hi_uint32 ui_crg_dying_gasp_pre_int;       /* 0x94 - DYING_GASP�жϼĴ��� */
    volatile hi_uint32 ui_crg_dying_gasp_int_mask;      /* 0x98 - DYING_GASP�ж�����Ĵ��� */
    volatile hi_uint32 ui_crg_dying_gasp_int_insert;    /* 0x9c - DYING_GASP�жϲ���Ĵ��� */
    volatile hi_uint32 ui_crg_reserved_3[24];           /* 0xa0:fc - ���� */
    volatile hi_uint32 ui_crg_sc_chip_id;               /* 0x100 - оƬID�Ĵ��� */
    volatile hi_uint32 ui_crg_crg_rev0;                 /* 0x104 - ECOԤ���Ĵ���0 */
}hi_crg_reg_s;

/*
  * SC_PERCTRL4 reg
  */
typedef union
{
    hi_uint32 ui_value;
    struct
    {
        hi_uint32 ui_sfc_freq_sel           : 2   ; // [1..0] SFC�ӿڵĹ���Ƶ��ѡ��
        hi_uint32 ui_ddr_clk_freq           : 1   ; // [2] ��ʼ�ϵ�ʱ��DDRC����ʱ��Ƶ��ѡ��
        hi_uint32 ui_Reserved_0             : 1   ; // [3]  ����
        hi_uint32 ui_ecs_ddr_freq           : 2   ; // [5..4] A9��DDRC��ʱ�ӱ�����ϵ����
        hi_uint32 ui_Reserved_1             : 1   ; // [6]  ����
        hi_uint32 ui_ddrc_aclk_freq_sel     : 1   ; // [7] DDRC����ʱ��Ƶ�ʺ�DDRPHY��ʱ�ӱ���ѡ��
        hi_uint32 ui_Reserved_2             : 2   ; // [9..8]  ����
        hi_uint32 ui_wdg_rst_time_cfg       : 9   ; // [18..10] ���Ź���λʱ������
        hi_uint32 ui_test_clk_ctrl          : 5   ; // [23..19] ����ʱ��TEST_CLK������Լ�ʱ�Ӽ��ģ����ʱ��Դѡ������ź�
        hi_uint32 ui_det_clk_en             : 1   ; // [24] ʱ�Ӽ��ģ��ļ��ʹ���ź�
        hi_uint32 ui_Reserved_3             : 3   ; // [27..25]  ����
        hi_uint32 ui_fephy_clk_det_sel      : 2   ; // [29..28] FEPHY�Ĳο�ʱ�ӵĲ������������TX��RX)
        hi_uint32 ui_Reserved_4             : 2   ; // [31..30]  ����
    }st_bits;
} hi_crg_sc_perctrl4_u;


/*ι������*/
extern hi_void hi_kernel_wdg_en(hi_uint32 ui_en);

/*����Ӳ����ʱ���� */
extern hi_void hi_kernel_wdg_set_reset_time(hi_uint32 ui_time);

/*�幷����*/
extern hi_void hi_kernel_wdg_clear(hi_void);

/* ����Ƿ�����ι�� */
extern hi_uint32 hi_kernel_wdg_check(hi_void);


#endif /*__HI_WATCHDOG_H__*/

