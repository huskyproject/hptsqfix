# include Husky-Makefile-Config
include ../huskymak.cfg

OBJS    = hptsqfix$(OBJ)
SRC_DIR = src/

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

CDEFS=-D$(OSTYPE) $(ADDCDEFS)

all: $(OBJS) hptsqfix$(EXE)

%$(OBJ): $(SRC_DIR)%.c
		$(CC) $(CFLAGS) $(CDEFS) $(SRC_DIR)$*.c

hptsqfix$(EXE): $(OBJS)
		$(CC) $(LFLAGS) -o hptsqfix$(EXE) $(OBJS) $(LIBS)

clean:
		-$(RM) $(RMOPT) *$(OBJ)
		-$(RM) $(RMOPT) *~
		-$(RM) $(RMOPT) core
		-$(RM) $(RMOPT) hptsqfix$(EXE)

distclean: clean
		-$(RM) $(RMOPT) hptsqfix$(EXE)

install: hptsqfix$(EXE)
		$(INSTALL) $(IBOPT) hptsqfix$(EXE) $(BINDIR)

uninstall:
		$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)hptsqfix$(EXE)
