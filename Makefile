# hptsqfix/Makefile
#
# This file is part of hptsqfix, part of the Husky fidonet software project
# Use with GNU version of make v.3.82 or later
# Requires: husky enviroment
#

# Version
hptsqfix_g1:=$(GREP) -Po 'define\s+hptsqfix_VER_MAJOR\s+\K\d+'
hptsqfix_g2:=$(GREP) -Po 'define\s+hptsqfix_VER_MINOR\s+\K\d+'
hptsqfix_g3:=$(GREP) -Po 'define\s+hptsqfix_VER_PATCH\s+\K\d+'
hptsqfix_g4:=$(GREP) -Po 'char\s+cvs_date\[\]\s*=\s*"\K\d+-\d+-\d+'
hptsqfix_VERMAJOR := $(shell $(hptsqfix_g1) $(hptsqfix_ROOTDIR)$(hptsqfix_H_DIR)version.h)
hptsqfix_VERMINOR := $(shell $(hptsqfix_g2) $(hptsqfix_ROOTDIR)$(hptsqfix_H_DIR)version.h)
hptsqfix_VERPATCH := $(shell $(hptsqfix_g3) $(hptsqfix_ROOTDIR)$(hptsqfix_H_DIR)version.h)
hptsqfix_cvsdate  := $(shell $(hptsqfix_g4) $(hptsqfix_ROOTDIR)$(hptsqfix_H_DIR)cvsdate.h)
hptsqfix_reldate  := $(subst -,,$(hptsqfix_cvsdate))
hptsqfix_VER      := $(hptsqfix_VERMAJOR).$(hptsqfix_VERMINOR).$(hptsqfix_reldate)

hptsqfix_LIBS := $(smapi_TARGET_BLD) $(huskylib_TARGET_BLD)

hptsqfix_CFLAGS = $(CFLAGS)
hptsqfix_CDEFS := $(CDEFS) -DUNAME=\"$(UNAME)\" -I$(smapi_ROOTDIR) \
                          -I$(huskylib_ROOTDIR) -I$(hptsqfix_ROOTDIR)$(hptsqfix_H_DIR)

hptsqfix_SRC  = $(hptsqfix_SRCDIR)hptsqfix.c
hptsqfix_OBJS = $(addprefix $(hptsqfix_OBJDIR),$(notdir $(hptsqfix_SRC:.c=$(_OBJ))))
hptsqfix_DEPS = $(addprefix $(hptsqfix_DEPDIR),$(notdir $(hptsqfix_SRC:.c=$(_DEP))))

hptsqfix_TARGET     = hptsqfix$(_EXE)
hptsqfix_TARGET_BLD = $(hptsqfix_BUILDDIR)$(hptsqfix_TARGET)
hptsqfix_TARGET_DST = $(BINDIR_DST)$(hptsqfix_TARGET)

ifdef MAN1DIR
    hptsqfix_MAN1PAGES := hptsqfix.1
    hptsqfix_MAN1BLD := $(hptsqfix_BUILDDIR)$(hptsqfix_MAN1PAGES).gz
    hptsqfix_MAN1DST := $(DESTDIR)$(MAN1DIR)$(DIRSEP)$(hptsqfix_MAN1PAGES).gz
endif

.PHONY: hptsqfix_all hptsqfix_install hptsqfix_uninstall hptsqfix_clean \
        hptsqfix_distclean hptsqfix_depend hptsqfix_rmdir_DEP hptsqfix_rm_DEPS \
        hptsqfix_clean_OBJ hptsqfix_main_distclean

hptsqfix_all: $(hptsqfix_TARGET_BLD) $(hptsqfix_MAN1BLD)

ifneq ($(MAKECMDGOALS), depend)
    ifneq ($(MAKECMDGOALS), distclean)
        ifneq ($(MAKECMDGOALS), uninstall)
            include $(hptsqfix_DEPS)
        endif
    endif
endif


# Build application
$(hptsqfix_TARGET_BLD): $(hptsqfix_OBJS) $(hptsqfix_LIBS) | do_not_run_make_as_root
	$(CC) $(LFLAGS) $(EXENAMEFLAG) $@ $^

