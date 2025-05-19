#ifndef EX_MEM_HPP
#define EX_MEM_HPP

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(EX_MEM) {
  sc_in_clk clk;
  sc_in<bool> reset;
  sc_in<sc_uint<32>> ula_result_in, data_in_1;
  sc_out<sc_uint<32>> ula_result_out, data_out_1;
  sc_in<sc_uint<4>> rd_in;
  sc_out<sc_uint<4>> rd_out;
  sc_in<bool> mem_to_reg_in, reg_write_in, mem_read_in, mem_write_in;
  sc_out<bool> mem_to_reg_out, reg_write_out, mem_read_out, mem_write_out;

  void latch() {
    if (reset.read()) {
      ula_result_out.write(0);
      data_out_1.write(0);
      rd_out.write(0);
      mem_to_reg_out.write(false);
      reg_write_out.write(false);
      mem_read_out.write(false);
      mem_write_out.write(false);
    } else {
      ula_result_out.write(ula_result_in.read());
      data_out_1.write(data_in_1.read());
      rd_out.write(rd_in.read());
      mem_to_reg_out.write(mem_to_reg_in.read());
      reg_write_out.write(reg_write_in.read());
      mem_read_out.write(mem_read_in.read());
      mem_write_out.write(mem_write_in.read());

      /*  std::cout << sc_time_stamp() << " EX_MEM: " << std::endl;
       std::cout << "ula_result_out: " << ula_result_out.read() << std::endl;
       std::cout << "data_out_1: " << data_out_1.read() << std::endl;
       std::cout << "rd_out: " << rd_out.read() << std::endl;
       std::cout << "mem_to_reg_out: " << mem_to_reg_out.read() << std::endl;
       std::cout << "reg_write_out: " << reg_write_out.read() << std::endl;
       std::cout << "mem_read_out: " << mem_read_out.read() << std::endl;
       std::cout << "mem_write_out: " << mem_write_out.read() << std::endl; */
    }
  }

  SC_CTOR(EX_MEM) {
    SC_METHOD(latch);
    sensitive << clk.pos();
  }
};

#endif // !EX_MEM_HPP