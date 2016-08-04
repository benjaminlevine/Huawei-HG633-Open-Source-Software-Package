#
# Makefile for USB Core files and filesystem
#
ifeq ($(CONFIG_USB_HSAN_USB3_0),y)

usbcore-objs	:= hi_usb_usb3.o hi_hub_usb3.o hcd.o urb.o message.o hi_driver_usb3.o \
			config.o file.o buffer.o hi_sysfs_usb3.o endpoint.o \
			devio.o notify.o generic.o quirks.o
else
usbcore-objs	:= usb.o hub.o hcd.o urb.o message.o driver.o \
			config.o file.o buffer.o sysfs.o endpoint.o \
			devio.o notify.o generic.o quirks.o
endif

ifeq ($(CONFIG_PCI),y)
	usbcore-objs	+= hcd-pci.o
endif

ifeq ($(CONFIG_USB_DEVICEFS),y)
	usbcore-objs	+= inode.o devices.o
endif

obj-$(CONFIG_USB)	+= usbcore.o

ifeq ($(CONFIG_USB_DEBUG),y)
EXTRA_CFLAGS += -DDEBUG
endif
EXTRA_CFLAGS			+= $(if $(CONFIG_FMC_USBNET_HW_CDC_DRIVER), -DCONFIG_FMC_USBNET_HW_CDC_DRIVER)
