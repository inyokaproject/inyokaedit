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

TEMPLATE       = lib
CONFIG        += staticlib
DEPENDPATH    += .
INCLUDEPATH   += .

MOC_DIR       = ./.moc
OBJECTS_DIR   = ./.objs
UI_DIR        = ./.ui
RCC_DIR       = ./.rcc

QT           += core xml network

HEADERS      += CParser.h \
                CParseImgMap.h \
                CParseLinks.h \
                CParseTemplates.h \
                CProvisionalTplParser.h

SOURCES      += CParser.cpp \
                CParseImgMap.cpp \
                CParseLinks.cpp \
                CParseTemplates.cpp \
                CProvisionalTplParser.cpp
