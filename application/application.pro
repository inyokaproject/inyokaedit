#  This file is part of InyokaEdit.
#  Copyright (C) 2011-2020 The InyokaEdit developers
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
#  along with InyokaEdit.  If not, see <https://www.gnu.org/licenses/>.

TEMPLATE      = app

unix: !macx {
    TARGET    = inyokaedit
} else {
    TARGET    = InyokaEdit
}

DESTDIR       = ../

VERSION       = 0.25.0
QMAKE_TARGET_PRODUCT     = "InyokaEdit"
QMAKE_TARGET_DESCRIPTION = "Editor for Inyoka based portals"
QMAKE_TARGET_COPYRIGHT   = "(C) 2011-2020 The InyokaEdit developers"

DEFINES      += APP_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\" \
                APP_VERSION=\"\\\"$$VERSION\\\"\" \
                APP_COPY=\"\\\"$$QMAKE_TARGET_COPYRIGHT\\\"\" \
                APP_DESC=\"\\\"$$QMAKE_TARGET_DESCRIPTION\\\"\"

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += core gui widgets network printsupport xml
CONFIG       += c++11

CONFIG(debug, debug|release) {
  CONFIG     += warn_on
  DEFINES    += QT_DEPRECATED_WARNINGS
  DEFINES    += QT_DISABLE_DEPRECATED_BEFORE=0x051500
}

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
                session.h \
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
                findreplace.cpp \
                plugins.cpp \
                texteditor.cpp \
                session.cpp \
                settings.cpp \
                settingsdialog.cpp \
                syntaxcheck.cpp \
                upload.cpp \
                xmlparser.cpp \
                utils.cpp

FORMS        += inyokaedit.ui \
                findreplace.ui \
                settingsdialog.ui

RESOURCES     = res/inyokaedit_resources.qrc \
                res/translations.qrc
win32:RC_FILE = res/inyokaedit_win.rc

macx {
    ICON             = res/images/icon.icns
    QMAKE_INFO_PLIST = res/Info.plist
}

TRANSLATIONS += lang/inyokaedit_de.ts \
                lang/inyokaedit_nl.ts
