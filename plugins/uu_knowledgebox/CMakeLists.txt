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

project(uu_knowledgebox
    VERSION 1.3.0
    DESCRIPTION "ubuntuusers.de knowledge box selection plugin for InyokaEdit"
    HOMEPAGE_URL "https://github.com/inyokaproject/inyokaedit"
)
set(PROJECT_COPYRIGHT "(C) 2013-present - Thorsten Roth")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(${QT_MAIN_VERSION} ${QT_MINIMUM_VERSION}
    REQUIRED Core Widgets LinguistTools
)

add_library(uu_knowledgebox SHARED)

set_target_properties(uu_knowledgebox
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
)

target_compile_features(uu_knowledgebox PUBLIC cxx_std_17)
target_compile_definitions(uu_knowledgebox
    PRIVATE
        PLUGIN_NAME="${PROJECT_NAME}"
        PLUGIN_VERSION="${PROJECT_VERSION}"
        PLUGIN_COPY="${PROJECT_COPYRIGHT}"
        QT_NO_FOREACH
        $<$<CONFIG:DEBUG>:QT_DISABLE_DEPRECATED_BEFORE=0x060900>
)

target_link_libraries(uu_knowledgebox
    PRIVATE
        ${QT_MAIN_VERSION}::Core
        ${QT_MAIN_VERSION}::Widgets
)

set(HEADERS
    uu_knowledgebox.h
)

set(SOURCES
    uu_knowledgebox.cpp
)

set(FORMS
    uu_knowledgebox.ui
)

set(RESOURCES
    res/uu_knowledgebox_resources.qrc
)

set(TRANSLATIONS
    lang/uu_knowledgebox_de.ts
    lang/uu_knowledgebox_en.ts
    lang/uu_knowledgebox_nl.ts
)

if (GENERATE_TS)
    qt6_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/plugins/uu_knowledgebox/ ${TRANSLATIONS} OPTIONS -no-obsolete)
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
qt_add_resources(uu_knowledgebox translations_uu_knowledge
    PREFIX "/"
    FILES ${QM_FILES}
)

target_sources(uu_knowledgebox PRIVATE ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})


if(LINUX)
    include(GNUInstallDirs)
    install(TARGETS uu_knowledgebox DESTINATION ${CMAKE_INSTALL_LIBDIR}/inyokaedit/plugins)
endif()
