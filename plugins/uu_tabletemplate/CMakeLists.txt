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

project(uu_tabletemplate
    VERSION 1.5.0
    DESCRIPTION "ubuntuusers.de table template plugin for InyokaEdit"
    HOMEPAGE_URL "https://github.com/inyokaproject/inyokaedit"
)
set(PROJECT_COPYRIGHT "(C) 2012-present - Christian Schärf, Thorsten Roth")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(${QT_MAIN_VERSION} ${QT_MINIMUM_VERSION}
    REQUIRED Core Network Widgets LinguistTools
)

add_library(uu_tabletemplate SHARED)

target_compile_features(uu_tabletemplate PUBLIC cxx_std_17)
target_compile_definitions(uu_tabletemplate
    PRIVATE
        PLUGIN_NAME="${PROJECT_NAME}"
        PLUGIN_VERSION="${PROJECT_VERSION}"
        PLUGIN_COPY="${PROJECT_COPYRIGHT}"
        QT_NO_FOREACH
        $<$<CONFIG:DEBUG>:QT_DISABLE_DEPRECATED_BEFORE=0x060600>
)

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
    res/translations.qrc
)

set(TRANSLATIONS
    lang/uu_tabletemplate_de.ts
    lang/uu_tabletemplate_en.ts
    lang/uu_tabletemplate_nl.ts
)

qt_add_translation(QM_FILES ${TRANSLATIONS})
add_custom_target(translations_${PROJECT_NAME} ALL DEPENDS ${QM_FILES})

target_sources(uu_tabletemplate PRIVATE ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})


if(LINUX)
    include(GNUInstallDirs)
    install(TARGETS uu_tabletemplate DESTINATION ${CMAKE_INSTALL_LIBDIR}/inyokaedit/plugins)
endif()
