#ifndef ID_EX_HPP
#define ID_EX_HPP

#include "register/Register.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

template <unsigned int DATA_BITS = 32, unsigned int PC_BITS = 8,
          unsigned int REGISTER_BITS = 4>
SC_MODULE(ID_EX) {
  sc_in_clk clock;
  sc_in<bool> reset;

  // Entradas
  sc_in<sc_uint<REGISTER_BITS>> reg_address_1_in;
  sc_in<sc_uint<REGISTER_BITS>> reg_address_2_in;
  sc_in<sc_uint<REGISTER_BITS>> reg_address_3_in;
  sc_in<sc_int<DATA_BITS>> immediate_i_in;
  sc_in<sc_uint<PC_BITS>> immediate_j_in;
  sc_in<sc_int<DATA_BITS>> read_data_1_in;
  sc_in<sc_int<DATA_BITS>> read_data_2_in;
  sc_in<bool> reg_write_enable_in;
  sc_in<bool> mem_read_enable_in;
  sc_in<bool> mem_write_enable_in;
  sc_in<bool> alu_src_in;
  sc_in<bool> jump_in;
  sc_in<bool> branch_z_in;
  sc_in<bool> branch_n_in;
  sc_in<bool> reg_dst_in;
  sc_in<sc_uint<4>> alu_op_in;
  sc_in<sc_uint<PC_BITS>> next_pc_in;

  // Saídas
  sc_out<sc_uint<REGISTER_BITS>> reg_address_1_out;
  sc_out<sc_uint<REGISTER_BITS>> reg_address_2_out;
  sc_out<sc_uint<REGISTER_BITS>> reg_address_3_out;
  sc_out<sc_int<DATA_BITS>> immediate_i_out;
  sc_out<sc_uint<PC_BITS>> immediate_j_out;
  sc_out<sc_int<DATA_BITS>> read_data_1_out;
  sc_out<sc_int<DATA_BITS>> read_data_2_out;
  sc_out<bool> reg_write_enable_out;
  sc_out<bool> mem_read_enable_out;
  sc_out<bool> mem_write_enable_out;
  sc_out<bool> alu_src_out;
  sc_out<bool> jump_out;
  sc_out<bool> branch_z_out;
  sc_out<bool> branch_n_out;
  sc_out<bool> reg_dst_out;
  sc_out<sc_uint<4>> alu_op_out;
  sc_out<sc_uint<PC_BITS>> next_pc_out;

  // Registradores
  Register<REGISTER_BITS, true> *reg_address_1, *reg_address_2, *reg_address_3;
  Register<DATA_BITS, true> *immediate_i, *read_data_1, *read_data_2;
  Register<PC_BITS, true> *immediate_j, *next_pc;
  Register<1, true> *reg_write_enable, *mem_read_enable, *mem_write_enable;
  Register<1, true> *alu_src, *jump, *branch_z, *branch_n, *reg_dst;
  Register<4, true> *alu_op;

  SC_CTOR(ID_EX) {
    // Instanciando registradores de endereços
    reg_address_1 = new Register<REGISTER_BITS, true>("reg_address_1");
    reg_address_1->clock(clock);
    reg_address_1->reset(reset);
    reg_address_1->write_enable(true); // Sempre ativa
    reg_address_1->input(reg_address_1_in);
    reg_address_1->output(reg_address_1_out);

    reg_address_2 = new Register<REGISTER_BITS, true>("reg_address_2");
    reg_address_2->clock(clock);
    reg_address_2->reset(reset);
    reg_address_2->write_enable(true);
    reg_address_2->input(reg_address_2_in);
    reg_address_2->output(reg_address_2_out);

    reg_address_3 = new Register<REGISTER_BITS, true>("reg_address_3");
    reg_address_3->clock(clock);
    reg_address_3->reset(reset);
    reg_address_3->write_enable(true);
    reg_address_3->input(reg_address_3_in);
    reg_address_3->output(reg_address_3_out);

    // Imediatos e dados lidos
    immediate_i = new Register<DATA_BITS, true>("immediate_i");
    immediate_i->clock(clock);
    immediate_i->reset(reset);
    immediate_i->write_enable(true);
    immediate_i->input(immediate_i_in);
    immediate_i->output(immediate_i_out);

    immediate_j = new Register<PC_BITS, true>("immediate_j");
    immediate_j->clock(clock);
    immediate_j->reset(reset);
    immediate_j->write_enable(true);
    immediate_j->input(immediate_j_in);
    immediate_j->output(immediate_j_out);

    read_data_1 = new Register<DATA_BITS, true>("read_data_1");
    read_data_1->clock(clock);
    read_data_1->reset(reset);
    read_data_1->write_enable(true);
    read_data_1->input(read_data_1_in);
    read_data_1->output(read_data_1_out);

    read_data_2 = new Register<DATA_BITS, true>("read_data_2");
    read_data_2->clock(clock);
    read_data_2->reset(reset);
    read_data_2->write_enable(true);
    read_data_2->input(read_data_2_in);
    read_data_2->output(read_data_2_out);

    // Sinais de controle (1 bit)
    reg_write_enable = new Register<1, true>("reg_write_enable");
    reg_write_enable->clock(clock);
    reg_write_enable->reset(reset);
    reg_write_enable->write_enable(true);
    reg_write_enable->input(reg_write_enable_in);
    reg_write_enable->output(reg_write_enable_out);

    mem_read_enable = new Register<1, true>("mem_read_enable");
    mem_read_enable->clock(clock);
    mem_read_enable->reset(reset);
    mem_read_enable->write_enable(true);
    mem_read_enable->input(mem_read_enable_in);
    mem_read_enable->output(mem_read_enable_out);

    mem_write_enable = new Register<1, true>("mem_write_enable");
    mem_write_enable->clock(clock);
    mem_write_enable->reset(reset);
    mem_write_enable->write_enable(true);
    mem_write_enable->input(mem_write_enable_in);
    mem_write_enable->output(mem_write_enable_out);

    alu_src = new Register<1, true>("alu_src");
    alu_src->clock(clock);
    alu_src->reset(reset);
    alu_src->write_enable(true);
    alu_src->input(alu_src_in);
    alu_src->output(alu_src_out);

    jump = new Register<1, true>("jump");
    jump->clock(clock);
    jump->reset(reset);
    jump->write_enable(true);
    jump->input(jump_in);
    jump->output(jump_out);

    branch_z = new Register<1, true>("branch_z");
    branch_z->clock(clock);
    branch_z->reset(reset);
    branch_z->write_enable(true);
    branch_z->input(branch_z_in);
    branch_z->output(branch_z_out);

    branch_n = new Register<1, true>("branch_n");
    branch_n->clock(clock);
    branch_n->reset(reset);
    branch_n->write_enable(true);
    branch_n->input(branch_n_in);
    branch_n->output(branch_n_out);

    reg_dst = new Register<1, true>("reg_dst");
    reg_dst->clock(clock);
    reg_dst->reset(reset);
    reg_dst->write_enable(true);
    reg_dst->input(reg_dst_in);
    reg_dst->output(reg_dst_out);

    alu_op = new Register<4, true>("alu_op");
    alu_op->clock(clock);
    alu_op->reset(reset);
    alu_op->write_enable(true);
    alu_op->input(alu_op_in);
    alu_op->output(alu_op_out);

    next_pc = new Register<PC_BITS, true>("next_pc");
    next_pc->clock(clock);
    next_pc->reset(reset);
    next_pc->write_enable(true);
    next_pc->input(next_pc_in);
    next_pc->output(next_pc_out);
  }
};

#endif // !ID_EX_HPP