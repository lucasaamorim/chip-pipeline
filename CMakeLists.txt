# cmake_minimum_required(VERSION 3.20)
# project(chip-pipeline VERSION 0.1.0 LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Diretório padrão para instalar dependências
# include(FetchContent)

# # Dependências
# FetchContent_Declare(
#    systemc
#    GIT_REPOSITORY https://github.com/accellera-official/systemc.git
#    GIT_TAG 3.0.1
# )
# FetchContent_MakeAvailable(systemc)

# # Incluindo cabeçalhos
# include_directories(include testbench)

# # Definindo sources
# file(GLOB SOURCES src/*.cpp)

# # Gerando o executável
# add_executable(chip-pipeline ${SOURCES} ${TESTBENCH})

# # Linkando com o systemc
# target_link_libraries(chip-pipeline systemc)


cmake_minimum_required(VERSION 3.10)
project(chip_pipeline VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Caminho para o SystemC
set(SYSTEMC_HOME "$ENV{HOME}/systemc/install")

# Diretórios de include
include_directories(
    ${SYSTEMC_HOME}/include
    ${CMAKE_SOURCE_DIR}/src/components
    ${CMAKE_SOURCE_DIR}/src/pipeline
    ${CMAKE_SOURCE_DIR}/src/control
)

# Diretórios de bibliotecas
link_directories(${SYSTEMC_HOME}/lib-linux64)

# Fontes principais
file(GLOB_RECURSE COMPONENT_SOURCES src/components/**/*.cpp)
file(GLOB_RECURSE PIPELINE_SOURCES src/pipeline/**/*.cpp)
file(GLOB_RECURSE CONTROL_SOURCES src/control/*.cpp)
file(GLOB_RECURSE PROCESSOR_SOURCES src/processor/*.cpp)

# Executável principal (simulador completo)
add_executable(chip_pipeline
    ${COMPONENT_SOURCES}
    ${PIPELINE_SOURCES}
    ${CONTROL_SOURCES}
    ${PROCESSOR_SOURCES}
    src/main.cpp
)

target_link_libraries(chip_pipeline systemc)

# Para testes separados: opcional
enable_testing()

# Arquivos de testbench com final Testbench.cpp
file(GLOB_RECURSE TESTBENCH_SOURCES src/components/**/*Testbench.hpp)

foreach(test_file ${TESTBENCH_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} systemc)
endforeach()


# RPath para evitar variáveis de ambiente
set_target_properties(chip_pipeline PROPERTIES
    BUILD_RPATH ${SYSTEMC_HOME}/lib-linux64
)
