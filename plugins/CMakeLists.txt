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

option(PLUGIN_SPELLCHECKER_NUSPELL "Enable nuspell spellchecker plugin" OFF)
if(PLUGIN_SPELLCHECKER_NUSPELL)
    add_subdirectory(spellchecker-nuspell)
endif()

option(PLUGIN_SPELLCHECKER_HUNSPELL "Enable hunspell spellchecker plugin" ON)
if(PLUGIN_SPELLCHECKER_HUNSPELL)
    add_subdirectory(spellchecker-hunspell)
endif()

option(PLUGIN_UU_TABLETEMPLATE "Enable uu_tabletemplate plugin" ON)
if(PLUGIN_UU_TABLETEMPLATE)
    add_subdirectory(uu_tabletemplate)
endif()

option(PLUGIN_UU_KNOWLEDGEBOX "Enable uu_knowledgebox plugin" ON)
if(PLUGIN_UU_KNOWLEDGEBOX)
    add_subdirectory(uu_knowledgebox)
endif()

option(PLUGIN_HIGHLIGHTER "Enable highlighter plugin" ON)
if(PLUGIN_HIGHLIGHTER)
    add_subdirectory(highlighter)
endif()

option(PLUGIN_HOTKEY "Enable hotkey plugin" ON)
if(PLUGIN_HOTKEY)
    add_subdirectory(hotkey)
endif()
