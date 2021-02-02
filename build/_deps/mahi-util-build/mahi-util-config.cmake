# if mahi::util target has yet to be defined
if(NOT TARGET mahi::util)
    # get directory mahi-util-config.cmake was found in
    get_filename_component(MAHI_UTIL_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
    # include find dependecny macro
    include(CMakeFindDependencyMacro)
    # include the appropriate targets file
    include("${MAHI_UTIL_CMAKE_DIR}/mahi-util-targets.cmake")
endif()

# set mahi::util libaries
set(MAHI_UTIL_LIBRARIES mahi::util)
