cmd_vmlinux := /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/bin/ccache /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/output/host/usr/bin/arm-hsan-linux-uclibcgnueabi-ld -EL  -p --no-undefined -X --build-id -o vmlinux -T arch/arm/kernel/vmlinux.lds arch/arm/kernel/head.o arch/arm/kernel/init_task.o  init/built-in.o --start-group  usr/built-in.o  arch/arm/kernel/built-in.o  arch/arm/mm/built-in.o  arch/arm/common/built-in.o  arch/arm/mach-sd56xx/built-in.o  kernel/built-in.o  mm/built-in.o  fs/built-in.o  ipc/built-in.o  security/built-in.o  crypto/built-in.o  block/built-in.o  arch/arm/lib/lib.a  lib/lib.a  arch/arm/lib/built-in.o  lib/built-in.o  drivers/built-in.o  sound/built-in.o  firmware/built-in.o  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/package/atp/libraries/private/bhalapi/kernel/src/built-in.o  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/package/atp/libraries/private/ssp/kernel/src/built-in.o  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/package/atp/applications/diag8021ag/kernel/src/built-in.o  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/package/atp/applications/ledservice/kernel/tt/hg633/built-in.o  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/package/atp/configuration/monitor/kernel/src/built-in.o  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/package/atp/network/route/kernel/src/built-in.o  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/package/atp/network/wan/kernel/smux/built-in.o  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/driver/component/wlan/rtl8192cd/hisi/built-in.o  /home/luokunling/open-HG633/BR-R5-HG633HG658v2-20140408/driver/hisi/sd5610/source/k_space/built-in.o  net/built-in.o --end-group .tmp_kallsyms2.o
