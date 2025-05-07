#ifndef INSTRUCTION_FETCH_TESTBENCH_HPP
#define INSTRUCTION_FETCH_TESTBENCH_HPP

#include "InstructionFetch.hpp"
#include <systemc.h>
#include <vector>

SC_MODULE(InstructionFetchTestbench) {
   sc_clock clock;
   sc_signal<bool> reset;
   sc_signal<bool> write_enable;
   sc_signal<sc_uint<32>> instruction;

   InstructionFetch<8, 32>* fetch;

   SC_CTOR(InstructionFetchTestbench) : clock("clock", 10, SC_NS) {
      fetch = new InstructionFetch<8, 32>("instruction_fetch");
      fetch->clock(clock);
      fetch->reset(reset);
      fetch->write_enable(write_enable);
      fetch->instruction(instruction);

      SC_THREAD(run);
   }

   void run() {
      // Programa fictício
      std::vector<sc_uint<32>> program
        = { 0xDEADBEEF, 0x12345678, 0xABCDEF00, 0x0BADF00D };
      fetch->load_program(program);

      reset.write(true);
      write_enable.write(false);
      wait(2, SC_NS);

      reset.write(false);
      wait(2, SC_NS);

      write_enable.write(true); // Habilita incremento de PC
      wait(2, SC_NS);

      for (int i = 0; i < 6; ++i) {
         wait(10, SC_NS);
         std::cout << "@" << sc_time_stamp() << " | Instruction = " << std::hex
                   << instruction.read() << std::endl;
      }

      sc_stop();
   }
};

#endif // INSTRUCTION_FETCH_TESTBENCH_HPP
