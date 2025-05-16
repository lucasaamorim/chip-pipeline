/// @file Multiplexer.hpp
/// @author Gabriel Victor

#ifndef MULTIPLEXER_HPP
#define MULTIPLEXER_HPP

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

template <unsigned int DATA_BITS = 32, unsigned int SELECT_BITS = 1,
          typename T = sc_int<DATA_BITS>>
SC_MODULE(Multiplexer) {
public:
  static const unsigned int INPUTS_COUNT = 1 << SELECT_BITS;

  // Entradas
  sc_in<bool> clock;
  sc_in<bool> reset;
  sc_vector<sc_in<T>> inputs;
  sc_in<sc_uint<SELECT_BITS>> select;

  // Saída
  sc_out<T> output;

  void process() {
    if (reset.read()) {
      output.write(0);
    } else {
      unsigned int sel = select.read();
      if (sel < INPUTS_COUNT) {
        output.write(inputs[sel].read());
      } else {
        output.write(0);
      }
    }
  }

  SC_CTOR(Multiplexer)
      : inputs("inputs", INPUTS_COUNT) // inicializa sc_vector
  {
    SC_METHOD(process);
    dont_initialize();
    sensitive << reset << select;
    for (unsigned int i = 0; i < INPUTS_COUNT; ++i) {
      sensitive << inputs[i];
    }
  }
};

#endif // MULTIPLEXER_HPP