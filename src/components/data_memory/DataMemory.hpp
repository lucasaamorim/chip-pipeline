#ifndef DATA_MEMORY_HPP
#define DATA_MEMORY_HPP

#include <iostream>
#include <systemc>


using namespace sc_core;
using namespace sc_dt;

SC_MODULE(DataMemory) {
  sc_in_clk clk;
  sc_in<bool> reset, mem_write, mem_read;
  sc_in<sc_uint<32>> address, write_data;
  sc_out<sc_uint<32>> data;

  sc_uint<32> memory[256];

  void next_data() {
    if (reset.read()) {
      data.write(0);
    } else {
      if (mem_write.read()) {
        memory[address.read() / 4] = write_data.read();
      }
      if (mem_read.read()) {
        data.write(memory[address.read() / 4]);
      }
    }
  }

  SC_CTOR(DataMemory) {
    SC_METHOD(next_data);
    sensitive << address << mem_write << mem_read << write_data << clk.pos();
  }
};
#endif // !DATA_MEMORY_HPP