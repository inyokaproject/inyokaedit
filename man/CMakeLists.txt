# SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
# SPDX-License-Identifier: GPL-3.0-or-later

set(MAN_FILES
    inyokaedit.1.in
    de/inyokaedit.1.in
)

find_program(GZIP gzip REQUIRED)

set(___PREFIX___ ${CMAKE_INSTALL_PREFIX})
foreach(file ${MAN_FILES})
    get_filename_component(file_stripped ${file} NAME_WLE)
    get_filename_component(file_dir ${file} DIRECTORY)
    get_filename_component(man_category_ext ${file_stripped} LAST_EXT)
    string(SUBSTRING ${man_category_ext} 1 -1 man_category)
    if(file_dir)
        set(file_dir ${file_dir}/)
    endif()
    set(man_path ${file_dir}man${man_category}/${file_stripped})
    set(man_path_absolute ${CMAKE_CURRENT_BINARY_DIR}/${man_path})
    list(APPEND MAN_FILES_STRIPPED ${man_path}.gz)
    list(APPEND MAN_FILES_STRIPPED_ABSOLUTE ${man_path_absolute}.gz)
    configure_file(${file} ${man_path_absolute})
    add_custom_command(OUTPUT ${man_path_absolute}.gz
        COMMAND ${GZIP} -k -f9 ${man_path_absolute}
        DEPENDS ${man_path_absolute}
    )
endforeach()

add_custom_target(man
    ALL
    DEPENDS ${MAN_FILES_STRIPPED_ABSOLUTE}
)


include(GNUInstallDirs)
foreach(man_path ${MAN_FILES_STRIPPED})
    get_filename_component(man_dir ${man_path} DIRECTORY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${man_path} DESTINATION ${CMAKE_INSTALL_MANDIR}/${man_dir})
endforeach()
