#include "Memory.hpp"

template <unsigned int ADDR_BITS, unsigned int DATA_BITS>
Memory<ADDR_BITS, DATA_BITS>::Memory(sc_module_name name) : sc_module(name) {
   SC_METHOD(read);
   sensitive << address;

   SC_METHOD(write);
   sensitive << clock.pos();
}

template <unsigned int ADDR_BITS, unsigned int DATA_BITS>
void Memory<ADDR_BITS, DATA_BITS>::read() {
   data_out.write(memory[address.read()]);
}

template <unsigned int ADDR_BITS, unsigned int DATA_BITS>
void Memory<ADDR_BITS, DATA_BITS>::write() {
   if (write_enable.read()) {
      memory[address.read()] = data_in.read();
   }
}

template <unsigned int ADDR_BITS, unsigned int DATA_BITS>
void Memory<ADDR_BITS, DATA_BITS>::init_memory(
  std::vector<sc_uint<DATA_BITS>> const& program) {
   for (size_t i = 0; i < program.size() && i < (1 << ADDR_BITS); i++) {
      memory[i] = program[i];
   }
}

template class Memory<8, 32>;