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

project(spellchecker
    VERSION 1.2.3
    DESCRIPTION "Hunspell based spell checker plugin for InyokaEdit"
    HOMEPAGE_URL "https://github.com/inyokaproject/inyokaedit"
)
set(PROJECT_COPYRIGHT "(C) 2011 Volker Götz, 2014-2021 - Thorsten Roth")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(${QT_MAIN_VERSION} ${QT_MINIMUM_VERSION}
    REQUIRED Core Widgets LinguistTools
)
find_package(HUNSPELL REQUIRED)
if (QT6)
    find_package(Qt6 ${QT_MINIMUM_VERSION} REQUIRED Core5Compat)
endif()

add_library(spellchecker SHARED)

target_compile_features(spellchecker PUBLIC cxx_std_11)
target_compile_definitions(spellchecker
    PRIVATE
        PLUGIN_NAME="${PROJECT_NAME}"
        PLUGIN_VERSION="${PROJECT_VERSION}"
        PLUGIN_COPY="${PROJECT_COPYRIGHT}"
        QT_NO_FOREACH
)
target_compile_options(spellchecker
    PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_DEPRECATED_WARNINGS QT_DISABLE_DEPRECATED_BEFORE=0x060000>
        -fno-sized-deallocation
)
target_link_libraries(spellchecker
    PRIVATE
        ${QT_MAIN_VERSION}::Core
        ${QT_MAIN_VERSION}::Widgets
        $<$<BOOL:${QT6}>:Qt6::Core5Compat>
)

set(HEADERS
    spellcheckdialog.h
    spellchecker.h
)

set(SOURCES
    spellcheckdialog.cpp
    spellchecker.cpp
)

set(FORMS
    spellcheckdialog.ui
)

set(RESOURCES
    res/spellchecker_resources.qrc
    res/translations.qrc
)

set(TRANSLATIONS
    lang/spellchecker_de.ts
    lang/spellchecker_en.ts
    lang/spellchecker_nl.ts
)

qt_add_translation(QM_FILES ${TRANSLATIONS})
add_custom_target(translations_${PROJECT_NAME} ALL DEPENDS ${QM_FILES})

target_sources(spellchecker PRIVATE ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})


if(LINUX)
    include(GNUInstallDirs)
    install(TARGETS spellchecker DESTINATION ${CMAKE_INSTALL_LIBDIR}/inyokaedit/plugins)
endif()
