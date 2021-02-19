add_executable(comport_detect comport_detect.cpp ${SRC_BSP})
target_link_libraries(comport_detect mahi::gui mahi::com)