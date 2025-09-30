##########################################################################################################
# Copyright © 2025 Sergey Smolyannikov aka brainstream                                                   #
#                                                                                                        #
# This file is part of the Sol2D Texture Packer.                                                         #
#                                                                                                        #
# Sol2D Texture Packer is free software: you can redistribute it and/or modify it under  the terms of    #
# the GNU General Public License as published by the Free Software Foundation, either version 3 of the   #
# License, or (at your option) any later version.                                                        #
#                                                                                                        #
# Sol2D Texture Packer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      #
# without even the implied warranty of  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.             #
# See the GNU General Public License for more details.                                                   #
#                                                                                                        #
# You should have received a copy of the GNU General Public License along with MailUnit.                 #
# If not, see <http://www.gnu.org/licenses/>.                                                            #
#                                                                                                        #
##########################################################################################################

cmake_minimum_required(VERSION 3.16)

project("Sol2D Texture Packer" VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Xml)
add_subdirectory(third_party/RectangleBinPack)

set(S2TP_LIB_TARGET Library)
set(S2TP_CLI_TARGET CLI)
set(S2TP_GUI_TARGET GUI)

set(S2TP_LIB_BIN s2texpacker)
set(S2TP_CLI_BIN s2tp)
set(S2TP_GUI_BIN s2texpacker)

file(GLOB_RECURSE S2TP_LIB_SRC
    lib/LibSol2dTexturePacker/*.h
    lib/LibSol2dTexturePacker/*.cpp
)

file(GLOB_RECURSE S2TP_CLI_SRC
    cli/Sol2dTexturePackerCli/*.h
    cli/Sol2dTexturePackerCli/*.cpp
)

file(GLOB_RECURSE S2TP_GUI_SRC
    gui/Sol2dTexturePackerGui/*.h
    gui/Sol2dTexturePackerGui/*.cpp
    gui/Sol2dTexturePackerGui/*.ui
    gui/Sol2dTexturePackerGui/*.qrc
)

qt_add_library(
    ${S2TP_LIB_TARGET}
    STATIC
    MANUAL_FINALIZATION
    ${S2TP_LIB_SRC}
)
qt_add_executable(
    ${S2TP_CLI_TARGET}
    MANUAL_FINALIZATION
    ${S2TP_CLI_SRC}
)
qt_add_executable(
    ${S2TP_GUI_TARGET}
    MANUAL_FINALIZATION
    ${S2TP_GUI_SRC}
)

if(MSVC)
    set(S2TP_COMPILE_DEFINOTIONS /W4 /WX)
else()
    set(S2TP_COMPILE_DEFINOTIONS -Wall -Wextra -Wpedantic -Werror)
endif()
target_compile_options(${S2TP_LIB_TARGET} PRIVATE ${S2TP_COMPILE_DEFINOTIONS})
target_compile_options(${S2TP_CLI_TARGET} PRIVATE ${S2TP_COMPILE_DEFINOTIONS})
target_compile_options(${S2TP_GUI_TARGET} PRIVATE ${S2TP_COMPILE_DEFINOTIONS})

target_link_libraries(
    ${S2TP_LIB_TARGET}
    PRIVATE
    RectangleBinPack
    Qt6::Gui
    Qt6::Xml
)
target_link_libraries(
    ${S2TP_CLI_TARGET}
    PRIVATE
    Qt6::Gui
    ${S2TP_LIB_TARGET}
)
target_link_libraries(
    ${S2TP_GUI_TARGET}
    PRIVATE
    Qt6::Widgets
    ${S2TP_LIB_TARGET}
)

set_target_properties(
    ${S2TP_LIB_TARGET}
    PROPERTIES
    OUTPUT_NAME ${S2TP_LIB_BIN}
)
set_target_properties(
    ${S2TP_CLI_TARGET}
    PROPERTIES
    OUTPUT_NAME ${S2TP_CLI_BIN}
)
set_target_properties(
    ${S2TP_GUI_TARGET}
    PROPERTIES
    OUTPUT_NAME ${S2TP_GUI_BIN}
)

target_include_directories(
    ${S2TP_LIB_TARGET}
    PRIVATE
    lib
    third_party
)
target_include_directories(
    ${S2TP_CLI_TARGET}
    PRIVATE
    lib
    cli
)
target_include_directories(
    ${S2TP_GUI_TARGET}
    PRIVATE
    lib
    gui
)

add_compile_definitions(
    __S2TP_VERSION="${CMAKE_PROJECT_VERSION}"
    __S2TP_ORG="sol2d"
    __S2TP_APP="Sol2d Texture Packer"
)
target_compile_definitions(
    ${S2TP_LIB_TARGET}
    PRIVATE
    __S2TP_LIB
    __S2TP_BIN="${S2TP_LIB_BIN}"
)
target_compile_definitions(
    ${S2TP_CLI_TARGET}
    PRIVATE
    __S2TP_BIN="${S2TP_CLI_BIN}"
)
target_compile_definitions(
    ${S2TP_GUI_TARGET}
    PRIVATE
    __S2TP_BIN="${S2TP_GUI_BIN}"
)

qt_finalize_target(${S2TP_LIB_TARGET})
qt_finalize_executable(${S2TP_CLI_TARGET})
qt_finalize_executable(${S2TP_GUI_TARGET})
