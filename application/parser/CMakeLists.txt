# SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
# SPDX-License-Identifier: GPL-3.0-or-later

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
