#ifndef ADDER_HPP
#define ADDER_HPP

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

template <unsigned int DATA_BITS = 32> SC_MODULE(Adder) {
  sc_in<bool> clock;
  sc_in<bool> reset;

  sc_in<sc_uint<DATA_BITS>> input_1;
  sc_in<sc_uint<DATA_BITS>> input_2;

  sc_out<sc_uint<DATA_BITS>> output;

  void process() {
    if (reset.read()) {
      output.write(0);
    } else {
      output.write(input_1.read() + input_2.read());
    }
  }

  SC_CTOR(Adder) {
    SC_METHOD(process);
    dont_initialize();
    sensitive << input_1 << input_2;
  }
};

#endif