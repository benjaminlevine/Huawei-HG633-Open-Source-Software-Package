################### depend objects
OBJS = attr.o document.o element.o ixml.o ixmlmembuf.o ixmlparser.o namedNodeMap.o node.o nodeList.o

OUTPUT_OBJS:=$(patsubst %,$(O)%,$(OBJS))
######################### compile options
CFLAGS = -Wl,-allow-shlib-undefined
CFLAGS += -I$(TOPDIR)/package/atp/libraries/opensrc/libupnp/ixml/inc
CFLAGS += -I$(TOPDIR)/package/atp/libraries/opensrc/libupnp/ixml/src/inc
CFLAGS += -I$(TOPDIR)/package/atp/libraries/opensrc/libupnp/ixml/src/inc
CFLAGS += -I$(STAGING_DIR)/usr/include/libupnp

################### Yourself lib(if no, fill nothing)
EXTRA_LIB =

ixml: $(O)libixml.so

$(O)libixml.so: $(OUTPUT_OBJS)
	$(CC) $(CFLAGS) -fPIC -shared $^ -o $@

include $(TOPDIR)/package/atp/rule.mak
