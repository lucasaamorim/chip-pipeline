#include "Register.hpp"

template <unsigned int DATA_BITS>
Register<DATA_BITS>::Register(sc_module_name name) : sc_module(name) {
   SC_METHOD(process);
   sensitive << clock.pos() << reset.pos();
}

template <unsigned int DATA_BITS>
void Register<DATA_BITS>::process() {
   if (reset.read()) {
      data.write(0);
   } else if (write_enable.read()) {
      data.write(data_in.read());
   }

   data_out.write(data.read());
}

template class Register<32>;
template class Register<8>;