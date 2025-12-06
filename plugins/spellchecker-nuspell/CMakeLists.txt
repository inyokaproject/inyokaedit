# SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
# SPDX-License-Identifier: GPL-3.0-or-later

project(spellchecker-nuspell
    VERSION 1.1.1
    DESCRIPTION "Nuspell based spell checker plugin for InyokaEdit"
    HOMEPAGE_URL "https://github.com/inyokaproject/inyokaedit"
)
set(PROJECT_COPYRIGHT "(C) 2011 Volker Goetz, 2014-present - Thorsten Roth")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(${QT_MAIN_VERSION} ${QT_MINIMUM_VERSION}
    REQUIRED Core Widgets LinguistTools
)
find_package(NUSPELL REQUIRED)

add_library(spellchecker-nuspell SHARED)

set_target_properties(spellchecker-nuspell
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
)

target_compile_features(spellchecker-nuspell PUBLIC cxx_std_17)
target_compile_definitions(spellchecker-nuspell
    PRIVATE
        PLUGIN_NAME="${PROJECT_NAME}"
        PLUGIN_VERSION="${PROJECT_VERSION}"
        PLUGIN_COPY="${PROJECT_COPYRIGHT}"
        QT_NO_FOREACH
        $<$<CONFIG:DEBUG>:QT_DISABLE_DEPRECATED_BEFORE=0x060900>
)

if(MSVC)
    target_compile_options(spellchecker-nuspell
        PRIVATE
            $<$<CONFIG:DEBUG>:/W4 /permissive->
    )
else()
    target_compile_options(spellchecker-nuspell
        PRIVATE
            $<$<CONFIG:DEBUG>:-Wall -Wextra -pedantic>
    )
endif()

target_link_libraries(spellchecker-nuspell
    PRIVATE
        ${QT_MAIN_VERSION}::Core
        ${QT_MAIN_VERSION}::Widgets
        ${NUSPELL_LIBRARIES}
)

include_directories(${NUSPELL_INCLUDE_DIRS})

set(HEADERS
    nuspellcheckdialog.h
    spellchecker-nuspell.h
)

set(SOURCES
    nuspellcheckdialog.cpp
    spellchecker-nuspell.cpp
)

set(FORMS
    nuspellcheckdialog.ui
)

set(RESOURCES
    res/spellchecker-nuspell_resources.qrc
)

set(TRANSLATIONS
    lang/spellchecker-nuspell_de.ts
    lang/spellchecker-nuspell_en.ts
    lang/spellchecker-nuspell_nl.ts
)

if (GENERATE_TS)
    qt6_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/plugins/spellchecker-nuspell/ ${TRANSLATIONS} OPTIONS -no-obsolete)
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
qt_add_resources(spellchecker-nuspell translations_nuspell
    PREFIX "/"
    FILES ${QM_FILES}
)

target_sources(spellchecker-nuspell PRIVATE ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})


if(LINUX)
    include(GNUInstallDirs)
    install(TARGETS spellchecker-nuspell DESTINATION ${CMAKE_INSTALL_LIBDIR}/inyokaedit/plugins)
endif()
