#ifndef REGISTER_BANK_HPP
#define REGISTER_BANK_HPP

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(RegisterBank) {
  sc_in_clk clk;
  sc_in<bool> reset, write_enable;
  sc_in<sc_uint<4>> read_address_1, read_address_2, write_address;
  sc_in<sc_uint<32>> data_in;
  sc_out<sc_uint<32>> data_out_1, data_out_2;

  sc_uint<32> registers[16];

  void fetch() {
    if (reset.read()) {
      for (int i = 1; i < 16; i++) {
        registers[i] = 0;
      }
    } else if (write_enable.read() && write_address.read() != 0) {
      registers[write_address.read()] = data_in.read();
    }

    data_out_1.write(registers[read_address_1.read()]);
    data_out_2.write(registers[read_address_2.read()]);
  }

  SC_CTOR(RegisterBank) {
    SC_METHOD(fetch);
    sensitive << write_enable << write_address << data_in << read_address_1
              << read_address_2;
  }
};

#endif // !REGISTER_BANK_HPP