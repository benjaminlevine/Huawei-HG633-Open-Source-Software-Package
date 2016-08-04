/******************************************************************************
  �ļ�����: hotplug.c
  ��������: 
  �汾����: V1.0

  ��������: D2013_10_25
  ��������: zhouyu 00204772

  �޸ļ�¼: 
            ���ɳ���.
******************************************************************************/
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/smp.h>
#include <linux/completion.h>

#include <asm/cacheflush.h>

#include <mach/typedef.h>

extern volatile hi_int32 pen_release;

static DECLARE_COMPLETION(cpu_killed);

/******************************************************************************
  ��������:  set cpu enter lowpower
  �������:  ��
  �������:  ��
  ��������:  
  ������ע:  
******************************************************************************/
static inline hi_void cpu_enter_lowpower(hi_void)
{
	hi_uint32 v;

	flush_cache_all();
	
	asm volatile(
	"	mcr	p15, 0, %1, c7, c5, 0\n"
	"	mcr	p15, 0, %1, c7, c10, 4\n"
	
	/* Turn off coherency */
	"	mrc	p15, 0, %0, c1, c0, 1\n"
	"	bic	%0, %0, #0x20\n"
	"	mcr	p15, 0, %0, c1, c0, 1\n"
	"	mrc	p15, 0, %0, c1, c0, 0\n"
	"	bic	%0, %0, #0x04\n"
	"	mcr	p15, 0, %0, c1, c0, 0\n"
	  : "=&r" (v)
	  : "r" (0)
	  : "cc");
}

/******************************************************************************
  ��������:  set cpu leave lowpower
  �������:  ��
  �������:  ��
  ��������:  
  ������ע:  
******************************************************************************/
static inline hi_void cpu_leave_lowpower(hi_void)
{
	hi_uint32 v;

	asm volatile(
	"mrc	p15, 0, %0, c1, c0, 0\n"
	"	orr	%0, %0, #0x04\n"
	"	mcr	p15, 0, %0, c1, c0, 0\n"
	"	mrc	p15, 0, %0, c1, c0, 1\n"
	"	orr	%0, %0, #0x20\n"
	"	mcr	p15, 0, %0, c1, c0, 1\n"
	  : "=&r" (v)
	  :
	  : "cc");
}

/******************************************************************************
  ��������:  platform do lowpower
  �������:  ui_cpu: cpu num
  �������:  ��
  ��������:  
  ������ע:  
******************************************************************************/
static inline hi_void platform_do_lowpower(hi_uint32 ui_cpu)
{
	/*
	 * there is no power-control hardware on this platform, so all
	 * we can do is put the core into WFI; this is safe as the calling
	 * code will have already disabled interrupts
	 */
	for (;;)
	{
		/* here's the WFI */
		asm(".word	0xe320f003\n"
		    :
		    :
		    : "memory", "cc");

		if (pen_release == ui_cpu)
		{
			/* OK, proper wakeup, we're done */
			break;
		}

		/*
		 * getting here, means that we have come out of WFI without
		 * having been woken up - this shouldn't happen
		 *
		 * The trouble is, letting people know about this is not really
		 * possible, since we are currently running incoherently, and
		 * therefore cannot safely call printk() or anything else
		 */
#ifdef DEBUG
		printk("CPU%u: spurious wakeup call\n", ui_cpu);
#endif
	}
}

/******************************************************************************
  ��������:  platform kill cpu
  �������:  ui_cpu: cpu num
  �������:  ��
  ��������:  
  ������ע:  
******************************************************************************/
hi_int32 platform_cpu_kill(hi_uint32 ui_cpu)
{
	return wait_for_completion_timeout(&cpu_killed, 5000);
}

/******************************************************************************
  ��������:  platform-specific code to shutdown a CPU
  �������:  ui_cpu: cpu num
  �������:  ��
  ��������:  
  ������ע:  Called with IRQs disabled
******************************************************************************/
hi_void platform_cpu_die(hi_uint32 ui_cpu)
{
#ifdef DEBUG
	hi_uint32 ui_this_cpu = hard_smp_processor_id();

	if (ui_cpu != ui_this_cpu)
	{
		printk(KERN_CRIT "Eek! platform_cpu_die running on %u, should be %u\n", ui_this_cpu, ui_cpu);
		BUG();
	}
#endif

	printk(KERN_NOTICE "CPU%u: shutdown\n", ui_cpu);
	complete(&cpu_killed);

	/* we're ready for shutdown now, so do it */
	cpu_enter_lowpower();
	platform_do_lowpower(ui_cpu);

	/* bring this CPU back into the world of cache coherency, and then restore interrupts */
	cpu_leave_lowpower();
}

/******************************************************************************
  ��������:  shut down cpu
  �������:  ui_cpu: cpu num
  �������:  ��
  ��������:  
  ������ע:  
******************************************************************************/
hi_int32 mach_cpu_disable(hi_uint32 ui_cpu)
{
	/* we don't allow CPU 0 to be shutdown (it is still too special  e.g. clock tick interrupts) */
	return ui_cpu == 0 ? -EPERM : 0;
}

