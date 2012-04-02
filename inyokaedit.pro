 HEADERS      += CInyokaEdit.h \
                 CHighlighter.h \
                 CTextEditor.h \
                 CInsertSyntaxElement.h \
                 qtfindreplacedialog/findreplaceform.h \
                 qtfindreplacedialog/findreplacedialog.h \
                 qtfindreplacedialog/findform.h \
                 qtfindreplacedialog/finddialog.h \
                 CProgressDialog.h \
                 CSettings.h \
                 CInterwiki.h \
                 CParser.h \
                 CDownload.h \
                 CFileOperations.h \
                 CParseLinks.h

 SOURCES      += main.cpp \
                 CInyokaEdit.cpp \
                 CHighlighter.cpp \
                 CTextEditor.cpp \
                 CInsertSyntaxElement.cpp \
                 qtfindreplacedialog/findreplaceform.cpp \
                 qtfindreplacedialog/findreplacedialog.cpp \
                 qtfindreplacedialog/findform.cpp \
                 qtfindreplacedialog/finddialog.cpp \
                 CProgressDialog.cpp \
                 CSettings.cpp \
                 CInterwiki.cpp \
                 CParser.cpp \
                 CDownload.cpp \
                 CFileOperations.cpp \
                 CParseLinks.cpp

 RESOURCES     = inyokaedit_resources.qrc

 TEMPLATE      = app

 QT           += core \
                 gui \
                 webkit \
                 xml

 FORMS        += CInyokaEdit.ui \
                 qtfindreplacedialog/findreplaceform.ui \
                 qtfindreplacedialog/findreplacedialog.ui \
                 CProgressDialog.ui \
                 CSpellCheckDialog.ui

 CODECFORSRC   = UTF-8
 CODECFORTR    = UTF-8

unix {
  !DISABLE_SPELLCHECKER {
    system(pkg-config --exists hunspell) {
      LIBS         += -lhunspell
      message("Checking for hunspell... ok")
    } else {
    system(pkg-config --exists hunspell-1.3) {
      LIBS         += -lhunspell-1.3
      message("Checking for hunspell-1.3... ok")
    } else {
    system(pkg-config --exists hunspell-1.2) {
      LIBS         += -lhunspell-1.2
      message("Checking for hunspell-1.2... ok")
    } else {
    message("Checking for hunspell... no")
    warning("spellchecker disabled")
    CONFIG += DISABLE_SPELLCHECKER
    }}}
  }

data.path      = /usr/share/inyokaedit
data.files    += iWikiLinks GetInyokaStyles
lang.path      = /usr/share/inyokaedit/lang
lang.files    += lang/*.qm
desktop.path   = /usr/share/applications
desktop.files += inyokaedit.desktop
pixmap.path    = /usr/share/pixmaps
pixmap.files  += images/inyokaedit_64x64.png
man.path       = /usr/share
man.files     += man
target.path    = /usr/bin
 INSTALLS     += data lang desktop pixmap man target
}

win32 {
  !DISABLE_SPELLCHECKER {
    exists(windows_files/hunspell-mingw/bin/libhunspell.dll) {
      LIBS         += $$PWD/windows_files/hunspell-mingw/bin/libhunspell.dll
      message("Checking for hunspell... ok")
    } else {
    message("Checking for hunspell... no")
    warning("spellchecker disabled")
    CONFIG += DISABLE_SPELLCHECKER
    }
  }
 RC_FILE       = windows_files/inyokaedit.rc
}

!DISABLE_SPELLCHECKER {
 HEADERS      += CSpellChecker.h \
                 CSpellCheckDialog.h

 SOURCES      += CSpellChecker.cpp \
                 CSpellCheckDialog.cpp
}

DISABLE_SPELLCHECKER {
 DEFINES      += DISABLE_SPELLCHECKER

 unix {
  disablespell.commands += patch -p1 <disablespell.patch
  QMAKE_EXTRA_TARGETS += disablespell
  PRE_TARGETDEPS += disablespell
  QMAKE_POST_LINK += patch -Rp1 <disablespell.patch
 }
}

 TRANSLATIONS += lang/inyokaedit_de.ts
