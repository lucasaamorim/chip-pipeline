#include <systemc.h>
#include "InstructionFetchTestbench.hpp"

int sc_main(int argc, char* argv[]) {
   // Cria o módulo de testbench
   InstructionFetchTestbench testbench("testbench");

   // Inicia a simulação
   sc_start();

   return 0;
}
