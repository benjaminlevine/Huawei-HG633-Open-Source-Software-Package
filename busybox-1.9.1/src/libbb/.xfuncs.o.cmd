cmd_libbb/xfuncs.o := /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/bin/ccache /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/bin/arm-hsan-linux-uclibcgnueabi-gcc -Wp,-MD,libbb/.xfuncs.o.d   -std=gnu99 -Iinclude -Ilibbb  -I/home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/build/busybox-1.9.1/src/libbb -include include/autoconf.h -D_GNU_SOURCE -DNDEBUG -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D"BB_VER=\"v1.9.1\"" -DBB_BT=AUTOCONF_TIMESTAMP -D_FORTIFY_SOURCE=2  -Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Os -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-guess-branch-probability -funsigned-char -static-libgcc -falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1  -Wdeclaration-after-statement -Wno-pointer-sign -I/home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include -I/openssl -fno-strict-aliasing    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(xfuncs)"  -D"KBUILD_MODNAME=KBUILD_STR(xfuncs)" -c -o libbb/xfuncs.o libbb/xfuncs.c

deps_libbb/xfuncs.o := \
  libbb/xfuncs.c \
    $(wildcard include/config/debug.h) \
    $(wildcard include/config/verbose/resolution/errors.h) \
    $(wildcard include/config/ture/ipv6.h) \
    $(wildcard include/config/feature/clean/up.h) \
    $(wildcard include/config/selinux.h) \
    $(wildcard include/config/ioctl/hex2str/error.h) \
  include/libbb.h \
    $(wildcard include/config/locale/support.h) \
    $(wildcard include/config/feature/shadowpasswds.h) \
    $(wildcard include/config/lfs.h) \
    $(wildcard include/config/feature/buffers/go/on/stack.h) \
    $(wildcard include/config/buffer.h) \
    $(wildcard include/config/ubuffer.h) \
    $(wildcard include/config/feature/buffers/go/in/bss.h) \
    $(wildcard include/config/feature/ipv6.h) \
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
  libbb/../atpconfig/atpcommon.h \
    $(wildcard include/config/feature/atp/wget/zip.h) \
    $(wildcard include/config/feature/atp/ftp/zip.h) \
    $(wildcard include/config/feature/atp/tftp/zip.h) \
    $(wildcard include/config/feature/atp/mcast/zip.h) \
  include/platform.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atputil.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/linux/if_link.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/linux/types.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm/types.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm-generic/int-ll64.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/linux/posix_types.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/linux/stddef.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/asm/posix_types.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/linux/netlink.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/linux/socket.h \
    $(wildcard include/config/compat.h) \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atptypes.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/malloc.h \
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
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atpcbbid.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atpconst.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/atpdeprecated.h \
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/msgapi.h \
    $(wildcard include/config/stats.h) \
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
    $(wildcard include/config/st.h) \
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
  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/arm-unknown-linux-uclibcgnueabi/sysroot/usr/include/caltime.h \

libbb/xfuncs.o: $(deps_libbb/xfuncs.o)

$(deps_libbb/xfuncs.o):
