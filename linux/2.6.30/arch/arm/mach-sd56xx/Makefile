#
# Makefile for the linux kernel.
#


KBUILD_AFLAGS	:=$(KBUILD_AFLAGS:-msoft-float=-Wa,-mfpu=softvfp+vfp)
LDFLAGS		+=--no-warn-mismatch

obj-y				:= core.o clock.o watchdog.o
obj-y				+= cache-hil2.o
obj-$(CONFIG_PCI)	+= pcie.o

#SMP
obj-$(CONFIG_SMP)	+= smp.o headsmp.o
obj-$(CONFIG_LOCAL_TIMERS)	+= localtimer.o
obj-$(CONFIG_HOTPLUG_CPU)	+= hotplug.o