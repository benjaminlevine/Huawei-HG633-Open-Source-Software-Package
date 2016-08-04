#############################################################
#
# linux kernel
#
#############################################################
#include dir.atp.mak
LINUX_DEPENDENCIES = install-all-atpkconfigs 

ifeq ($(BUILD_LINUX_FAKEROOT),y)
LINUX_DEPENDENCIES += host-fakeroot
FAKEROOT = fakeroot
else
FAKEROOT =
endif

LINUX_VERSION=$(call qstrip,$(BR2_LINUX_KERNEL_VERSION))

LINUX_MAKE_FLAGS = \
	HOSTCC="$(HOSTCC)" \
	HOSTCFLAGS="$(HOSTCFLAGS)" \
	HUAWEI_DRIVERS_DIR="$(HUAWEI_DRIVERS_DIR)" \
	KBUILD_MODPOST_WARN=1 \
	CROSS_COMPILE="$(CCACHE) $(TARGET_CROSS)"

# Compute the arch path, since i386 and x86_64 are in arch/x86 and not
# in arch/$(KERNEL_ARCH). Even if the kernel creates symbolic links
# for bzImage, arch/i386 and arch/x86_64 do not exist when copying the
# defconfig file.
ifeq ($(KERNEL_ARCH),i386)
KERNEL_ARCH_PATH=$(LINUX_DIR)/$(LINUX_VERSION)/arch/x86
else ifeq ($(KERNEL_ARCH),x86_64)
KERNEL_ARCH_PATH=$(LINUX_DIR)/$(LINUX_VERSION)/arch/x86
else
KERNEL_ARCH_PATH=$(LINUX_DIR)/$(LINUX_VERSION)/arch/$(BR2_ARCH)
endif

ifeq ($(BR2_LINUX_KERNEL_USE_DEFCONFIG),y)
KERNEL_DEFAULT_FILE = $(KERNEL_ARCH_PATH)/configs/$(call qstrip,$(BR2_LINUX_KERNEL_DEFCONFIG))_defconfig
else ifeq ($(BR2_LINUX_KERNEL_USE_CUSTOM_CONFIG),y)
KERNEL_DEFAULT_FILE = $(LINUX_DIR)/$(BR2_LINUX_KERNEL_CUSTOM_CONFIG_FILE)
endif

ifeq ($(MAKECMDGOALS),atpcore-buildall)
KERNEL_CONFIG_FILE=$(O)/.config
KERNEL_INCLUDE_DIR=$(O)/include
else
KERNEL_CONFIG_FILE=$(KERNEL_DIR)/.config
KERNEL_INCLUDE_DIR=$(KERNEL_DIR)/include
endif

ifeq ($(BR2_LINUX_KERNEL_IMAGE_TARGET_CUSTOM),y)
LINUX_IMAGE_NAME=$(call qstrip,$(BR2_LINUX_KERNEL_IMAGE_TARGET_NAME))
else
ifeq ($(BR2_LINUX_KERNEL_UIMAGE),y)
ifeq ($(KERNEL_ARCH),blackfin)
# a uImage, but with a different file name
LINUX_IMAGE_NAME=vmImage
else
LINUX_IMAGE_NAME=uImage
endif
ifneq ($(BUILD_HISI_BOOT),y)
LINUX_DEPENDENCIES+=host-uboot-tools
endif
else ifeq ($(BR2_LINUX_KERNEL_BZIMAGE),y)
LINUX_IMAGE_NAME=bzImage
else ifeq ($(BR2_LINUX_KERNEL_ZIMAGE),y)
LINUX_IMAGE_NAME=zImage
else ifeq ($(BR2_LINUX_KERNEL_VMLINUX_BIN),y)
LINUX_IMAGE_NAME=vmlinux.bin
else ifeq ($(BR2_LINUX_KERNEL_VMLINUX),y)
LINUX_IMAGE_NAME=vmlinux
endif
endif

define LINUX_CHANGELOGS
V1.00:
	1. First release by l00135113.

endef

