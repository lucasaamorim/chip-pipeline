#include <systemc.h>
#include "ProcessorTestbench.hpp"

int sc_main(int argc, char* argv[]) {
   // Cria o módulo de testbench
   ProcessorTestbench testbench("testbench");

   // Inicia a simulação
   sc_start();

   return 0;
}
