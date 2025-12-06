# SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
# SPDX-License-Identifier: GPL-3.0-or-later

string(TOLOWER ${PROJECT_NAME} target_name)

find_package(${QT_MAIN_VERSION} ${QT_MINIMUM_VERSION} REQUIRED Widgets)

set(HEADERS
    templates.h
)

set(SOURCES
    templates.cpp
)

target_sources(${target_name} PRIVATE ${HEADERS} ${SOURCES})
target_link_libraries(${target_name} PRIVATE ${QT_MAIN_VERSION}::Widgets)
