#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "mahi::com" for configuration "RelWithDebInfo"
set_property(TARGET mahi::com APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(mahi::com PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C;CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/mahi-com.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS mahi::com )
list(APPEND _IMPORT_CHECK_FILES_FOR_mahi::com "${_IMPORT_PREFIX}/lib/mahi-com.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
