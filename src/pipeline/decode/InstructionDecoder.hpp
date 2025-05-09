#ifndef INSTRUCTION_DECODER_HPP
#define INSTRUCTION_DECODER_HPP

#include "RegisterBank.hpp"
#include <systemc.h>

template <unsigned int ADDR_BITS = 4, unsigned int DATA_BITS = 32>
SC_MODULE(InstructionDecoder) {
   sc_in<bool> clock;
   sc_in<bool> reset;
   sc_in<sc_uint<DATA_BITS>> instruction;
   sc_in<sc_uint<DATA_BITS>> write_data;

   sc_out<sc_uint<DATA_BITS>> first_read_data;
   sc_out<sc_uint<DATA_BITS>> second_read_data;

   // Sinais internos para o banco de registradores
   sc_signal<sc_uint<ADDR_BITS>> reg_addr1;
   sc_signal<sc_uint<ADDR_BITS>> reg_addr2;
   sc_signal<sc_uint<ADDR_BITS>> reg_addr_out;

   // Sinais de controle internos
   sc_signal<bool> write_enable;

   RegisterBank<ADDR_BITS, DATA_BITS>* register_bank;

   SC_CTOR(InstructionDecoder);

   private:
   void process();
   void decode_instruction();
};

#endif // INSTRUCTION_DECODER_HPP
