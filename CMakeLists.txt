cmake_minimum_required(VERSION 3.10) # Menor versão não deprecada
project(circuito_pipeline VERSION 0.0.0 LANGUAGES C CXX)

set (CMAKE_PREFIX_PATH /opt/systemc)
find_package(SystemCLanguage CONFIG REQUIRED)

set (CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD} CACHE STRING
"C++ standard to build all targets. Minimum version is C++17.")

set (CMAKE_CXX_STANDARD_REQUIRED ${SystemC_CXX_STANDARD_REQUIRED} CACHE BOOL
"The with CMAKE_CXX_STANDARD selected C++ standard is a requirement.")

add_executable(main src/main.cpp)
target_link_libraries(main SystemC::systemc)