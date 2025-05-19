#ifndef INSTRUCTION_MEMORY_HPP
#define INSTRUCTION_MEMORY_HPP

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(InstructionMemory) {
   sc_in_clk clk;
   sc_in<bool> reset;
   sc_in<sc_uint<32>> address;
   sc_out<sc_uint<32>> instruction;

   sc_uint<32> memory[256];

   void next_instruction() {
      if (reset.read()) {
         instruction.write(0);
      } else {
         instruction.write(memory[address.read() / 4]);
      }
   }

   void write(sc_uint<32> addr, sc_uint<32> value) { memory[addr] = value; }

   SC_CTOR(InstructionMemory) {
      SC_METHOD(next_instruction);
      sensitive << address << clk.pos();
   }
};
#endif // !INSTRUCTION_MEMORY_HPP