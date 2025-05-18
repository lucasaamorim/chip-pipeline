cmake_minimum_required(VERSION 3.20)
project(chip-pipeline VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependências do SystemC via FetchContent
include(FetchContent)

FetchContent_Declare(
    systemc
    GIT_REPOSITORY https://github.com/accellera-official/systemc.git
    GIT_TAG 3.0.1
)

# Fazendo o download e adicionando o SystemC ao projeto
FetchContent_MakeAvailable(systemc)

# Diretórios de cabeçalhos
include_directories(chip-pipeline src/components src/pipeline src/chip)

# Arquivos de fontes
file(GLOB SOURCES src/*/*.cpp)
file(GLOB_RECURSE TESTBENCH_SOURCES testbench/*.cpp)

# Gerando o executável principal
add_executable(chip-pipeline src/main.cpp ${SOURCES})

# Linkando o SystemC com o executável principal
target_link_libraries(chip-pipeline systemc)

# Gerando os executáveis para os testbenches
foreach(TESTBENCH_FILE ${TESTBENCH_SOURCES})
  # Obtém o nome do arquivo sem a extensão
  get_filename_component(TESTBENCH_NAME ${TESTBENCH_FILE} NAME_WE)

  # Adicionando o diretório dos cabeçalhos
  include_directories(${TESTBENCH_NAME} src/components src/pipeline)

  # Definindo o alvo para cada testbench
  add_executable(${TESTBENCH_NAME} ${TESTBENCH_FILE} ${SOURCES})
  
  # Linkando com o SystemC
  target_link_libraries(${TESTBENCH_NAME} systemc)
  
  # Se necessário, adicionar mais opções ou dependências de testes aqui
endforeach()
