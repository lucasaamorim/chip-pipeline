#ifndef IF_ID_HPP
#define IF_ID_HPP

#include <iostream>
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(IF_ID) {
   sc_in_clk clock;
   sc_in<bool> reset, stall;

   sc_in<sc_uint<32>> pc_in;
   sc_out<sc_uint<32>> pc_out;
   sc_in<sc_uint<32>> instruction_in;
   sc_out<sc_uint<32>> instruction_out;

   void latch() {
      if (reset.read()) {
         pc_out.write(0);
         instruction_out.write(0);
      } else if (stall.read()) {
         instruction_out.write(0);
         /// Não executa nenhuma ação.
      } else {
         pc_out.write(pc_in.read());
         instruction_out.write(instruction_in.read());
      }
   }

   SC_CTOR(IF_ID) {
      SC_METHOD(latch);
      sensitive << clock.pos();
   }
};

#endif // !IF_ID_HPP