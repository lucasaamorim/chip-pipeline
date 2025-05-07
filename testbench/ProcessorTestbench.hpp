#ifndef PROCESSOR_TESTBENCH_HPP
#define PROCESSOR_TESTBENCH_HPP

#include "Processor.hpp"
#include <systemc.h>
#include <vector>

SC_MODULE(ProcessorTestbench) {
   sc_clock clock;
   sc_signal<bool> reset;

   // Sinais do processador
   sc_signal<sc_uint<32>> instruction;
   sc_signal<sc_uint<32>> write_data;
   sc_signal<sc_uint<32>> first_read_data;
   sc_signal<sc_uint<32>> second_read_data;
   sc_signal<bool> write_enable;

   Processor* processor;

   SC_CTOR(ProcessorTestbench) : clock("clock", 10, SC_NS) {
      processor = new Processor("processor");
      processor->clock(clock);
      processor->reset(reset);

      processor->instruction(instruction);
      processor->write_data(write_data);
      processor->first_read_data(first_read_data);
      processor->second_read_data(second_read_data);
      processor->write_enable(write_enable);

      SC_THREAD(run);
   }

   void load_program(const std::vector<sc_uint<32>>& program);
   void run();
};

void ProcessorTestbench::load_program(const std::vector<sc_uint<32>>& program) {
   // Inicializa a memória de instruções do processador
   processor->load_program(program);
}

void ProcessorTestbench::run() {
   // Programa fictício com instruções de exemplo
   std::vector<sc_uint<32>> program = {
      0x00000001, // Instrução fictícia (exemplo)
      0x00000002 // Outra instrução fictícia
   };

   // Carrega o programa na memória de instruções
   load_program(program);

   // Reset inicial
   reset.write(true);
   wait(10, SC_NS);
   reset.write(false);
   wait(10, SC_NS);

   // Ativa a leitura de instruções e o incremento do PC
   write_enable.write(true);
   wait(10, SC_NS);

   // Simula a execução por alguns ciclos
   for (int i = 0; i < 6; ++i) {
      wait(10, SC_NS);
      std::cout << "@" << sc_time_stamp() << " | Instruction: 0x" << std::hex
                << instruction.read() << " | R1: " << first_read_data.read()
                << " | R2: " << second_read_data.read() << std::endl;
   }

   // Finaliza a simulação
   sc_stop();
}

#endif // PROCESSOR_TESTBENCH_HPP
