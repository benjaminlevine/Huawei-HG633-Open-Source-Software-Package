/*
 * This file contains glue for Atheros ath spi flash interface
 * Primitives are ath_spi_*
 * mtd flash implements are ath_flash_*
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/mtd/map.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <asm/delay.h>
#include <asm/io.h>
#include <asm/div64.h>

#include <linux/sched.h>

#include <atheros.h>
#include "ath_flash.h"

#include <linux/root_dev.h>
#include "atpnandparts.h"
#include "kerneltag.h"

/* this is passed in as a boot parameter by bootloader */
extern int __ath_flash_size;

#define IMAGE_MAIN_F             "MAINFF"
#define IMAGE_SLAVE_F            "SLAVEFF"
#define IMAGE_MAIN_S             "MAINSS"
#define IMAGE_SLAVE_S            "SLAVESS"
#define BOOTPARAMS_LENGTH 0x0800  //R5: Do the same with u-boot

extern char g_blparms_buf[BOOTPARAMS_LENGTH];

#define WINDOW_ADDR 0x9f000000
#define WINDOW_SIZE 0x400000
#define BUSWIDTH 	2
static struct map_info ar9342_map = {
       .name = "ar9342",
       .size = WINDOW_SIZE,
       .bankwidth = BUSWIDTH,
       .phys = WINDOW_ADDR,
};

/*
 * statics
 */
static void ath_spi_write_enable(void);
static void ath_spi_poll(void);
#if !defined(ATH_SST_FLASH)
static void ath_spi_write_page(uint32_t addr, uint8_t * data, int len);
#endif
static void ath_spi_sector_erase(uint32_t addr);

static const char *part_probes[] __initdata = { "cmdlinepart", "RedBoot", NULL };

static DECLARE_MUTEX(ath_flash_sem);

/* GLOBAL FUNCTIONS */
void
ath_flash_spi_down(void)
{
	down(&ath_flash_sem);
}

void
ath_flash_spi_up(void)
{
	up(&ath_flash_sem);
}

EXPORT_SYMBOL(ath_flash_spi_down);
EXPORT_SYMBOL(ath_flash_spi_up);

#define ATH_FLASH_SIZE_2MB          (2*1024*1024)
#define ATH_FLASH_SIZE_4MB          (4*1024*1024)
#define ATH_FLASH_SIZE_8MB          (8*1024*1024)
#define ATH_FLASH_SECTOR_SIZE_64KB  (64*1024)
#define ATH_FLASH_SECTOR_SIZE_4KB   (4*1024)
#define ATH_FLASH_PG_SIZE_256B       256
#define ATH_FLASH_NAME               "ath-nor0"
/*
 * bank geometry
 */
typedef struct ath_flash_geom {
	uint32_t size;
	uint32_t sector_size;
	uint32_t nsectors;
	uint32_t pgsize;
} ath_flash_geom_t;

ath_flash_geom_t flash_geom_tbl[ATH_FLASH_MAX_BANKS] = {
	{
		.size		= ATH_FLASH_SIZE_4MB,
		.sector_size	= ATH_FLASH_SECTOR_SIZE_4KB,
		.pgsize		= ATH_FLASH_PG_SIZE_256B
	}
};

static int
ath_flash_probe(void)
{
	return 0;
}

#if defined(ATH_SST_FLASH)
void
ath_spi_flash_unblock(void)
{
	ath_spi_write_enable();
	ath_spi_bit_banger(ATH_SPI_CMD_WRITE_SR);
	ath_spi_bit_banger(0x0);
	ath_spi_go();
	ath_spi_poll();
}
#endif

static int
ath_flash_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	int nsect, s_curr, s_last;
	uint64_t  res;

	if (instr->addr + instr->len > mtd->size)
		return (-EINVAL);

	ath_flash_spi_down();

	res = instr->len;
	do_div(res, mtd->erasesize);
	nsect = res;

	if (((uint32_t)instr->len) % mtd->erasesize)
		nsect ++;

	res = instr->addr;
	do_div(res,mtd->erasesize);
	s_curr = res;

	s_last  = s_curr + nsect;

	do {
		ath_spi_sector_erase(s_curr * ATH_SPI_SECTOR_SIZE);
		/* Need to leave some CPU time to softirq for running LED */
		printk(".");
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout((10 * HZ) /1000);
	} while (++s_curr < s_last);

	ath_spi_done();

	ath_flash_spi_up();

	if (instr->callback) {
		instr->state |= MTD_ERASE_DONE;
		instr->callback(instr);
	}

	return 0;
}

