#include "InstructionDecoder.hpp"

template <unsigned int ADDR_BITS, unsigned int DATA_BITS>
InstructionDecoder<ADDR_BITS, DATA_BITS>::InstructionDecoder(
  sc_module_name name)
    : sc_module(name) {
   register_bank = new RegisterBank<ADDR_BITS, DATA_BITS>("register_bank");

   register_bank->clock(clock);
   register_bank->reset(reset);
   register_bank->write_enable(write_enable);
   register_bank->data_in(write_data);
   register_bank->write_address(reg_addr_out);
   register_bank->first_read_address(reg_addr1);
   register_bank->second_read_address(reg_addr2);
   register_bank->first_read_data(first_read_data);
   register_bank->second_read_data(second_read_data);

   SC_METHOD(process);
   sensitive << clock.pos();
}

template <unsigned int ADDR_BITS, unsigned int DATA_BITS>
void InstructionDecoder<ADDR_BITS, DATA_BITS>::process() {
   decode_instruction();
   write_enable.write(false);
}

template <unsigned int ADDR_BITS, unsigned int DATA_BITS>
void InstructionDecoder<ADDR_BITS, DATA_BITS>::decode_instruction() {
   sc_uint<DATA_BITS> instr = instruction.read();

   // Padrão tipo R: [31-26] opcode, [25-22] rs, [21-18] rt, [17-14] rd
   reg_addr1.write(instr.range(25, 22)); // rs
   reg_addr2.write(instr.range(21, 18)); // rt
   reg_addr_out.write(instr.range(17, 14)); // rd
}

// Instanciação explícita
template class InstructionDecoder<4, 32>;
template class InstructionDecoder<8, 32>;
