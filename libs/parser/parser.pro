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

TEMPLATE     = lib
DESTDIR      = ../
DEFINES     += PARSER_LIBRARY
DEPENDPATH  += ../templates
INCLUDEPATH += ../templates

VERSION      = 1.0.1
QMAKE_TARGET_DESCRIPTION = "Parser library for InyokaEdit"
QMAKE_TARGET_COPYRIGHT   = "(C) 2011-2014 The InyokaEdit developers"

MOC_DIR      = ./.moc
OBJECTS_DIR  = ./.objs
UI_DIR       = ./.ui
RCC_DIR      = ./.rcc

QT          += core xml network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS     += CParser.h \
               CParseImgMap.h \
               CParseLinks.h \
               CParseTemplates.h \
               CProvisionalTplParser.h

SOURCES     += CParser.cpp \
               CParseImgMap.cpp \
               CParseLinks.cpp \
               CParseTemplates.cpp \
               CProvisionalTplParser.cpp

win32 {
    LIBS     += $$OUT_PWD/../templates1.dll
}

unix {
    QMAKE_RPATHDIR += /usr/lib/inyokaedit   # Comment for debugging

    LIBS           += -L../ -ltemplates

    target.path = /usr/lib/inyokaedit
    INSTALLS += target
}
