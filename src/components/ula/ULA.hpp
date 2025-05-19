#ifndef ULA_HPP
#define ULA_HPP

#include <iostream>
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(ULA) {
   sc_in_clk clk;
   sc_in<bool> reset;
   sc_in<sc_uint<32>> in1;
   sc_in<sc_uint<32>> in2;
   sc_in<sc_uint<3>> func;
   sc_out<sc_uint<32>> out;

   void exec() {
      if (reset.read()) {
         out.write(0);
      } else {
         /*          std::cout << sc_time_stamp() << " ULA: " << std::endl;
                  std::cout << "in1: " << in1.read() << " in2: " << in2.read()
                            << " func: " << func.read() << std::endl; */
         switch (func.read()) {
            case 0:
               out.write(in1.read() & in2.read());
               break;
            case 1:
               out.write(in1.read() | in2.read());
               break;
            case 2:
               out.write(in1.read() ^ in2.read());
               break;
            case 3:
               if (in1.read() > in2.read()) {
                  out.write(1);
               } else {
                  out.write(0);
               }
               break;
            case 4:
               out.write(in1.read() + in2.read());
               break;
            case 5:
               out.write(in1.read() - in2.read());
               break;
            case 6:
               out.write(~in1.read());
               break;
            default:
               out.write(0);
               break;
         }
      }
   }

   SC_CTOR(ULA) {
      SC_METHOD(exec);
      sensitive << in1 << in2 << func << clk.pos();
   }
};

#endif // !ULA_HPP