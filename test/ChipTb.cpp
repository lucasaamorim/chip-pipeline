#include <iostream>
#include <systemc>
#include <vector>

#include "../src/pipeline/Chip.hpp"

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(Testbench) {
   sc_clock clk;
   sc_signal<bool> reset;
   Chip* chip;

   SC_CTOR(Testbench) : clk("clk", 10, SC_NS) {
      chip = new Chip("chip");
      chip->clk(clk);
      chip->reset(reset);
      SC_THREAD(run);
   }

   // Instruction builders
   sc_uint<32> makeR(unsigned opc, unsigned rs, unsigned rt, unsigned rd) {
      sc_uint<32> ins = 0;
      ins.range(31, 26) = opc;
      ins.range(25, 22) = rs;
      ins.range(21, 18) = rt;
      ins.range(17, 14) = rd;
      return ins;
   }
   sc_uint<32> makeI(unsigned opc, unsigned rs, unsigned rd, unsigned imm) {
      sc_uint<32> ins = 0;
      ins.range(31, 26) = opc;
      ins.range(25, 22) = rs;
      ins.range(21, 18) = rd;
      ins.range(17, 0) = imm;
      return ins;
   }
   sc_uint<32> makeJ(unsigned opc, unsigned offset) {
      sc_uint<32> ins = 0;
      ins.range(31, 26) = opc;
      ins.range(25, 0) = offset;
      return ins;
   }

   void run() {
      // Reset assertion
      reset.write(true);
      wait(2, SC_NS);
      reset.write(false);

      // Program: demonstrates forwarding and jump
      // std::vector<sc_uint<32>> prog = {
      //    makeI(0b100111, 0, 1, 10), // ADDI R1 = 10
      //    makeI(0b100111, 1, 2, 20), // ADDI R2 = R1 + 20
      //    makeR(0b000011, 1, 2, 3), // AND  R3 = R2 & R1
      //    makeJ(0b111111, 2), // J skip next 2
      //    makeI(0b100010, 0, 4, 15), // ORI  R4 (skipped)
      //    makeI(0b100001, 1, 4, 3), // XORI R4 (skipped)
      //    makeI(0b101000, 3, 4, 5), // SUBI R4 = R3 - 5
      //    makeI(0b100111, 1, 5, 5), // ADDI R5 = R3 + 5
      //    0 // NOP
      // };

      std::vector<sc_uint<32>> prog = {
         makeI(0b100111, 0, 2, 100),   // ADDI R2 = 100 
         makeI(0b100111, 0, 3, 10),    // ADDI R3 = 10
         makeI(0b101001, 2, 3, 0),     // ST R3 → MEM[100]
         makeI(0b100110, 2, 4, 0),     // LD R4 = MEM[R2 + 0]
         makeI(0b100111, 4, 5, 5),     // ADDI R5 = R4 + 5 (depende do load)
         0                             // NOP
      };

      chip->initialize_instructions(prog);

      // Trace setup
      sc_trace_file* tf = sc_create_vcd_trace_file("chip_tb");
   

      // IF
      sc_trace(tf, chip->clk, "clk");
      sc_trace(tf, chip->reset, "reset");
      sc_trace(tf, chip->pc_out_signal, "IF/PC");

      // ID
      sc_trace(tf, chip->if_id_pc_signal, "ID/IF_ID_PC");
      sc_trace(tf, chip->if_id_instruction_signal, "ID/IF_ID_Instruction");
      sc_trace(tf, chip->decoder_rs_signal, "ID/RS");
      sc_trace(tf, chip->decoder_rt_signal, "ID/RT");
      sc_trace(tf, chip->decoder_rd_signal, "ID/RD");
      sc_trace(tf, chip->decoder_immediate_i_signal, "ID/ImmI");
      sc_trace(tf, chip->rb_data_out_1_signal, "ID/RegData1");
      sc_trace(tf, chip->rb_data_out_2_signal, "ID/RegData2");
      sc_trace(tf, chip->decoder_alu_src_signal, "ID/ALUSrc");
      sc_trace(tf, chip->decoder_alu_op_signal, "ID/ALUOp");
      sc_trace(tf, chip->decoder_reg_write_signal, "ID/RegWrite");
      sc_trace(tf, chip->decoder_mem_to_reg_signal, "ID/MemToReg");
      sc_trace(tf, chip->decoder_mem_read_signal, "ID/MemRead");
      sc_trace(tf, chip->decoder_mem_write_signal, "ID/MemWrite");

      // EX
      sc_trace(tf, chip->id_ex_data_out_1_signal, "EX/Operand1");
      sc_trace(tf, chip->id_ex_data_out_2_signal, "EX/Operand2");
      sc_trace(tf, chip->mux_ex_rs_val_out_signal, "EX/MuxRSVal");
      sc_trace(tf, chip->mux_ex_rt_val_out_signal, "EX/MuxRTVal");
      sc_trace(tf, chip->ula_result_signal, "EX/ULA_Result");
      sc_trace(tf, chip->mux_ex_rd_out_signal, "EX/RD_Mux_Out");
      sc_trace(tf, chip->rs_sel_signal, "EX/Forward_RS_Sel");
      sc_trace(tf, chip->rt_sel_signal, "EX/Forward_RT_Sel");
      sc_trace(tf, chip->id_ex_alu_op_out, "EX/ALUOp");

      // EX
      sc_trace(tf, chip->ex_mem_ula_result_out_signal, "MEM/Addr");
      sc_trace(tf, chip->ex_mem_data_out_1_signal, "MEM/DataToWrite");
      sc_trace(tf, chip->ex_mem_mem_read_out_signal, "MEM/MemRead");
      sc_trace(tf, chip->ex_mem_mem_write_out_signal, "MEM/MemWrite");
      sc_trace(tf, chip->data_memory_data_signal, "MEM/MemDataRead");

      // WB
      sc_trace(tf, chip->mem_wb_data_out_signal, "WB/DataMemOut");
      sc_trace(tf, chip->mem_wb_ula_result_out_signal, "WB/ULA_Result");
      sc_trace(tf, chip->mux_wb_val_out_signal, "WB/WriteBackData");
      sc_trace(tf, chip->mem_wb_rd_out_signal, "WB/RD");
      sc_trace(tf, chip->mem_wb_reg_write_out_signal, "WB/RegWrite");
      sc_trace(tf, chip->mem_wb_mem_to_reg_out_signal, "WB/MemToReg");



      // Prev register snapshot using C array
      uint32_t prev[16] = { 0 };

      // Run cycles and report writes
      const int cycles = 20;
      for (int cycle = 0; cycle < cycles; ++cycle) {
         wait(clk.posedge_event());
         std::cout << "Cycle " << cycle;
         // Check each register
         for (int r = 1; r < 16; ++r) {
            uint32_t val = chip->register_bank->registers[r].to_uint();
            if (val != prev[r]) {
               std::cout << " | R" << r << "=" << val;
               prev[r] = val;
            }
         }
         std::cout << std::endl;
      }

      sc_close_vcd_trace_file(tf);
      std::cout << "Simulation done." << std::endl;
      sc_stop();
   }
};

int sc_main(int argc, char* argv[]) {
   Testbench tb("tb");
   sc_start();
   return 0;
}
