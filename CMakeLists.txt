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

cmake_minimum_required(VERSION 3.14)
project(InyokaEdit
    VERSION 0.28.1
    DESCRIPTION "Editor for Inyoka based portals"
    HOMEPAGE_URL "https://github.com/inyokaproject/inyokaedit"
    LANGUAGES C CXX
)
set(PROJECT_COPYRIGHT "(C) 2011-present The InyokaEdit developers")

option(BUILD_APPLICATION "Build main application" ON)
option(BUILD_COMMUNITY_ONLY "Build community package only" OFF)

set(QT_MAIN_VERSION Qt6)
set(QT_MINIMUM_VERSION 6.0)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(LINUX ON)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(WINDOWS ON)
endif()

if (NOT BUILD_COMMUNITY_ONLY)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
    include(cmake/preview-deps.cmake)

    add_subdirectory(plugins)
    if (BUILD_APPLICATION)
        add_subdirectory(application)
    endif()
endif()

if(LINUX)
    if (BUILD_APPLICATION)
        add_subdirectory(man)
    endif()        

    if(COMMUNITY STREQUAL "community/ubuntuusers_de")
        include(GNUInstallDirs)
        install(DIRECTORY community/ubuntuusers_de DESTINATION ${CMAKE_INSTALL_DATADIR}/inyokaedit/community)
    endif()
endif()
