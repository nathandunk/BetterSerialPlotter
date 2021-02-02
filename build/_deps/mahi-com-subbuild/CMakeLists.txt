# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.14.0-rc2)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(mahi-com-populate NONE)

include(ExternalProject)
ExternalProject_Add(mahi-com-populate
                     "UPDATE_DISCONNECTED" "False" "GIT_REPOSITORY" "https://github.com/mahilab/mahi-com.git"
                    SOURCE_DIR          "C:/Git/BetterSerialPlotter/build/_deps/mahi-com-src"
                    BINARY_DIR          "C:/Git/BetterSerialPlotter/build/_deps/mahi-com-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)
