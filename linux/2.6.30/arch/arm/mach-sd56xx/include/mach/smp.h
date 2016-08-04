/******************************************************************************
  �ļ�����: smp.h
  ��������: SMPͷ�ļ�
  �汾����: V1.0

  ��������: D2013_10_25
  ��������: zhouyu 00204772

  �޸ļ�¼: 
            ���ɳ���.
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