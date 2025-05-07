#include "Processor.hpp"

Processor::Processor(sc_module_name name) : sc_module(name) {
   fetch
     = new InstructionFetch<MEMORY_ADDR_BITS, DATA_BITS>("instruction_fetch");
   decoder = new InstructionDecoder<REGISTER_ADDR_BITS, DATA_BITS>(
     "instruction_decoder");

   // Conectar as portas e os sinais
   fetch->clock(clock);
   fetch->reset(reset);
   fetch->write_enable(write_enable);
   fetch->instruction(instruction);

   decoder->clock(clock);
   decoder->reset(reset);
   decoder->instruction(instruction);
   decoder->write_data(write_data);
   decoder->first_read_data(first_read_data);
   decoder->second_read_data(second_read_data);
}

void Processor::load_program(const std::vector<sc_uint<DATA_BITS>>& program) {
   fetch->load_program(program);
}
