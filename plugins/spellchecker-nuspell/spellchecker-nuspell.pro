#  This file is part of InyokaEdit.
#  Copyright (C) 2014-present The InyokaEdit developers
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
TARGET        = spellchecker-nuspell
DESTDIR       = ../

VERSION       = 1.1.1
QMAKE_TARGET_DESCRIPTION = "Nuspell based spell checker plugin for InyokaEdit"
QMAKE_TARGET_COPYRIGHT   = "(C) 2011 Volker G&ouml;tz, 2014-present - Thorsten Roth"

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
  DEFINES    += QT_DISABLE_DEPRECATED_BEFORE=0x060900
}

HEADERS      += nuspellcheckdialog.h \
                spellchecker-nuspell.h

SOURCES      += nuspellcheckdialog.cpp \
                spellchecker-nuspell.cpp

FORMS        += nuspellcheckdialog.ui

RESOURCES     = res/spellchecker-nuspell_resources.qrc \
                res/translations.qrc

TRANSLATIONS += lang/spellchecker-nuspell_de.ts \
                lang/spellchecker-nuspell_en.ts \
                lang/spellchecker-nuspell_nl.ts

unix:LIBS    += -lnuspell

win32 {
 equals(QT_ARCH, "x86_64") {
  INCLUDEPATH += c:/tools/vcpkg/installed/x64-windows/include
  LIBS        += -L"c:/tools/vcpkg/installed/x64-windows/lib" -lnuspell
 } else {
  INCLUDEPATH += c:/tools/vcpkg/installed/x86-windows/include
  LIBS        += -L"c:/tools/vcpkg/installed/x86-windows/lib" -lnuspell
 }
}
