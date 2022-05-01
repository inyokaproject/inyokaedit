#  This file is part of InyokaEdit.
#  Copyright (C) 2014-2022 The InyokaEdit developers
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
TARGET        = highlighter
DESTDIR       = ../

VERSION       = 1.3.2
QMAKE_TARGET_DESCRIPTION = "Syntax highlighter plugin for InyokaEdit"
QMAKE_TARGET_COPYRIGHT   = "(C) 2014-2022 - Thorsten Roth"

DEFINES      += PLUGIN_NAME=\\\"$$TARGET\\\" \
                PLUGIN_VERSION=\"\\\"$$VERSION\\\"\" \
                PLUGIN_COPY=\"\\\"$$QMAKE_TARGET_COPYRIGHT\\\"\"

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

include(../../application/templates/templates.pri)

CONFIG       += c++11
DEFINES      += QT_NO_FOREACH

CONFIG(debug, debug|release) {
  CONFIG     += warn_on
  DEFINES    += QT_DEPRECATED_WARNINGS
  DEFINES    += QT_DISABLE_DEPRECATED_BEFORE=0x060000
}

HEADERS      += highlighter.h \
                syntaxhighlighter.h

SOURCES      += highlighter.cpp \
                syntaxhighlighter.cpp

FORMS        += highlighter.ui

RESOURCES     = res/highlighter_resources.qrc \
                res/translations.qrc

TRANSLATIONS += lang/highlighter_de.ts \
                lang/highlighter_en.ts \
                lang/highlighter_nl.ts
