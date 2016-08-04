################### depend objects
OBJS = main.o magic.o fsm.o lcp.o ipcp.o upap.o chap-new.o md5.o ccp.o \
	   ecp.o auth.o options.o demand.o utils.o sys-linux.o ipxcp.o tty.o \
	   eap.o chap-md5.o session.o

######################### compile options
CFLAGS = -O0 -Wall

#ifneq ($(wildcard $(LIBCDIR)/lib/libcrypt.*),)
LIBS += -lcrypt -lgplutil
#LIBS += $(LIBCDIR)/lib/libcrypt.a
#endif

# Uncomment the next 2 lines to include support for Microsoft's
# MS-CHAP authentication protocol.  Also, edit plugins/radius/Makefile.linux.
CHAPMS=y
USE_CRYPT=y
# Don't use MSLANMAN unless you really know what you're doing.
#MSLANMAN=y
# Uncomment the next line to include support for MPPE.  CHAPMS (above) must
# also be enabled.  Also, edit plugins/radius/Makefile.linux.
MPPE=y

# Uncomment the next line to include support for PPP packet filtering.
# This requires that the libpcap library and headers be installed
# and that the kernel driver support PPP packet filtering.
#FILTER=y

# Uncomment the next line to enable multilink PPP (enabled by default)
# Linux distributions: Please leave multilink ENABLED in your builds
# of pppd!
#HAVE_MULTILINK=y

# Uncomment the next line to enable the TDB database (enabled by default.)
# If you enable multilink, then TDB is automatically enabled also.
# Linux distributions: Please leave TDB ENABLED in your builds.
#USE_TDB=y

#HAS_SHADOW=y
#USE_PAM=y
#HAVE_INET6=y

# Enable plugins
PLUGIN=y

# Enable Microsoft proprietary Callback Control Protocol
#CBCP=y

# Enable EAP SRP-SHA1 authentication (requires libsrp)
#USE_SRP=y

#MAXOCTETS=y

#INCLUDE_DIRS= -I../include

COMPILE_FLAGS= -D_linux_=1 -DHAVE_PATHS_H -DHAVE_MMAP

#CFLAGS= $(COPTS) $(COMPILE_FLAGS) $(INCLUDE_DIRS) '-DDESTDIR="/bin"'
# For complile warning
CFLAGS += -fno-strict-aliasing

ifdef CHAPMS
CFLAGS   += -DCHAPMS=1
NEEDDES=y
OBJS += md4.o chap_ms.o
HEADERS	+= md4.h chap_ms.h
ifdef MSLANMAN
CFLAGS   += -DMSLANMAN=1
endif
ifdef MPPE
CFLAGS   += -DMPPE=1
endif
endif

# EAP SRP-SHA1
ifdef USE_SRP
CFLAGS	+= -DUSE_SRP -DOPENSSL -I/usr/local/ssl/include
EXTRA_LIBS	+= -lsrp -L/usr/local/ssl/lib -lcrypto
TARGETS	+= srp-entry
EXTRAINSTALL = $(INSTALL) -s -c -m 555 srp-entry $(BINDIR)/srp-entry
MANPAGES += srp-entry.8
EXTRACLEAN += srp-entry.o
NEEDDES=y
else
# OpenSSL has an integrated version of SHA-1, and its implementation
# is incompatible with this local SHA-1 implementation.  We must use
# one or the other, not both.
#PPPDSRCS += sha1.c
#HEADERS += sha1.h
OBJS += sha1.o
endif

ifdef HAS_SHADOW
CFLAGS   += -DHAS_SHADOW
#EXTRA_LIBS     += -lshadow $(LIBS)
endif

#ifneq ($(wildcard /usr/include/crypt.h),)
CFLAGS  += -DHAVE_CRYPT_H=1
#EXTRA_LIBS	+= -lcrypt
#endif

ifdef NEEDDES
ifndef USE_CRYPT
EXTRA_LIBS     += -ldes
else
CFLAGS   += -DUSE_CRYPT=1
endif
OBJS += pppcrypt.o
HEADERS += pppcrypt.h
endif

# For "Pluggable Authentication Modules", see ftp.redhat.com:/pub/pam/.
ifdef USE_PAM
CFLAGS   += -DUSE_PAM
EXTRA_LIBS     += -lpam -ldl
endif

# Multi-linnk
ifdef HAVE_MULTILINK
	# Multilink implies the use of TDB
	USE_TDB=y

	CFLAGS += -DHAVE_MULTILINK
	PPPDSRCS += multilink.c
	OBJS += multilink.o
endif

# TDB
ifdef USE_TDB
	CFLAGS += -DUSE_TDB=1
	PPPDSRCS += tdb.c spinlock.c
	OBJS += tdb.o spinlock.o
	HEADERS += tdb.h spinlock.h
endif

# Lock library binary for Linux is included in 'linux' subdirectory.
ifdef LOCKLIB
EXTRA_LIBS     += -llock
CFLAGS   += -DLOCKLIB=1
endif

ifdef PLUGIN
CFLAGS	+= -DPLUGIN
LDFLAGS	+= -Wl,-E
EXTRA_LIBS	+= -ldl
endif

ifdef FILTER
ifneq ($(wildcard /usr/include/pcap-bpf.h),)
EXTRA_LIBS    += -lpcap
CFLAGS  += -DPPP_FILTER
endif
endif

ifdef HAVE_INET6
     PPPDSRCS += ipv6cp.c eui64.c
     HEADERS  += ipv6cp.h eui64.h
     OBJS += ipv6cp.o eui64.o
     CFLAGS   += -DINET6=1
endif

ifdef CBCP
     PPPDSRCS += cbcp.c
     OBJS += cbcp.o
     CFLAGS += -DCBCP_SUPPORT
     HEADERS += cbcp.h
endif

ifdef MAXOCTETS
     CFLAGS += -DMAXOCTETS
endif

################### depend lib
EXTRA_LIB += -lcrypt -ldl

include $(TOPDIR)/package/atp/rule.mak

