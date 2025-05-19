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
      std::vector<sc_uint<32>> prog = {
         makeI(0b100111, 0, 1, 10), // ADDI R1 = 10
         makeI(0b100111, 1, 2, 20), // ADDI R2 = R1 + 20
         makeR(0b000011, 1, 2, 3), // AND  R3 = R2 & R1
         makeJ(0b111111, 2), // J skip next 2
         makeI(0b100010, 0, 4, 15), // ORI  R4 (skipped)
         makeI(0b100001, 1, 4, 3), // XORI R5 (skipped)
         makeI(0b101000, 3, 4, 5), // SUBI R6 = R3 - 5
         0 // NOP
      };
      chip->initialize_instructions(prog);

      // Trace setup
      sc_trace_file* tf = sc_create_vcd_trace_file("chip_tb");
      sc_trace(tf, clk, "clk");
      sc_trace(tf, reset, "reset");
      sc_trace(tf, chip->pc_out_signal, "PC");
      sc_trace(tf, chip->if_id_instruction_signal, "INSTRUCTION");

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