define LINUX_INSTALL_HEADER_CMDS
	$(Q)cd $$(KERNEL_DIR)/include/;	rm -f asm; ln -sf asm-$(ARCH) asm;
ifeq ($(MAKECMDGOALS),atpcore-buildall)
	cd $$(KERNEL_DIR); $(TARGET_ATP_ENVS) HUAWEI_DRIVERS_DIR="$(HUAWEI_DRIVERS_DIR)" $(MAKE1) ARCH=$(ARCH) silentoldconfig O=$(O);
else
	cd $$(KERNEL_DIR); $(TARGET_ATP_ENVS) HUAWEI_DRIVERS_DIR="$(HUAWEI_DRIVERS_DIR)" $(MAKE1) ARCH=$(ARCH) silentoldconfig;
endif
	if [ ! -d $(STAGING_DIR)/usr/include/linux/ ]; then \
		mkdir -p $(STAGING_DIR)/usr/include/linux/; \
	fi; 
	if [ -f $(KERNEL_INCLUDE_DIR)/linux/autoconf.h ]; then \
		cp -f $(KERNEL_INCLUDE_DIR)/linux/autoconf.h $(STAGING_DIR)/usr/include/linux/autoconf.h; \
	fi; 
	cp -Rf $(KERNEL_DIR)/include/mtd $(STAGING_DIR)/usr/include
	cp -Rf $(KERNEL_DIR)/include/linux/compiler.h $(STAGING_DIR)/usr/include/linux
	mkdir -p $(HOST_DIR)/usr/include/linux
	cp -Rf $(KERNEL_DIR)/include/linux/compiler.h $(HOST_DIR)/usr/include/linux
ifeq ($(HISI_CHIP_NAME),"sd5610")
	cp -Rf $(KERNEL_DIR)/include/linux/pci.h $(STAGING_DIR)/usr/include/linux
	cp -Rf $(KERNEL_DIR)/include/linux/pci.h $(HOST_DIR)/usr/include/linux
endif
	if [ -f $(KERNEL_INCLUDE_DIR)/linux/generated/autoconf.h ]; then cp -Rf $(KERNEL_INCLUDE_DIR)/generated/autoconf.h $(STAGING_DIR)/usr/include/linux/; fi
	if [ -f $(KERNEL_INCLUDE_DIR)/autoconf.h ]; then cp -Rf $(KERNEL_INCLUDE_DIR)/autoconf.h $(STAGING_DIR)/usr/include/linux/; fi
	if [[ "$(strip $(CHIP_NAME))" = "rt63365" ]]; then \
	cp -Rf $(KERNEL_DIR)/include/mtd $(STAGING_DIR)/usr/include; \
	cp -f $(KERNEL_DIR)/include/asm-mips/socket.h $(STAGING_DIR)/usr/include/asm; \
	elif [[ "$(strip $(CHIP_NAME))" = "rtl8676s" ]]; then \
	cp -f $(KERNEL_DIR)/include/asm-mips/socket.h $(STAGING_DIR)/usr/include/asm; \
	fi
endef
#Add the "KBUILD_MODPOST_WARN=1" to avoid report the ERROR when compile the driver module ko
#
# change priority for MT7620 "in ram" ram rootfs.
# build modules -> install modules -> build linux
#
ifeq ($(MT7620),y)
define LINUX_BUILD_CMDS
	echo "===aaaa=BUILD_ENDIAN=$(BUILD_ENDIAN)=="
	rm -f $$(LINUX_BDIR)/.stamp_installtarget
	cd $$(KERNEL_DIR)/include/;	rm -f asm; ln -sf asm-$(ARCH) asm
	$(TARGET_ATP_ENVS) $(FAKEROOT) $(MAKE) $(LINUX_MAKE_FLAGS) -C $(KERNEL_DIR)  ARCH=$(ARCH) modules	
	$(TARGET_ATP_ENVS) $(FAKEROOT) $(MAKE) $(LINUX_MAKE_FLAGS) -C $(KERNEL_DIR)  ARCH=$(ARCH) modules_install	
	$(TARGET_ATP_ENVS) $(FAKEROOT) $(MAKE) $(LINUX_MAKE_FLAGS) -C $(KERNEL_DIR) ARCH=$(ARCH) $(LINUX_IMAGE_NAME)
