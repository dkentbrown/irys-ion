cmake_minimum_required(VERSION 3.24)
project(IrysIon VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(IRYSION_BUILD_TESTS "Build deterministic validation tests" ON)
option(IRYSION_FETCH_RAYLIB "Allow the pinned Raylib fallback when no package is installed" ON)

find_package(raylib 6.0 QUIET)
if(NOT raylib_FOUND)
  if(NOT IRYSION_FETCH_RAYLIB)
    message(FATAL_ERROR "Raylib 6.0 was not found. Set CMAKE_PREFIX_PATH to the Raylib prefix or enable IRYSION_FETCH_RAYLIB.")
  endif()
  include(FetchContent)
  set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/6.0.tar.gz
    URL_HASH SHA256=9a9076afc1e767c119fb701ad99640393e5d07c96ff6a47435e5b44b5a00ac1d)
  FetchContent_MakeAvailable(raylib)
endif()

add_library(irys_ion_core
  src/core.cpp
)
target_include_directories(irys_ion_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)
target_compile_features(irys_ion_core PUBLIC cxx_std_20)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(irys_ion_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(irys-ion
  src/main.cpp
  src/game.cpp
)
target_link_libraries(irys-ion PRIVATE irys_ion_core raylib)
target_compile_definitions(irys-ion PRIVATE
  IRYSION_RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
  IRYSION_VERSION="${PROJECT_VERSION}")
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(irys-ion PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(APPLE)
  target_link_libraries(irys-ion PRIVATE "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
endif()

include(CTest)
if(BUILD_TESTING AND IRYSION_BUILD_TESTS)
  add_executable(irys-ion-tests tests/core_tests.cpp)
  target_link_libraries(irys-ion-tests PRIVATE irys_ion_core)
  add_test(NAME deterministic-core COMMAND irys-ion-tests)
  set_tests_properties(deterministic-core PROPERTIES
    ENVIRONMENT "IRYSION_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}")
endif()
