# Generic Makefile for hptsqfix

ifeq ($(DEBIAN), 1)
# Every Debian-Source-Paket has one included.
include debian/huskymak.cfg
else
include ../huskymak.cfg
endif

ifeq ($(DEBUG), 1)
  CFLAGS = $(DEBCFLAGS) -Ih -I$(INCDIR) $(WARNFLAGS)
  LFLAGS = $(DEBLFLAGS)
else
  CFLAGS = $(OPTCFLAGS) -Ih -I$(INCDIR) $(WARNFLAGS)
  LFLAGS = $(OPTLFLAGS)
endif

ifeq ($(SHORTNAME), 1)
  LIBS  = -L$(LIBDIR) -lfidoconf -lsmapi
else
  LIBS  = -L$(LIBDIR) -lfidoconfig -lsmapi
endif

CDEFS=-D$(OSTYPE) -DUNAME=\"$(UNAME)\" $(ADDCDEFS)

OBJS    = hptsqfix$(OBJ)
SRC_DIR = src/

%$(OBJ): $(SRC_DIR)%.c
	$(CC) $(CFLAGS) $(CDEFS) $(SRC_DIR)$*.c

hptsqfix$(EXE): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o hptsqfix$(EXE)

man: man/hptsqfix.1
	gzip -9c man/hptsqfix.1 > hptsqfix.1.gz

clean:
	-$(RM) $(RMOPT) *$(OBJ)
	-$(RM) $(RMOPT) *~
	-$(RM) $(RMOPT) core
	-$(RM) $(RMOPT) hptsqfix$(EXE)

distclean: clean
	-$(RM) $(RMOPT) hptsqfix$(EXE)
	-$(RM) $(RMOPT) hptsqfix.1.gz

all: hptsqfix$(EXE) man

install: all
	$(INSTALL) $(IBOPT) hptsqfix$(EXE) $(BINDIR)
ifdef MANDIR
	-$(MKDIR) $(MKDIROPT) $(MANDIR)$(DIRSEP)man1
	$(INSTALL) $(IMOPT) hptsqfix.1.gz $(MANDIR)$(DIRSEP)man1
endif

uninstall:
	$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)hptsqfix$(EXE)
ifdef MANDIR
	$(RM) $(RMOPT) $(MANDIR)$(DIRSEP)man1$(DIRSEP)hptsqfix.1.gz
endif