endef
else
define LINUX_BUILD_CMDS
	echo "===aaaa=BUILD_ENDIAN=$(BUILD_ENDIAN)=="
	rm -f $$(LINUX_BDIR)/.stamp_installtarget
	cd $$(KERNEL_DIR)/include/;	rm -f asm; ln -sf asm-$(ARCH) asm
	$(TARGET_ATP_ENVS) $(FAKEROOT) $(MAKE) $(LINUX_MAKE_FLAGS) -C $(KERNEL_DIR) ARCH=$(ARCH) $(LINUX_IMAGE_NAME)
	$(TARGET_ATP_ENVS) $(FAKEROOT) $(MAKE) $(LINUX_MAKE_FLAGS) -C $(KERNEL_DIR)  ARCH=$(ARCH) modules
ifeq ($(RT63365),y)
	$(Q)$(TARGET_ATP_ENVS) ARCH=$(ARCH) $(MAKE1) -C $$(MTKCHIP_DIR)/$(CHIP_NAME)/$(DRIVER_RELEASE_VERSION)/wlan_$(MTK_WLAN_SUB_CHIP) all;
	$(Q)install -m 0777 $$(MTKCHIP_DIR)/$(CHIP_NAME)/$(DRIVER_RELEASE_VERSION)/wlan_$(MTK_WLAN_SUB_CHIP)/os/linux/rt$(MTK_WIFI_CHIP_SET)ap.ko $(TARGET_DIR)/lib/extra/$(MTK_WLAN_SUB_CHIP)ap.ko;
endif
	$(TARGET_ATP_ENVS) $(FAKEROOT) $(MAKE) $(LINUX_MAKE_FLAGS) -C $(KERNEL_DIR)  ARCH=$(ARCH) modules_install	
endef
endif		


define LINUX_CLEAN_CMDS
	@echo "=$(LINUX_DIR)==";\
	$(TARGET_ATP_ENVS) HUAWEI_DRIVERS_DIR="$(HUAWEI_DRIVERS_DIR)" $(MAKE) -C $(KERNEL_DIR) ARCH=$(ARCH) clean
ifeq ($(RT63365),y)
	$(Q)$(TARGET_ATP_ENVS) ARCH=$(ARCH) $(MAKE1) -C $$(MTKCHIP_DIR)/$(CHIP_NAME)/$(DRIVER_RELEASE_VERSION)/wlan_$(MTK_WLAN_SUB_CHIP) clean;
endif
	@echo "==clean atpkconfig===";\
	make remove-all-atpkconfigs;
endef


ifeq ($(BR2_LINUX_KERNEL_VMLINUX),y)
define LINUX_INSTALL_TARGET_CMDS
	cp $(KERNEL_DIR)/vmlinux $(BINARIES_DIR)/.
	cp $(KERNEL_DIR)/System.map $(BINARIES_DIR)/sysmap/
	$(STRIP) --remove-section=.note --remove-section=.comment $(BINARIES_DIR)/vmlinux
	$(OBJCOPY) -O binary $(BINARIES_DIR)/vmlinux $(BINARIES_DIR)/vmlinux.bin
	$(LZMA_CMD) $(call qstrip,$(ATP_LZMA_OPT))

ifeq ($(RT63365),y)
	$(Q)mkdir -p $(TARGET_DIR)/etc/Wireless/RT2860AP;
	install -m 770	$$(MTKCHIP_DIR)/ko/RT2860AP.dat $(TARGET_DIR)/etc/Wireless/RT2860AP;
endif

ifeq ($(BUILD_QLZMA),y)
	cp $(BINARIES_DIR)/$(ATP_LINUX_FILENAME) $(BINARIES_DIR)/$(ATP_LINUX_FILENAME).qlzma
	${MKIMAGE_CMD} -A mips -O linux -T kernel -C lzma \
		-a 0x${LDADDR} -e ${ENTRY} -n "Linux Kernel Image"    \
		-d $(BINARIES_DIR)/$(ATP_LINUX_FILENAME).qlzma $(BINARIES_DIR)/$(ATP_LINUX_FILENAME)
	rm -f $(BINARIES_DIR)/$(ATP_LINUX_FILENAME).qlzma 
