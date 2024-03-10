#  This file is part of InyokaEdit.
#  Copyright (C) 2015-present The InyokaEdit developers
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
TARGET        = hotkey
DESTDIR       = ../

VERSION       = 1.3.0
QMAKE_TARGET_DESCRIPTION = "Hotkey plugin for InyokaEdit"
QMAKE_TARGET_COPYRIGHT   = "(C) 2015-present - Thorsten Roth"

DEFINES      += PLUGIN_NAME=\\\"$$TARGET\\\" \
                PLUGIN_VERSION=\"\\\"$$VERSION\\\"\" \
                PLUGIN_COPY=\"\\\"$$QMAKE_TARGET_COPYRIGHT\\\"\"

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += widgets
CONFIG       += c++17
DEFINES      += QT_NO_FOREACH

CONFIG(debug, debug|release) {
  CONFIG     += warn_on
  DEFINES    += QT_DISABLE_DEPRECATED_BEFORE=0x060600
}

HEADERS      += hotkey.h

SOURCES      += hotkey.cpp

FORMS        += hotkey.ui

RESOURCES     = res/hotkey_resources.qrc \
                res/translations.qrc

TRANSLATIONS += lang/hotkey_de.ts \
                lang/hotkey_en.ts \
                lang/hotkey_nl.ts
