ifneq ($(wildcard config.mak),)
include config.mak
endif

INSTALL_FILE := install -m644 -D
INSTALL_PROGRAM := install -m755 -D
INSTALL_DIR := install -m755 -d

CP := cp -rf
RM := rm -f
LN := ln -s
GZIP := gzip -f9
QMAKE := qmake-qt4
LRELEASE := lrelease-qt4
MAKE := make

ICON_SIZES = 16 24 32 48 64 96 128 256 512

MAKEFILE = inyokaedit.mk
INFILES = \
	data/menu \
	man/inyokaedit.1 \
	man/de/inyokaedit.1

exec_prefix = $(prefix)
bindir = $(prefix)/bin
libdir = $(prefix)/lib
dataroot = $(prefix)/share


all:
	$(QMAKE) -o $(MAKEFILE)
	$(MAKE) -f $(MAKEFILE)
	$(LRELEASE) application/lang/*.ts
	$(LRELEASE) plugins/knowledgebox/lang/*.ts
	$(LRELEASE) plugins/spellchecker/lang/*.ts
	$(LRELEASE) plugins/tabletemplate/lang/*.ts

install: install-inyokaedit install-plugins install-hook

infiles:
	$(foreach FILE,$(INFILES),sed -e 's@___PREFIX___@$(prefix)@g' $(FILE).in > $(FILE) ; )

install-inyokaedit: infiles
	$(INSTALL_DIR) $(DESTDIR)/etc/apport
	$(INSTALL_DIR) $(DESTDIR)$(bindir)
	$(INSTALL_DIR) $(DESTDIR)$(libdir)/inyokaedit
	$(INSTALL_DIR) $(DESTDIR)$(dataroot)/applications
	$(INSTALL_DIR) $(DESTDIR)$(dataroot)/apport
	$(INSTALL_DIR) $(DESTDIR)$(dataroot)/icons
	$(INSTALL_DIR) $(DESTDIR)$(dataroot)/inyokaedit/lang
	$(INSTALL_DIR) $(DESTDIR)$(dataroot)/menu
	$(INSTALL_DIR) $(DESTDIR)$(dataroot)/mime/packages
	$(INSTALL_DIR) $(DESTDIR)$(dataroot)/pixmaps
	$(INSTALL_DIR) $(DESTDIR)$(mandir)/man1
	$(INSTALL_DIR) $(DESTDIR)$(mandir)/de/man1
	$(INSTALL_PROGRAM) inyokaedit           $(DESTDIR)$(libdir)/inyokaedit
	$(INSTALL_PROGRAM) data/inyokaedit      $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) data/GetInyokaStyles $(DESTDIR)$(dataroot)/inyokaedit
	$(INSTALL_PROGRAM) data/GetIWLs         $(DESTDIR)$(dataroot)/inyokaedit
	$(INSTALL_FILE) apport/inyokaedit-crashdb.conf        $(DESTDIR)/etc/apport
	$(INSTALL_FILE) apport/source_inyokaedit.py           $(DESTDIR)$(dataroot)/apport/package-hooks
	$(INSTALL_FILE) application/lang/inyokaedit_de.qm     $(DESTDIR)$(dataroot)/inyokaedit/lang
	$(INSTALL_FILE) application/res/images/inyokaedit.xpm $(DESTDIR)$(dataroot)/pixmaps
	$(INSTALL_FILE) data/ExtendedDict.txt   $(DESTDIR)$(dataroot)/inyokaedit
	$(INSTALL_FILE) data/inyokaedit.desktop $(DESTDIR)$(dataroot)/applications
	$(INSTALL_FILE) data/inyokaedit.xml     $(DESTDIR)$(dataroot)/mime/packages
	$(INSTALL_FILE) data/menu               $(DESTDIR)$(dataroot)/menu/inyokaedit
	$(INSTALL_FILE) man/inyokaedit.1        $(DESTDIR)$(mandir)/man1
	$(INSTALL_FILE) man/de/inyokaedit.1     $(DESTDIR)$(mandir)/de/man1
	$(CP) application/res/images/hicolor    $(DESTDIR)$(dataroot)/icons
	$(CP) application/iWikiLinks            $(DESTDIR)$(dataroot)/inyokaedit
	$(CP) application/templates             $(DESTDIR)$(dataroot)/inyokaedit

install-plugins:
	$(INSTALL_DIR) $(DESTDIR)$(libdir)/inyokaedit/plugins
	$(INSTALL_DIR) $(DESTDIR)$(dataroot)/inyokaedit/lang
	$(INSTALL_FILE) plugins/libknowledgebox.so  $(DESTDIR)$(libdir)/inyokaedit/plugins
	$(INSTALL_FILE) plugins/libspellchecker.so  $(DESTDIR)$(libdir)/inyokaedit/plugins
	$(INSTALL_FILE) plugins/libtabletemplate.so $(DESTDIR)$(libdir)/inyokaedit/plugins
	$(INSTALL_FILE) plugins/knowledgebox/lang/knowledgebox_de.qm   $(DESTDIR)$(dataroot)/inyokaedit/lang
	$(INSTALL_FILE) plugins/spellchecker/lang/spellchecker_de.qm   $(DESTDIR)$(dataroot)/inyokaedit/lang
	$(INSTALL_FILE) plugins/tabletemplate/lang/tabletemplate_de.qm $(DESTDIR)$(dataroot)/inyokaedit/lang

install-hook:
	$(GZIP) $(DESTDIR)$(mandir)/man1/inyokaedit.1
	$(GZIP) $(DESTDIR)$(mandir)/de/man1/inyokaedit.1

uninstall:
	$(RM) -r $(DESTDIR)$(libdir)/inyokaedit
	$(RM) -r $(DESTDIR)$(dataroot)/inyokaedit
	$(RM) $(DESTDIR)$(bindir)/inyokaedit
	$(RM) $(DESTDIR)/etc/apport/inyokaedit-crashdb.conf
	$(RM) $(DESTDIR)$(dataroot)/apport/package-hooks/source_inyokaedit.py
	$(RM) $(DESTDIR)$(dataroot)/pixmaps/inyokaedit.xpm
	$(RM) $(DESTDIR)$(dataroot)/applications/inyokaedit.desktop
	$(RM) $(DESTDIR)$(dataroot)/mime/packages/inyokaedit.xml
	$(RM) $(DESTDIR)$(dataroot)/menu/inyokaedit
	$(RM) $(DESTDIR)$(mandir)/man1/inyokaedit.1*
	$(RM) $(DESTDIR)$(mandir)/de/man1/inyokaedit.1*
	$(RM) $(DESTDIR)$(dataroot)/icons/hicolor/scalable/apps/inyokaedit.svg
	$(foreach SIZE,$(ICON_SIZES),$(RM) $(DESTDIR)$(dataroot)/icons/hicolor/$(SIZE)x$(SIZE)/apps/inyokaedit.png ;)


clean:
	[ ! -f $(MAKEFILE) ] || $(MAKE) -f $(MAKEFILE) clean
	$(RM) $(INFILES)
	$(RM) plugins/*.so
	$(RM) application/lang/*.qm
	$(RM) plugins/knowledgebox/lang/*.qm
	$(RM) plugins/spellchecker/lang/*.qm
	$(RM) plugins/tabletemplate/lang/*.qm

distclean: clean
	[ ! -f $(MAKEFILE) ] || $(MAKE) -f $(MAKEFILE) distclean
	$(RM) config.mak

