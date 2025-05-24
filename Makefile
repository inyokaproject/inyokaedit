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
QMAKE := qmake6
LRELEASE := /usr/lib/qt6/bin/lrelease
MAKE := make

ICON_SIZES = 16 24 32 48 64 96 128 256 512

MAKEFILE  = inyokaedit.mk
MAKEFILE2 = plugins.mk
INFILES   = \
	  man/inyokaedit.1 \
	  man/de/inyokaedit.1

exec_prefix = $(prefix)
bindir = $(prefix)/bin
libdir = $(prefix)/lib
dataroot = $(prefix)/share


all:	app allplugins

app:
	$(LRELEASE) application/lang/*.ts
	$(QMAKE) $(preview) application/application.pro -o application/$(MAKEFILE)
	$(MAKE) -C application -f $(MAKEFILE)

allplugins:
	$(LRELEASE) plugins/highlighter/lang/*.ts
	$(LRELEASE) plugins/hotkey/lang/*.ts
	$(LRELEASE) plugins/spellchecker-hunspell/lang/*.ts
#	$(LRELEASE) plugins/spellchecker-nuspell/lang/*.ts
	$(LRELEASE) plugins/uu_knowledgebox/lang/*.ts
	$(LRELEASE) plugins/uu_tabletemplate/lang/*.ts
	$(QMAKE) $(preview) plugins/plugins.pro -o plugins/$(MAKEFILE2)
	$(MAKE) -C plugins -f $(MAKEFILE2)

install: install-inyokaedit install-plugins install-data-ubuntuusersde install-hook

infiles:
	$(foreach FILE,$(INFILES),sed -e 's#@___PREFIX___#/$(prefix)#g' $(FILE).in > $(FILE) ; )

install-inyokaedit: infiles
	$(INSTALL_DIR) $(DESTDIR)$(bindir)
	$(INSTALL_DIR) $(DESTDIR)$(dataroot)/applications
	$(INSTALL_DIR) $(DESTDIR)$(dataroot)/metainfo
	$(INSTALL_DIR) $(DESTDIR)$(dataroot)/mime/packages
	$(INSTALL_DIR) $(DESTDIR)$(mandir)/man1
	$(INSTALL_DIR) $(DESTDIR)$(mandir)/de/man1
	$(INSTALL_DIR) $(DESTDIR)$(dataroot)/icons
	$(INSTALL_PROGRAM) inyokaedit       $(DESTDIR)$(bindir)
	$(INSTALL_FILE) application/data/unix/org.inyokaproject.inyokaedit.desktop      $(DESTDIR)$(dataroot)/applications
	$(INSTALL_FILE) application/data/unix/org.inyokaproject.inyokaedit.metainfo.xml $(DESTDIR)$(dataroot)/metainfo
	$(INSTALL_FILE) application/data/unix/org.inyokaproject.inyokaedit.xml $(DESTDIR)$(dataroot)/mime/packages
	$(INSTALL_FILE) man/inyokaedit.1    $(DESTDIR)$(mandir)/man1
	$(INSTALL_FILE) man/de/inyokaedit.1 $(DESTDIR)$(mandir)/de/man1
	$(CP) application/icons/hicolor     $(DESTDIR)$(dataroot)/icons

install-plugins:
	$(INSTALL_DIR) $(DESTDIR)$(libdir)/inyokaedit/plugins
	$(INSTALL_FILE) plugins/libhighlighter.so               $(DESTDIR)$(libdir)/inyokaedit/plugins
	$(INSTALL_FILE) plugins/libhotkey.so                    $(DESTDIR)$(libdir)/inyokaedit/plugins
	$(INSTALL_FILE) plugins/libspellchecker-hunspell.so     $(DESTDIR)$(libdir)/inyokaedit/plugins
#	$(INSTALL_FILE) plugins/libspellchecker-nuspell.so      $(DESTDIR)$(libdir)/inyokaedit/plugins
	$(INSTALL_FILE) plugins/libuu_knowledgebox.so           $(DESTDIR)$(libdir)/inyokaedit/plugins
	$(INSTALL_FILE) plugins/libuu_tabletemplate.so          $(DESTDIR)$(libdir)/inyokaedit/plugins

install-data-ubuntuusersde:
ifeq ($(community),community/ubuntuusers_de)
	$(INSTALL_DIR) $(DESTDIR)$(dataroot)/inyokaedit/community
	$(CP) community/ubuntuusers_de $(DESTDIR)$(dataroot)/inyokaedit/community
endif

install-hook:
	$(GZIP) $(DESTDIR)$(mandir)/man1/inyokaedit.1
	$(GZIP) $(DESTDIR)$(mandir)/de/man1/inyokaedit.1

uninstall:
	$(RM) -r $(DESTDIR)$(libdir)/inyokaedit
	$(RM) -r $(DESTDIR)$(dataroot)/inyokaedit
	$(RM) $(DESTDIR)$(bindir)/inyokaedit
	$(RM) $(DESTDIR)$(dataroot)/applications/org.inyokaproject.inyokaedit.desktop
	$(RM) $(DESTDIR)$(dataroot)/metainfo/org.inyokaproject.inyokaedit.metainfo.xml
	$(RM) $(DESTDIR)$(dataroot)/mime/packages/org.inyokaproject.inyokaedit.xml
	$(RM) $(DESTDIR)$(mandir)/man1/inyokaedit.1*
	$(RM) $(DESTDIR)$(mandir)/de/man1/inyokaedit.1*
	$(RM) $(DESTDIR)$(dataroot)/icons/hicolor/scalable/apps/org.inyokaproject.inyokaedit.svg
	$(foreach SIZE,$(ICON_SIZES),$(RM) $(DESTDIR)$(dataroot)/icons/hicolor/$(SIZE)x$(SIZE)/apps/org.inyokaproject.inyokaedit.png ;)

clean:
	[ ! -f application/$(MAKEFILE) ] || $(MAKE) -C application -f $(MAKEFILE) clean
	[ ! -f plugins/$(MAKEFILE2) ] || $(MAKE) -C plugins -f $(MAKEFILE2) clean
	$(RM) $(INFILES)
	$(RM) plugins/*.so

distclean: clean
	[ ! -f application/$(MAKEFILE) ] || $(MAKE) -C application -f $(MAKEFILE) distclean
	[ ! -f plugins/$(MAKEFILE2) ] || $(MAKE) -C plugins -f $(MAKEFILE2) distclean
	$(RM) config.mak
