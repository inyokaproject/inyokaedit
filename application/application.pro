#  This file is part of InyokaEdit.
#  Copyright (C) 2011-2021 The InyokaEdit developers
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

win32:VERSION = 0.27.0.0
else:VERSION  = 0.27.0

QMAKE_TARGET_PRODUCT     = "InyokaEdit"
QMAKE_TARGET_DESCRIPTION = "Editor for Inyoka based portals"
QMAKE_TARGET_COPYRIGHT   = "(C) 2011-2021 The InyokaEdit developers"

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
DEFINES      += QT_NO_FOREACH

CONFIG(debug, debug|release) {
  CONFIG     += warn_on
  DEFINES    += QT_DEPRECATED_WARNINGS
  DEFINES    += QT_DISABLE_DEPRECATED_BEFORE=0x060000
}

isEmpty(PREVIEW) {
  qtHaveModule(webkitwidgets) {
    QT        += webkitwidgets
    DEFINES   += USEQTWEBKIT
    message("Qt webkitwidgets module found!")
  } else {
    qtHaveModule(webenginewidgets) {
      QT      += webenginewidgets
      DEFINES += USEQTWEBENGINE
      message("Qt webenginewidgets module found!")
    } else {
      DEFINES += NOPREVIEW
      message("Neither QtWebKit nor QtWebEngine installation found!")
      message("Building without included preview.")
    }
  }
} else {
  message("Preview set to \"$$PREVIEW\"")
  equals(PREVIEW, "useqtwebkit") {
    QT        += webkitwidgets
    DEFINES   += USEQTWEBKIT
  } else {
    equals(PREVIEW, "useqtwebengine") {
      QT      += webenginewidgets
      DEFINES += USEQTWEBENGINE
    } else {
      DEFINES += NOPREVIEW
      message("Building without included preview.")
    }
  }
}

include(templates/templates.pri)
include(parser/parser.pri)

HEADERS       += inyokaedit.h \
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

SOURCES       += main.cpp \
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

FORMS         += inyokaedit.ui \
                 findreplace.ui \
                 settingsdialog.ui

RESOURCES      = data/data.qrc \
                 lang/translations.qrc

TRANSLATIONS  += lang/inyokaedit_de.ts \
                 lang/inyokaedit_en.ts \
                 lang/inyokaedit_nl.ts

win32:RC_ICONS = icons/inyokaedit.ico

macx {
  ICON             = icons/icon.icns
  QMAKE_INFO_PLIST = data/mac/Info.plist
}
