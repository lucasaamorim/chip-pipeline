#ifndef MEM_WB_HPP
#define MEM_WB_HPP

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(MEM_WB) {
  sc_in_clk clk;
  sc_in<bool> reset;

  sc_in<sc_uint<32>> ula_result_in, data_in_1;
  sc_out<sc_uint<32>> ula_result_out, data_out_1;
  sc_in<sc_uint<4>> rd_in;
  sc_out<sc_uint<4>> rd_out;
  sc_in<bool> reg_write_in, mem_to_reg_in;
  sc_out<bool> reg_write_out, mem_to_reg_out;

  void latch() {
    if (reset.read()) {
      ula_result_out.write(0);
      data_out_1.write(0);
      rd_out.write(0);
      reg_write_out.write(false);
      mem_to_reg_out.write(false);
    } else {
      ula_result_out.write(ula_result_in.read());
      data_out_1.write(data_in_1.read());
      rd_out.write(rd_in.read());
      reg_write_out.write(reg_write_in.read());
      mem_to_reg_out.write(mem_to_reg_in.read());
    }
  }

  SC_CTOR(MEM_WB) {
    SC_METHOD(latch);
    sensitive << clk.pos();
  }
};

#endif // !MEM_WB_HPP