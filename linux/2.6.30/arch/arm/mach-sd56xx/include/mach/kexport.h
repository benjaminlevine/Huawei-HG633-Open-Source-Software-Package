
#ifndef __KEXPORT_H__
#define __KEXPORT_H__


/* sd56xx get chip id */
typedef enum
{
    HI_CHIP_ID_H_E,
    HI_CHIP_ID_T_E,
    HI_CHIP_ID_NONE_E,
} HI_CHIP_ID_E;

/* sd56xx get boot type */
typedef enum
{
    HI_BOOT_ROM = 0x0,
    HI_BOOT_SPIFLASH,
    HI_BOOT_NANDFLASH,
}HI_BOOT_DEV_E;

// sd56xx kernel export functions
HI_CHIP_ID_E hi_kernel_get_chip_id(void);
HI_BOOT_DEV_E hi_kernel_get_boot_sel(void);
int hi_kernel_acp_on(void);
uint hi_kernel_ahb_clk_get(uint *pui_ahb_clk);

#endif  //#ifndef    __KEXPORT_H__


