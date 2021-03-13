add_executable(serialize_test serialize_test.cpp ${SRC_BSP})
target_link_libraries(serialize_test mahi::gui mahi::com)