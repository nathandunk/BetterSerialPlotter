# if mahi::com target has yet to be defined
if(NOT TARGET mahi::com)
    # get directory mahi-com-config.cmake was found in
    get_filename_component(MAHI_COM_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
    # include find dependecny macro
    include(CMakeFindDependencyMacro)
    # include find modules
    list(APPEND CMAKE_MODULE_PATH "${MAHI_COM_CMAKE_DIR}/Modules")
    # include the appropriate targets file
    include("${MAHI_COM_CMAKE_DIR}/mahi-com-targets.cmake")
endif()

# set mahi::com libaries
set(MAHI_COM_LIBRARIES mahi::com)
