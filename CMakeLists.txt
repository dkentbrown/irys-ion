cmake_minimum_required(VERSION 3.20)

project(irys_ion_player_foundation VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(raylib 6.0 REQUIRED)

add_library(player_foundation_core
    src/player_foundation.cpp
    src/rig.cpp
)
target_include_directories(player_foundation_core PUBLIC include)
target_compile_options(player_foundation_core PRIVATE
    $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall -Wextra -Wpedantic -Wconversion>
)

add_executable(irys_ion_foundation MACOSX_BUNDLE src/main.cpp)
target_link_libraries(irys_ion_foundation PRIVATE player_foundation_core raylib)
target_compile_definitions(irys_ion_foundation PRIVATE IRYS_ION_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
set_target_properties(irys_ion_foundation PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME "Irys+Ion Player Foundation"
    MACOSX_BUNDLE_GUI_IDENTIFIER "dev.dkentbrown.irys-ion.player-foundation"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1.0"
)

include(CTest)
if(BUILD_TESTING)
    add_executable(player_foundation_tests tests/player_foundation_tests.cpp)
    target_link_libraries(player_foundation_tests PRIVATE player_foundation_core)
    target_compile_definitions(player_foundation_tests PRIVATE IRYS_ION_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
    add_test(NAME player_foundation_tests COMMAND player_foundation_tests)
endif()
