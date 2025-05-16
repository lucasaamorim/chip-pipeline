#ifndef REGISTER_HPP
#define REGISTER_HPP

#include <iostream>
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

template <unsigned int BITS = 32, bool READ_BEFORE_WRITE = false,
          typename T = sc_int<BITS>>
SC_MODULE(Register) {
  sc_in<bool> clock;
  sc_in<bool> reset;
  sc_in<bool> write_enable;

  sc_in<T> input;
  sc_out<T> output;

  T data;

  void process() {
    if constexpr (READ_BEFORE_WRITE) {
      // Primeiro lê, depois salva
      output.write(data);

      if (reset.read()) {
        data = 0;
      } else if (write_enable.read()) {
        data = input.read();
      }
    } else {
      // Primeiro salva, depois lê
      if (reset.read()) {
        data = 0;
      } else if (write_enable.read()) {
        data = input.read();
      }

      output.write(data);
    }
  }

  SC_CTOR(Register) {
    SC_METHOD(process);
    dont_initialize();
    sensitive << clock.pos() << reset.pos() << input;
  }
};

#endif ///!REGISTER_HPP