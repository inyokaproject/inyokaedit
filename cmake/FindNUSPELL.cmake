# - Try to find NUSPELL
# Once done this will define
#
#  NUSPELL_FOUND - system has NUSPELL
#  NUSPELL_INCLUDE_DIRS - the NUSPELL include directory
#  NUSPELL_LIBRARIES - The libraries needed to use NUSPELL

find_package(PkgConfig)
pkg_check_modules(PKG_NUSPELL QUIET nuspell)

find_path(NUSPELL_INCLUDE_DIRS
          NAMES dictionary.hxx
          PATH_SUFFIXES nuspell
          HINTS ${PKG_NUSPELL_INCLUDE_DIRS}
)
find_library(NUSPELL_LIBRARIES
             NAMES ${PKG_NUSPELL_LIBRARIES} nuspell nuspell-5.1 hunspell-5.0 libnuspell
             HINTS ${PKG_NUSPELL_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NUSPELL
                                  REQUIRED_VARS NUSPELL_LIBRARIES NUSPELL_INCLUDE_DIRS
                                  VERSION_VAR PKG_NUSPELL_VERSION
)

mark_as_advanced(NUSPELL_INCLUDE_DIRS NUSPELL_LIBRARIES)
