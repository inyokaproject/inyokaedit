#  This file is part of InyokaEdit.
#  Copyright (C) 2014-2021 The InyokaEdit developers
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
#
#
#  This file incorporates work covered by the following copyright and
#  permission notice:
#
#  Copyright (c) 2011, Volker Götz
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#
#  1. Redistributions of source code must retain the above copyright notice, this
#     list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
#  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

TEMPLATE      = lib
CONFIG       += plugin
TARGET        = spellchecker
DESTDIR       = ../

VERSION       = 1.2.3
QMAKE_TARGET_DESCRIPTION = "Hunspell based spell checker plugin for InyokaEdit"
QMAKE_TARGET_COPYRIGHT   = "(C) 2011 Volker Götz, 2014-2021 - Thorsten Roth"

DEFINES      += PLUGIN_NAME=\\\"$$TARGET\\\" \
                PLUGIN_VERSION=\"\\\"$$VERSION\\\"\" \
                PLUGIN_COPY=\"\\\"$$QMAKE_TARGET_COPYRIGHT\\\"\"

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += widgets
greaterThan(QT_MAJOR_VERSION, 5) {
  QT         += core5compat
}
CONFIG       += c++11
DEFINES      += QT_NO_FOREACH

CONFIG(debug, debug|release) {
  CONFIG     += warn_on
  DEFINES    += QT_DEPRECATED_WARNINGS
  DEFINES    += QT_DISABLE_DEPRECATED_BEFORE=0x060000
}

HEADERS      += spellcheckdialog.h \
                spellchecker.h

SOURCES      += spellcheckdialog.cpp \
                spellchecker.cpp

FORMS        += spellcheckdialog.ui

RESOURCES     = res/spellchecker_resources.qrc \
                res/translations.qrc

TRANSLATIONS += lang/spellchecker_de.ts \
                lang/spellchecker_en.ts \
                lang/spellchecker_nl.ts

win32:LIBS   += $$PWD/windows_files/hunspell-mingw/bin/libhunspell.dll
unix:LIBS    += -lhunspell