static int
ath_flash_read(struct mtd_info *mtd, loff_t from, size_t len,
		  size_t *retlen, u_char *buf)
{
	uint32_t addr = from | 0xbf000000;

	if (!len)
		return (0);
	if (from + len > mtd->size)
		return (-EINVAL);

	ath_flash_spi_down();

	memcpy(buf, (uint8_t *)(addr), len);
	*retlen = len;

	ath_flash_spi_up();

	return 0;
}

#if defined(ATH_SST_FLASH)
static int
ath_flash_write(struct mtd_info *mtd, loff_t dst, size_t len,
		   size_t * retlen, const u_char * src)
{
	uint32_t val;

	//printk("write len: %lu dst: 0x%x src: %p\n", len, dst, src);

	*retlen = len;

	for (; len; len--, dst++, src++) {
		ath_spi_write_enable();	// dont move this above 'for'
		ath_spi_bit_banger(ATH_SPI_CMD_PAGE_PROG);
		ath_spi_send_addr(dst);

		val = *src & 0xff;
		ath_spi_bit_banger(val);

		ath_spi_go();
		ath_spi_poll();
	}
	/*
	 * Disable the Function Select
	 * Without this we can't re-read the written data
	 */
	ath_reg_wr(ATH_SPI_FS, 0);

	if (len) {
		*retlen -= len;
		return -EIO;
	}
	return 0;
}
#else
static int
ath_flash_write(struct mtd_info *mtd, loff_t to, size_t len,
		   size_t *retlen, const u_char *buf)
{
	int total = 0, len_this_lp, bytes_this_page;
	uint32_t addr = 0;
	u_char *mem;

	ath_flash_spi_down();

	while (total < len) {
		mem = (u_char *) (buf + total);
		addr = to + total;
		bytes_this_page =
		    ATH_SPI_PAGE_SIZE - (addr % ATH_SPI_PAGE_SIZE);
		len_this_lp = min(((int)len - total), bytes_this_page);

		ath_spi_write_page(addr, mem, len_this_lp);
		total += len_this_lp;
	}

	ath_spi_done();

	ath_flash_spi_up();

	*retlen = len;
	return 0;
}
#endif

static struct mtd_info *get_mtd_named(char *name)
{
	int i;
	struct mtd_info *mtd;

	for (i = 0; i < MAX_MTD_DEVICES; i++) {
		mtd = get_mtd_device(NULL, i);
		if (mtd) {
			if (strcmp(mtd->name, name) == 0)
				return(mtd);
			put_mtd_device(mtd);
		}
	}

	return(NULL);
}

/*
 * sets up flash_info and returns size of FLASH (bytes)
 */
