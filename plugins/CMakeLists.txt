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

option(SPELLCHECKER "Enable spellchecker plugin" ON)
if(SPELLCHECKER)
    add_subdirectory(spellchecker)
endif()

option(UU_TABLETEMPLATE "Enable uu_tabletemplate plugin" ON)
if(UU_TABLETEMPLATE)
    add_subdirectory(uu_tabletemplate)
endif()

option(UU_KNOWLEDGEBOX "Enable uu_knowledgebox plugin" ON)
if(UU_KNOWLEDGEBOX)
    add_subdirectory(uu_knowledgebox)
endif()

option(HIGHLIGHTER "Enable highlighter plugin" ON)
if(HIGHLIGHTER)
    add_subdirectory(highlighter)
endif()

option(HOTKEY "Enable hotkey plugin" ON)
if(HOTKEY)
    add_subdirectory(hotkey)
endif()
