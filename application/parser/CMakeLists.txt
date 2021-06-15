#  This file is part of InyokaEdit.
#  Copyright (C) 2011-2021 The InyokaEdit developers
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

string(TOLOWER ${PROJECT_NAME} target_name)

set(HEADERS
    parser.h
    macros.h
    parseimgmap.h
    parselinks.h
    parselist.h
    parsetable.h
    parsetemplates.h
    parsetextformats.h
    parsetxtmap.h
    provisionaltplparser.h
)

set(SOURCES
    parser.cpp
    macros.cpp
    parseimgmap.cpp
    parselinks.cpp
    parselist.cpp
    parsetable.cpp
    parsetemplates.cpp
    parsetextformats.cpp
    parsetxtmap.cpp
    provisionaltplparser.cpp
)

target_sources(${target_name} PRIVATE ${HEADERS} ${SOURCES})
