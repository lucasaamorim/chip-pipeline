#include "InstructionFetch.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;
using namespace std;

SC_MODULE(InstructionFetchTb) {
  sc_clock clock;
  sc_signal<bool> reset;

  sc_signal<sc_uint<32>> instruction;
  sc_signal<sc_uint<32>> next_pc;

  InstructionFetch<32, 8> *instruction_fetch;

  SC_CTOR(InstructionFetchTb) : clock("clock", 10, SC_NS) {
    instruction_fetch = new InstructionFetch<32, 8>("InstructionFetch");
    instruction_fetch->clock(clock);
    instruction_fetch->reset(reset);
    instruction_fetch->instruction(instruction);
    instruction_fetch->next_pc(next_pc);

    SC_THREAD(run_test);
  }

  void run_test() {
    // Inicializa a memória com algumas instruções fictícias
    std::vector<sc_uint<32>> instrs = {
        0xA0000001, // instrução 1
        0xA0000002, // instrução 2
        0xA0000003, // instrução 3
        0xA0000004  // instrução 4
    };

    instruction_fetch->initialize_memory(instrs);

    // Reset inicial
    reset.write(true);
    wait(10, SC_NS); // Um ciclo de clock
    reset.write(false);

    // Aguarda alguns ciclos
    for (int i = 0; i < 4; ++i) {
      wait(10, SC_NS); // clock edge
      std::cout << "Cycle " << i + 1 << ":" << std::endl;
      std::cout << "  Instrução: 0x" << std::hex << instruction.read()
                << std::endl;
      std::cout << "  Próximo PC: " << std::dec << next_pc.read() << std::endl;
    }

    sc_stop(); // Finaliza a simulação
  }
};

int sc_main(int argc, char *argv[]) {
  InstructionFetchTb tb("InstructionFetchTb");
  sc_start();
  return 0;
}
