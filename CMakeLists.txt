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

cmake_minimum_required(VERSION 3.14)
project(InyokaEdit
    VERSION 0.26.0
    DESCRIPTION "Editor for Inyoka based portals"
    HOMEPAGE_URL "https://github.com/inyokaproject/inyokaedit"
    LANGUAGES CXX
)
set(PROJECT_COPYRIGHT "(C) 2011-2021 The InyokaEdit developers")

option(QT6 "Use Qt6" OFF)

if (QT6)
    set(QT_MAIN_VERSION Qt6)
    set(QT_MINIMUM_VERSION 6.0)
else()
    set(QT_MAIN_VERSION Qt5)
    set(QT_MINIMUM_VERSION 5.9)
    if(NOT COMMAND qt_add_translation)
        function(qt_add_translation _qm_files)
            qt5_add_translation("${_qm_files}" ${ARGN})
            set("${_qm_files}" "${${_qm_files}}" PARENT_SCOPE)
        endfunction()
    endif()
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(cmake/preview-deps.cmake)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(LINUX ON)
endif()


add_subdirectory(plugins)
add_subdirectory(application)

if(LINUX)
    add_subdirectory(man)

    if(COMMUNITY STREQUAL "community/ubuntuusers_de")
        include(GNUInstallDirs)
        install(DIRECTORY community/ubuntuusers_de DESTINATION ${CMAKE_INSTALL_DATADIR}/inyokaedit/community)
    endif()
endif()
