#  This file is part of InyokaEdit.
#  Copyright (C) 2011-2014 The InyokaEdit developers
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
TARGET         = inyokaedit
DESTDIR        = ../
DEPENDPATH    += ./src
INCLUDEPATH   += ./src

VERSION        = 0.13.0
QMAKE_TARGET_PRODUCT     = "InyokaEdit"
QMAKE_TARGET_DESCRIPTION = "Editor for Inyoka based portals"
QMAKE_TARGET_COPYRIGHT   = "(C) 2011-2014 The InyokaEdit developers"

DEFINES      += APP_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\" \
                APP_VERSION=\"\\\"$$VERSION\\\"\"

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += core gui xml network
lessThan(QT_MAJOR_VERSION, 5): QT += webkit
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport webkitwidgets

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
                # src/CUpload.h \
                src/CUtils.h \
                src/CXmlParser.h \
                src/IEditorPlugin.h

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
                # src/CUpload.cpp \
                src/CUtils.cpp \
                src/CXmlParser.cpp

FORMS        += src/CInyokaEdit.ui \
                src/CFindReplace.ui \
                src/CProgressDialog.ui \
                src/CSettingsDialog.ui \
                src/CTableTemplate.ui

RESOURCES     = res/inyokaedit_resources.qrc

TRANSLATIONS += lang/inyokaedit_de.ts

win32 {
    RC_FILE = res/inyokaedit.rc
}
