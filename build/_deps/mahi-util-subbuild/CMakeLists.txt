# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.14.1)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(mahi-util-populate NONE)

include(ExternalProject)
ExternalProject_Add(mahi-util-populate
                     "UPDATE_DISCONNECTED" "False" "GIT_REPOSITORY" "https://github.com/mahilab/mahi-util.git"
                    SOURCE_DIR          "C:/Git/BetterSerialPlotter/build/_deps/mahi-util-src"
                    BINARY_DIR          "C:/Git/BetterSerialPlotter/build/_deps/mahi-util-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)
