/// @file Multiplexer.hpp
/// @author Gabriel Victor

#ifndef MULTIPLEXER_HPP
#define MULTIPLEXER_HPP

#include <systemc.h>

template <unsigned int DATA_BITS = 32> SC_MODULE(Multiplexer) {
public:
  // Entradas
  sc_in<bool> clock;
  sc_in<bool> reset;

  sc_in<sc_uint<DATA_BITS>> input_1;
  sc_in<sc_uint<DATA_BITS>> input_2;
  sc_in<bool> select;

  // Saída
  sc_out<sc_uint<DATA_BITS>> output;

  void process() {
    if (reset.read()) {
      output.write(0);
    } else {
      output.write(select.read() ? input_2.read() : input_1.read());
    }
  }

  SC_CTOR(Multiplexer) {
    SC_METHOD(process);
    dont_initialize();
    sensitive << input_1 << input_2 << select;
  }
};

#endif // MULTIPLEXER_HPP