# Compile .c files
$(hptsqfix_OBJS): $(hptsqfix_SRC) | $(hptsqfix_OBJDIR)
	$(CC) $(hptsqfix_CFLAGS) $(hptsqfix_CDEFS) -o $@ $<

$(hptsqfix_OBJDIR): | $(hptsqfix_BUILDDIR) do_not_run_make_as_root
	[ -d $@ ] || $(MKDIR) $(MKDIROPT) $@


# Build man pages
ifdef MAN1DIR
    $(hptsqfix_MAN1BLD): $(hptsqfix_MANDIR)$(hptsqfix_MAN1PAGES) | do_not_run_make_as_root
	gzip -c $< > $@
else
    $(hptsqfix_MAN1BLD): ;
endif


# Install
ifneq ($(MAKECMDGOALS), install)
    hptsqfix_install: ;
else
    hptsqfix_install: $(hptsqfix_TARGET_DST) hptsqfix_install_man ;
endif

$(hptsqfix_TARGET_DST): $(hptsqfix_TARGET_BLD) | $(DESTDIR)$(BINDIR)
	$(INSTALL) $(IBOPT) $< $(DESTDIR)$(BINDIR); \
	$(TOUCH) "$@"

ifndef MAN1DIR
    hptsqfix_install_man: ;
else
    hptsqfix_install_man: $(hptsqfix_MAN1DST)

    $(hptsqfix_MAN1DST): $(hptsqfix_MAN1BLD) | $(DESTDIR)$(MAN1DIR)
	$(INSTALL) $(IMOPT) $< $(DESTDIR)$(MAN1DIR); $(TOUCH) "$@"
endif


# Clean
hptsqfix_clean: hptsqfix_clean_OBJ
	-[ -d "$(hptsqfix_OBJDIR)" ] && $(RMDIR) $(hptsqfix_OBJDIR) || true

hptsqfix_clean_OBJ:
	-$(RM) $(RMOPT) $(hptsqfix_OBJDIR)*

# Distclean
hptsqfix_distclean: hptsqfix_main_distclean hptsqfix_rmdir_DEP
	-[ -d "$(hptsqfix_BUILDDIR)" ] && $(RMDIR) $(hptsqfix_BUILDDIR) || true

hptsqfix_rmdir_DEP: hptsqfix_rm_DEPS
	-[ -d "$(hptsqfix_DEPDIR)" ] && $(RMDIR) $(hptsqfix_DEPDIR) || true

hptsqfix_rm_DEPS:
	-$(RM) $(RMOPT) $(hptsqfix_DEPDIR)*

hptsqfix_main_distclean: hptsqfix_clean
	-$(RM) $(RMOPT) $(hptsqfix_TARGET_BLD)
ifdef MAN1DIR
	-$(RM) $(RMOPT) $(hptsqfix_MAN1BLD)
endif


# Uninstall
hptsqfix_uninstall:
	-$(RM) $(RMOPT) $(hptsqfix_TARGET_DST)
ifdef MAN1DIR
	-$(RM) $(RMOPT) $(hptsqfix_MAN1DST)
endif


# Depend
ifeq ($(MAKECMDGOALS),depend)
hptsqfix_depend: $(hptsqfix_DEPS) ;

# Build a dependency makefile for the source file
$(hptsqfix_DEPS): $(hptsqfix_DEPDIR)%$(_DEP): $(hptsqfix_SRCDIR)%.c | $(hptsqfix_DEPDIR)
	@set -e; rm -f $@; \
	$(CC) -MM $(hptsqfix_CFLAGS) $(hptsqfix_CDEFS) $< > $@.$$$$; \
	sed 's,\($*\)$(_OBJ)[ :]*,$(hptsqfix_OBJDIR)\1$(_OBJ) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(hptsqfix_DEPDIR): | $(hptsqfix_BUILDDIR) do_not_run_depend_as_root
	[ -d $@ ] || $(MKDIR) $(MKDIROPT) $@
endif

$(hptsqfix_BUILDDIR):
	[ -d $@ ] || $(MKDIR) $(MKDIROPT) $@
