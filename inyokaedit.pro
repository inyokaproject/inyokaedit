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
                 xml

 FORMS        += CInyokaEdit.ui \
                 qtfindreplacedialog/findreplaceform.ui \
                 qtfindreplacedialog/findreplacedialog.ui \
                 CProgressDialog.ui \
                 CSpellCheckDialog.ui

 CODECFORSRC   = UTF-8
 CODECFORTR    = UTF-8

defineTest(pkgconfigcheck) {
  isEmpty(2) {
    PKG_STR="Checking for $${1}..."
  } else {
    PKG_STR="Checking $$1 is at least version $${2}..."
  }

  system(pkg-config --exists $$1) {
    !isEmpty(2) {
      !system(pkg-config $$1 --atleast-version $$2) {
        message("$$PKG_STR no")
        return(false)
      }
    }
  CONFIG += link_pkgconfig
    PKGCONFIG += $$1
    export(CONFIG)
    export(PKGCONFIG)
    message("$$PKG_STR ok")
    return(true)
  }
  message("$$PKG_STR no")
  return(false)
}

unix {
  !DISABLE_SPELLCHECKER {
    !pkgconfigcheck(hunspell) {
      CONFIG += DISABLE_SPELLCHECKER
      warning("spellchecker disabled")
    }
  }

  !DISABLE_WEBVIEW {
    !pkgconfigcheck(QtWebKit, 4.0) {
      CONFIG += DISABLE_WEBVIEW
      warning("webview disabled")
    }
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
}

!DISABLE_WEBVIEW {
 QT           += webkit
}

DISABLE_WEBVIEW {
 DEFINES      += DISABLE_WEBVIEW
}

 TRANSLATIONS += lang/inyokaedit_de.ts
