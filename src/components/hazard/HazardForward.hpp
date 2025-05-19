#ifndef HAZARD_FORWARD_HPP
#define HAZARD_FORWARD_HPP

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(HazardForward) {
  sc_in_clk clk;
  sc_in<bool> reset;
  sc_in<sc_uint<4>> rs_in, rt_in, rd_1_in, rd_2_in;
  sc_in<bool> reg_write_1, reg_write_2, alu_src;
  sc_out<sc_uint<2>> rs_sel, rt_sel;

  void process() {
    if (reset.read()) {
      rs_sel.write(0);
      rt_sel.write(0);
    } else {
      if (reg_write_2.read() && rd_2_in.read() == rs_in.read()) {
        rs_sel.write(2);
      } else if (reg_write_1.read() && rd_1_in.read() == rs_in.read()) {
        rs_sel.write(1);
      } else {
        rs_sel.write(0);
      }

      if (alu_src.read()) {
        rt_sel.write(3);
      } else if (reg_write_2.read() && rd_2_in.read() == rt_in.read()) {
        rt_sel.write(2);
      } else if (reg_write_1.read() && rd_1_in.read() == rt_in.read()) {
        rt_sel.write(1);
      } else {
        rt_sel.write(0);
      }
    }
  }

  SC_CTOR(HazardForward) {
    SC_METHOD(process);
    sensitive << rs_in << rt_in << rd_1_in << rd_2_in << reg_write_1
              << reg_write_2;
  }
};

#endif // !HAZARD_FORWARD_HPP
