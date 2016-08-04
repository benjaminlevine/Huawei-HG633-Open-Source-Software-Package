DESTDIR=
SBINDIR=$(TARGET_DIR)/bin
CONFDIR=$(TARGET_DIR)/etc
DOCDIR=

#NETEM = 1
#GENL = 1
#MISC = 1
#IP = 1
#TC = 1
include ./cfg

# Path to parent kernel include files directory
#KERNEL_INCLUDE:=$(KERNEL_DIR)/include
#LIBC_INCLUDE=$(KERNEL_DIR)/include/linux

# Path to db_185.h include
#DBM_INCLUDE:=/usr/include

#DEFINES= -DRESOLVE_HOSTNAMES

#options if you have a bind>=4.9.4 libresolv (or, maybe, glibc)
LDLIBS=-lresolv
ADDLIB=

#options for decnet
ADDLIB+=dnet_ntop.o dnet_pton.o

#options for ipx
ADDLIB+=ipx_ntop.o ipx_pton.o

#CC = gcc
#HOSTCC = gcc
CCOPTS = -D_GNU_SOURCE -O2 -Wstrict-prototypes -Wall
#CFLAGS = $(CCOPTS) $(GLIBCFIX) -I$(KERNEL_INCLUDE) -I../include -I../glibc-include $(DEFINES)
CFLAGS = $(CCOPTS) -I../include $(DEFINES)
YACCFLAGS = -d -t -v

# start of y42304 2008-12-20:优化编译选项，减少空间
CFLAGS += $(VENDOR_APP_CFLAGS) $(ATPFLAGS)
# end of y42304 2008-4-19: 优化编译选项，减少空间
LDLIBS += -L../lib -lnetlink -lutil

SUBDIRS=lib 

ifdef IP
SUBDIRS +=ip 
endif
ifdef TC
SUBDIRS += tc
endif

ifdef NETEM
SUBDIRS+=netem 
endif

ifdef GENL
SUBDIRS+=genl 
endif

ifdef MISC
SUBDIRS+=misc
endif

LIBNETLINK=../lib/libnetlink.a ../lib/libutil.a

all: Config
	@set -e; \
	for i in $(SUBDIRS); \
	do $(MAKE) $(MFLAGS) -C $$i; done

Config:
	sh configure $(KERNEL_INCLUDE)

install: all
ifdef TC
	install -m 0755 tc/tc $(DESTDIR)$(SBINDIR)
	$(STRIP) $(TARGET_DIR)/bin/tc
endif
ifdef IP
	install -m 0755 ip/ip $(DESTDIR)$(SBINDIR)
	$(STRIP) $(TARGET_DIR)/bin/ip
endif
ifdef MISC
ifdef MISC_SS
	install -m 0755 misc/ss $(DESTDIR)$(SBINDIR)
	$(STRIP) $(TARGET_DIR)/bin/ss
endif
ifdef MISC_NSTAT
	install -m 0755 misc/nstat $(DESTDIR)$(SBINDIR)
	$(STRIP) $(TARGET_DIR)/bin/nstat
endif
ifdef MISC_IFSTAT
	install -m 0755 misc/ifstat $(DESTDIR)$(SBINDIR)
	$(STRIP) $(TARGET_DIR)/bin/ifstat
endif
ifdef MISC_RTACCT
	install -m 0755 misc/rtacct $(DESTDIR)$(SBINDIR)
	$(STRIP) $(TARGET_DIR)/bin/rtacct
endif
ifdef MISC_LNSTAT
	install -m 0755 misc/lnstat $(DESTDIR)$(SBINDIR)
	$(STRIP) $(TARGET_DIR)/bin/lnstat
endif
ifdef MISC_RTSTAT
	install -m 0755 misc/rtstat $(DESTDIR)$(SBINDIR)
	$(STRIP) $(TARGET_DIR)/bin/rtstat
endif
endif

snapshot:
	echo "static const char SNAPSHOT[] = \""`date +%y%m%d`"\";" \
		> include/SNAPSHOT.h

dynamic: install

clean:
	rm -f cscope.*
	@for i in $(SUBDIRS) doc; \
	do $(MAKE) $(MFLAGS) -C $$i clean; done

clobber: clean
	rm -f Config

distclean: clobber

cscope:
	cscope -b -q -R -Iinclude -sip -slib -smisc -snetem -stc

.EXPORT_ALL_VARIABLES:
