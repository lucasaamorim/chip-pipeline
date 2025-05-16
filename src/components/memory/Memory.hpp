#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <systemc>
#include <vector>

using namespace sc_core;
using namespace sc_dt;

template <unsigned int DATA_BITS = 32, unsigned int ADDRESS_BITS = 8,
          typename T = sc_int<DATA_BITS>>
SC_MODULE(Memory) {
  static const unsigned int MEMORY_SIZE = 1 << ADDRESS_BITS;

  sc_in<bool> clock;
  sc_in<bool> reset;
  sc_in<bool> write_enable;
  sc_in<bool> read_enable;

  sc_in<sc_uint<ADDRESS_BITS>> address;

  sc_in<T> input;
  sc_out<T> output;

  T memory[MEMORY_SIZE];

  void process() {
    if (write_enable.read()) {
      memory[address.read()] = input.read();
    }

    if (read_enable.read()) {
      output.write(memory[address.read()]);
    }
  }

  void initialize(std::vector<T> data) {
    for (int i = 0; i < MEMORY_SIZE && i < data.size(); i++) {
      memory[i] = data[i];
    }
  }

  void reset_memory() {
    for (int i = 0; i < MEMORY_SIZE; i++) {
      memory[i] = 0;
    }
  }

  SC_CTOR(Memory) {
    SC_METHOD(process);
    dont_initialize();
    sensitive << clock.pos();
  }
};

#endif ///! MEMORY_HPP