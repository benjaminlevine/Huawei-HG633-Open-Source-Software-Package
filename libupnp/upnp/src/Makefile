################### depend objects
OBJS = inet_pton.o
LIBUPNP_SUB_DIRS = api gena genlib soap ssdp urlconfig uuid \
		   genlib/client_table \
		   genlib/miniserver \
		   genlib/service_table \
		   genlib/util genlib/net \
		   genlib/net/uri \
		   genlib/net/http

OBJS += $(patsubst %.c, %.o, $(foreach dir, $(LIBUPNP_SUB_DIRS), $(wildcard $(dir)/*.c)))

######################### compile options
CFLAGS = -Wl,-allow-shlib-undefined -Wno-strict-aliasing -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64
CFLAGS += -I$(STAGING_DIR)/usr/include/libupnp
CFLAGS += -I$(HOST_DIR)/usr/include/libupnp

################### Yourself lib(if no, fill nothing)
EXTRA_LIB =

include $(TOPDIR)/package/atp/rule.mak
