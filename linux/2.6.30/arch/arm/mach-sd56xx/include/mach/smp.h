/******************************************************************************
  文件名称: smp.h
  功能描述: SMP头文件
  版本描述: V1.0

  创建日期: D2013_10_25
  创建作者: zhouyu 00204772

  修改记录: 
            生成初稿.
******************************************************************************/
#ifndef __HI_SMP_H__
#define __HI_SMP_H__

#include <asm/hardware/gic.h>

/*
 * We use IRQ1 as the IPI
 */
static inline void smp_cross_call(const struct cpumask *pst_mask)
{
    gic_raise_softirq(pst_mask, 1);
}

#endif /*__HI_SMP_H__*/