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

project(spellchecker-hunspell
    VERSION 1.4.1
    DESCRIPTION "Hunspell based spell checker plugin for InyokaEdit"
    HOMEPAGE_URL "https://github.com/inyokaproject/inyokaedit"
)
set(PROJECT_COPYRIGHT "(C) 2011 Volker Goetz, 2014-present - Thorsten Roth")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(${QT_MAIN_VERSION} ${QT_MINIMUM_VERSION}
    REQUIRED Core Widgets LinguistTools
)
find_package(HUNSPELL REQUIRED)

add_library(spellchecker-hunspell SHARED)

set_target_properties(spellchecker-hunspell
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
)

target_compile_features(spellchecker-hunspell PUBLIC cxx_std_17)
target_compile_definitions(spellchecker-hunspell
    PRIVATE
        PLUGIN_NAME="${PROJECT_NAME}"
        PLUGIN_VERSION="${PROJECT_VERSION}"
        PLUGIN_COPY="${PROJECT_COPYRIGHT}"
        QT_NO_FOREACH
        $<$<CONFIG:DEBUG>:QT_DISABLE_DEPRECATED_BEFORE=0x060900>
)

target_link_libraries(spellchecker-hunspell
    PRIVATE
        ${QT_MAIN_VERSION}::Core
        ${QT_MAIN_VERSION}::Widgets
        ${HUNSPELL_LIBRARIES}
)

include_directories(${HUNSPELL_INCLUDE_DIRS})

set(HEADERS
    hunspellcheckdialog.h
    spellchecker-hunspell.h
)

set(SOURCES
    hunspellcheckdialog.cpp
    spellchecker-hunspell.cpp
)

set(FORMS
    hunspellcheckdialog.ui
)

set(RESOURCES
    res/spellchecker-hunspell_resources.qrc
)

set(TRANSLATIONS
    lang/spellchecker-hunspell_de.ts
    lang/spellchecker-hunspell_en.ts
    lang/spellchecker-hunspell_nl.ts
)

if (GENERATE_TS)
    qt6_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/plugins/spellchecker-hunspell/ ${TRANSLATIONS} OPTIONS -no-obsolete)
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
qt_add_resources(spellchecker-hunspell translations_hunspell
    PREFIX "/"
    FILES ${QM_FILES}
)

target_sources(spellchecker-hunspell PRIVATE ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})


if(LINUX)
    include(GNUInstallDirs)
    install(TARGETS spellchecker-hunspell DESTINATION ${CMAKE_INSTALL_LIBDIR}/inyokaedit/plugins)
endif()
