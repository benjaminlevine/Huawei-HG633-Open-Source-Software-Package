################### depend objects
OBJS = FreeList.o LinkedList.o ThreadPool.o TimerThread.o

OUTPUT_OBJS:=$(patsubst %,$(O)%,$(OBJS))
######################### compile options
#
# Must define _GNU_SOURCE to use pthread_rwlock_xxxx
# See features.h of uclibc
#
CFLAGS = -D_GNU_SOURCE -Wl,-allow-shlib-undefined
CFLAGS += -I$(STAGING_DIR)/usr/include/libupnp

################### Yourself lib(if no, fill nothing)
EXTRA_LIB =

threadutil: $(O)libthreadutil.so

$(O)libthreadutil.so: $(OUTPUT_OBJS)
	$(CC) $(CFLAGS) -fPIC -shared $^ -o $@

include $(TOPDIR)/package/atp/rule.mak
