cmake_minimum_required(VERSION 3.0.0)
project(lotusc VERSION 1.0.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)

# add compiler arguments
add_compile_options(-std=c++20 -Wall -Wextra)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# find dependencies
find_package(Threads)
find_package(spdlog REQUIRED)
find_package(cereal REQUIRED)

# add source files
file(GLOB_RECURSE sources src/*.cpp src/*.hpp )
add_executable(lotusc ${sources})

# link dependencies and fix include path
set(MEMFLOW_DIR "${CMAKE_SOURCE_DIR}/deps/memflow")
target_link_libraries(lotusc 
    debug "${MEMFLOW_DIR}/target/debug/libmemflow_win32_ffi.a"
    debug "${MEMFLOW_DIR}/target/debug/libmemflow_ffi.a"
    optimized "${MEMFLOW_DIR}/target/release/libmemflow_win32_ffi.a"
    optimized "${MEMFLOW_DIR}/target/release/libmemflow_ffi.a"
    spdlog::spdlog
    cereal
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
)

target_include_directories(
    lotusc PUBLIC
    "${MEMFLOW_DIR}/memflow-ffi"
    "${MEMFLOW_DIR}/memflow-win32-ffi"
    "${CMAKE_SOURCE_DIR}/deps"
    "${CMAKE_SOURCE_DIR}/src")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})