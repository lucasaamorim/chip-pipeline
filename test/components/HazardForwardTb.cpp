#include <systemc>
#include "hazard/HazardForward.hpp"

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(Testbench) {
   sc_clock clk;
   sc_signal<bool> reset;
   sc_signal<sc_uint<4>> rs_in, rt_in, rd_1_in, rd_2_in;
   sc_signal<bool> reg_write_1, reg_write_2, alu_src;
   sc_signal<sc_uint<2>> rs_sel, rt_sel;

   HazardForward hf;

   SC_CTOR(Testbench)
       : clk("clk", 10, SC_NS), hf("hf") {
      hf.clk(clk);
      hf.reset(reset);
      hf.rs_in(rs_in);
      hf.rt_in(rt_in);
      hf.rd_1_in(rd_1_in);
      hf.rd_2_in(rd_2_in);
      hf.reg_write_1(reg_write_1);
      hf.reg_write_2(reg_write_2);
      hf.alu_src(alu_src);
      hf.rs_sel(rs_sel);
      hf.rt_sel(rt_sel);

      SC_THREAD(run);
   }

   void print_select(const char* desc) {
      std::cout << sc_time_stamp() << " " << desc
                << " | rs_sel=" << rs_sel.read()
                << " | rt_sel=" << rt_sel.read() << std::endl;
   }

   void run() {
      reset.write(true);
      wait(10, SC_NS);
      reset.write(false);

      // Caso 1: Encaminhamento de rd_2 para rs
      rs_in.write(5);
      rd_2_in.write(5);
      reg_write_2.write(true);
      reg_write_1.write(false);
      alu_src.write(false);
      wait(10, SC_NS);
      print_select("Forward de rd_2 para rs");

      // Caso 2: Encaminhamento de rd_1 para rs
      rs_in.write(3);
      rd_1_in.write(3);
      reg_write_1.write(true);
      reg_write_2.write(false);
      wait(10, SC_NS);
      print_select("Forward de rd_1 para rs");

      // Caso 3: Sem forwarding
      rs_in.write(2);
      rd_1_in.write(4);
      rd_2_in.write(6);
      reg_write_1.write(true);
      reg_write_2.write(true);
      wait(10, SC_NS);
      print_select("Sem forward");

      // Caso 4: Forward para rt, via rd_2
      rt_in.write(7);
      rd_2_in.write(7);
      reg_write_2.write(true);
      alu_src.write(false);
      wait(10, SC_NS);
      print_select("Forward de rd_2 para rt");

      // Caso 5: alu_src = 1 deve forçar rt_sel = 3
      alu_src.write(true);
      wait(10, SC_NS);
      print_select("ALU_SRC = 1 força rt_sel = 3");

      sc_stop();
   }
};

int sc_main(int argc, char* argv[]) {
   Testbench tb("tb");
   sc_start();
   return 0;
}
