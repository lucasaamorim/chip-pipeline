#ifndef PC_HPP
#define PC_HPP

#include <iostream>
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(PC) {
   sc_in_clk clk;
   sc_in<bool> reset, enable;
   sc_in<sc_uint<32>> in;
   sc_out<sc_uint<32>> out;

   void next_instruction() {
      if (reset.read()) {
         out.write(0);
      } else {
         out.write(in.read());
      }
   }

   SC_CTOR(PC) {
      SC_METHOD(next_instruction);
      dont_initialize();
      sensitive << clk.pos() << reset;
   }
};

#endif // !PC_HPP