#  This file is part of InyokaEdit.
#  Copyright (C) 2014-2019 The InyokaEdit developers
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

TEMPLATE      = lib
CONFIG       += plugin
TARGET        = spellchecker
DESTDIR       = ../

VERSION       = 1.2.0
QMAKE_TARGET_DESCRIPTION = "Hunspell based spell checker plugin for InyokaEdit"
QMAKE_TARGET_COPYRIGHT   = "(C) 2014-2019 - Thorsten Roth"

DEFINES      += PLUGIN_NAME=\\\"$$TARGET\\\" \
                PLUGIN_VERSION=\"\\\"$$VERSION\\\"\" \
                PLUGIN_COPY=\"\\\"$$QMAKE_TARGET_COPYRIGHT\\\"\"

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += widgets

CONFIG       += warn_on c++11
DEFINES      += QT_DEPRECATED_WARNINGS
DEFINES      += QT_DISABLE_DEPRECATED_BEFORE=0x050900

HEADERS      += spellcheckdialog.h \
                spellchecker.h

SOURCES      += spellcheckdialog.cpp \
                spellchecker.cpp

FORMS        += spellcheckdialog.ui

RESOURCES     = res/spellchecker_resources.qrc \
                res/translations.qrc

TRANSLATIONS += lang/spellchecker_de.ts \
                lang/spellchecker_nl.ts

win32:LIBS   += $$PWD/windows_files/hunspell-mingw/bin/libhunspell.dll
unix:LIBS    += -lhunspell

# Workaround for automatic build on Launchpad and new Hunspell version (>= v1.5)
# E.g. Ubuntu 16.04 (and before) using Qt <= v5.5.x with old Hunspell v1.3
equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 5) {
  DEFINES    += USE_NEW_HUNSPELL
}
