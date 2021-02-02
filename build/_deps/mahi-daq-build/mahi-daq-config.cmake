# if mahi::daq target has yet to be defined
if(NOT TARGET mahi::daq)
    # get directory mahi-daq-config.cmake was found in
    get_filename_component(MAHI_DAQ_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
    # include find dependecny macro
    include(CMakeFindDependencyMacro)
    # include find modules
    list(APPEND CMAKE_MODULE_PATH "${MAHI_DAQ_CMAKE_DIR}/Modules")
    # find dependencies and set targets filename
    if (MSVC)
        find_package(HIL     QUIET)
        find_package(NIDAQmx QUIET)
        find_package(Sensoray826 QUIET)
    endif()
    # include the appropriate targets file
    include("${MAHI_DAQ_CMAKE_DIR}/mahi-daq-targets.cmake")
endif()

# set mahi::daq libaries
set(MAHI_DAQ_LIBRARIES mahi::daq)
