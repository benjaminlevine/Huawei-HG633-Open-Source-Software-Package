/******************************************************************************
  文件名称: headsmp.S
  功能描述: CPU1内核入口
  版本描述: V1.0

  创建日期: D2013_10_25
  创建作者: zhouyu 00204772

  修改记录: 
            生成初稿.
******************************************************************************/
#include <linux/linkage.h>
#include <linux/init.h>

__INIT

/*
 * sd56xx specific entry point for secondary CPUs.  This provides
 * a "holding pen" into which all secondary cores are held until we're
 * ready for them to initialise.
 */
ENTRY(sd56xx_secondary_startup)
	mrc	p15, 0, r0, c0, c0, 5	@read cpu id
	and	r0, r0, #0x3
	adr	r4, 1f
	ldmia	r4, {r5, r6}		@r6 <- &pen_release
	sub	r4, r4, r5				@r4 = 0
	add	r6, r6, r4				@r6 = r6
pen:
	ldr	r7, [r6]				@r7 <- pen_release
	cmp	r7, r0					@compare pen_release to cpu id
	bne	pen						@wait for cpu0 to init secondary cpu(by __cpu_up)

	/*
	 * we've been released from the holding pen: secondary_stack
	 * should now contain the SVC stack for this core
	 */
	b	secondary_startup

1:	
	.long	.
	.long	pen_release			@set by boot_secondary pen_release = ui_cpu
