#ifndef ID_EX_HPP
#define ID_EX_HPP

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(ID_EX) {
   sc_in_clk clk;
   sc_in<bool> reset;

   sc_in<sc_uint<32>> data_in_1, data_in_2;
   sc_out<sc_uint<32>> data_out_1, data_out_2;

   sc_in<sc_uint<4>> rs_in, rt_in, rd_in;
   sc_out<sc_uint<4>> rs_out, rt_out, rd_out;

   sc_in<sc_uint<32>> immediate_in;
   sc_out<sc_uint<32>> immediate_out;

   sc_in<bool> reg_dst_in, alu_src_in, mem_to_reg_in, reg_write_in, mem_read_in,
     mem_write_in;
   sc_out<bool> reg_dst_out, alu_src_out, mem_to_reg_out, reg_write_out,
     mem_read_out, mem_write_out;

   sc_in<sc_uint<3>> alu_op_in;
   sc_out<sc_uint<3>> alu_op_out;

   void latch() {
      if (reset.read()) {
         data_out_1.write(0);
         data_out_2.write(0);
         rs_out.write(0);
         rt_out.write(0);
         rd_out.write(0);
         immediate_out.write(0);
         reg_dst_out.write(false);
         alu_src_out.write(false);
         mem_to_reg_out.write(false);
         reg_write_out.write(false);
         mem_read_out.write(false);
         mem_write_out.write(false);
         alu_op_out.write(0);
      } else {
         data_out_1.write(data_in_1.read());
         data_out_2.write(data_in_2.read());
         rs_out.write(rs_in.read());
         rt_out.write(rt_in.read());
         rd_out.write(rd_in.read());
         immediate_out.write(immediate_in.read());
         reg_dst_out.write(reg_dst_in.read());
         alu_src_out.write(alu_src_in.read());
         mem_to_reg_out.write(mem_to_reg_in.read());
         reg_write_out.write(reg_write_in.read());
         mem_read_out.write(mem_read_in.read());
         mem_write_out.write(mem_write_in.read());
         alu_op_out.write(alu_op_in.read());
      }
   }

   SC_CTOR(ID_EX) {
      SC_METHOD(latch);
      sensitive << clk.pos();
   }
};

#endif // !ID_EX_HPP