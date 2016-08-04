SSOBJ=ss.o ssfilter.o
LNSTATOBJ=lnstat.o lnstat_util.o

#MISC_SS = 1
#MISC_NSTAT = 1
#MISC_IFSTAT = 1
#MISC_RTACCT = 1
#MISC_LNSTAT = 1
#MISC_ARPD = 1
include ../cfg

TARGETS =

ifdef MISC_SS
TARGETS+=ss 
endif

ifdef MISC_NSTAT
TARGETS+=nstat 
endif

ifdef MISC_IFSTAT
TARGETS+=ifstat 
endif

ifdef MISC_RTACCT
TARGETS+=rtacct
endif

ifdef MISC_LNSTAT
TARGETS+=lnstat
endif

ifdef MISC_ARPD
TARGETS += arpd
endif

include ../Config

all: $(TARGETS)

ss: $(SSOBJ) $(LIBUTIL)

nstat: nstat.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o nstat nstat.c -lm

ifstat: ifstat.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o ifstat ifstat.c $(LIBNETLINK) -lm

rtacct: rtacct.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o rtacct rtacct.c $(LIBNETLINK) -lm

arpd: arpd.c
	$(CC) $(CFLAGS) -I$(DBM_INCLUDE) $(LDFLAGS) -o arpd arpd.c $(LIBNETLINK) -ldb -lpthread

ssfilter.c: ssfilter.y
	bison ssfilter.y -o ssfilter.c

lnstat: $(LNSTATOBJ)

install: all
	install -m 0755 $(TARGETS) $(DESTDIR)$(SBINDIR)
	ln -sf lnstat $(DESTDIR)$(SBINDIR)/rtstat
	ln -sf lnstat $(DESTDIR)$(SBINDIR)/ctstat

clean:
	rm -f *.o $(TARGETS) ssfilter.c
