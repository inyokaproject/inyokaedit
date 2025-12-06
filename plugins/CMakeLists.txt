# SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
# SPDX-License-Identifier: GPL-3.0-or-later

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
