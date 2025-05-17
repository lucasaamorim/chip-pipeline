#ifndef EXECUTE_HPP
#define EXECUTE_HPP

#include "alu/ALU.hpp"
#include "mux/Multiplexer.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

template <unsigned int DATA_BITS = 32, unsigned int REGISTER_BITS = 4>
SC_MODULE(Execute) {
  // Clock e reset
  sc_in_clk clock;
  sc_in<bool> reset;

  // Entradas de dados
  sc_in<sc_int<DATA_BITS>> read_data_1;
  sc_in<sc_int<DATA_BITS>> read_data_2;
  sc_in<sc_int<DATA_BITS>> immediate_i;

  // Registradores da instrução (endereços)
  sc_in<sc_uint<REGISTER_BITS>> reg_address_2;
  sc_in<sc_uint<REGISTER_BITS>> reg_address_3;

  // Controle
  sc_in<sc_uint<1>> alu_src;
  sc_in<sc_uint<1>> reg_dst;
  sc_in<sc_uint<4>> alu_op;

  // Saídas
  sc_out<sc_int<DATA_BITS>> alu_result;
  sc_out<sc_uint<REGISTER_BITS>> write_register;

  // Componentes internos
  ALU<DATA_BITS> *alu;
  Multiplexer<DATA_BITS, 1, sc_int<DATA_BITS>> *alu_mux;
  Multiplexer<REGISTER_BITS, 1, sc_uint<REGISTER_BITS>> *reg_dst_mux;

    // Sinais intermediários para conexão
  sc_signal<sc_int<DATA_BITS>> alu_mux_out_sig;
  sc_signal<sc_int<DATA_BITS>> alu_result_sig;
  sc_signal<sc_uint<REGISTER_BITS>> reg_dst_mux_out_sig;
  sc_signal<bool> zero_sig;
  sc_signal<bool> negative_sig;

  // Sinais intermediários para conexão
  void assign_outputs() {
    if (reset.read()) {
      alu_result.write(0);
      write_register.write(0);
    } else {
      alu_result.write(alu_result_sig.read());
      write_register.write(reg_dst_mux_out_sig.read());
    }
  }

  void exec_process() {
    // Método vazio, apenas para garantir avaliação da ALU combinacional
  }


  SC_CTOR(Execute) {
    // Instancia mux para alu_src
    alu_mux = new Multiplexer<DATA_BITS, 1, sc_int<DATA_BITS>>("alu_mux");
    alu_mux->clock(clock);
    alu_mux->reset(reset);
    alu_mux->inputs[0](read_data_2);
    alu_mux->inputs[1](immediate_i);
    alu_mux->select(alu_src);
    alu_mux->output(alu_mux_out_sig);

    // Instancia mux para reg_dst
    reg_dst_mux = new Multiplexer<REGISTER_BITS, 1, sc_uint<REGISTER_BITS>>("reg_dst_mux");
    reg_dst_mux->clock(clock);
    reg_dst_mux->reset(reset);
    reg_dst_mux->inputs[0](reg_address_2);
    reg_dst_mux->inputs[1](reg_address_3);
    reg_dst_mux->select(reg_dst);
    reg_dst_mux->output(reg_dst_mux_out_sig);

    // Instancia ALU
    alu = new ALU<DATA_BITS>("alu");
    alu->operand_a_in(read_data_1);
    alu->operand_b_in(alu_mux_out_sig);
    alu->operation_select_in(alu_op);
    alu->result_out(alu_result_sig);
    alu->zero_flag_out(zero_sig);
    alu->negative_flag_out(negative_sig);

    // Assign das saídas 
    SC_METHOD(assign_outputs);
    sensitive << alu_result_sig << reg_dst_mux_out_sig << zero_sig << negative_sig << reset;

    SC_METHOD(exec_process);
    sensitive << read_data_1 << alu_mux_out_sig << alu_op;
  }
};

#endif // EXECUTE_HPP
