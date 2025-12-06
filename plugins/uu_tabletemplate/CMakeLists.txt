# SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
# SPDX-License-Identifier: GPL-3.0-or-later

project(uu_tabletemplate
    VERSION 1.5.0
    DESCRIPTION "ubuntuusers.de table template plugin for InyokaEdit"
    HOMEPAGE_URL "https://github.com/inyokaproject/inyokaedit"
)
set(PROJECT_COPYRIGHT "(C) 2012-present - Christian Schaerf, Thorsten Roth")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(${QT_MAIN_VERSION} ${QT_MINIMUM_VERSION}
    REQUIRED Core Network Widgets LinguistTools
)

add_library(uu_tabletemplate SHARED)

set_target_properties(uu_tabletemplate
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
)

target_compile_features(uu_tabletemplate PUBLIC cxx_std_17)
target_compile_definitions(uu_tabletemplate
    PRIVATE
        PLUGIN_NAME="${PROJECT_NAME}"
        PLUGIN_VERSION="${PROJECT_VERSION}"
        PLUGIN_COPY="${PROJECT_COPYRIGHT}"
        QT_NO_FOREACH
        $<$<CONFIG:DEBUG>:QT_DISABLE_DEPRECATED_BEFORE=0x060900>
)

if(MSVC)
    target_compile_options(uu_tabletemplate
        PRIVATE
            $<$<CONFIG:DEBUG>:/W4 /permissive->
    )
else()
    target_compile_options(uu_tabletemplate
        PRIVATE
            $<$<CONFIG:DEBUG>:-Wall -Wextra -pedantic>
    )
endif()

target_link_libraries(uu_tabletemplate
    PRIVATE
        ${QT_MAIN_VERSION}::Core
        ${QT_MAIN_VERSION}::Network
)

set_preview_dependencies(uu_tabletemplate)

add_subdirectory(../../application/templates ${CMAKE_CURRENT_BINARY_DIR}/templates)
add_subdirectory(../../application/parser ${CMAKE_CURRENT_BINARY_DIR}/parser)

set(HEADERS
    uu_tabletemplate.h
    ../../application/syntaxcheck.h
    ../../application/utils.h
)

set(SOURCES
    uu_tabletemplate.cpp
    ../../application/syntaxcheck.cpp
    ../../application/utils.cpp
)

set(FORMS
    uu_tabletemplate.ui
)

set(RESOURCES
    res/uu_tabletemplate_resources.qrc
)

set(TRANSLATIONS
    lang/uu_tabletemplate_de.ts
    lang/uu_tabletemplate_en.ts
    lang/uu_tabletemplate_nl.ts
)

if (GENERATE_TS)
    qt6_create_translation(QM_FILES ${CMAKE_SOURCE_DIR}/plugins/uu_tabletemplate/ ${TRANSLATIONS} OPTIONS -no-obsolete)
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
qt_add_resources(uu_tabletemplate translations_uu_table
    PREFIX "/"
    FILES ${QM_FILES}
)

target_sources(uu_tabletemplate PRIVATE ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})


if(LINUX)
    include(GNUInstallDirs)
    install(TARGETS uu_tabletemplate DESTINATION ${CMAKE_INSTALL_LIBDIR}/inyokaedit/plugins)
endif()