endif
	
ifeq ($(BUILD_MTKLZMA),y)
	${MKIMAGE_CMD} -A mips -O linux -T kernel -C lzma \
		-a 0x${LDADDR} -e ${ENTRY} -n "Linux Kernel Image"    \
		-d $(BINARIES_DIR)/vmlinux.bin.lzma $(BINARIES_DIR)/$(ATP_LINUX_FILENAME)
	rm -f $(BINARIES_DIR)/vmlinux.bin.lzma
endif
	
ifeq ($(strip $(BROADCOM_RELEASE_VERSION)),"4.12L08")
	rm -r $(TARGET_DIR)/etc/adsl;\
	mkdir $(TARGET_DIR)/etc/adsl
	install -m 770 $(TOPDIR)/$(BCMCHIP_DIR)/$(CHIP_NAME)/$(DRIVER_RELEASE_VERSION)/adsl/*.bin $(TARGET_DIR)/etc/adsl
endif
	
	rm -f $(BINARIES_DIR)/vmlinux 
	rm -f $(BINARIES_DIR)/vmlinux.bin
endef
else
ifeq ($(BR2_LINUX_KERNEL_ZIMAGE),y)
define LINUX_INSTALL_TARGET_CMDS
	cp $(KERNEL_DIR)/vmlinux $(BINARIES_DIR)/.
	cp $(KERNEL_DIR)/System.map $(BINARIES_DIR)/sysmap/
	$(OBJCOPY) -O binary -R .note -R .note.gnu.build-id -R .comment -S $(BINARIES_DIR)/vmlinux $(BINARIES_DIR)/vmlinux.bin
	$(LZMA_CMD) $(call qstrip,$(ATP_LZMA_OPT))
	
ifeq ($(HISI_CHIP_NAME),"sd5115")	
	cp $(BINARIES_DIR)/$(ATP_LINUX_FILENAME) $(BINARIES_DIR)/$(ATP_LINUX_FILENAME).lzma
	$(HOST_DIR)/usr/bin/mkimage  -A arm -O linux -T kernel -C lzma \
	  -a 0x80e08000 -e 0x80e08000 -n "Linux-2.6.30"    \
		-d $(BINARIES_DIR)/$(ATP_LINUX_FILENAME).lzma $(BINARIES_DIR)/$(ATP_LINUX_FILENAME)
else ifeq ($(HISI_CHIP_NAME),"sd5610")
	cp $(BINARIES_DIR)/$(ATP_LINUX_FILENAME) $(BINARIES_DIR)/$(ATP_LINUX_FILENAME).lzma
	$(HOST_DIR)/usr/bin/mkimage  -A arm -O linux -T kernel -C lzma \
	  -a 0x81208000 -e 0x81208000 -n "Linux-2.6.30"    \
		-d $(BINARIES_DIR)/$(ATP_LINUX_FILENAME).lzma $(BINARIES_DIR)/$(ATP_LINUX_FILENAME)
endif

	rm -f $(BINARIES_DIR)/vmlinux 
	rm -f $(BINARIES_DIR)/vmlinux.bin
endef
else
define LINUX_INSTALL_TARGET_CMDS
	$(Q)cp $$(KERNEL_ARCH_PATH)/boot/uImage  $(BINARIES_DIR)/$(ATP_LINUX_FILENAME);
ifneq ($(BR2_TARGET_ROOTFS_JFFS2),)
	cp $$(KERNEL_ARCH_PATH)/boot/uImage $(TARGET_DIR)/$(ATP_LINUX_FILENAME);
endif		
	cp $(KERNEL_DIR)/System.map $(BINARIES_DIR)/sysmap/
endef
endif
endif

# Target type
LINUX_TYPE =kernel

$(eval $(call ATPTARGETS,linux,linux))
