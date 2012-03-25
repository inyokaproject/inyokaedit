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
                 CParseLinks.h \
                 CSpellChecker.h \
                 CSpellCheckDialog.h

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
                 CParseLinks.cpp \
                 CSpellChecker.cpp \
                 CSpellCheckDialog.cpp

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
 LIBS         += -lhunspell

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
 LIBS         += $$PWD/windows_files/hunspell-mingw/bin/libhunspell.dll
 RC_FILE       = windows_files/inyokaedit.rc
}

 TRANSLATIONS += lang/inyokaedit_de.ts
