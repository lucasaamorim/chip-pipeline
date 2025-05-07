#ifndef REGISTER_HPP
#define REGISTER_HPP

#include <systemc.h>

template <unsigned int DATA_BITS = 32>
SC_MODULE(Register) {
   sc_in<bool> clock;
   sc_in<bool> reset;
   sc_in<bool> write_enable;
   sc_in<sc_uint<DATA_BITS>> data_in;
   sc_out<sc_uint<DATA_BITS>> data_out;

   // Internos
   sc_signal<sc_uint<DATA_BITS>> data;

   SC_CTOR(Register);

   private:
   void process();
};

#endif // !REGISTER_HPP