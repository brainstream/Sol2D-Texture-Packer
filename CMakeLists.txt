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
    Qt6::Core
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
    OUTPUT_NAME s2texpacker
)
set_target_properties(
    ${S2TP_CLI_TARGET}
    PROPERTIES
    OUTPUT_NAME s2tp
)
set_target_properties(
    ${S2TP_GUI_TARGET}
    PROPERTIES
    OUTPUT_NAME s2texpacker
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
)
target_compile_definitions(
    ${S2TP_LIB_TARGET}
    PRIVATE
    __S2TP_LIB
)

qt_finalize_target(${S2TP_LIB_TARGET})
qt_finalize_executable(${S2TP_CLI_TARGET})
qt_finalize_executable(${S2TP_GUI_TARGET})
