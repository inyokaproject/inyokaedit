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

string(TOLOWER ${PROJECT_NAME} target_name)

find_package(${QT_MAIN_VERSION} ${QT_MINIMUM_VERSION} REQUIRED Widgets)

set(HEADERS
    templates.h
)

set(SOURCES
    templates.cpp
)

target_sources(${target_name} PRIVATE ${HEADERS} ${SOURCES})
target_link_libraries(${target_name} PRIVATE ${QT_MAIN_VERSION}::Widgets)
