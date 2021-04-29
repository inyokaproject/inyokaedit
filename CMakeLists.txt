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

cmake_minimum_required(VERSION 3.13)
project(InyokaEdit
    VERSION 0.26.0
    DESCRIPTION "Editor for Inyoka based portals"
    HOMEPAGE_URL "https://github.com/inyokaproject/inyokaedit"
)
set(PROJECT_COPYRIGHT "(C) 2011-2021 The InyokaEdit developers")

set(QT_MINIMUM_VERSION 5.9)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(cmake/preview-deps.cmake)

add_subdirectory(plugins)
add_subdirectory(application)
