#include "InstructionFetch.hpp"

template <unsigned int ADDR_BITS, unsigned int DATA_BITS>
InstructionFetch<ADDR_BITS, DATA_BITS>::InstructionFetch(sc_module_name name)
    : sc_module(name) {
   // Instancia o registrador de 32 bits (PC)
   pc_register = new Register<ADDR_BITS>("pc_register");
   pc_register->clock(clock);
   pc_register->reset(reset);
   pc_register->write_enable(write_enable);
   pc_register->data_in(pc_next);
   pc_register->data_out(pc);

   SC_METHOD(increment_pc);
   sensitive << clock.pos() << reset.pos();

   // Instancia a memória (ROM de instruções)
   instruction_memory = new Memory<ADDR_BITS, DATA_BITS>("instruction_memory");
   instruction_memory->clock(clock);
   instruction_memory->write_enable(mem_write_enable);
   instruction_memory->address(pc);
   instruction_memory->data_in(dummy_input);
   instruction_memory->data_out(memory_data);

   SC_METHOD(assign_address);
   sensitive << pc;
}

template <unsigned int ADDR_BITS, unsigned int DATA_BITS>
void InstructionFetch<ADDR_BITS, DATA_BITS>::assign_address() {
   instruction.write(memory_data.read());
}

template <unsigned int ADDR_BITS, unsigned int DATA_BITS>
void InstructionFetch<ADDR_BITS, DATA_BITS>::increment_pc() {
   if (reset.read()) {
      pc_next.write(0);
   } else {
      pc_next.write(pc.read() + 1);
   }
}

template <unsigned int ADDR_BITS, unsigned int DATA_BITS>
void InstructionFetch<ADDR_BITS, DATA_BITS>::load_program(
  const std::vector<sc_uint<DATA_BITS>>& program) {
   instruction_memory->init_memory(program);
}

template class InstructionFetch<8, 32>;