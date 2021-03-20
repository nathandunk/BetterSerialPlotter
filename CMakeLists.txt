# MIT License
#
# Copyright (c) 2020 Nathan Dunkelberger
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# Author(s): Nathan Dunkelberger (n8dunk@gmail.com)

cmake_minimum_required(VERSION 3.13.0)

# create project
project(BSP VERSION 0.1.0 LANGUAGES CXX)

# defines conventional GNU isntallation directories
include(GNUInstallDirs)

# set compiler flags
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -O3") # all warnings
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W3 /MP") # warning level 4
endif()

# Use fetch content to get libraries that meii is dependent on
include(FetchContent)

# MAHI GUI
FetchContent_Declare(mahi-gui GIT_REPOSITORY https://github.com/mahilab/mahi-gui.git)
FetchContent_MakeAvailable(mahi-gui)

# MAHI COM
FetchContent_Declare(mahi-com GIT_REPOSITORY https://github.com/mahilab/mahi-com.git)
FetchContent_MakeAvailable(mahi-com)

set(SRC_BSP 
    ${CMAKE_SOURCE_DIR}/src/BetterSerialPlotter/Utility.cpp
    ${CMAKE_SOURCE_DIR}/src/BetterSerialPlotter/BSP.cpp
    ${CMAKE_SOURCE_DIR}/src/BetterSerialPlotter/Plot.cpp
    ${CMAKE_SOURCE_DIR}/src/BetterSerialPlotter/Widget.cpp
    ${CMAKE_SOURCE_DIR}/src/BetterSerialPlotter/DataPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/BetterSerialPlotter/SerialManager.cpp
    ${CMAKE_SOURCE_DIR}/src/BetterSerialPlotter/PlotMonitor.cpp
    ${CMAKE_SOURCE_DIR}/src/BetterSerialPlotter/SerialMonitor.cpp
    ${CMAKE_SOURCE_DIR}/src/BetterSerialPlotter/Serialization.cpp
)

include_directories(include)

add_executable(BetterSerialPlotter src/main.cpp ${SRC_BSP} icon/BSP_icon.rc)

# add_subdirectory(test)

target_link_libraries(BetterSerialPlotter mahi::gui mahi::com)