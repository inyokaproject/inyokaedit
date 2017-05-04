#  This file is part of InyokaEdit.
#  Copyright (C) 2011-2017 The InyokaEdit developers
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

VERSION       = 1.2.1
QMAKE_TARGET_DESCRIPTION = "Table template plugin for InyokaEdit"
QMAKE_TARGET_COPYRIGHT   = "(C) 2011-2017 The InyokaEdit developers"

DEFINES      += PLUGIN_NAME=\\\"$$TARGET\\\" \
                PLUGIN_VERSION=\"\\\"$$VERSION\\\"\"

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += network
equals(QT_MAJOR_VERSION, 4) {  # Qt < 5
    QT += webkit
} else {  # Qt >= 5
    # Qt < 5.6
    lessThan(QT_MINOR_VERSION, 6): QT += webkitwidgets
    # Qt >= 5.6
    greaterThan(QT_MINOR_VERSION, 5): QT += webenginewidgets
}

include(../../application/templates/templates.pri)
include(../../application/parser/parser.pri)

HEADERS      += CTableTemplate.h

SOURCES      += CTableTemplate.cpp

FORMS        += CTableTemplate.ui

RESOURCES     = res/tabletemplate_resources.qrc

TRANSLATIONS += lang/tabletemplate_de.ts
