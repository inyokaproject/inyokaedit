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

VERSION       = 0.20.0
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

QT           += core gui widgets network printsupport xml

qtHaveModule(webkitwidgets) {
  QT         += webkitwidgets
  DEFINES    += USEQTWEBKIT
} else {
  qtHaveModule(webenginewidgets) {
    QT       += webenginewidgets
  } else {
    error("Neither QtWebKit nor QtWebEngine installation found!")
  }
}

include(templates/templates.pri)
include(parser/parser.pri)

HEADERS      += inyokaedit.h \
                download.h \
                downloadimg.h \
                fileoperations.h \
                findreplace.h \
                plugins.h \
                texteditor.h \
                settings.h \
                settingsdialog.h \
                syntaxcheck.h \
                upload.h \
                utils.h \
                xmlparser.h \
                ieditorplugin.h

SOURCES      += main.cpp \
                inyokaedit.cpp \
                download.cpp \
                downloadimg.cpp \
                fileoperations.cpp \
                findreplace.cpp  \
                plugins.cpp \
                texteditor.cpp \
                settings.cpp \
                settingsdialog.cpp \
                syntaxcheck.cpp \
                upload.cpp \
                xmlparser.cpp \
                utils.cpp

FORMS        += inyokaedit.ui \
                findreplace.ui \
                settingsdialog.ui

RESOURCES     = res/inyokaedit_resources.qrc
win32:RC_FILE = res/inyokaedit_win.rc

TRANSLATIONS += lang/inyokaedit_de.ts
