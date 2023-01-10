#  This file is part of InyokaEdit.
#  Copyright (C) 2011-present The InyokaEdit developers
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

INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD

HEADERS     += $$PWD/parser.h \
               $$PWD/macros.h \
               $$PWD/parseimgmap.h \
               $$PWD/parselinks.h \
               $$PWD/parselist.h \
               $$PWD/parsetable.h \
               $$PWD/parsetemplates.h \
               $$PWD/parsetextformats.h \
               $$PWD/parsetxtmap.h \
               $$PWD/provisionaltplparser.h

SOURCES     += $$PWD/parser.cpp \
               $$PWD/macros.cpp \
               $$PWD/parseimgmap.cpp \
               $$PWD/parselinks.cpp \
               $$PWD/parselist.cpp \
               $$PWD/parsetable.cpp \
               $$PWD/parsetemplates.cpp \
               $$PWD/parsetextformats.cpp \
               $$PWD/parsetxtmap.cpp \
               $$PWD/provisionaltplparser.cpp
