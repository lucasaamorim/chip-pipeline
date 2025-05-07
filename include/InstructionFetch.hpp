#ifndef INSTRUCTION_FETCH_HPP
#define INSTRUCTION_FETCH_HPP

#include "Memory.hpp"
#include "Register.hpp"
#include <systemc.h>

template <unsigned int ADDR_BITS = 8, unsigned int DATA_BITS = 32>
SC_MODULE(InstructionFetch) {
   sc_in<bool> clock;
   sc_in<bool> reset;
   sc_in<bool> write_enable;
   sc_out<sc_uint<DATA_BITS>> instruction;

   // Signals
   sc_signal<sc_uint<ADDR_BITS>> pc;
   sc_signal<sc_uint<ADDR_BITS>> pc_next;
   sc_signal<sc_uint<DATA_BITS>> memory_data;

   // Auxiliary signals
   sc_signal<bool> mem_write_enable;
   sc_signal<sc_uint<DATA_BITS>> dummy_input;

   // Components
   Register<ADDR_BITS>* pc_register;
   Memory<ADDR_BITS, DATA_BITS>* instruction_memory;

   SC_CTOR(InstructionFetch);

   void load_program(const std::vector<sc_uint<DATA_BITS>>& program);

   private:
   // Methods
   void assign_address();
   void increment_pc();
};

#endif // !INSTRUCTION_FETCH_HPP