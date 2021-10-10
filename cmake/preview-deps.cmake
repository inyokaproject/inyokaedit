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

macro(set_preview_dependencies target_name)
    if(NOT PREVIEW)
        find_package(${QT_MAIN_VERSION} QUIET ${QT_MINIMUM_VERSION} OPTIONAL_COMPONENTS WebEngineWidgets)
        if(Qt5WebKitWidgets_FOUND)
            target_link_libraries(${target_name} PRIVATE ${QT_MAIN_VERSION}::WebEngineWidgets)
            target_compile_definitions(${target_name} PRIVATE USEQTWEBENGINE)
            message(STATUS "Qt webenginewidgets module found!")
        else()
            find_package(${QT_MAIN_VERSION} QUIET ${QT_MINIMUM_VERSION} OPTIONAL_COMPONENTS WebKitWidgets)
            if(Qt5WebEngineWidgets_FOUND)
                target_link_libraries(${target_name} PRIVATE ${QT_MAIN_VERSION}::WebKitWidgets)
                target_compile_definitions(${target_name} PRIVATE USEQTWEBKIT)
                message(STATUS "Qt webkitwidgets module found!")
            else()
                target_compile_definitions(${target_name} PRIVATE NOPREVIEW)
                message(STATUS "Neither QtWebKit nor QtWebEngine installation found!")
                message(STATUS "Building without included preview.")
            endif()
        endif()
    else()
        message(STATUS "Preview set to \"${PREVIEW}\"")
        if(PREVIEW STREQUAL "useqtwebkit")
            find_package(${QT_MAIN_VERSION} ${QT_MINIMUM_VERSION} REQUIRED WebKitWidgets)
            target_link_libraries(${target_name} PRIVATE ${QT_MAIN_VERSION}::WebKitWidgets)
            target_compile_definitions(${target_name} PRIVATE USEQTWEBKIT)
        elseif(PREVIEW STREQUAL "useqtwebengine")
            find_package(${QT_MAIN_VERSION} ${QT_MINIMUM_VERSION} REQUIRED WebEngineWidgets)
            target_link_libraries(${target_name} PRIVATE ${QT_MAIN_VERSION}::WebEngineWidgets)
            target_compile_definitions(${target_name} PRIVATE USEQTWEBENGINE)
        else()
            target_compile_definitions(${target_name} PRIVATE NOPREVIEW)
            message(STATUS "Building without included preview.")
        endif()
    endif()
endmacro()
