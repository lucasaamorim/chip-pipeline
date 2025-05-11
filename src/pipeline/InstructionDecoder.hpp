#ifndef INSTRUCT_DECODER_HPP
#define INSTRUCT_DECODER_HPP

#include "register/bank/RegisterBank.hpp"
#include <iostream>
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

template <unsigned int DATA_BITS = 32, unsigned int REGISTER_BITS = 4,
          unsigned int PC_ADDRESS_BITS = 8, unsigned int INSTRUCTION_BITS = 32>
SC_MODULE(InstructionDecoder) {
  static const unsigned int OPCODE_BITS = 6;

  sc_in<bool> clock;
  sc_in<bool> reset;
  sc_in<bool> write_enable;

  sc_in<sc_uint<INSTRUCTION_BITS>> instruction;
  sc_in<sc_uint<PC_ADDRESS_BITS>> next_pc;
  sc_in<sc_uint<REGISTER_BITS>> write_address_in;
  sc_in<sc_uint<DATA_BITS>> write_data;

  sc_out<sc_uint<DATA_BITS>> read_data_1;
  sc_out<sc_uint<DATA_BITS>> read_data_2;
  sc_out<sc_uint<REGISTER_BITS>> reg_address_1;
  sc_out<sc_uint<REGISTER_BITS>> reg_address_2;
  sc_out<sc_uint<REGISTER_BITS>> reg_address_3;
  sc_out<sc_uint<DATA_BITS>> immediate_i;
  sc_out<sc_uint<PC_ADDRESS_BITS>> immediate_j;

  RegisterBank<DATA_BITS, REGISTER_BITS> *register_bank;

  void process() {
    int opcode_init = DATA_BITS - 1;
    int opcode_end = DATA_BITS - OPCODE_BITS;

    int rs_init = opcode_end - 1;
    int rs_end = opcode_end - REGISTER_BITS;
    int rt_init = rs_end - 1;
    int rt_end = rs_end - REGISTER_BITS;
    int rd_init = rt_end - 1;
    int rd_end = rt_end - REGISTER_BITS;

    int imm_init = rt_end - 1;
    int imm_end = 0;

    int immj_init = opcode_end - 1;
    int immj_end = opcode_end - PC_ADDRESS_BITS;

    reg_address_1.write(instruction.read().range(rs_init, rs_end));
    reg_address_2.write(instruction.read().range(rt_init, rt_end));
    reg_address_3.write(instruction.read().range(rd_init, rd_end));
    immediate_i.write(instruction.read().range(imm_init, imm_end));
    immediate_j.write(instruction.read().range(immj_init, immj_end));
  }

  SC_CTOR(InstructionDecoder) {
    register_bank = new RegisterBank<DATA_BITS, REGISTER_BITS>("register_bank");

    register_bank->clock(clock);
    register_bank->reset(reset);
    register_bank->write_enable(write_enable);
    register_bank->read_address_1(reg_address_1);
    register_bank->read_address_2(reg_address_2);
    register_bank->write_address(write_address_in);
    register_bank->input(write_data);
    register_bank->output_1(read_data_1);
    register_bank->output_2(read_data_2);

    SC_METHOD(process);
    sensitive << clock.pos();
  }
};

#endif // !INSTRUCT_DECODER_HPP
