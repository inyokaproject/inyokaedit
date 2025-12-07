# SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.14)
project(InyokaEdit
    VERSION 0.28.4
    DESCRIPTION "Editor for Inyoka based portals"
    HOMEPAGE_URL "https://github.com/inyokaproject/inyokaedit"
    LANGUAGES C CXX
)
set(PROJECT_COPYRIGHT "(C) 2011-present The InyokaEdit developers")

option(INY_BUILD_APPLICATION "Build main application" ON)
option(INY_BUILD_COMMUNITY_ONLY "Build community package only" OFF)
option(GENERATE_TS "Regenerate translation source files" OFF)

set(QT_MAIN_VERSION Qt6)
set(QT_MINIMUM_VERSION 6.2)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(LINUX ON)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(WINDOWS ON)
endif()

if (NOT INY_BUILD_COMMUNITY_ONLY)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
    include(cmake/preview-deps.cmake)

    add_subdirectory(plugins)
    if (INY_BUILD_APPLICATION)
        add_subdirectory(application)
    endif()
endif()

if(LINUX)
    if (INY_BUILD_APPLICATION AND NOT INY_BUILD_COMMUNITY_ONLY)
        add_subdirectory(man)
    endif()        

    if(INY_COMMUNITY STREQUAL "community/ubuntuusers_de")
        message(STATUS "Including community/ubuntuusers_de")
        include(GNUInstallDirs)
        install(DIRECTORY community/ubuntuusers_de DESTINATION ${CMAKE_INSTALL_DATADIR}/inyokaedit/community)
    endif()
endif()
