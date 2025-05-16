#include "InstructionFetch.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(InstructionFetchTb) {
  sc_clock clock;
  sc_signal<bool> reset;

  sc_signal<sc_uint<1>> jump_pc_enable;
  sc_signal<sc_uint<8>> jump_pc;

  sc_signal<sc_uint<32>> instruction;
  sc_signal<sc_uint<8>> next_pc;

  InstructionFetch<32, 8> *instruction_fetch;

  SC_CTOR(InstructionFetchTb) : clock("clock", 10, SC_NS) {
    instruction_fetch = new InstructionFetch<32, 8>("InstructionFetch");
    instruction_fetch->clock(clock);
    instruction_fetch->reset(reset);

    instruction_fetch->jump_pc_enable(jump_pc_enable);
    instruction_fetch->jump_pc(jump_pc);

    instruction_fetch->instruction(instruction);
    instruction_fetch->next_pc(next_pc);

    SC_THREAD(run_test);
  }

  void run_test() {
    // Inicializa a memória com algumas instruções fictícias
    std::vector<sc_uint<32>> instrs = {
        0xA0000001, // instrução 1 (PC = 0)
        0xA0000002, // instrução 2 (PC = 1)
        0xA0000003, // instrução 3 (PC = 2)
        0xA0000004, // instrução 4 (PC = 3)
        0xA00000FF  // instrução 5 (PC = 4)
    };

    instruction_fetch->initialize_memory(instrs);

    // Reset inicial
    reset.write(true);
    jump_pc_enable.write(0);
    jump_pc.write(0);
    wait(10, SC_NS); // Um ciclo de clock

    reset.write(false);
    wait(10, SC_NS); // Primeiro ciclo de execução

    // Executa alguns ciclos sem jump
    for (int i = 0; i < 3; ++i) {
      std::cout << "Cycle " << i + 1 << ":" << std::endl;
      std::cout << "  Instrução: 0x" << std::hex << instruction.read()
                << std::endl;
      std::cout << "  Próximo PC: " << std::dec << next_pc.read() << std::endl;
      wait(10, SC_NS);
    }

    // Agora simula um jump para o endereço 4
    jump_pc_enable.write(1);
    jump_pc.write(4);
    wait(10, SC_NS); // Aplicando o jump

    // Desliga o jump enable para voltar ao fluxo normal
    jump_pc_enable.write(0);
    wait(10, SC_NS); // Mais um ciclo

    // Mostra resultado do jump
    std::cout << "Após jump:" << std::endl;
    std::cout << "  Instrução: 0x" << std::hex << instruction.read()
              << std::endl;
    std::cout << "  Próximo PC: " << std::dec << next_pc.read() << std::endl;

    sc_stop(); // Finaliza a simulação
  }
};

int sc_main(int argc, char *argv[]) {
  InstructionFetchTb tb("InstructionFetchTb");
  sc_start();
  return 0;
}
