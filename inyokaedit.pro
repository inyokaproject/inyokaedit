#  This file is part of InyokaEdit.
#  Copyright (C) 2011-2013 The InyokaEdit developers
#  
#  InyokaEdit is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  InyokaEdit is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.

TEMPLATE       = app
DEPENDPATH    += ./src
INCLUDEPATH   += ./src

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += core gui webkit xml network

CODECFORSRC   = UTF-8
CODECFORTR    = UTF-8

HEADERS      += src/CInyokaEdit.h \
                src/CDownload.h \
                src/CDownloadImg.h \
                src/CFileOperations.h \
                src/CFindReplace.h \
                src/CHighlighter.h \
                src/CParser.h \
                src/CParseImgMap.h \
                src/CParseLinks.h \
                src/CParseTemplates.h \
                src/CProgressDialog.h \
                src/CProvisionalTplParser.h \
                src/CTableTemplate.h \
                src/CTemplates.h \
                src/CTextEditor.h \
                src/CSettings.h \
                src/CSettingsDialog.h \
                src/CXmlParser.h

SOURCES      += src/main.cpp \
                src/CInyokaEdit.cpp \
                src/CDownload.cpp \
                src/CDownloadImg.cpp \
                src/CFileOperations.cpp \
                src/CFindReplace.cpp  \
                src/CHighlighter.cpp \
                src/CParser.cpp \
                src/CParseImgMap.cpp \
                src/CParseLinks.cpp \
                src/CParseTemplates.cpp \
                src/CProgressDialog.cpp \
                src/CProvisionalTplParser.cpp \
                src/CTableTemplate.cpp \
                src/CTemplates.cpp \
                src/CTextEditor.cpp \
                src/CSettings.cpp \
                src/CSettingsDialog.cpp \
                src/CXmlParser.cpp

RESOURCES     = res/inyokaedit_resources.qrc

FORMS        += src/CInyokaEdit.ui \
                src/CFindReplace.ui \
                src/CProgressDialog.ui \
                src/CSettingsDialog.ui \
                src/CSpellCheckDialog.ui \
                src/CTableTemplate.ui

TRANSLATIONS += lang/inyokaedit_de.ts

defineTest(pkgconfigcheck) { 
    isEmpty(2):PKG_STR = "Checking for $${1}..."
    else:PKG_STR = "Checking $$1 is at least version $${2}..."
    system(pkg-config --exists $$1) { 
        !isEmpty(2):!system(pkg-config $$1 --atleast-version $$2) { 
            message("$$PKG_STR no")
            return(false)
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
    !DISABLE_SPELLCHECKER:!pkgconfigcheck(hunspell) { 
        CONFIG += DISABLE_SPELLCHECKER
        warning("spellchecker disabled")
    }
    data.path = /usr/share/inyokaedit
    data.files += iWikiLinks \
        templates \
        GetInyokaStyles
    lang.path = /usr/share/inyokaedit/lang
    lang.files += lang/*.qm
    desktop.path = /usr/share/applications
    desktop.files += inyokaedit.desktop
    pixmap.path = /usr/share/pixmaps
    pixmap.files += res/images/inyokaedit_64x64.png
    man.path = /usr/share
    man.files += man
    mime.path = /usr/share/mime/packages
    mime.files += inyokaedit.xml
    target.path = /usr/bin
    INSTALLS += data \
        lang \
        desktop \
        pixmap \
        man \
        mime \
        target
}

win32 { 
    !DISABLE_SPELLCHECKER { 
        exists(windows_files/hunspell-mingw/bin/libhunspell.dll) { 
            LIBS += $$PWD/windows_files/hunspell-mingw/bin/libhunspell.dll
            message("Checking for hunspell... ok")
        }
        else { 
            message("Checking for hunspell... no")
            warning("spellchecker disabled")
            CONFIG += DISABLE_SPELLCHECKER
        }
    }
    RC_FILE = res/inyokaedit.rc
}

!DISABLE_SPELLCHECKER { 
    HEADERS += src/CSpellChecker.h \
               src/CSpellCheckDialog.h
    SOURCES += src/CSpellChecker.cpp \
               src/CSpellCheckDialog.cpp
}
DISABLE_SPELLCHECKER:DEFINES += DISABLE_SPELLCHECKER
