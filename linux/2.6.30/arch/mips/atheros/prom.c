#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/bootmem.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>

#include <atheros.h>

#include "kerneltag.h"

#define BOOTPARAMS_LENGTH 0x0800  //R5: Do the same with u-boot

char g_blparms_buf[BOOTPARAMS_LENGTH];
int g_bootparamslength = 0;
EXPORT_SYMBOL(g_blparms_buf);
EXPORT_SYMBOL(g_bootparamslength);


int __ath_flash_size;

void __init prom_init(void)
{
	int memsz = 0x2000000, argc = fw_arg0, i;
	char **arg = (char **)fw_arg1;

	printk("flash_size passed from bootloader = %d\n", (int)fw_arg3);
	__ath_flash_size = fw_arg3;

	/*
	 * if user passes kernel args, ignore the default one
	 */
	if (argc > 1) {
		arcs_cmdline[0] = '\0';

		for (i = 1; i < argc; i++)
			printk("arg %d: %s\n", i, arg[i]);

		/*
		 * arg[0] is "g", the rest is boot parameters
		 */
		for (i = 1; i < argc; i++) {
			if (strlen(arcs_cmdline) + strlen(arg[i] + 1)
			    >= sizeof(arcs_cmdline))
				break;
			strcat(arcs_cmdline, arg[i]);
			strcat(arcs_cmdline, " ");
		}
	}

	// mips_machgroup = MACH_GROUP_ATHEROS;

	mips_machtype = CONFIG_ATH_MACH_TYPE;

	/*
	 * By default, use all available memory. You can override this
	 * to use, say, 8MB by specifying "mem=8M" as an argument on the
	 * linux bootup command line.
	 */
	add_memory_region(0, memsz, BOOT_MEM_RAM);

#if 0
	printk("\n");
	for (i = 0; i<256; i++)
	{
		printk("0x%x ",*(char *)(0x80000000 + i)); 
		if (0 == ((i+1)%16))
		{
			printk("\n");
		}
	}
#endif
	printk("\n");

    char id[16];
    char upgflag[16];
	
	memcpy(g_blparms_buf, (char *)(0x80000000), BOOTPARAMS_LENGTH);

	memset(id, 0, ATP_BOARD_ID_LEN);
    memcpy(id, g_blparms_buf, ATP_BOARD_ID_LEN);
    printk("id:%s \n", id);

    memset(upgflag, 0,ATP_BOARD_ID_LEN);
    memcpy(upgflag, g_blparms_buf + ATP_BOARD_ID_LEN + ATP_BOARD_ID_LEN, ATP_BOARD_ID_LEN);
    printk("upgflag:%s \n", upgflag);

#if 0
    ATP_UPG_Tag_ST st_tag;

    //cfe传递的参数放入tag结构中
    memcpy(st_tag.flashLayout.stFlashLayoutInfo.astFlashInfo,
           g_blparms_buf + ATP_BOARD_ID_LEN + ATP_BOARD_ID_LEN + ATP_BOARD_ID_LEN,
           ATP_TAG_FLASH_MAX_INDEX*8);

    for(i = 0; i<ATP_TAG_FLASH_MAX_INDEX; i++)
    {
        printk("addr:%d,len:%d \n",
            st_tag.flashLayout.stFlashLayoutInfo.astFlashInfo[i].ulAddress,
            st_tag.flashLayout.stFlashLayoutInfo.astFlashInfo[i].ulLen);
    }

	for (i = 0; i<ATP_TAG_FLASH_MAX_INDEX; i= i + 8)
    {
        printk("0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x\n",
               g_blparms_buf[i+1024 + 0]&0xff,
               g_blparms_buf[i+1024 + 1]&0xff,
               g_blparms_buf[i+1024 + 2]&0xff,
               g_blparms_buf[i+1024 + 3]&0xff,
               g_blparms_buf[i+1024 + 4]&0xff,
               g_blparms_buf[i+1024 + 5]&0xff,
               g_blparms_buf[i+1024 + 6]&0xff,
               g_blparms_buf[i+1024 + 7]&0xff);
    }
#endif
}

void __init prom_free_prom_memory(void)
{
}
