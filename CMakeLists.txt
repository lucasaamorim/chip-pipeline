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

# Caminho para o SystemC já instalado
set(SYSTEMC_HOME "$ENV{HOME}/systemc/install")

# Inclui diretórios
include_directories(
    ${SYSTEMC_HOME}/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/testbench
)

# Link de bibliotecas
link_directories(${SYSTEMC_HOME}/lib-linux64)

# Fontes
file(GLOB SOURCES src/*.cpp testbench/*.cpp)

# Executável
add_executable(chip_pipeline ${SOURCES})

# Link com SystemC
target_link_libraries(chip_pipeline systemc)

# RPath para evitar export de LD_LIBRARY_PATH
set_target_properties(chip_pipeline PROPERTIES
    BUILD_RPATH ${SYSTEMC_HOME}/lib-linux64
)