static int __init ath_flash_init(void)
{
	int i, np;
	ath_flash_geom_t *geom;
	struct mtd_info *mtd;
    struct mtd_info *mtd_tmp;
	struct mtd_partition *mtd_parts;
	uint8_t index;
    char upgflag[16];
    unsigned int ulMainOrSlave;

	init_MUTEX(&ath_flash_sem);

#if defined(ATH_SST_FLASH)
	ath_reg_wr_nf(ATH_SPI_CLOCK, 0x3);
	ath_spi_flash_unblock();
	ath_reg_wr(ATH_SPI_FS, 0);
#else
#ifndef CONFIG_MACH_AR934x
	ath_reg_wr_nf(ATH_SPI_CLOCK, 0x43);
#endif
#endif
	for (i = 0; i < ATH_FLASH_MAX_BANKS; i++) {

		index = ath_flash_probe();
		geom = &flash_geom_tbl[index];

		/* set flash size to value from bootloader if it passed valid value */
		/* otherwise use the default 4MB.                                   */
		if (__ath_flash_size >= 4 && __ath_flash_size <= 16)
			geom->size = __ath_flash_size * 1024 * 1024;

		mtd = kmalloc(sizeof(struct mtd_info), GFP_KERNEL);
		if (!mtd) {
			printk("Cant allocate mtd stuff\n");
			return -1;
		}
		memset(mtd, 0, sizeof(struct mtd_info));

		mtd->name		= ATH_FLASH_NAME;
		mtd->type		= MTD_NORFLASH;
		mtd->flags		= MTD_CAP_NORFLASH | MTD_WRITEABLE;
		mtd->size		= geom->size;
		mtd->erasesize		= geom->sector_size;
		mtd->numeraseregions	= 0;
		mtd->eraseregions	= NULL;
		mtd->owner		= THIS_MODULE;
		mtd->erase		= ath_flash_erase;
		mtd->read		= ath_flash_read;
		mtd->write		= ath_flash_write;
		mtd->writesize		= ATH_FLASH_PG_SIZE_256B;
#if 0
		np = parse_mtd_partitions(mtd, part_probes, &mtd_parts, 0);
#else
        memset(upgflag, 0, ATP_BOARD_ID_LEN);
        memcpy(upgflag, g_blparms_buf + ATP_BOARD_ID_LEN + ATP_BOARD_ID_LEN,ATP_BOARD_ID_LEN);

        /* 这里的len是为了不挂载Tag，它不是jffs2格式的 */
        if((0 == memcmp(upgflag, IMAGE_SLAVE_S, strlen(IMAGE_SLAVE_S)))
           || (0 == memcmp(upgflag, IMAGE_MAIN_F, strlen(IMAGE_MAIN_F))))
        {
            ulMainOrSlave = 1;
        }
        else
        {
            ulMainOrSlave = 0;
        }

        mtd->ecclayout = NULL;
        atp_setup_mtd_partitions(mtd, g_blparms_buf + ATP_BOARD_ID_LEN + ATP_BOARD_ID_LEN + ATP_BOARD_ID_LEN,
                                 ulMainOrSlave, &mtd_parts, &np);
#if 0
        printk("ar9342: flash device 0x%08x at 0x%08x\n", WINDOW_SIZE, WINDOW_ADDR);
		ar9342_map.virt = ioremap_nocache(WINDOW_ADDR, WINDOW_SIZE);
        if (!ar9342_map.virt) {
   		    printk("ar9342: Failed to ioremap\n");
		    return -EIO;
	    }

	    simple_map_init(&ar9342_map);
#endif
#endif
#if 0
        /* check if boot from SPI flash */
        mtd = do_map_probe("spiflash_probe", NULL);

        if (!mtd || np == 0) {
		    iounmap(ar9342_map.virt);
		    return -ENXIO;
	    }

        mtd->owner = THIS_MODULE;
#endif
        printk("ATP_MTD_SYSTEM\n");
        add_mtd_partitions(mtd, mtd_parts, np);

        mtd_tmp = get_mtd_named("rootfs");
	    printk("mtd->index :%d\n",mtd_tmp->index);

	    if (mtd_tmp) 
	    {
		    ROOT_DEV = MKDEV(MTD_BLOCK_MAJOR, mtd_tmp->index);
		    put_mtd_device(mtd_tmp);
	    }
	    else
	    {
		    printk("MKDEV NOK\n");
	    }
	}

	return 0;
}

static void __exit ath_flash_exit(void)
{
	/*
	 * nothing to do
	 */
}

/*
 * Primitives to implement flash operations
 */
static void
ath_spi_write_enable()
{
	ath_reg_wr_nf(ATH_SPI_FS, 1);
	ath_reg_wr_nf(ATH_SPI_WRITE, ATH_SPI_CS_DIS);
	ath_spi_bit_banger(ATH_SPI_CMD_WREN);
	ath_spi_go();
}

static void
ath_spi_poll()
{
	int rd;

	do {
		ath_reg_wr_nf(ATH_SPI_WRITE, ATH_SPI_CS_DIS);
		ath_spi_bit_banger(ATH_SPI_CMD_RD_STATUS);
		ath_spi_delay_8();
		rd = (ath_reg_rd(ATH_SPI_RD_STATUS) & 1);
	} while (rd);
}

static void
ath_spi_write_page(uint32_t addr, uint8_t *data, int len)
{
	int i;
	uint8_t ch;

	ath_spi_write_enable();
	ath_spi_bit_banger(ATH_SPI_CMD_PAGE_PROG);
	ath_spi_send_addr(addr);

	for (i = 0; i < len; i++) {
		ch = *(data + i);
		ath_spi_bit_banger(ch);
	}

	ath_spi_go();
	ath_spi_poll();
}

static void
ath_spi_sector_erase(uint32_t addr)
{
	ath_spi_write_enable();
	ath_spi_bit_banger(ATH_SPI_CMD_SECTOR_ERASE);
	ath_spi_send_addr(addr);
	ath_spi_go();
#if 0
	/*
	 * Do not touch the GPIO's unnecessarily. Might conflict
	 * with customer's settings.
	 */
	display(0x7d);
#endif
	ath_spi_poll();
}

module_init(ath_flash_init);
module_exit(ath_flash_exit);
