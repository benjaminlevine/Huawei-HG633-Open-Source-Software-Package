cmd_drivers/usb/misc/usbtest.o := /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/bin/ccache /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/bin/arm-hsan-linux-uclibcgnueabi-gcc -Wp,-MD,drivers/usb/misc/.usbtest.o.d  -nostdinc -isystem /opt/toolchain_hisi_ipsec/arm-hsan-linux-uclibcgnueabi/bin/../lib/gcc/arm-hsan-linux-uclibcgnueabi/4.4.6/include -Iinclude  -I/home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -Iinclude -I/home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include -include include/linux/autoconf.h -mlittle-endian -Iarch/arm/mach-sd56xx/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -I/home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/staging/usr/include -Os -marm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fno-omit-frame-pointer -fno-optimize-sibling-calls -Wdeclaration-after-statement -Wno-pointer-sign -fwrapv -fno-dwarf2-cfi-asm -DCFG_LINUX_NET_PACKED   -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(usbtest)"  -D"KBUILD_MODNAME=KBUILD_STR(usbtest)"  -c -o drivers/usb/misc/usbtest.o drivers/usb/misc/usbtest.c

deps_drivers/usb/misc/usbtest.o := \
  drivers/usb/misc/usbtest.c \
  include/linux/kernel.h \
    $(wildcard include/config/lbd.h) \
    $(wildcard include/config/mips/brcm.h) \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /opt/toolchain_hisi_ipsec/arm-hsan-linux-uclibcgnueabi/bin/../lib/gcc/arm-hsan-linux-uclibcgnueabi/4.4.6/include/stdarg.h \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
    $(wildcard include/config/brcm/bounce.h) \
  include/linux/compiler-gcc4.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/linkage.h \
  include/linux/stddef.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  include/linux/posix_types.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/posix_types.h \
  include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/bitops.h \
    $(wildcard include/config/smp.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/system.h \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/fa526.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/sa110.h) \
    $(wildcard include/config/cpu/32v6k.h) \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
    $(wildcard include/config/x86.h) \
  include/linux/typecheck.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/irqflags.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/hwcap.h \
  include/asm-generic/cmpxchg-local.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  include/asm-generic/bitops/hweight.h \
  include/asm-generic/bitops/lock.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/ratelimit.h \
  include/linux/param.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/param.h \
    $(wildcard include/config/hz.h) \
  include/linux/dynamic_debug.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/swab.h \
  include/linux/byteorder/generic.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/div64.h \
  include/linux/errno.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/linux/section-names.h \
  include/linux/stringify.h \
  include/linux/slab.h \
    $(wildcard include/config/slab/debug.h) \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/slob.h) \
    $(wildcard include/config/debug/slab.h) \
  include/linux/gfp.h \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/highmem.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/unevictable/lru.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/arch/populates/node/map.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/cgroup/mem/res/ctlr.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/hsan.h) \
    $(wildcard include/config/arm/thumbee.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/vfpv3.h) \
    $(wildcard include/config/iwmmxt.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
    $(wildcard include/config/mmu.h) \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
  include/linux/prefetch.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/processor.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/cache.h \
    $(wildcard include/config/aeabi.h) \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/spinlock_types.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/usb/hsan/usb3/0.h) \
    $(wildcard include/config/generic/hardirqs.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/spinlock.h \
  include/linux/spinlock_api_smp.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/atomic.h \
  include/asm-generic/atomic.h \
  include/linux/wait.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/current.h \
  include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/linux/seqlock.h \
  include/linux/nodemask.h \
  include/linux/bitmap.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/string.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/linux/bounds.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/page.h \
    $(wildcard include/config/cpu/copy/v3.h) \
    $(wildcard include/config/cpu/copy/v4wt.h) \
    $(wildcard include/config/cpu/copy/v4wb.h) \
    $(wildcard include/config/cpu/copy/feroceon.h) \
    $(wildcard include/config/cpu/copy/fa.h) \
    $(wildcard include/config/cpu/xscale.h) \
    $(wildcard include/config/cpu/copy/v6.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/glue.h \
    $(wildcard include/config/cpu/arm610.h) \
    $(wildcard include/config/cpu/arm710.h) \
    $(wildcard include/config/cpu/abrt/lv4t.h) \
    $(wildcard include/config/cpu/abrt/ev4.h) \
    $(wildcard include/config/cpu/abrt/ev4t.h) \
    $(wildcard include/config/cpu/abrt/ev5tj.h) \
    $(wildcard include/config/cpu/abrt/ev5t.h) \
    $(wildcard include/config/cpu/abrt/ev6.h) \
    $(wildcard include/config/cpu/abrt/ev7.h) \
    $(wildcard include/config/cpu/pabrt/ifar.h) \
    $(wildcard include/config/cpu/pabrt/noifar.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/memory.h \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/dram/size.h) \
    $(wildcard include/config/dram/base.h) \
  include/linux/const.h \
  arch/arm/mach-sd56xx/include/mach/memory.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/sizes.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/sparsemem/vmemmap.h) \
  include/asm-generic/page.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/memory/hotremove.h) \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/linux/rwsem-spinlock.h \
  include/linux/srcu.h \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
  include/linux/cpumask.h \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
  include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/smp.h \
    $(wildcard include/config/local/timers.h) \
  arch/arm/mach-sd56xx/include/mach/smp.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/hardware/gic.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/topology.h \
  include/asm-generic/topology.h \
  include/linux/slab_def.h \
    $(wildcard include/config/kmemtrace.h) \
  include/trace/kmemtrace.h \
  include/linux/tracepoint.h \
    $(wildcard include/config/tracepoints.h) \
  include/linux/rcupdate.h \
    $(wildcard include/config/classic/rcu.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/preempt/rcu.h) \
  include/linux/completion.h \
  include/linux/rcutree.h \
    $(wildcard include/config/rcu/fanout.h) \
    $(wildcard include/config/no/hz.h) \
    $(wildcard include/config/rcu/cpu/stall/detector.h) \
  include/linux/kmalloc_sizes.h \
  include/linux/mm.h \
    $(wildcard include/config/sysctl.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/swap.h) \
    $(wildcard include/config/shmem.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/hibernation.h) \
  include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/debug/virtual.h) \
  include/linux/rbtree.h \
  include/linux/prio_tree.h \
  include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/mm/owner.h) \
    $(wildcard include/config/mmu/notifier.h) \
  include/linux/auxvec.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/auxvec.h \
  include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/mmu.h \
    $(wildcard include/config/cpu/has/asid.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/pgtable.h \
  include/asm-generic/4level-fixup.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/proc-fns.h \
    $(wildcard include/config/cpu/32.h) \
    $(wildcard include/config/cpu/arm7tdmi.h) \
    $(wildcard include/config/cpu/arm720t.h) \
    $(wildcard include/config/cpu/arm740t.h) \
    $(wildcard include/config/cpu/arm9tdmi.h) \
    $(wildcard include/config/cpu/arm920t.h) \
    $(wildcard include/config/cpu/arm922t.h) \
    $(wildcard include/config/cpu/arm925t.h) \
    $(wildcard include/config/cpu/arm926t.h) \
    $(wildcard include/config/cpu/arm940t.h) \
    $(wildcard include/config/cpu/arm946e.h) \
    $(wildcard include/config/cpu/arm1020.h) \
    $(wildcard include/config/cpu/arm1020e.h) \
    $(wildcard include/config/cpu/arm1022.h) \
    $(wildcard include/config/cpu/arm1026.h) \
    $(wildcard include/config/cpu/mohawk.h) \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/cpu/v6.h) \
    $(wildcard include/config/cpu/v7.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/cpu-single.h \
  arch/arm/mach-sd56xx/include/mach/vmalloc.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/pgtable-hwdef.h \
  include/asm-generic/pgtable.h \
  include/linux/page-flags.h \
    $(wildcard include/config/pageflags/extended.h) \
    $(wildcard include/config/have/mlocked/page/bit.h) \
    $(wildcard include/config/ia64/uncached/allocator.h) \
    $(wildcard include/config/s390.h) \
  include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
  include/linux/percpu.h \
    $(wildcard include/config/have/dynamic/per/cpu/area.h) \
  include/linux/pfn.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/percpu.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  include/linux/percpu-defs.h \
  include/linux/module.h \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/markers.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/sysfs.h) \
  include/linux/stat.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/stat.h \
  include/linux/time.h \
  include/linux/math64.h \
  include/linux/kmod.h \
  include/linux/elf.h \
  include/linux/elf-em.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/elf.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/user.h \
  include/linux/kobject.h \
  include/linux/sysfs.h \
  include/linux/kref.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ppc64.h) \
  include/linux/marker.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/local.h \
  include/asm-generic/local.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/module.h \
    $(wildcard include/config/arm/unwind.h) \
  include/linux/scatterlist.h \
    $(wildcard include/config/debug/sg.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/scatterlist.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/io.h \
  arch/arm/mach-sd56xx/include/mach/io.h \
  include/linux/usb.h \
    $(wildcard include/config/usb/devicefs.h) \
    $(wildcard include/config/usb/mon.h) \
    $(wildcard include/config/usb/device/class.h) \
    $(wildcard include/config/pm.h) \
    $(wildcard include/config/usb/suspend.h) \
    $(wildcard include/config/usb/device/type/display.h) \
    $(wildcard include/config/atp/usb/adaptor.h) \
  include/linux/mod_devicetable.h \
  include/linux/usb/ch9.h \
    $(wildcard include/config/size.h) \
    $(wildcard include/config/att/one.h) \
    $(wildcard include/config/att/selfpower.h) \
    $(wildcard include/config/att/wakeup.h) \
    $(wildcard include/config/att/battery.h) \
  include/linux/delay.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/delay.h \
  include/linux/interrupt.h \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/preempt/hardirqs.h) \
    $(wildcard include/config/preempt/rt.h) \
    $(wildcard include/config/generic/irq/probe.h) \
    $(wildcard include/config/debug/shirq.h) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  include/linux/hardirq.h \
    $(wildcard include/config/virt/cpu/accounting.h) \
  include/linux/smp_lock.h \
    $(wildcard include/config/lock/kernel.h) \
  include/linux/sched.h \
    $(wildcard include/config/preempt/softirqs.h) \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/bcm/schedaudit.h) \
    $(wildcard include/config/detect/softlockup.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/posix/mqueue.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/user/sched.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/x86/ptrace/bts.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/debug/stack/usage.h) \
    $(wildcard include/config/preempt/bkl.h) \
    $(wildcard include/config/group/sched.h) \
  include/linux/capability.h \
    $(wildcard include/config/security/file/capabilities.h) \
  include/linux/timex.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/timex.h \
  arch/arm/mach-sd56xx/include/mach/timex.h \
  include/linux/jiffies.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/cputime.h \
  include/asm-generic/cputime.h \
  include/linux/sem.h \
  include/linux/ipc.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/ipcbuf.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/sembuf.h \
  include/linux/signal.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/signal.h \
  include/asm-generic/signal.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/sigcontext.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/siginfo.h \
  include/asm-generic/siginfo.h \
  include/linux/path.h \
  include/linux/pid.h \
  include/linux/proportions.h \
  include/linux/percpu_counter.h \
  include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
  include/linux/rtmutex.h \
    $(wildcard include/config/debug/rt/mutexes.h) \
  include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  include/linux/resource.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/resource.h \
  include/asm-generic/resource.h \
  include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects/free.h) \
  include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  include/linux/latencytop.h \
  include/linux/cred.h \
  include/linux/key.h \
  include/linux/sysctl.h \
    $(wildcard include/config/vlan0.h) \
    $(wildcard include/config/atp/hybrid/greaccel.h) \
    $(wildcard include/config/atp/skb/limit.h) \
    $(wildcard include/config/atp/gre/fon.h) \
    $(wildcard include/config/dt/qos.h) \
    $(wildcard include/config/atp/hybrid/reorder.h) \
    $(wildcard include/config/atp/route/balance.h) \
    $(wildcard include/config/atp/conntrack/clean.h) \
    $(wildcard include/config/nf/conntrack/h323.h) \
    $(wildcard include/config/nf/conntrack/rtsp.h) \
    $(wildcard include/config/nf/conntrack/pptp.h) \
    $(wildcard include/config/nf/conntrack/ipsec.h) \
    $(wildcard include/config/nf/conntrack/sip.h) \
    $(wildcard include/config/imq.h) \
    $(wildcard include/config/reserve/ct/session.h) \
    $(wildcard include/config/nf/conntrack/br/autobind.h) \
    $(wildcard include/config/dpi/parse.h) \
    $(wildcard include/config/firewall/log.h) \
    $(wildcard include/config/nf/conntrack/ftp.h) \
    $(wildcard include/config/arch/sd56xx.h) \
    $(wildcard include/config/support/atp/ipv6/enable.h) \
  include/linux/aio.h \
    $(wildcard include/config/aio.h) \
  include/linux/workqueue.h \
  include/linux/aio_abi.h \
  include/linux/uio.h \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/ftrace/nmi/enter.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/hardirq.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/irq.h \
  arch/arm/mach-sd56xx/include/mach/irqs.h \
  include/linux/irq_cpustat.h \
  include/linux/device.h \
    $(wildcard include/config/build/sd5610t.h) \
    $(wildcard include/config/debug/devres.h) \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/pm/runtime.h) \
  include/linux/semaphore.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/device.h \
    $(wildcard include/config/dmabounce.h) \
  include/linux/pm_wakeup.h \
  include/linux/fs.h \
    $(wildcard include/config/dnotify.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/inotify.h) \
    $(wildcard include/config/debug/writecount.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/fs/xip.h) \
    $(wildcard include/config/migration.h) \
  include/linux/limits.h \
  include/linux/ioctl.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist.h \
  include/linux/radix-tree.h \
  include/linux/fiemap.h \
  include/linux/quota.h \
  include/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/nfs_fs_i.h \
  include/linux/nfs.h \
  include/linux/sunrpc/msg_prot.h \
  include/linux/fcntl.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/linux/2.6.30/arch/arm/include/asm/fcntl.h \
  include/asm-generic/fcntl.h \
  include/linux/err.h \

drivers/usb/misc/usbtest.o: $(deps_drivers/usb/misc/usbtest.o)

$(deps_drivers/usb/misc/usbtest.o):
