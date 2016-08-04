/*
 * Generic show_mem() implementation
 *
 * Copyright (C) 2008 Johannes Weiner <hannes@saeurebad.de>
 * All code subject to the GPL version 2.
 */

#include <linux/mm.h>
#include <linux/nmi.h>
#include <linux/quicklist.h>

#ifdef CONFIG_HIMEM_DUMPFILE
extern int hw_ssp_set_dump_info(const char *fmt, ...);
#endif

#ifdef CONFIG_CRASH_DUMPFILE
#include "atpcrash.h"
#endif
void show_mem(void)
{
	pg_data_t *pgdat;
	unsigned long total = 0, reserved = 0, shared = 0,
		nonshared = 0, highmem = 0;
#ifdef CONFIG_CRASH_DUMPFILE
	ATP_KRNL_CRASH_AppendInfo("Mem-Info:\n");
#endif	
#ifdef CONFIG_HIMEM_DUMPFILE
        hw_ssp_set_dump_info("Mem-Info:\n");
#endif
	printk(KERN_INFO "Mem-Info:\n");
	show_free_areas();

	for_each_online_pgdat(pgdat) {
		unsigned long i, flags;

		pgdat_resize_lock(pgdat, &flags);
		for (i = 0; i < pgdat->node_spanned_pages; i++) {
			struct page *page;
			unsigned long pfn = pgdat->node_start_pfn + i;

			if (unlikely(!(i % MAX_ORDER_NR_PAGES)))
				touch_nmi_watchdog();

			if (!pfn_valid(pfn))
				continue;

			page = pfn_to_page(pfn);

			if (PageHighMem(page))
				highmem++;

			if (PageReserved(page))
				reserved++;
			else if (page_count(page) == 1)
				nonshared++;
			else if (page_count(page) > 1)
				shared += page_count(page) - 1;

			total++;
		}
		pgdat_resize_unlock(pgdat, &flags);
	}

	printk(KERN_INFO "%lu pages RAM\n", total);
#ifdef CONFIG_CRASH_DUMPFILE	
	ATP_KRNL_CRASH_AppendInfo("%lu pages RAM\n", total);
#endif

#ifdef CONFIG_HIMEM_DUMPFILE
        hw_ssp_set_dump_info("%lu pages RAM\n", total);
#endif

#ifdef CONFIG_HIGHMEM
	printk(KERN_INFO "%lu pages HighMem\n", highmem);
#ifdef CONFIG_CRASH_DUMPFILE
	ATP_KRNL_CRASH_AppendInfo("%lu pages HighMem\n", highmem);
#endif

#ifdef CONFIG_HIMEM_DUMPFILE
        hw_ssp_set_dump_info("%lu pages HighMem\n", highmem);
#endif

#endif
	printk(KERN_INFO "%lu pages reserved\n", reserved);
	printk(KERN_INFO "%lu pages shared\n", shared);
	printk(KERN_INFO "%lu pages non-shared\n", nonshared);
#ifdef CONFIG_CRASH_DUMPFILE	
	ATP_KRNL_CRASH_AppendInfo("%lu pages reserved\n", reserved);
	ATP_KRNL_CRASH_AppendInfo("%lu pages shared\n", shared);
	ATP_KRNL_CRASH_AppendInfo("%lu pages non-shared\n", nonshared);				
#endif	

#ifdef CONFIG_HIMEM_DUMPFILE
        hw_ssp_set_dump_info(KERN_INFO "%lu pages reserved\n", reserved);
        hw_ssp_set_dump_info(KERN_INFO "%lu pages shared\n", shared);
        hw_ssp_set_dump_info(KERN_INFO "%lu pages non-shared\n", nonshared);        
#endif

#ifdef CONFIG_QUICKLIST
	printk(KERN_INFO "%lu pages in pagetable cache\n",
		quicklist_total_size());
#ifdef CONFIG_CRASH_DUMPFILE
	ATP_KRNL_CRASH_AppendInfo("%lu pages in pagetable cache\n",
		quicklist_total_size());				
#endif

#ifdef CONFIG_HIMEM_DUMPFILE
        hw_ssp_set_dump_info(KERN_INFO "%lu pages in pagetable cache\n",
		quicklist_total_size());
#endif

#endif

}
