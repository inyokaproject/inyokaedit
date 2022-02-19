#  This file is part of InyokaEdit.
#  Copyright (C) 2012-2022 The InyokaEdit developers
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

TEMPLATE      = lib
CONFIG       += plugin
TARGET        = uu_tabletemplate
DESTDIR       = ../

VERSION       = 1.4.1
QMAKE_TARGET_DESCRIPTION = "ubuntuusers.de table template plugin for InyokaEdit"
QMAKE_TARGET_COPYRIGHT   = "(C) 2012-2022 - Christian Schärf, Thorsten Roth"

DEFINES      += PLUGIN_NAME=\\\"$$TARGET\\\" \
                PLUGIN_VERSION=\"\\\"$$VERSION\\\"\" \
                PLUGIN_COPY=\"\\\"$$QMAKE_TARGET_COPYRIGHT\\\"\"

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += network
CONFIG       += c++11
DEFINES      += QT_NO_FOREACH

CONFIG(debug, debug|release) {
  CONFIG     += warn_on
  DEFINES    += QT_DEPRECATED_WARNINGS
  DEFINES    += QT_DISABLE_DEPRECATED_BEFORE=0x060000
}

isEmpty(PREVIEW) {
  qtHaveModule(webenginewidgets) {
    QT         += webenginewidgets
    DEFINES    += USEQTWEBENGINE
    message("Qt webenginewidgetswebkitwidgets module found!")
  } else {
    qtHaveModule(webkitwidgets) {
      QT       += webkitwidgets
      DEFINES  += USEQTWEBKIT
      message("Qt webkitwidgets module found!")
    } else {
      DEFINES  += NOPREVIEW
      message("Neither QtWebKit nor QtWebEngine installation found!")
      message("Building without included preview.")
    }
  }
} else {
  message("Preview set to \"$$PREVIEW\"")
  equals(PREVIEW, "useqtwebkit") {
    QT         += webkitwidgets
    DEFINES    += USEQTWEBKIT
  } else {
    equals(PREVIEW, "useqtwebengine") {
      QT         += webenginewidgets
      DEFINES    += USEQTWEBENGINE
    } else {
      DEFINES  += NOPREVIEW
      message("Building without included preview.")
    }
  }
}

include(../../application/templates/templates.pri)
include(../../application/parser/parser.pri)

HEADERS      += uu_tabletemplate.h \
                ../../application/syntaxcheck.h \
                ../../application/utils.h

SOURCES      += uu_tabletemplate.cpp \
                ../../application/syntaxcheck.cpp \
                ../../application/utils.cpp

FORMS        += uu_tabletemplate.ui

RESOURCES     = res/uu_tabletemplate_resources.qrc \
                res/translations.qrc

TRANSLATIONS += lang/uu_tabletemplate_de.ts \
                lang/uu_tabletemplate_en.ts \
                lang/uu_tabletemplate_nl.ts
