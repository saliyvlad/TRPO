cmake_minimum_required(VERSION 3.20)
project(TRMM_Benchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "")

# OpenMP
find_package(OpenMP REQUIRED)
if(MSVC)
    add_compile_options(/openmp)
endif()

# Библиотека TRMM
add_library(trmm_lib src/trmm.cpp)
target_include_directories(trmm_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(trmm_lib PUBLIC OpenMP::OpenMP_CXX)

# OpenBLAS: Точные пути из вашего сообщения
set(OPENBLAS_INC "C:/vcpkg/packages/openblas_x64-windows/include/openblas")
set(OPENBLAS_LIB "C:/vcpkg/packages/openblas_x64-windows/lib/openblas.lib")

# Фолбэк на стандартную папку installed, если в packages нет .lib файла
if(NOT EXISTS ${OPENBLAS_LIB})
    set(OPENBLAS_LIB "C:/vcpkg/installed/x64-windows/lib/openblas.lib")
endif()

if(NOT EXISTS "${OPENBLAS_INC}/cblas.h")
    message(FATAL_ERROR "cblas.h не найден по пути: ${OPENBLAS_INC}")
endif()

message(STATUS "Using OpenBLAS headers: ${OPENBLAS_INC}")
message(STATUS "Using OpenBLAS library: ${OPENBLAS_LIB}")

target_include_directories(trmm_lib PUBLIC ${OPENBLAS_INC})
target_link_libraries(trmm_lib PUBLIC ${OPENBLAS_LIB})

# Исполняемые файлы
add_executable(benchmark src/benchmark.cpp)
target_link_libraries(benchmark PRIVATE trmm_lib)

enable_testing()
add_executable(test_trmm tests/test_trmm.cpp)
target_link_libraries(test_trmm PRIVATE trmm_lib)
add_test(NAME TRMM_Test COMMAND test_trmm)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")