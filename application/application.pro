#  This file is part of InyokaEdit.
#  Copyright (C) 2011-2017 The InyokaEdit developers
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

TEMPLATE      = app

unix: !macx {
    TARGET    = inyokaedit
} else {
    TARGET    = InyokaEdit
}

DESTDIR       = ../

VERSION       = 0.19.0
QMAKE_TARGET_PRODUCT     = "InyokaEdit"
QMAKE_TARGET_DESCRIPTION = "Editor for Inyoka based portals"
QMAKE_TARGET_COPYRIGHT   = "(C) 2011-2017 The InyokaEdit developers"

DEFINES      += APP_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\" \
                APP_VERSION=\"\\\"$$VERSION\\\"\" \
                APP_COPY=\"\\\"$$QMAKE_TARGET_COPYRIGHT\\\"\"

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += core gui network
equals(QT_MAJOR_VERSION, 4) {  # Qt < 5
    QT += webkit
} else {  # Qt >= 5
    QT += widgets printsupport

    # Qt < 5.6
    lessThan(QT_MINOR_VERSION, 6): QT += webkitwidgets
    # Qt >= 5.6
    greaterThan(QT_MINOR_VERSION, 5): QT += webenginewidgets
}

include(templates/templates.pri)
include(parser/parser.pri)

HEADERS      += CInyokaEdit.h \
                CDownload.h \
                CDownloadImg.h \
                CFileOperations.h \
                CFindReplace.h \
                CPlugins.h \
                CProgressDialog.h \
                CTextEditor.h \
                CSettings.h \
                CSettingsDialog.h \
                CUpload.h \
                CUtils.h \
                IEditorPlugin.h

SOURCES      += main.cpp \
                CInyokaEdit.cpp \
                CDownload.cpp \
                CDownloadImg.cpp \
                CFileOperations.cpp \
                CFindReplace.cpp  \
                CPlugins.cpp \
                CProgressDialog.cpp \
                CTextEditor.cpp \
                CSettings.cpp \
                CSettingsDialog.cpp \
                CUpload.cpp \
                CUtils.cpp

FORMS        += CInyokaEdit.ui \
                CFindReplace.ui \
                CProgressDialog.ui \
                CSettingsDialog.ui

RESOURCES     = res/inyokaedit_resources.qrc
win32:RC_FILE = res/inyokaedit.rc

TRANSLATIONS += lang/inyokaedit_de.ts
