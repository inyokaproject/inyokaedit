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

project(highlighter
    VERSION 1.4.2
    DESCRIPTION "Syntax highlighter plugin for InyokaEdit"
    HOMEPAGE_URL "https://github.com/inyokaproject/inyokaedit"
)
set(PROJECT_COPYRIGHT "(C) 2014-present - Thorsten Roth")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(${QT_MAIN_VERSION} ${QT_MINIMUM_VERSION}
    REQUIRED Core Widgets LinguistTools
)

add_library(highlighter SHARED)

set_target_properties(highlighter
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
)

target_compile_features(highlighter PUBLIC cxx_std_17)
target_compile_definitions(highlighter
    PRIVATE
        PLUGIN_NAME="${PROJECT_NAME}"
        PLUGIN_VERSION="${PROJECT_VERSION}"
        PLUGIN_COPY="${PROJECT_COPYRIGHT}"
        QT_NO_FOREACH
        $<$<CONFIG:DEBUG>:QT_DISABLE_DEPRECATED_BEFORE=0x060900>
)

target_link_libraries(highlighter
    PRIVATE
        ${QT_MAIN_VERSION}::Core
        ${QT_MAIN_VERSION}::Widgets
)

add_subdirectory(../../application/templates ${CMAKE_CURRENT_BINARY_DIR}/templates)

set(HEADERS
    highlighter.h
    syntaxhighlighter.h
)

set(SOURCES
    highlighter.cpp
    syntaxhighlighter.cpp
)

set(FORMS
    highlighter.ui
)

set(RESOURCES
    res/highlighter_resources.qrc
)

set(TRANSLATIONS
    lang/highlighter_de.ts
    lang/highlighter_en.ts
    lang/highlighter_nl.ts
)

if (GENERATE_TS)
    qt6_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/plugins/highlighter/ ${TRANSLATIONS} OPTIONS -no-obsolete)
else ()
    qt6_add_translation(QM_FILES ${TRANSLATIONS})
endif ()
add_custom_target(translations_${PROJECT_NAME} ALL DEPENDS ${QM_FILES})
foreach(qm_file ${QM_FILES})
    get_filename_component(qm_filename "${qm_file}" NAME)
    set_source_files_properties(
        "${qm_file}"
        PROPERTIES QT_RESOURCE_ALIAS "${qm_filename}"
    )
endforeach()
qt_add_resources(highlighter translations_highlighter
    PREFIX "/"
    FILES ${QM_FILES}
)

target_sources(highlighter PRIVATE ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})


if(LINUX)
    include(GNUInstallDirs)
    install(TARGETS highlighter DESTINATION ${CMAKE_INSTALL_LIBDIR}/inyokaedit/plugins)
endif()
