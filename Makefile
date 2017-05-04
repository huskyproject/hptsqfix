# Generic Makefile for hptsqfix

ifeq ($(DEBIAN), 1)
# Every Debian-Source-Paket has one included.
include /usr/share/husky/huskymak.cfg
else ifdef RPM_BUILD_ROOT
# RPM build requires all files to be in the source directory
include huskymak.cfg
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

LIBS  = -L$(LIBDIR) -lsmapi -lhusky

CDEFS=-D$(OSTYPE) -DUNAME=\"$(UNAME)\" $(ADDCDEFS)

OBJS    = hptsqfix$(_OBJ)
SRC_DIR = src/

default: all

%$(_OBJ): $(SRC_DIR)%.c
	$(CC) $(CFLAGS) $(CDEFS) $(SRC_DIR)$*.c

hptsqfix$(_EXE): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o hptsqfix$(_EXE)

hptsqfix.1.gz: man/hptsqfix.1
	gzip -9c man/hptsqfix.1 > hptsqfix.1.gz

clean:
	-$(RM) $(RMOPT) *$(_OBJ)
	-$(RM) $(RMOPT) *~
	-$(RM) $(RMOPT) core
	-$(RM) $(RMOPT) hptsqfix$(_EXE)

distclean: clean
	-$(RM) $(RMOPT) hptsqfix$(_EXE)
	-$(RM) $(RMOPT) hptsqfix.1.gz

all: hptsqfix$(_EXE) hptsqfix.1.gz

install: all
	$(MKDIR) $(MKDIROPT) $(DESTDIR)$(BINDIR)
	$(INSTALL) $(IBOPT) hptsqfix$(_EXE) $(DESTDIR)$(BINDIR)
ifdef MANDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(MANDIR)$(DIRSEP)man1
	$(INSTALL) $(IMOPT) hptsqfix.1.gz $(DESTDIR)$(MANDIR)$(DIRSEP)man1
endif
ifdef HTMLDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(HTMLDIR)$(DIRSEP)hptsqfix
endif

uninstall:
	-$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)hptsqfix$(_EXE)
ifdef MANDIR
	-$(RM) $(RMOPT) $(MANDIR)$(DIRSEP)man1$(DIRSEP)hptsqfix.1.gz
endif
ifdef HTMLDIR
	-$(RM) $(RMOPT) $(DESTDIR)$(HTMLDIR)$(DIRSEP)hptsqfix
endif
