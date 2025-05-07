#ifndef REGISTER_BANK_HPP
#define REGISTER_BANK_HPP

#include "Register.hpp"
#include <systemc>
#include <vector>

template <unsigned int ADDR_BITS = 4, unsigned int DATA_BITS = 32>
SC_MODULE(RegisterBank) {
   sc_in<bool> clock;
   sc_in<bool> reset;
   sc_in<bool> write_enable;
   sc_in<sc_uint<ADDR_BITS>> first_read_address;
   sc_in<sc_uint<ADDR_BITS>> second_read_address;
   sc_in<sc_uint<ADDR_BITS>> write_address;
   sc_in<sc_uint<DATA_BITS>> data_in;
   sc_out<sc_uint<DATA_BITS>> first_read_data;
   sc_out<sc_uint<DATA_BITS>> second_read_data;

   // Registros
   Register<DATA_BITS>* registers[1 << ADDR_BITS];
   sc_signal<sc_uint<DATA_BITS>> register_outputs[1 << ADDR_BITS];

   SC_CTOR(RegisterBank);

   private:
   void process();
};
#endif //! REGISTER_BANK_HPP