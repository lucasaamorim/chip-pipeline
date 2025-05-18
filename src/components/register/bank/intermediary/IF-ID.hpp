#ifndef IF_ID_HPP
#define IF_ID_HPP

#include "register/Register.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

template <unsigned int DATA_BITS = 32, unsigned int PC_BITS = 8>
SC_MODULE(IF_ID) {
  sc_in_clk clock;
  sc_in<bool> reset;
  sc_in<bool> write_enable;

  sc_in<sc_uint<DATA_BITS>> instruction_in;
  sc_in<sc_uint<PC_BITS>> next_pc_in;

  sc_out<sc_uint<DATA_BITS>> instruction_out;
  sc_out<sc_uint<PC_BITS>> next_pc_out;

  Register<DATA_BITS, true, sc_uint<DATA_BITS>> *instruction;
  Register<PC_BITS, true, sc_uint<PC_BITS>> *next_pc;

  SC_CTOR(IF_ID) {
    /// Inicializando o registrador de instruções
    instruction = new Register<DATA_BITS, true, sc_uint<DATA_BITS>>("instruction");
    instruction->clock(clock);
    instruction->reset(reset);
    instruction->write_enable(write_enable);
    instruction->input(instruction_in);
    instruction->output(instruction_out);

    /// Inicializando o registrador de PC
    next_pc = new Register<PC_BITS, true, sc_uint<PC_BITS>>("next_pc");
    next_pc->clock(clock);
    next_pc->reset(reset);
    next_pc->write_enable(write_enable);
    next_pc->input(next_pc_in);
    next_pc->output(next_pc_out);
  }
};
#endif // !IF_ID_HPP