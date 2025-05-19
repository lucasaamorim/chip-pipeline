#ifndef MUX_HPP
#define MUX_HPP

#include <iostream>
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

// A parametrizable N-to-1 multiplexer where N = 2^SEL_WIDTH
template <unsigned int DATA_WIDTH, unsigned int SEL_WIDTH>
SC_MODULE(Mux) {
   // Number of inputs
   static const unsigned int INPUTS_COUNT = 1u << SEL_WIDTH;

   // Ports
   sc_in_clk clk;
   sc_in<bool> reset;
   sc_vector<sc_in<sc_uint<DATA_WIDTH>>> in; // vector of data inputs
   sc_in<sc_uint<SEL_WIDTH>> sel; // select lines
   sc_out<sc_uint<DATA_WIDTH>> out; // output

   // Process: on reset drive 0, else forward selected input
   void process() {
      if (reset.read()) {
         out.write(0);
      } else {
         /*          if (name() == std::string("tb.chip.mux_ex_rt_val")) {

                     std::cout << sc_time_stamp() << " [" << name()
                               << "] processando...\n";
                     std::cout << "sel: " << sel.read() << std::endl;
                     for (unsigned int i = 0; i < INPUTS_COUNT; ++i) {
                        std::cout << "in[" << i << "]: " << in[i].read() <<
            std::endl;
                     }
                  } */

         unsigned idx = sel.read().to_uint();
         // guard against out-of-range select
         if (idx < INPUTS_COUNT) {
            out.write(in[idx].read());
         } else {
            out.write(0);
         }
      }
   }

   // Constructor: initialize vector size and register process
   SC_CTOR(Mux)
       : clk("clk")
       , reset("reset")
       , in("in", INPUTS_COUNT)
       , sel("sel")
       , out("out") {
      SC_METHOD(process);
      dont_initialize();
      sensitive << sel << clk.pos();

      for (unsigned int i = 0; i < INPUTS_COUNT; ++i) {
         this->sensitive << in[i];
      }
   }
};

#endif // MUX_HPP