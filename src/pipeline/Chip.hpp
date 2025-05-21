#ifndef CHIP_HPP
#define CHIP_HPP

#include "data_memory/DataMemory.hpp"
#include "decoder/Decoder.hpp"
#include "hazard/HazardForward.hpp"
#include "instruction_memory/InstructionMemory.hpp"
#include "mux/Mux.hpp"
#include "pc/PC.hpp"
#include "register/EX_MEM.hpp"
#include "register/ID_EX.hpp"
#include "register/IF_ID.hpp"
#include "register/MEM_WB.hpp"
#include "register_bank/RegisterBank.hpp"
#include "ula/ULA.hpp"
#include <iostream>
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(Chip) {
   sc_in_clk clk;
   sc_in<bool> reset;

   /// MUX PC
   Mux<32, 1>* pc_mux;
   sc_signal<sc_uint<32>> pc_mux_out_signal;
   sc_signal<sc_uint<1>> pc_mux_select;

   /// PC
   PC* pc;
   sc_signal<sc_uint<32>> pc_in_signal;
   sc_signal<sc_uint<32>> pc_out_signal;
   sc_signal<sc_uint<32>> next_pc;
   sc_signal<sc_uint<32>> jump_pc;

   /// Instruction Memory
   InstructionMemory* instruction_memory;
   sc_signal<sc_uint<32>> instruction_memory_signal;

   /// IF_ID
   IF_ID* if_id;
   sc_signal<sc_uint<32>> if_id_pc_signal;
   sc_signal<sc_uint<32>> if_id_instruction_signal;
   sc_signal<bool> if_id_flush;
   sc_signal<bool> if_id_stall;

   /// Decoder
   Decoder* decoder;
   sc_signal<sc_uint<4>> decoder_rs_signal, decoder_rt_signal,
     decoder_rd_signal;
   sc_signal<sc_uint<32>> decoder_immediate_i_signal,
     decoder_immediate_j_signal;
   sc_signal<bool> decoder_reg_dst_signal, decoder_alu_src_signal,
     decoder_mem_to_reg_signal, decoder_reg_write_signal,
     decoder_mem_read_signal, decoder_mem_write_signal, decoder_jump_signal,
     decoder_jump_n_signal, decoder_jump_z_signal;
   sc_signal<sc_uint<3>> decoder_alu_op_signal;

   /// Register Bank
   RegisterBank* register_bank;
   sc_signal<sc_uint<4>> rb_read_address_1_signal, rb_read_address_2_signal;
   sc_signal<sc_uint<32>> rb_data_out_1_signal, rb_data_out_2_signal;

   /// ID_EX
   ID_EX* id_ex;
   sc_signal<sc_uint<32>> id_ex_data_out_1_signal, id_ex_data_out_2_signal;
   sc_signal<sc_uint<4>> id_ex_rs_out, id_ex_rt_out, id_ex_rd_out;
   sc_signal<sc_uint<32>> id_ex_immediate_out;
   sc_signal<bool> id_ex_reg_dst_out, id_ex_alu_src_out, id_ex_mem_to_reg_out,
     id_ex_reg_write_out, id_ex_mem_read_out, id_ex_mem_write_out;
   sc_signal<sc_uint<3>> id_ex_alu_op_out;

   /// MUX EX_RD
   Mux<4, 2>* mux_ex_rd;
   sc_signal<sc_uint<2>> mux_ex_rd_sel_signal;
   sc_signal<sc_uint<4>> mux_ex_rd_out_signal, mux_ex_rd_source_signal;

   /// Hazard Forward
   HazardForward* hazard_forward;
   sc_signal<sc_uint<2>> rs_sel_signal, rt_sel_signal, rs_data_signal;

   /// MUX EX_RS_VAL
   Mux<32, 2>* mux_ex_rs_val;
   sc_signal<sc_uint<32>> mux_ex_rs_val_out_signal;

   /// MUX EX_RT_VAL
   Mux<32, 2>* mux_ex_rt_val;
   sc_signal<sc_uint<32>> mux_ex_rt_val_out_signal;

   /// MUX EX_REG_DATA
   Mux<32, 2>* mux_ex_reg_data;
   sc_signal<sc_uint<32>> mux_ex_reg_data_out_signal;

   /// ULA
   ULA* ula;
   sc_signal<sc_uint<32>> ula_result_signal;

   /// EX_MEM
   EX_MEM* ex_mem;
   sc_signal<sc_uint<32>> ex_mem_ula_result_out_signal,
     ex_mem_data_out_1_signal;
   sc_signal<sc_uint<4>> ex_mem_rd_out_signal;
   sc_signal<bool> ex_mem_mem_to_reg_out_signal, ex_mem_reg_write_out_signal,
     ex_mem_mem_read_out_signal, ex_mem_mem_write_out_signal;

   /// Data Memory
   DataMemory* data_memory;
   sc_signal<sc_uint<32>> data_memory_data_signal;

   /// MEM_WB
   MEM_WB* mem_wb;
   sc_signal<sc_uint<32>> mem_wb_data_out_signal, mem_wb_ula_result_out_signal;
   sc_signal<sc_uint<4>> mem_wb_rd_out_signal;
   sc_signal<bool> mem_wb_mem_to_reg_out_signal, mem_wb_reg_write_out_signal;

   /// MUX MEM_TO_REG
   Mux<32, 1>* mux_wb_val;
   sc_signal<sc_uint<1>> mux_wb_val_sel_signal;
   sc_signal<sc_uint<32>> mux_wb_val_out_signal;

   /// Sinais TEMP
   sc_signal<bool> pc_enable;
   sc_signal<sc_uint<32>> mux_source;

   void next_pc_count() { next_pc.write(pc_out_signal.read() + 4); }

   void check_jump() {
      jump_pc.write(pc_out_signal.read() + decoder_immediate_j_signal.read() + 4);

      if (decoder_jump_signal.read()) {
         pc_mux_select.write(1);
      } else if (decoder_jump_n_signal.read() && ula_result_signal.read() < 0) {
         pc_mux_select.write(1);
      } else if (decoder_jump_z_signal.read()
        && ula_result_signal.read() == 0) {
         pc_mux_select.write(1);
      } else {
         pc_mux_select.write(0);
      }
   }

   void select_reg_dst() {
      if (id_ex_mem_to_reg_out.read()) {
         mux_ex_rd_sel_signal.write(0);
      } else if (id_ex_reg_dst_out.read()) {
         mux_ex_rd_sel_signal.write(1);
      } else {
         mux_ex_rd_sel_signal.write(2);
      }
   }

   void select_reg_data_write() {
      if (mem_wb_mem_to_reg_out_signal.read()) {
         mux_wb_val_sel_signal.write(1);
      } else {
         mux_wb_val_sel_signal.write(0);
      }
   }

   void print_data_write() {
      //  std::cout << "Clock: " << sc_time_stamp() << std::endl;
      //  std::cout << "Data para escrever na memória: " <<
      //  ex_mem_data_out_1_signal.read() << std::endl; std::cout << "Endereço
      //  da memória: " << ex_mem_ula_result_out_signal.read() << std::endl;
      //  std::cout << "Sinal mem_write: " << ex_mem_mem_write_out_signal.read()
      //  << std::endl;
   }

   SC_CTOR(Chip) {
      /// Sinais TEMP
      pc_enable.write(true);
      mux_source.write(0);

      /// PC MUX
      pc_mux = new Mux<32, 1>("pc_mux");
      pc_mux->clk(clk);
      pc_mux->reset(reset);
      pc_mux->in[0](next_pc);
      pc_mux->in[1](jump_pc);
      pc_mux->sel(pc_mux_select);
      pc_mux->out(pc_in_signal);

      /// PC
      pc = new PC("pc");
      pc->clk(clk);
      pc->reset(reset);
      pc->enable(pc_enable);
      pc->in(pc_in_signal);
      pc->out(pc_out_signal);

      /// Instruction Memory
      instruction_memory = new InstructionMemory("instruction_memory");
      instruction_memory->clk(clk);
      instruction_memory->reset(reset);
      instruction_memory->address(pc_out_signal);
      instruction_memory->instruction(instruction_memory_signal);

      /// IF_ID
      if_id = new IF_ID("if_id");
      if_id->clock(clk);
      if_id->reset(reset);
      if_id->pc_in(pc_out_signal);
      if_id->pc_out(if_id_pc_signal);
      if_id->instruction_in(instruction_memory_signal);
      if_id->instruction_out(if_id_instruction_signal);
      if_id->flush(if_id_flush);
      if_id->stall(if_id_stall);

      /// Decoder
      decoder = new Decoder("decoder");
      decoder->clk(clk);
      decoder->reset(reset);
      decoder->instruction(if_id_instruction_signal);
      decoder->rs(decoder_rs_signal);
      decoder->rt(decoder_rt_signal);
      decoder->rd(decoder_rd_signal);
      decoder->immediate_i(decoder_immediate_i_signal);
      decoder->immediate_j(decoder_immediate_j_signal);
      decoder->reg_dst(decoder_reg_dst_signal);
      decoder->alu_src(decoder_alu_src_signal);
      decoder->mem_to_reg(decoder_mem_to_reg_signal);
      decoder->reg_write(decoder_reg_write_signal);
      decoder->mem_read(decoder_mem_read_signal);
      decoder->mem_write(decoder_mem_write_signal);
      decoder->jump(decoder_jump_signal);
      decoder->jump_n(decoder_jump_n_signal);
      decoder->jump_z(decoder_jump_z_signal);
      decoder->alu_op(decoder_alu_op_signal);

      /// Register Bank
      register_bank = new RegisterBank("register_bank");
      register_bank->clk(clk);
      register_bank->reset(reset);
      register_bank->read_address_1(decoder_rs_signal);
      register_bank->read_address_2(decoder_rt_signal);
      register_bank->write_enable(mem_wb_reg_write_out_signal);
      register_bank->write_address(mem_wb_rd_out_signal);
      register_bank->data_in(mux_wb_val_out_signal);
      register_bank->data_out_1(rb_data_out_1_signal);
      register_bank->data_out_2(rb_data_out_2_signal);

      /// ID/EX
      id_ex = new ID_EX("id_ex");
      id_ex->clk(clk);
      id_ex->reset(reset);
      id_ex->data_in_1(rb_data_out_1_signal);
      id_ex->data_in_2(rb_data_out_2_signal);
      id_ex->data_out_1(id_ex_data_out_1_signal);
      id_ex->data_out_2(id_ex_data_out_2_signal);
      id_ex->rs_in(decoder_rs_signal);
      id_ex->rt_in(decoder_rt_signal);
      id_ex->rd_in(decoder_rd_signal);
      id_ex->rs_out(id_ex_rs_out);
      id_ex->rt_out(id_ex_rt_out);
      id_ex->rd_out(id_ex_rd_out);
      id_ex->immediate_in(decoder_immediate_i_signal);
      id_ex->immediate_out(id_ex_immediate_out);
      id_ex->reg_dst_in(decoder_reg_dst_signal);
      id_ex->reg_dst_out(id_ex_reg_dst_out);
      id_ex->alu_src_in(decoder_alu_src_signal);
      id_ex->alu_src_out(id_ex_alu_src_out);
      id_ex->mem_to_reg_in(decoder_mem_to_reg_signal);
      id_ex->mem_to_reg_out(id_ex_mem_to_reg_out);
      id_ex->reg_write_in(decoder_reg_write_signal);
      id_ex->reg_write_out(id_ex_reg_write_out);
      id_ex->mem_read_in(decoder_mem_read_signal);
      id_ex->mem_read_out(id_ex_mem_read_out);
      id_ex->mem_write_in(decoder_mem_write_signal);
      id_ex->mem_write_out(id_ex_mem_write_out);
      id_ex->alu_op_in(decoder_alu_op_signal);
      id_ex->alu_op_out(id_ex_alu_op_out);

      /// MUX EX_RD
      mux_ex_rd = new Mux<4, 2>("mux_ex_rd");
      mux_ex_rd->clk(clk);
      mux_ex_rd->reset(reset);
      mux_ex_rd->in[0](id_ex_rs_out);
      mux_ex_rd->in[1](id_ex_rd_out);
      mux_ex_rd->in[2](id_ex_rt_out);
      mux_ex_rd->in[3](mux_ex_rd_source_signal);
      mux_ex_rd->sel(mux_ex_rd_sel_signal);
      mux_ex_rd->out(mux_ex_rd_out_signal);

      /// HAZARD FORWARD
      hazard_forward = new HazardForward("hazard_forward");
      hazard_forward->clk(clk);
      hazard_forward->reset(reset);
      hazard_forward->rs_in(id_ex_rs_out);
      hazard_forward->rt_in(id_ex_rt_out);
      hazard_forward->rd_1_in(ex_mem_rd_out_signal);
      hazard_forward->rd_2_in(mem_wb_rd_out_signal);
      hazard_forward->reg_write_1(ex_mem_reg_write_out_signal);
      hazard_forward->reg_write_2(mem_wb_reg_write_out_signal);
      hazard_forward->alu_src(id_ex_alu_src_out);
      hazard_forward->read_mem(id_ex_mem_read_out);
      hazard_forward->write_mem(id_ex_mem_write_out);
      hazard_forward->rs_sel(rs_sel_signal);
      hazard_forward->rt_sel(rt_sel_signal);
      hazard_forward->rs_data(rs_data_signal);

      /// MUX EX_RS_VAL
      mux_ex_rs_val = new Mux<32, 2>("mux_ex_rs_val");
      mux_ex_rs_val->clk(clk);
      mux_ex_rs_val->reset(reset);
      mux_ex_rs_val->in[0](id_ex_data_out_1_signal);
      mux_ex_rs_val->in[1](ex_mem_ula_result_out_signal);
      mux_ex_rs_val->in[2](mem_wb_ula_result_out_signal);
      mux_ex_rs_val->in[3](id_ex_immediate_out);
      mux_ex_rs_val->sel(rs_sel_signal);
      mux_ex_rs_val->out(mux_ex_rs_val_out_signal);

      /// MUX EX_RT_VAL
      mux_ex_rt_val = new Mux<32, 2>("mux_ex_rt_val");
      mux_ex_rt_val->clk(clk);
      mux_ex_rt_val->reset(reset);
      mux_ex_rt_val->in[0](id_ex_data_out_2_signal);
      mux_ex_rt_val->in[1](ex_mem_ula_result_out_signal);
      mux_ex_rt_val->in[2](mem_wb_ula_result_out_signal);
      mux_ex_rt_val->in[3](id_ex_immediate_out);
      mux_ex_rt_val->sel(rt_sel_signal);
      mux_ex_rt_val->out(mux_ex_rt_val_out_signal);

      /// MUX EX_REG_DATA
      mux_ex_reg_data = new Mux<32, 2>("mux_ex_reg_data");
      mux_ex_reg_data->clk(clk);
      mux_ex_reg_data->reset(reset);
      mux_ex_reg_data->in[0](id_ex_data_out_1_signal);
      mux_ex_reg_data->in[1](ex_mem_ula_result_out_signal);
      mux_ex_reg_data->in[2](mem_wb_ula_result_out_signal);
      mux_ex_reg_data->in[3](mux_source); /// Não usa
      mux_ex_reg_data->sel(rs_data_signal);
      mux_ex_reg_data->out(mux_ex_reg_data_out_signal);

      /// ULA
      ula = new ULA("ula");
      ula->clk(clk);
      ula->reset(reset);
      ula->in1(mux_ex_rs_val_out_signal);
      ula->in2(mux_ex_rt_val_out_signal);
      ula->func(id_ex_alu_op_out);
      ula->out(ula_result_signal);

      /// EX_MEM
      ex_mem = new EX_MEM("ex_mem");
      ex_mem->clk(clk);
      ex_mem->reset(reset);
      ex_mem->ula_result_in(ula_result_signal);
      ex_mem->data_in_1(mux_ex_reg_data_out_signal);
      ex_mem->ula_result_out(ex_mem_ula_result_out_signal);
      ex_mem->data_out_1(ex_mem_data_out_1_signal);
      ex_mem->rd_in(mux_ex_rd_out_signal);
      ex_mem->rd_out(ex_mem_rd_out_signal);
      ex_mem->mem_to_reg_in(id_ex_mem_to_reg_out);
      ex_mem->mem_to_reg_out(ex_mem_mem_to_reg_out_signal);
      ex_mem->reg_write_in(id_ex_reg_write_out);
      ex_mem->reg_write_out(ex_mem_reg_write_out_signal);
      ex_mem->mem_read_in(id_ex_mem_read_out);
      ex_mem->mem_read_out(ex_mem_mem_read_out_signal);
      ex_mem->mem_write_in(id_ex_mem_write_out);
      ex_mem->mem_write_out(ex_mem_mem_write_out_signal);

      /// Data Memory
      data_memory = new DataMemory("data_memory");
      data_memory->clk(clk);
      data_memory->reset(reset);
      data_memory->mem_write(ex_mem_mem_write_out_signal);
      data_memory->mem_read(ex_mem_mem_read_out_signal);
      data_memory->address(ex_mem_ula_result_out_signal);
      data_memory->write_data(ex_mem_data_out_1_signal);
      data_memory->data(data_memory_data_signal);

      /// MEM_WB
      mem_wb = new MEM_WB("mem_wb");
      mem_wb->clk(clk);
      mem_wb->reset(reset);
      mem_wb->ula_result_in(ex_mem_ula_result_out_signal);
      mem_wb->data_in_1(data_memory_data_signal);
      mem_wb->ula_result_out(mem_wb_ula_result_out_signal);
      mem_wb->data_out_1(mem_wb_data_out_signal);
      mem_wb->rd_in(ex_mem_rd_out_signal);
      mem_wb->rd_out(mem_wb_rd_out_signal);
      mem_wb->reg_write_in(ex_mem_reg_write_out_signal);
      mem_wb->reg_write_out(mem_wb_reg_write_out_signal);
      mem_wb->mem_to_reg_in(ex_mem_mem_to_reg_out_signal);
      mem_wb->mem_to_reg_out(mem_wb_mem_to_reg_out_signal);

      /// MUX WB_VAL
      mux_wb_val = new Mux<32, 1>("mux_wb_val");
      mux_wb_val->clk(clk);
      mux_wb_val->reset(reset);
      mux_wb_val->in[0](mem_wb_ula_result_out_signal);
      mux_wb_val->in[1](mem_wb_data_out_signal);
      mux_wb_val->sel(mux_wb_val_sel_signal);
      mux_wb_val->out(mux_wb_val_out_signal);

      SC_METHOD(next_pc_count);
      sensitive << pc_out_signal << clk.pos();

      SC_METHOD(check_jump);
      sensitive << decoder_jump_signal << decoder_jump_n_signal
                << decoder_jump_z_signal << ula_result_signal << clk.pos();

      SC_METHOD(select_reg_dst);
      sensitive << id_ex_reg_dst_out << id_ex_mem_to_reg_out << clk.pos();

      SC_METHOD(select_reg_data_write);
      sensitive << mem_wb_mem_to_reg_out_signal << clk.pos();

      SC_METHOD(print_data_write);
      sensitive << clk.pos();
   }

   void initialize_instructions(std::vector<sc_uint<32>> instructions) {
      for (int i = 0; i < instructions.size(); i++) {
         instruction_memory->write(i, instructions[i]);
      }
   }
};

#endif // !CHIP_HPP