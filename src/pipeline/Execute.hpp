#ifndef EXECUTE_HPP
#define EXECUTE_HPP

#include "alu/ALU.hpp"
#include "mux/Multiplexer.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

template <unsigned int DATA_BITS = 32> SC_MODULE(Execute) {
  /// Entradas básicas
  sc_in_clk clock;
  sc_in<bool> reset;

  /// Entradas de fluxo
  sc_in<sc_uint<REGISTER_BITS>> reg_address_1;
  sc_in<sc_uint<REGISTER_BITS>> reg_address_2;
  sc_in<sc_uint<REGISTER_BITS>> reg_address_3;
  sc_in<bool> reg_write_enable;
  sc_in<bool> mem_read_enable;
  sc_in<bool> mem_write_enable;
  sc_out<bool> alu_src;
  sc_out<bool> jump;
  sc_out<bool> branch_z;
  sc_out<bool> branch_n;
  sc_out<bool> reg_dst;
  sc_out<sc_uint<4>> alu_op;

  /// Entradas de dados
  sc_in<sc_uint<DATA_BITS>> read_data_1;
  sc_in<sc_uint<DATA_BITS>> read_data_2;
  sc_in<sc_uint<DATA_BITS>> immediate_i;
  sc_in<sc_uint<PC_BITS>> immediate_j;

  ALU<DATA_BITS> *alu;
  Multiplexer<DATA_BITS, 1> *data_1_mux, *data_2_mux;
};

#endif // !EXECUTE_HPP
