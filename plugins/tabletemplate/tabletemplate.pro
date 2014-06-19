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
TARGET        = tabletemplate
DESTDIR       = ../
DEPENDPATH   += ../../libs/templates \
                ../../libs/parser
INCLUDEPATH  += ../../libs/templates \
                ../../libs/parser

VERSION       = 1.0.0
QMAKE_TARGET_DESCRIPTION = "Table template plugin for InyokaEdit"
QMAKE_TARGET_COPYRIGHT   = "(C) 2011-2014 The InyokaEdit developers"

DEFINES      += PLUGIN_NAME=\\\"$$TARGET\\\" \
                PLUGIN_VERSION=\"\\\"$$VERSION\\\"\"

LIBS          += -L../../libs -ltemplates \
                 -L../../libs -lparser

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += xml network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS      += CTableTemplate.h

SOURCES      += CTableTemplate.cpp

FORMS        += CTableTemplate.ui

RESOURCES     = res/tabletemplate_resources.qrc

TRANSLATIONS += lang/tabletemplate_de.ts

unix {
    !debug: QMAKE_RPATHDIR += /usr/lib/inyokaedit
    lang.path = /usr/share/inyokaedit/lang
    lang.files += lang/*.qm
    target.path = /usr/lib/inyokaedit/plugins
    INSTALLS += lang \
        target
}
