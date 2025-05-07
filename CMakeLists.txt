cmake_minimum_required(VERSION 3.20)
project(chip-pipeline VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Diretório padrão para instalar dependências
include(FetchContent)

# Dependências
FetchContent_Declare(
   systemc
   GIT_REPOSITORY https://github.com/accellera-official/systemc.git
   GIT_TAG 3.0.1
)
FetchContent_MakeAvailable(systemc)

# Incluindo cabeçalhos
include_directories(include testbench)

# Definindo sources
file(GLOB SOURCES src/*.cpp)

# Gerando o executável
add_executable(chip-pipeline ${SOURCES} ${TESTBENCH})

# Linkando com o systemc
target_link_libraries(chip-pipeline systemc)