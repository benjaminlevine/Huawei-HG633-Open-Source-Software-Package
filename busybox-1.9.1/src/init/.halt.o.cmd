cmd_init/halt.o := /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/bin/ccache /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/bin/arm-hsan-linux-uclibcgnueabi-gcc -Wp,-MD,init/.halt.o.d   -std=gnu99 -Iinclude -Ilibbb  -I/home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/build/busybox-1.9.1/src/libbb -include include/autoconf.h -D_GNU_SOURCE -DNDEBUG -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D"BB_VER=\"v1.9.1\"" -DBB_BT=AUTOCONF_TIMESTAMP -D_FORTIFY_SOURCE=2  -Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Os -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-guess-branch-probability -funsigned-char -static-libgcc -falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1  -Wdeclaration-after-statement -Wno-pointer-sign -I/home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include -I/openssl -fno-strict-aliasing    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(halt)"  -D"KBUILD_MODNAME=KBUILD_STR(halt)" -c -o init/halt.o init/halt.c

deps_init/halt.o := \
  init/halt.c \
    $(wildcard include/config/init.h) \
    $(wildcard include/config/feature/initrd.h) \
    $(wildcard include/config/feature/clean/up.h) \
  include/libbb.h \
    $(wildcard include/config/selinux.h) \
    $(wildcard include/config/locale/support.h) \
    $(wildcard include/config/feature/shadowpasswds.h) \
    $(wildcard include/config/lfs.h) \
    $(wildcard include/config/feature/buffers/go/on/stack.h) \
    $(wildcard include/config/buffer.h) \
    $(wildcard include/config/ubuffer.h) \
    $(wildcard include/config/feature/buffers/go/in/bss.h) \
    $(wildcard include/config/feature/ipv6.h) \
    $(wildcard include/config/ture/ipv6.h) \
    $(wildcard include/config/feature/prefer/applets.h) \
    $(wildcard include/config/busybox/exec/path.h) \
    $(wildcard include/config/getopt/long.h) \
    $(wildcard include/config/feature/pidfile.h) \
    $(wildcard include/config/feature/syslog.h) \
    $(wildcard include/config/feature/individual.h) \
    $(wildcard include/config/route.h) \
    $(wildcard include/config/gunzip.h) \
    $(wildcard include/config/bunzip2.h) \
    $(wildcard include/config/ktop.h) \
    $(wildcard include/config/ioctl/hex2str/error.h) \
    $(wildcard include/config/feature/editing.h) \
    $(wildcard include/config/feature/editing/history.h) \
    $(wildcard include/config/ture/editing/savehistory.h) \
    $(wildcard include/config/feature/editing/savehistory.h) \
    $(wildcard include/config/feature/tab/completion.h) \
    $(wildcard include/config/feature/username/completion.h) \
    $(wildcard include/config/feature/editing/vi.h) \
    $(wildcard include/config/inux.h) \
    $(wildcard include/config/feature/topmem.h) \
    $(wildcard include/config/pgrep.h) \
    $(wildcard include/config/pkill.h) \
    $(wildcard include/config/feature/devfs.h) \
  include/platform.h \
    $(wildcard include/config/werror.h) \
    $(wildcard include/config///.h) \
    $(wildcard include/config//nommu.h) \
    $(wildcard include/config//mmu.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/byteswap.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/byteswap.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/byteswap-common.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/endian.h \
    $(wildcard include/config/.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/features.h \
    $(wildcard include/config/c99.h) \
    $(wildcard include/config/c95.h) \
    $(wildcard include/config/ix.h) \
    $(wildcard include/config/ix2.h) \
    $(wildcard include/config/ix199309.h) \
    $(wildcard include/config/ix199506.h) \
    $(wildcard include/config/en.h) \
    $(wildcard include/config/en/extended.h) \
    $(wildcard include/config/x98.h) \
    $(wildcard include/config/en2k.h) \
    $(wildcard include/config/en2k8.h) \
    $(wildcard include/config/gefile.h) \
    $(wildcard include/config/gefile64.h) \
    $(wildcard include/config/e/offset64.h) \
    $(wildcard include/config/d.h) \
    $(wildcard include/config/c.h) \
    $(wildcard include/config/ile.h) \
    $(wildcard include/config/ntrant.h) \
    $(wildcard include/config/tify/level.h) \
    $(wildcard include/config/i.h) \
    $(wildcard include/config/ern/inlines.h) \
    $(wildcard include/config/ix/implicitly.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/uClibc_config.h \
    $(wildcard include/config/arm/oabi//.h) \
    $(wildcard include/config/arm/eabi//.h) \
    $(wildcard include/config/generic/arm//.h) \
    $(wildcard include/config/arm610//.h) \
    $(wildcard include/config/arm710//.h) \
    $(wildcard include/config/arm7tdmi//.h) \
    $(wildcard include/config/arm720t//.h) \
    $(wildcard include/config/arm920t//.h) \
    $(wildcard include/config/arm922t//.h) \
    $(wildcard include/config/arm926t//.h) \
    $(wildcard include/config/arm10t//.h) \
    $(wildcard include/config/arm1136jf/s//.h) \
    $(wildcard include/config/arm1176jz/s//.h) \
    $(wildcard include/config/arm1176jzf/s//.h) \
    $(wildcard include/config/arm/cortex/m3//.h) \
    $(wildcard include/config/arm/cortex/m1//.h) \
    $(wildcard include/config/arm/sa110//.h) \
    $(wildcard include/config/arm/sa1100//.h) \
    $(wildcard include/config/arm/xscale//.h) \
    $(wildcard include/config/arm/iwmmxt//.h) \
    $(wildcard include/config//.h) \
    $(wildcard include/config/link//.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/cdefs.h \
    $(wildcard include/config/espaces.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/endian.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/arpa/inet.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/netinet/in.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/stdint.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/wchar.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/wordsize.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/socket.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/uio.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/types.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/types.h \
  /opt/toolchain_hisi_ipsec/arm-hsan-linux-uclibcgnueabi/bin/../lib/gcc/arm-hsan-linux-uclibcgnueabi/4.4.6/include/stddef.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/typesizes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/pthreadtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/time.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/select.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/select.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/sigset.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/time.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/sysmacros.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/uio.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/socket.h \
  /opt/toolchain_hisi_ipsec/arm-hsan-linux-uclibcgnueabi/bin/../lib/gcc/arm-hsan-linux-uclibcgnueabi/4.4.6/include-fixed/limits.h \
  /opt/toolchain_hisi_ipsec/arm-hsan-linux-uclibcgnueabi/bin/../lib/gcc/arm-hsan-linux-uclibcgnueabi/4.4.6/include-fixed/syslimits.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/limits.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/posix1_lim.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/local_lim.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/linux/limits.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/uClibc_local_lim.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/posix2_lim.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/xopen_lim.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/stdio_lim.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/sockaddr.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm/socket.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm/sockios.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/in.h \
  /opt/toolchain_hisi_ipsec/arm-hsan-linux-uclibcgnueabi/bin/../lib/gcc/arm-hsan-linux-uclibcgnueabi/4.4.6/include/stdbool.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/mount.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/ioctl.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/ioctls.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm/ioctls.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm/ioctl.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm-generic/ioctl.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/ioctl-types.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/ttydefaults.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/ctype.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/uClibc_touplow.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/dirent.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/dirent.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/errno.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/errno.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/linux/errno.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm/errno.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm-generic/errno.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm-generic/errno-base.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/fcntl.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/fcntl.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/stat.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/stat.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/inttypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/mntent.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/stdio.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/paths.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/netdb.h \
    $(wildcard include/config/3/ascii/rules.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/rpc/netdb.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/siginfo.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/netdb.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/setjmp.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/setjmp.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/signal.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/signum.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/sigaction.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/sigcontext.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm/sigcontext.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/sigstack.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/ucontext.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/procfs.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/time.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/user.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/sigthread.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/uClibc_stdio.h \
    $(wildcard include/config/io/futexes//.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/wchar.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/uClibc_mutex.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/pthread.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sched.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/sched.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/uClibc_clk_tck.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/uClibc_pthread.h \
  /opt/toolchain_hisi_ipsec/arm-hsan-linux-uclibcgnueabi/bin/../lib/gcc/arm-hsan-linux-uclibcgnueabi/4.4.6/include/stdarg.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/stdlib.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/waitflags.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/waitstatus.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/alloca.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/string.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/poll.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/poll.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/mman.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/mman.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/mman-common.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/statfs.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/statfs.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/wait.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/resource.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/resource.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/termios.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/termios.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/unistd.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/posix_opt.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/environments.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/confname.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bits/getopt.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/utime.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/param.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/linux/param.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm/param.h \
  include/pwd_.h \
    $(wildcard include/config/use/bb/pwd/grp.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/pwd.h \
  include/grp_.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/grp.h \
  include/shadow_.h \
    $(wildcard include/config/use/bb/shadow.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/shadow.h \
  include/xatonum.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sys/reboot.h \
    $(wildcard include/config/cad.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atpconfig.h \
    $(wildcard include/config/vss/ssid.h) \
    $(wildcard include/config/ti/cfg/with/bootloader/name.h) \
    $(wildcard include/config/antenna.h) \
    $(wildcard include/config/reserved/size.h) \
    $(wildcard include/config/wifisystem/bootup.h) \
    $(wildcard include/config/backup.h) \
    $(wildcard include/config/iamge.h) \
    $(wildcard include/config/change.h) \
    $(wildcard include/config/size.h) \
    $(wildcard include/config/list.h) \
    $(wildcard include/config/wireless/enable.h) \
    $(wildcard include/config/vss/psk.h) \
    $(wildcard include/config/option125.h) \
    $(wildcard include/config/ti/cfg/without/bootloader/name.h) \
    $(wildcard include/config/file/length/d.h) \
    $(wildcard include/config/vss/wep.h) \
    $(wildcard include/config/config.h) \
    $(wildcard include/config/vss/appin.h) \
    $(wildcard include/config/countrycode.h) \
    $(wildcard include/config/wifibootmark.h) \
    $(wildcard include/config/ar.h) \
    $(wildcard include/config/wireless/on.h) \
    $(wildcard include/config/throughput.h) \
    $(wildcard include/config/size/r3.h) \
    $(wildcard include/config/reserved/size/r3.h) \
    $(wildcard include/config/libz.h) \
    $(wildcard include/config/partition.h) \
    $(wildcard include/config/ti/cfg.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/wlancmsapi.h \
    $(wildcard include/config/str.h) \
    $(wildcard include/config/st.h) \
    $(wildcard include/config/change/notifyregisterex.h) \
    $(wildcard include/config/change/notifyregister.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msgapi.h \
    $(wildcard include/config/stats.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atptypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/malloc.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atpconfig.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atpcbbid.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atpconst.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atpdeprecated.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/shmemapi.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/sysutil.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msgtypeswrapper.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/cfmcoremsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/msgcoremsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/gplmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/dmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/oammsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/udpechomsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/cupsprintermsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/crashmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/cwmpappmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/cwmpmsgtypes.h \
    $(wildcard include/config/status.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/logmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/miccoremsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/micmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/telnetdmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/tr111msgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/upgmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/upnpmsgtypes.h \
    $(wildcard include/config/upnp.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/ddnscmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/dhcp6cmsgtypes.h \
    $(wildcard include/config/file/path.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/dhcp6smsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/dhcpcmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/dhcpsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/dnsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/ipcheckmsgtypes.h \
    $(wildcard include/config/obj.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/pppcmsgtypes.h \
    $(wildcard include/config/failed.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/routemsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/sntpmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/kcmsmonitormsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/ledservicecmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/usbmntcmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/crashcmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/cwmpcmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/sysinfocmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/webcmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/dhcpscmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/lanethcmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/igmpcmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/ipcheckcmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/qosmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/radvdcmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/wancmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/wlancmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/xtmcmsmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/cmsmainmsgtypes.h \
    $(wildcard include/config/set.h) \
    $(wildcard include/config/get.h) \
    $(wildcard include/config/add.h) \
    $(wildcard include/config/del.h) \
    $(wildcard include/config/get/inst.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/webmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/../weberrdef.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/climsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msg/bhalmsgtypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atptraceapi.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atpdebugapi.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/strfileapi.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/cfmapi.h \
    $(wildcard include/config/last/current.h) \
    $(wildcard include/config/reserve/part/paras.h) \
    $(wildcard include/config/service.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/cfmadaptapi.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/httpapi.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/xmlapi.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/openssl/ssl.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/cyassl/ssl.h \
    $(wildcard include/config/her/change/bug.h) \
    $(wildcard include/config/t/type/bug.h) \
    $(wildcard include/config/partial/write.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/cyassl/ctaocrypt/settings.h \
    $(wildcard include/config/t/math.h) \
    $(wildcard include/config/t/buffers/1024.h) \
    $(wildcard include/config/dows/api.h) \
    $(wildcard include/config/ssl/memory.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/cyassl/ctaocrypt/visibility.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/cyassl/version.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/openssl/md5.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atplog.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atplogdef.h \
    $(wildcard include/config/packet/mirror.h) \
    $(wildcard include/config/packet/capture.h) \
    $(wildcard include/config/ip/conn/del.h) \
    $(wildcard include/config/ip/conn/add.h) \
    $(wildcard include/config/ip/conn/set.h) \
    $(wildcard include/config/ppp/conn/del.h) \
    $(wildcard include/config/ppp/conn/add.h) \
    $(wildcard include/config/ppp/conn/set.h) \
    $(wildcard include/config/server.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atptracedef.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atpflashapi.h \
    $(wildcard include/config/build/r3pkg.h) \
    $(wildcard include/config/reserved.h) \
    $(wildcard include/config/reserved/r3.h) \
    $(wildcard include/config/r3.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/mtd/mtd-user.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/mtd/mtd-abi.h \
    $(wildcard include/config/hsan.h) \
    $(wildcard include/config/brcmnand/mtd/extension.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/linux/types.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm/types.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm-generic/int-ll64.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/linux/posix_types.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/linux/stddef.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm/posix_types.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/linux/autoconf.h \
    $(wildcard include/config/xz/dec/x86.h) \
    $(wildcard include/config/frame/warn.h) \
    $(wildcard include/config/compat/brk.h) \
    $(wildcard include/config/mtd/hsan/sfc/reg/base/address/len.h) \
    $(wildcard include/config/mtd/hsan/sfc/cs0/buffer/base/address/len.h) \
    $(wildcard include/config/ip6/nf/target/reject.h) \
    $(wildcard include/config/mtd/hsan/sfc/cs1/buffer/base/address/len.h) \
    $(wildcard include/config/netfilter/netlink/queue.h) \
    $(wildcard include/config/nf/nat/proto/gre.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/usb/hsan/ohci/iobase.h) \
    $(wildcard include/config/vmsplit/3g.h) \
    $(wildcard include/config/usb/serial.h) \
    $(wildcard include/config/devkmem.h) \
    $(wildcard include/config/have/function/tracer.h) \
    $(wildcard include/config/ipv6/multiple/tables.h) \
    $(wildcard include/config/use/generic/smp/helpers.h) \
    $(wildcard include/config/xz/dec/bcj.h) \
    $(wildcard include/config/ip6/nf/raw.h) \
    $(wildcard include/config/support/atp/ipv6/enable.h) \
    $(wildcard include/config/timerfd.h) \
    $(wildcard include/config/mtd/map/bank/width/1.h) \
    $(wildcard include/config/eventfd.h) \
    $(wildcard include/config/ext2/fs/xattr.h) \
    $(wildcard include/config/blk/dev/sd.h) \
    $(wildcard include/config/has/dma.h) \
    $(wildcard include/config/mtd/map/bank/width/2.h) \
    $(wildcard include/config/netfilter/xt/match/dscp.h) \
    $(wildcard include/config/netfilter/xt/match/dns/flt.h) \
    $(wildcard include/config/ext3/fs/xattr.h) \
    $(wildcard include/config/netfilter/xt/match/mark.h) \
    $(wildcard include/config/mtd/map/bank/width/4.h) \
    $(wildcard include/config/usb/hsan/xhci/iobase.h) \
    $(wildcard include/config/netfilter/xt/match/time.h) \
    $(wildcard include/config/arm/gic.h) \
    $(wildcard include/config/build/component/wlan.h) \
    $(wildcard include/config/nf/nat/irc.h) \
    $(wildcard include/config/ppp/filter.h) \
    $(wildcard include/config/bridge/ebt/ip6.h) \
    $(wildcard include/config/nf/nat/h323.h) \
    $(wildcard include/config/cpu/tlb/v7.h) \
    $(wildcard include/config/netdev/10000.h) \
    $(wildcard include/config/rcu/fanout.h) \
    $(wildcard include/config/blk/dev.h) \
    $(wildcard include/config/spi/master.h) \
    $(wildcard include/config/bridge/ebt/broute.h) \
    $(wildcard include/config/netfilter/xt/target/tcpmss.h) \
    $(wildcard include/config/has/ioport.h) \
    $(wildcard include/config/nf/conntrack/ipv4.h) \
    $(wildcard include/config/have/arch/kgdb.h) \
    $(wildcard include/config/jffs2/fs/debug.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/usb/arch/has/ehci.h) \
    $(wildcard include/config/usb/usbnet.h) \
    $(wildcard include/config/mtd/complex/mappings.h) \
    $(wildcard include/config/nf/conntrack/ipv6.h) \
    $(wildcard include/config/pci/quirks.h) \
    $(wildcard include/config/ext2/fs.h) \
    $(wildcard include/config/xz/dec/arm.h) \
    $(wildcard include/config/ext3/fs.h) \
    $(wildcard include/config/mtd/hsan/sfc/chipnum.h) \
    $(wildcard include/config/ppp/ondemand/v4/v6/separate.h) \
    $(wildcard include/config/hfsplus/fs.h) \
    $(wildcard include/config/dnotify.h) \
    $(wildcard include/config/usb/hsan/ehci/iosize.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/usb/led.h) \
    $(wildcard include/config/nf/defrag/ipv4.h) \
    $(wildcard include/config/nf/nat/ftp.h) \
    $(wildcard include/config/netfilter/xtables.h) \
    $(wildcard include/config/generic/irq/probe.h) \
    $(wildcard include/config/rtc/lib.h) \
    $(wildcard include/config/nf/conntrack/mark.h) \
    $(wildcard include/config/defconfig/list.h) \
    $(wildcard include/config/pppoatm.h) \
    $(wildcard include/config/netfilter/netlink.h) \
    $(wildcard include/config/zboot/rom/bss.h) \
    $(wildcard include/config/slab.h) \
    $(wildcard include/config/inotify.h) \
    $(wildcard include/config/flatmem/manual.h) \
    $(wildcard include/config/hotplug.h) \
    $(wildcard include/config/chip/product/name.h) \
    $(wildcard include/config/usb/arch/has/ohci.h) \
    $(wildcard include/config/devport.h) \
    $(wildcard include/config/bridge/ebt/mark/t.h) \
    $(wildcard include/config/ipv6.h) \
    $(wildcard include/config/usb/hsan/ehci/irqnum.h) \
    $(wildcard include/config/usb/hsan/ohci/iosize.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/default/busclk.h) \
    $(wildcard include/config/vfat/fs.h) \
    $(wildcard include/config/bridge/ebt/arp.h) \
    $(wildcard include/config/ip6/nf/target/log.h) \
    $(wildcard include/config/hsan/lan1/phyport.h) \
    $(wildcard include/config/slhc.h) \
    $(wildcard include/config/mach/sd56xx.h) \
    $(wildcard include/config/embedded.h) \
    $(wildcard include/config/hsan/lan2/phyport.h) \
    $(wildcard include/config/fuse/fs.h) \
    $(wildcard include/config/1.h) \
    $(wildcard include/config/hsan/edb/level.h) \
    $(wildcard include/config/hsan/lan3/phyport.h) \
    $(wildcard include/config/usb/hsan/xhci/hcd/module.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/hsan/waneth/phyid.h) \
    $(wildcard include/config/hsan/lan4/phyport.h) \
    $(wildcard include/config/igmp.h) \
    $(wildcard include/config/bridge/port/relay.h) \
    $(wildcard include/config/usb/arch/has/xhci.h) \
    $(wildcard include/config/nf/conntrack/rtcp.h) \
    $(wildcard include/config/tcp/gather/ack.h) \
    $(wildcard include/config/arch/sd56xx.h) \
    $(wildcard include/config/hisi/chip/name.h) \
    $(wildcard include/config/xz/dec/ia64.h) \
    $(wildcard include/config/usb/hsan/ohci/irqnum.h) \
    $(wildcard include/config/usb/hsan/xhci/iosize.h) \
    $(wildcard include/config/inet.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/usb/qmitty.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/scsi.h) \
    $(wildcard include/config/mtd/rom.h) \
    $(wildcard include/config/tcp/cong/cubic.h) \
    $(wildcard include/config/nf/conntrack/tftp.h) \
    $(wildcard include/config/bridge/ebt/ftos/t.h) \
    $(wildcard include/config/alignment/trap.h) \
    $(wildcard include/config/cc/optimize/for/size.h) \
    $(wildcard include/config/proc/page/monitor.h) \
    $(wildcard include/config/sys/supports/apm/emulation.h) \
    $(wildcard include/config/usb/hsan/xhci/irqnum.h) \
    $(wildcard include/config/nf/conntrack/pptp.h) \
    $(wildcard include/config/ip6/nf/match/eui64.h) \
    $(wildcard include/config/pageflags/extended.h) \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/jffs2/rtime.h) \
    $(wildcard include/config/swap.h) \
    $(wildcard include/config/ext2/fs/posix/acl.h) \
    $(wildcard include/config/hsan/lan1/macindex.h) \
    $(wildcard include/config/ext3/fs/posix/acl.h) \
    $(wildcard include/config/cpu/cache/vipt.h) \
    $(wildcard include/config/crc32.h) \
    $(wildcard include/config/hsan/lan2/macindex.h) \
    $(wildcard include/config/nf/conntrack/rtsp.h) \
    $(wildcard include/config/oabi/compat.h) \
    $(wildcard include/config/hsan/lan3/macindex.h) \
    $(wildcard include/config/crc16.h) \
    $(wildcard include/config/ip/nf/iptables.h) \
    $(wildcard include/config/hsan/lan4/macindex.h) \
    $(wildcard include/config/fat/default/iocharset.h) \
    $(wildcard include/config/cpu/cp15.h) \
    $(wildcard include/config/ext3/defaults/to/ordered.h) \
    $(wildcard include/config/usb/serial/option/module.h) \
    $(wildcard include/config/bridge/nf/ebtables.h) \
    $(wildcard include/config/cpu/pabrt/ifar.h) \
    $(wildcard include/config/dhcps/option125.h) \
    $(wildcard include/config/netfilter/advanced.h) \
    $(wildcard include/config/outer/cache.h) \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/bridge/ebt/ip/iprange.h) \
    $(wildcard include/config/unix.h) \
    $(wildcard include/config/support/special/flashlayout.h) \
    $(wildcard include/config/serial/core.h) \
    $(wildcard include/config/signalfd.h) \
    $(wildcard include/config/serial/hsan.h) \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lock/kernel.h) \
    $(wildcard include/config/scsi/wait/scan/module.h) \
    $(wildcard include/config/nf/conntrack/proc/compat.h) \
    $(wildcard include/config/ip/fib/hash.h) \
    $(wildcard include/config/smux.h) \
    $(wildcard include/config/anon/inodes.h) \
    $(wildcard include/config/html/partition.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/slabinfo.h) \
    $(wildcard include/config/partition/advanced.h) \
    $(wildcard include/config/chelsio/t3/depends.h) \
    $(wildcard include/config/mtd/blkdevs.h) \
    $(wildcard include/config/netfilter/xt/target/notrack.h) \
    $(wildcard include/config/pcieportbus.h) \
    $(wildcard include/config/arm/amba.h) \
    $(wildcard include/config/nls/default.h) \
    $(wildcard include/config/mtd/partitions.h) \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/aeabi.h) \
    $(wildcard include/config/generic/hardirqs/no//do/irq.h) \
    $(wildcard include/config/jffs2/fs.h) \
    $(wildcard include/config/eth/phy/ksz8041.h) \
    $(wildcard include/config/have/ide.h) \
    $(wildcard include/config/firewall.h) \
    $(wildcard include/config/netfilter/xt/target/skiplog.h) \
    $(wildcard include/config/pci/syscall.h) \
    $(wildcard include/config/cpu/32v7.h) \
    $(wildcard include/config/netfilter/xt/match/recent/proc/compat.h) \
    $(wildcard include/config/ipv6/tunnel.h) \
    $(wildcard include/config/xz/dec/powerpc.h) \
    $(wildcard include/config/sysctl/syscall.h) \
    $(wildcard include/config/have/clk.h) \
    $(wildcard include/config/usb/storage/module.h) \
    $(wildcard include/config/hsan/waneth/ifname.h) \
    $(wildcard include/config/arch/suspend/possible.h) \
    $(wildcard include/config/elf/core.h) \
    $(wildcard include/config/atm/drivers.h) \
    $(wildcard include/config/ipv6/mroute.h) \
    $(wildcard include/config/mtd/char.h) \
    $(wildcard include/config/netfilter/xt/match/limit.h) \
    $(wildcard include/config/scsi/dma.h) \
    $(wildcard include/config/ip/nf/target/redirect.h) \
    $(wildcard include/config/default/iosched.h) \
    $(wildcard include/config/hsan/extlan/ifname.h) \
    $(wildcard include/config/legacy/ptys.h) \
    $(wildcard include/config/netfilter/xt/match/state.h) \
    $(wildcard include/config/usb/serial/generic.h) \
    $(wildcard include/config/inet/tunnel.h) \
    $(wildcard include/config/syn/cookies.h) \
    $(wildcard include/config/netfilter/xt/target/dscp.h) \
    $(wildcard include/config/netfilter/xt/target/mark.h) \
    $(wildcard include/config/have/generic/dma/coherent.h) \
    $(wildcard include/config/scsi/multi/lun.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/ext2/fs/xip.h) \
    $(wildcard include/config/has/tls/reg.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/support/max/inactive/memory.h) \
    $(wildcard include/config/generic/clockevents/build.h) \
    $(wildcard include/config/generic/hweight.h) \
    $(wildcard include/config/usb/printer.h) \
    $(wildcard include/config/cpu/copy/v6.h) \
    $(wildcard include/config/squashfs/fragment/cache/size.h) \
    $(wildcard include/config/squashfs.h) \
    $(wildcard include/config/wireless.h) \
    $(wildcard include/config/ip/advanced/router.h) \
    $(wildcard include/config/ip/nf/queue.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/bridge/ebt/mark.h) \
    $(wildcard include/config/hsan/lan1/phyid.h) \
    $(wildcard include/config/bridge/ebt/802/3.h) \
    $(wildcard include/config/sysvipc/sysctl.h) \
    $(wildcard include/config/hsan/lan2/phyid.h) \
    $(wildcard include/config/nls/utf8.h) \
    $(wildcard include/config/nf/nat/tftp.h) \
    $(wildcard include/config/hsan/lan3/phyid.h) \
    $(wildcard include/config/ipv6/sit.h) \
    $(wildcard include/config/hsan/lan4/phyid.h) \
    $(wildcard include/config/proc/sysctl.h) \
    $(wildcard include/config/bridge/ebt/time.h) \
    $(wildcard include/config/ip6/nf/iptables.h) \
    $(wildcard include/config/support/spiflash/protect.h) \
    $(wildcard include/config/qos/ipv6.h) \
    $(wildcard include/config/bridge/ebt/vlan.h) \
    $(wildcard include/config/bridge/ebt/pkttype.h) \
    $(wildcard include/config/imq/behavior/ab.h) \
    $(wildcard include/config/net/ipip.h) \
    $(wildcard include/config/hw/common/8021ag.h) \
    $(wildcard include/config/nf/nat/pptp.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/build/sd5610h.h) \
    $(wildcard include/config/shmem.h) \
    $(wildcard include/config/xz/dec/test.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/nf/nat/rtsp.h) \
    $(wildcard include/config/ip/multiple/tables.h) \
    $(wildcard include/config/bridge/ebt/qinq.h) \
    $(wildcard include/config/squashfs/xz.h) \
    $(wildcard include/config/zboot/rom/text.h) \
    $(wildcard include/config/nls/codepage/437.h) \
    $(wildcard include/config/support/atp.h) \
    $(wildcard include/config/stacktrace/support.h) \
    $(wildcard include/config/build/component.h) \
    $(wildcard include/config/usb/net/hw/cdc/driver.h) \
    $(wildcard include/config/uevent/helper/path.h) \
    $(wildcard include/config/pppoe.h) \
    $(wildcard include/config/have/kretprobes.h) \
    $(wildcard include/config/nls/codepage/936.h) \
    $(wildcard include/config/zlib/deflate.h) \
    $(wildcard include/config/igmp/max/group/number.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/usb/support.h) \
    $(wildcard include/config/netfilter/xt/match/conntrack.h) \
    $(wildcard include/config/cpu/cache/v7.h) \
    $(wildcard include/config/hardirqs/sw/resend.h) \
    $(wildcard include/config/usb/test.h) \
    $(wildcard include/config/tmpfs.h) \
    $(wildcard include/config/ask/ip/fib/hash.h) \
    $(wildcard include/config/generic/time.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/misc/devices.h) \
    $(wildcard include/config/cpu/cp15/mmu.h) \
    $(wildcard include/config/experimental.h) \
    $(wildcard include/config/usb/ehci/hcd.h) \
    $(wildcard include/config/netfilter/xt/match/connlimit.h) \
    $(wildcard include/config/virt/to/bus.h) \
    $(wildcard include/config/arp/reply/to/any/addr.h) \
    $(wildcard include/config/localversion.h) \
    $(wildcard include/config/usb/devicefs.h) \
    $(wildcard include/config/ip6/nf/match/rt.h) \
    $(wildcard include/config/zlib/inflate.h) \
    $(wildcard include/config/init/env/arg/limit.h) \
    $(wildcard include/config/build/component/wlan/chip.h) \
    $(wildcard include/config/netfilter/xt/target/connmark.h) \
    $(wildcard include/config/nf/conntrack.h) \
    $(wildcard include/config/firewall/log.h) \
    $(wildcard include/config/ip/prec/tos/remark.h) \
    $(wildcard include/config/atp/usb/adaptor.h) \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/compat/net/dev/ops.h) \
    $(wildcard include/config/have/kprobes.h) \
    $(wildcard include/config/usb/ohci/hcd.h) \
    $(wildcard include/config/iosched/noop.h) \
    $(wildcard include/config/stop/machine.h) \
    $(wildcard include/config/inet6/tunnel.h) \
    $(wildcard include/config/xz/dec/armthumb.h) \
    $(wildcard include/config/high/res/timers.h) \
    $(wildcard include/config/generic/clockevents.h) \
    $(wildcard include/config/ipv6/route/info.h) \
    $(wildcard include/config/lockdep/support.h) \
    $(wildcard include/config/local/timers.h) \
    $(wildcard include/config/msdos/partition.h) \
    $(wildcard include/config/ip/nf/mangle.h) \
    $(wildcard include/config/ipv6/sit/6rd.h) \
    $(wildcard include/config/default/noop.h) \
    $(wildcard include/config/vectors/base.h) \
    $(wildcard include/config/cpu/abrt/ev7.h) \
    $(wildcard include/config/hz.h) \
    $(wildcard include/config/cpu/32v6k.h) \
    $(wildcard include/config/cpu/has/asid.h) \
    $(wildcard include/config/chip/name.h) \
    $(wildcard include/config/hsan/lan1/ifname.h) \
    $(wildcard include/config/netfilter/xt/match/recent.h) \
    $(wildcard include/config/inet/diag.h) \
    $(wildcard include/config/tracing/support.h) \
    $(wildcard include/config/hsan/lan2/ifname.h) \
    $(wildcard include/config/nls/iso8859/1.h) \
    $(wildcard include/config/net/ethernet.h) \
    $(wildcard include/config/netfilter/xt/match/length.h) \
    $(wildcard include/config/hsan/lan3/ifname.h) \
    $(wildcard include/config/ip6/nf/queue.h) \
    $(wildcard include/config/hsan/lan4/ifname.h) \
    $(wildcard include/config/ip/nf/target/port/triggering.h) \
    $(wildcard include/config/sipalg/recoreddstip.h) \
    $(wildcard include/config/usb/ohci/little/endian.h) \
    $(wildcard include/config/ssb/possible.h) \
    $(wildcard include/config/mld/snooping.h) \
    $(wildcard include/config/mtd/hsan/sfc.h) \
    $(wildcard include/config/atp/conntrack/clean.h) \
    $(wildcard include/config/tick/oneshot.h) \
    $(wildcard include/config/vlan/8021q.h) \
    $(wildcard include/config/cpu/32.h) \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
    $(wildcard include/config/bridge.h) \
    $(wildcard include/config/netfilter.h) \
    $(wildcard include/config/base/full.h) \
    $(wildcard include/config/crc/ccitt.h) \
    $(wildcard include/config/ip/multicast.h) \
    $(wildcard include/config/mtd/block.h) \
    $(wildcard include/config/prevent/firmware/build.h) \
    $(wildcard include/config/hisi/chip.h) \
    $(wildcard include/config/ip/nf/filter.h) \
    $(wildcard include/config/generic/calibrate/delay.h) \
    $(wildcard include/config/has/iomem.h) \
    $(wildcard include/config/hisi/half/bridge.h) \
    $(wildcard include/config/netfilter/xt/match/multiport.h) \
    $(wildcard include/config/netfilter/xt/match/mac.h) \
    $(wildcard include/config/xz/dec/sparc.h) \
    $(wildcard include/config/wireless/ext/sysfs.h) \
    $(wildcard include/config/atp/ledservice/path.h) \
    $(wildcard include/config/module/force/unload.h) \
    $(wildcard include/config/igmp/max/msf.h) \
    $(wildcard include/config/spiflash/protect/ratio.h) \
    $(wildcard include/config/packet.h) \
    $(wildcard include/config/atp/netfilter/drop/finrst.h) \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/serial/core/console.h) \
    $(wildcard include/config/nf/ct/proto/gre.h) \
    $(wildcard include/config/generic/hardirqs.h) \
    $(wildcard include/config/atp/drop/arp.h) \
    $(wildcard include/config/ip6/nf/match/ipv6header.h) \
    $(wildcard include/config/netfilter/xt/match/iif.h) \
    $(wildcard include/config/fib/rules.h) \
    $(wildcard include/config/ipv6/ndisc/nodetype.h) \
    $(wildcard include/config/bounce.h) \
    $(wildcard include/config/have/aout.h) \
    $(wildcard include/config/scsi/proc/fs.h) \
    $(wildcard include/config/netfilter/xt/match/tcpmss.h) \
    $(wildcard include/config/wireless/ext.h) \
    $(wildcard include/config/bridge/ebt/t/filter.h) \
    $(wildcard include/config/arm/thumb.h) \
    $(wildcard include/config/build/rtl/new/ce/patch.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/bridge/ebt/wmm/mark.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
    $(wildcard include/config/mtd/hsan/sfc/reg/base/address.h) \
    $(wildcard include/config/mtd/hsan/sfc/cs0/buffer/base/address.h) \
    $(wildcard include/config/imq/num/devs.h) \
    $(wildcard include/config/mtd/hsan/sfc/cs1/buffer/base/address.h) \
    $(wildcard include/config/select/memory/model.h) \
    $(wildcard include/config/netfilter/xt/match/esp.h) \
    $(wildcard include/config/pass/up/dad/packets.h) \
    $(wildcard include/config/ppp/async.h) \
    $(wildcard include/config/ip/nf/target/reject.h) \
    $(wildcard include/config/jbd.h) \
    $(wildcard include/config/usb/arch/has/hcd.h) \
    $(wildcard include/config/default/tcp/cong.h) \
    $(wildcard include/config/lbd.h) \
    $(wildcard include/config/fat/fs.h) \
    $(wildcard include/config/usb/net/rmnet/ethernet/module.h) \
    $(wildcard include/config/nf/conntrack/irc.h) \
    $(wildcard include/config/ip/mroute.h) \
    $(wildcard include/config/cpu/v7.h) \
    $(wildcard include/config/ip6/nf/mangle.h) \
    $(wildcard include/config/nf/conntrack/h323.h) \
    $(wildcard include/config/nlattr.h) \
    $(wildcard include/config/fat/default/codepage.h) \
    $(wildcard include/config/nf/nat.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/nf/nat/pt.h) \
    $(wildcard include/config/build/2t3r.h) \
    $(wildcard include/config/nf/nat/needed.h) \
    $(wildcard include/config/fs/mbcache.h) \
    $(wildcard include/config/llc.h) \
    $(wildcard include/config/bridge/ebt/t/nat.h) \
    $(wildcard include/config/pci.h) \
    $(wildcard include/config/mld.h) \
    $(wildcard include/config/xz/dec.h) \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/mii.h) \
    $(wildcard include/config/bridge/ebt/ip.h) \
    $(wildcard include/config/nf/conntrack/ftp.h) \
    $(wildcard include/config/arm.h) \
    $(wildcard include/config/common/clkdev.h) \
    $(wildcard include/config/atm.h) \
    $(wildcard include/config/sysctl.h) \
    $(wildcard include/config/misc/filesystems.h) \
    $(wildcard include/config/ip/nf/match/misc.h) \
    $(wildcard include/config/have/oprofile.h) \
    $(wildcard include/config/hsan/waneth/phyport.h) \
    $(wildcard include/config/mtd.h) \
    $(wildcard include/config/netdev/1000.h) \
    $(wildcard include/config/zone/dma/flag.h) \
    $(wildcard include/config/imq.h) \
    $(wildcard include/config/ip6/nf/filter.h) \
    $(wildcard include/config/hsan/kconsole.h) \
    $(wildcard include/config/net.h) \
    $(wildcard include/config/atp/chip/name.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/fs/xip.h) \
    $(wildcard include/config/standalone.h) \
    $(wildcard include/config/netfilter/xt/match/iprange.h) \
    $(wildcard include/config/hsan/extlan/phyport.h) \
    $(wildcard include/config/mac/partition.h) \
    $(wildcard include/config/netdevices.h) \
    $(wildcard include/config/usb.h) \
    $(wildcard include/config/inet/tcp/diag.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/frame/pointer.h) \
    $(wildcard include/config/jffs2/zlib.h) \
    $(wildcard include/config/usb/hsan/ehci.h) \
    $(wildcard include/config/ppp/sync/tty.h) \
    $(wildcard include/config/spi.h) \
    $(wildcard include/config/efi/partition.h) \
    $(wildcard include/config/nls.h) \
    $(wildcard include/config/mtd/cfi/i1.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/mtd/cfi/i2.h) \
    $(wildcard include/config/ppp.h) \
    $(wildcard include/config/pci/legacy.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/ipv6/subtrees.h) \
    $(wildcard include/config/ipv6/router/pref.h) \
    $(wildcard include/config/ext2/fs/security.h) \
    $(wildcard include/config/ip/nf/target/allconenat.h) \
    $(wildcard include/config/qos.h) \
    $(wildcard include/config/ext3/fs/security.h) \
    $(wildcard include/config/ip/nf/target/log.h) \
    $(wildcard include/config/base/small.h) \
    $(wildcard include/config/usb/hsan/ohci.h) \
    $(wildcard include/config/ldm/partition.h) \
    $(wildcard include/config/binfmt/elf.h) \
    $(wildcard include/config/squashfs/embedded.h) \
    $(wildcard include/config/jffs2/fs/writebuffer.h) \
    $(wildcard include/config/stp.h) \
    $(wildcard include/config/hsan/waneth/macindex.h) \
    $(wildcard include/config/igmp/snooping.h) \
    $(wildcard include/config/legacy/pty/count.h) \
    $(wildcard include/config/ip/nf/target/masquerade.h) \
    $(wildcard include/config/have/mlock.h) \
    $(wildcard include/config/pcieaer.h) \
    $(wildcard include/config/bitreverse.h) \
    $(wildcard include/config/log/buf/shift.h) \
    $(wildcard include/config/cmdline.h) \
    $(wildcard include/config/build/component/realtek/wlan/chip.h) \
    $(wildcard include/config/usb/hsan/ehci/iobase.h) \
    $(wildcard include/config/usb/hsan/xhci.h) \
    $(wildcard include/config/highmem.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/bhal.h \
    $(wildcard include/config/atp/multi/nvram.h) \
    $(wildcard include/config/wlan/str.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/spiflash_protect.h \

init/halt.o: $(deps_init/halt.o)

$(deps_init/halt.o):
