# if mahi::robo target has yet to be defined
if(NOT TARGET mahi::robo)
    # get directory mahi-robo-config.cmake was found in
    get_filename_component(MAHI_ROBO_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
    # include find dependecny macro
    include(CMakeFindDependencyMacro)
    # include the appropriate targets file
    include("${MAHI_ROBO_CMAKE_DIR}/mahi-robo-targets.cmake")
endif()

# set mahi::robo libaries
set(MAHI_ROBO_LIBRARIES mahi::robo)
