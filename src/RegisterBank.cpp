#include "RegisterBank.hpp"

template <unsigned int ADDR_BITS, unsigned int DATA_BITS>
RegisterBank<ADDR_BITS, DATA_BITS>::RegisterBank(sc_module_name name)
    : sc_module(name) {
   for (int i = 0; i < (1 << ADDR_BITS); i++) {
      registers[i] = new Register<DATA_BITS>("register_" + std::to_string(i));
      registers[i]->clock(clock);
      registers[i]->reset(reset);
      registers[i]->write_enable(write_enable);
      registers[i]->data_in(data_in);
      registers[i]->data_out(register_outputs[i]);
   }

   SC_METHOD(process);
   sensitive << clock.pos();
}

template <unsigned int ADDR_BITS, unsigned int DATA_BITS>
void RegisterBank<ADDR_BITS, DATA_BITS>::process() {
   if (write_enable.read()) {
      unsigned int addr = write_address.read();
      if (addr != 0 && addr != ((1 << ADDR_BITS) - 1)) {
         registers[addr]->data.write(data_in.read());
      }
   }

   first_read_data.write(register_outputs[first_read_address.read()]);
   second_read_data.write(register_outputs[second_read_address.read()]);
}

template class RegisterBank<4, 32>;
template class RegisterBank<8, 32>;