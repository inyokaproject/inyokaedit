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

find_package(Qt5 ${QT_MINIMUM_VERSION}
    REQUIRED Core Gui Widgets Network PrintSupport Xml LinguistTools
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(inyokaedit)

target_compile_features(inyokaedit PUBLIC cxx_std_11)
target_compile_definitions(inyokaedit
    PRIVATE
        APP_NAME="${PROJECT_NAME}"
        APP_VERSION="${PROJECT_VERSION}"
        APP_COPY="${PROJECT_COPYRIGHT}"
        APP_DESC="${PROJECT_DESCRIPTION}"
        QT_NO_FOREACH
)
target_compile_options(inyokaedit
    PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_DEPRECATED_WARNINGS QT_DISABLE_DEPRECATED_BEFORE=0x060000>
        -fno-sized-deallocation
)
target_link_libraries(inyokaedit
    PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
        Qt5::PrintSupport
        Qt5::Xml
)

set_preview_dependencies(inyokaedit)

add_subdirectory(templates)
add_subdirectory(parser)

set(HEADERS
    inyokaedit.h
    download.h
    downloadimg.h
    fileoperations.h
    findreplace.h
    plugins.h
    texteditor.h
    session.h
    settings.h
    settingsdialog.h
    syntaxcheck.h
    upload.h
    utils.h
    xmlparser.h
    ieditorplugin.h
)

set(SOURCES
    main.cpp
    inyokaedit.cpp
    download.cpp
    downloadimg.cpp
    fileoperations.cpp
    findreplace.cpp
    plugins.cpp
    texteditor.cpp
    session.cpp
    settings.cpp
    settingsdialog.cpp
    syntaxcheck.cpp
    upload.cpp
    xmlparser.cpp
    utils.cpp
)

set(FORMS
    inyokaedit.ui
    findreplace.ui
    settingsdialog.ui
)

set(RESOURCES
    data/data.qrc
    lang/translations.qrc
)

set(TRANSLATIONS
    lang/inyokaedit_de.ts
    lang/inyokaedit_nl.ts
)

qt5_add_translation(QM_FILES ${TRANSLATIONS})
add_custom_target(translations ALL DEPENDS ${QM_FILES})

target_sources(inyokaedit PRIVATE ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})


if(LINUX)
    include(GNUInstallDirs)
    install(TARGETS inyokaedit DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES data/unix/org.inyokaproject.inyokaedit.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(FILES data/unix/org.inyokaproject.inyokaedit.metainfo.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
    install(FILES data/unix/inyokaedit.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/mime/packages)
    install(DIRECTORY icons/hicolor DESTINATION ${CMAKE_INSTALL_DATADIR}/icons)
endif()
