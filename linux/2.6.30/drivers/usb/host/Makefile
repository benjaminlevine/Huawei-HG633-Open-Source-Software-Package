#
# Makefile for USB Host Controller Drivers
#

ifeq ($(CONFIG_USB_DEBUG),y)
	EXTRA_CFLAGS		+= -DDEBUG
endif

isp1760-objs := isp1760-hcd.o isp1760-if.o
fhci-objs := fhci-hcd.o fhci-hub.o fhci-q.o fhci-mem.o \
	     fhci-tds.o fhci-sched.o
ifeq ($(CONFIG_FHCI_DEBUG),y)
fhci-objs += fhci-dbg.o
endif

obj-$(CONFIG_USB_WHCI_HCD)	+= whci/

ifeq ($(CONFIG_USB_HSAN_USB3_0),y)
obj-$(CONFIG_PCI)		+= hi_pci-quirks_usb3.o
else
obj-$(CONFIG_PCI)		+= pci-quirks.o
endif

obj-$(CONFIG_USB_EHCI_HCD)	+= ehci-hcd.o
obj-$(CONFIG_USB_OXU210HP_HCD)	+= oxu210hp-hcd.o
obj-$(CONFIG_USB_ISP116X_HCD)	+= isp116x-hcd.o
obj-$(CONFIG_USB_OHCI_HCD)	+= ohci-hcd.o
obj-$(CONFIG_USB_UHCI_HCD)	+= uhci-hcd.o
obj-$(CONFIG_USB_FHCI_HCD)	+= fhci.o
obj-$(CONFIG_USB_SL811_HCD)	+= sl811-hcd.o
obj-$(CONFIG_USB_SL811_CS)	+= sl811_cs.o
obj-$(CONFIG_USB_U132_HCD)	+= u132-hcd.o
obj-$(CONFIG_USB_R8A66597_HCD)	+= r8a66597-hcd.o
obj-$(CONFIG_USB_ISP1760_HCD)	+= isp1760.o
obj-$(CONFIG_USB_HWA_HCD)	+= hwa-hc.o

ifeq ($(CONFIG_HSAN),y)
ifeq ($(CONFIG_USB_HSAN_USB3_0),y)
hi_xhci-hcd-objs := hi_xhci_usb3.o hi_xhci-mem_usb3.o  hi_xhci-ring_usb3.o hi_xhci-hub_usb3.o hi_xhci-dbg.o hi_xhci-pci_usb3.o
obj-$(CONFIG_USB_HSAN_XHCI_HCD)      += hi_xhci-hcd.o
else
hi_xhci-hcd-objs := hi_xhci.o hi_xhci-mem.o  hi_xhci-ring.o hi_xhci-hub.o hi_xhci-dbg.o #hi_xhci-pci.o
obj-$(CONFIG_USB_XHCI_HCD)      += hi_xhci-hcd.o
endif
obj-$(CONFIG_HSAN)              += hiusb-sd56xx.o
endif
