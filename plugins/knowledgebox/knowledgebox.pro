#  This file is part of InyokaEdit.
#  Copyright (C) 2011-2014 The InyokaEdit developers
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
TARGET        = knowledgebox
DESTDIR       = ../

VERSION       = 1.0.1
QMAKE_TARGET_DESCRIPTION = "Knowledge box selection plugin for InyokaEdit"
QMAKE_TARGET_COPYRIGHT   = "(C) 2011-2014 The InyokaEdit developers"

DEFINES      += PLUGIN_NAME=\\\"$$TARGET\\\" \
                PLUGIN_VERSION=\"\\\"$$VERSION\\\"\"

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS      += CKnowledgeBox.h

SOURCES      += CKnowledgeBox.cpp

FORMS        += CKnowledgeBox.ui

RESOURCES     = res/knowledgebox_resources.qrc

TRANSLATIONS += lang/knowledgebox_de.ts

win32 {
    LIBS     += $$OUT_PWD/../../libs/templates1.dll
}

unix {
    LIBS           += -L../../libs -ltemplates
}
