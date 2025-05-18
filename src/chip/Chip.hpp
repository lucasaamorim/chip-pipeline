#ifndef CHIP_HPP
#define CHIP_HPP

#include "InstructionDecoder.hpp"
#include "InstructionFetch.hpp"
#include "register/bank/intermediary/IF-ID.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

constexpr unsigned int DATA_BITS = 32;
constexpr unsigned int PC_ADDRESS_BITS = 8;
constexpr unsigned int REGISTER_BITS = 4;

SC_MODULE(Chip) {
  /// Entradas
  sc_in_clk clock;
  sc_in<bool> reset;

  ///! Sinais
  ///@ Fase IF
  ///*  Entradas
  sc_signal<sc_uint<1>> jump_pc_enable_if;
  sc_signal<sc_uint<PC_ADDRESS_BITS>> jump_pc_if;

  ///*  Saídas
  sc_signal<sc_uint<PC_ADDRESS_BITS>> next_pc_if;
  sc_signal<sc_uint<DATA_BITS>> instruction_if;

  ///@ Fase ID
  ///*  Entradas
  sc_signal<bool> write_enable_id;
  sc_signal<sc_uint<PC_ADDRESS_BITS>> next_pc_id;
  sc_signal<sc_uint<DATA_BITS>> instruction_id;
  sc_signal<sc_uint<REGISTER_BITS>> write_address_in_id;
  sc_signal<sc_int<DATA_BITS>> write_data_id;
  sc_signal<sc_int<DATA_BITS>> alu_result_id;

  ///*  Saídas
  sc_signal<sc_uint<REGISTER_BITS>> reg_address_1_id;
  sc_signal<sc_uint<REGISTER_BITS>> reg_address_2_id;
  sc_signal<sc_uint<REGISTER_BITS>> reg_address_3_id;
  sc_signal<sc_int<DATA_BITS>> immediate_i_id;
  sc_signal<sc_int<DATA_BITS>> read_data_1_id;
  sc_signal<sc_int<DATA_BITS>> read_data_2_id;
  sc_signal<bool> reg_write_enable_id;
  sc_signal<bool> mem_read_enable_id;
  sc_signal<bool> mem_write_enable_id;
  sc_signal<bool> alu_src_id;
  sc_signal<bool> reg_dst_id;
  sc_signal<sc_uint<4>> alu_op_id;

  ///! Componentes
  ///@ Fase IF
  InstructionFetch<DATA_BITS, PC_ADDRESS_BITS> *instruction_fetch;
  IF_ID<DATA_BITS, PC_ADDRESS_BITS> *if_id;

  ///@ Fase ID
  InstructionDecoder<DATA_BITS, REGISTER_BITS, PC_ADDRESS_BITS, DATA_BITS>
      *instruction_decoder;

  SC_CTOR(Chip) {
    ///! Fase IF
    ///@ Instruction Fetch
    instruction_fetch =
        new InstructionFetch<DATA_BITS, PC_ADDRESS_BITS>("instruction_fetch");
    instruction_fetch->clock(clock);
    instruction_fetch->reset(reset);

    instruction_fetch->jump_pc_enable(jump_pc_enable_if);
    instruction_fetch->jump_pc(jump_pc_if);
    instruction_fetch->instruction(instruction_if);
    instruction_fetch->next_pc(next_pc_if);

    ///@ IF-ID
    if_id = new IF_ID<DATA_BITS, PC_ADDRESS_BITS>("if_id");
    if_id->clock(clock);
    if_id->reset(reset);

    if_id->instruction_in(instruction_if);
    if_id->next_pc_in(next_pc_if);

    if_id->instruction_out(instruction_id);
    if_id->next_pc_out(next_pc_id);

    ///! Fase ID
    ///@ Instruction Decoder
    instruction_decoder =
        new InstructionDecoder<DATA_BITS, REGISTER_BITS, PC_ADDRESS_BITS,
                               DATA_BITS>("instruction_decoder");
    instruction_decoder->clock(clock);
    instruction_decoder->reset(reset);

    instruction_decoder->write_enable(write_enable_id);
    instruction_decoder->instruction(instruction_id);
    instruction_decoder->next_pc(next_pc_id);
    instruction_decoder->write_address_in(write_address_in_id);
    instruction_decoder->write_data(write_data_id);
    instruction_decoder->alu_result(alu_result_id);

    instruction_decoder->reg_address_1(reg_address_1_id);
    instruction_decoder->reg_address_2(reg_address_2_id);
    instruction_decoder->reg_address_3(reg_address_3_id);
    instruction_decoder->immediate_i(immediate_i_id);
    instruction_decoder->read_data_1(read_data_1_id);
    instruction_decoder->read_data_2(read_data_2_id);
    instruction_decoder->reg_write_enable(reg_write_enable_id);
    instruction_decoder->mem_read_enable(mem_read_enable_id);
    instruction_decoder->mem_write_enable(mem_write_enable_id);
    instruction_decoder->alu_src(alu_src_id);
    instruction_decoder->reg_dst(reg_dst_id);
    instruction_decoder->alu_op(alu_op_id);

    instruction_decoder->jump_pc_enable(jump_pc_enable_if);
    instruction_decoder->next_pc_out(next_pc_if);
  }

  void initialize_instruction_memory(std::vector<sc_uint<DATA_BITS>> data) {
    instruction_fetch->initialize_memory(data);
  }
};

#endif // !CHIP_HPP