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

project(spellchecker-nuspell
    VERSION 1.1.1
    DESCRIPTION "Nuspell based spell checker plugin for InyokaEdit"
    HOMEPAGE_URL "https://github.com/inyokaproject/inyokaedit"
)
set(PROJECT_COPYRIGHT "(C) 2011 Volker G&ouml;tz, 2014-present - Thorsten Roth")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(${QT_MAIN_VERSION} ${QT_MINIMUM_VERSION}
    REQUIRED Core Widgets LinguistTools
)
find_package(NUSPELL REQUIRED)

add_library(spellchecker-nuspell SHARED)

target_compile_features(spellchecker-nuspell PUBLIC cxx_std_17)
target_compile_definitions(spellchecker-nuspell
    PRIVATE
        PLUGIN_NAME="${PROJECT_NAME}"
        PLUGIN_VERSION="${PROJECT_VERSION}"
        PLUGIN_COPY="${PROJECT_COPYRIGHT}"
        QT_NO_FOREACH
        $<$<CONFIG:DEBUG>:QT_DISABLE_DEPRECATED_BEFORE=0x060600>
)

target_link_libraries(spellchecker-nuspell
    PRIVATE
        ${QT_MAIN_VERSION}::Core
        ${QT_MAIN_VERSION}::Widgets
        ${NUSPELL_LIBRARIES}
)

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
    res/translations.qrc
)

set(TRANSLATIONS
    lang/spellchecker-nuspell_de.ts
    lang/spellchecker-nuspell_en.ts
    lang/spellchecker-nuspell_nl.ts
)

qt_add_translation(QM_FILES ${TRANSLATIONS})
add_custom_target(translations_${PROJECT_NAME} ALL DEPENDS ${QM_FILES})

target_sources(spellchecker-nuspell PRIVATE ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})


if(LINUX)
    include(GNUInstallDirs)
    install(TARGETS spellchecker-nuspell DESTINATION ${CMAKE_INSTALL_LIBDIR}/inyokaedit/plugins)
endif()
