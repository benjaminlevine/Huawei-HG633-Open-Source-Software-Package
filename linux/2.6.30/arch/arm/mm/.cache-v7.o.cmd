cmd_arch/arm/mm/cache-v7.o := /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/bin/ccache /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/bin/arm-hsan-linux-uclibcgnueabi-gcc -Wp,-MD,arch/arm/mm/.cache-v7.o.d  -nostdinc -isystem /opt/toolchain_hisi_ipsec/arm-hsan-linux-uclibcgnueabi/bin/../lib/gcc/arm-hsan-linux-uclibcgnueabi/4.4.6/include -Iinclude  -I/home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -Iinclude  -I/home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include -include include/linux/autoconf.h -mlittle-endian -Iarch/arm/mach-sd56xx/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -msoft-float       -c -o arch/arm/mm/cache-v7.o arch/arm/mm/cache-v7.S

deps_arch/arm/mm/cache-v7.o := \
  arch/arm/mm/cache-v7.S \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/linkage.h \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/linux/section-names.h \
  include/linux/stringify.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/smp.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/hwcap.h \
  arch/arm/mm/proc-macros.S \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/cpu/dcache/writethrough.h) \
  include/asm/asm-offsets.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/hsan.h) \
    $(wildcard include/config/arm/thumbee.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/vfpv3.h) \
    $(wildcard include/config/iwmmxt.h) \

arch/arm/mm/cache-v7.o: $(deps_arch/arm/mm/cache-v7.o)

$(deps_arch/arm/mm/cache-v7.o):
