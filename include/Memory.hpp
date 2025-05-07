#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <systemc.h>
#include <vector>

template <unsigned int ADDR_BITS = 8, unsigned int DATA_BITS = 32>
SC_MODULE(Memory) {
   sc_in<bool> clock;
   sc_in<bool> write_enable;
   sc_in<sc_uint<ADDR_BITS>> address;
   sc_in<sc_uint<DATA_BITS>> data_in;
   sc_out<sc_uint<DATA_BITS>> data_out;

   SC_CTOR(Memory);

   void init_memory(std::vector<sc_uint<DATA_BITS>> const& program);

   private:
   sc_uint<DATA_BITS> memory[1 << ADDR_BITS];
   void read();
   void write();
};

#endif // !MEMORY_HPP