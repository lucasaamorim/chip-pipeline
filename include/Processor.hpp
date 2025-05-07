#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "InstructionDecoder.hpp"
#include "InstructionFetch.hpp"
#include <systemc.h>

SC_MODULE(Processor) {
   // Parâmetros fixos
   static const unsigned int REGISTER_ADDR_BITS = 4;
   static const unsigned int MEMORY_ADDR_BITS = 8;
   static const unsigned int DATA_BITS = 32;

   // Portas
   sc_in<bool> clock;
   sc_in<bool> reset;

   // Internos
   sc_out<sc_uint<DATA_BITS>> instruction;
   sc_out<sc_uint<DATA_BITS>> write_data;
   sc_out<sc_uint<DATA_BITS>> first_read_data;
   sc_out<sc_uint<DATA_BITS>> second_read_data;
   sc_out<bool> write_enable;

   // Componentes
   InstructionFetch<MEMORY_ADDR_BITS, DATA_BITS>* fetch;
   InstructionDecoder<REGISTER_ADDR_BITS, DATA_BITS>* decoder;

   SC_CTOR(Processor);

   void load_program(const std::vector<sc_uint<DATA_BITS>>& program);
};

#endif // PROCESSOR_HPP
