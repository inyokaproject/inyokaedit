#  This file is part of InyokaEdit.
#  Copyright (C) 2014-2018 The InyokaEdit developers
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

VERSION       = 1.0.4
QMAKE_TARGET_DESCRIPTION = "Hunspell based spell checker plugin for InyokaEdit"
QMAKE_TARGET_COPYRIGHT   = "(C) 2014-2018 - Thorsten Roth"

DEFINES      += PLUGIN_NAME=\\\"$$TARGET\\\" \
                PLUGIN_VERSION=\"\\\"$$VERSION\\\"\" \
                PLUGIN_COPY=\"\\\"$$QMAKE_TARGET_COPYRIGHT\\\"\"

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += widgets

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
