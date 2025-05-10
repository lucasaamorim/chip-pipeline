#ifndef INSTRUCTION_FETCH_HPP
#define INSTRUCTION_FETCH_HPP

#include "arith/Adder.hpp"
#include "memory/Memory.hpp"
#include "register/Register.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

template <unsigned int DATA_BITS = 32, unsigned int ADDRESS_BITS = 8>
SC_MODULE(InstructionFetch) {
   sc_in<bool> clock;
   sc_in<bool> reset;

   sc_out<sc_uint<DATA_BITS>> instruction;
   sc_out<sc_uint<ADDRESS_BITS>> next_pc;

   Register<ADDRESS_BITS>* pc_register;
   Memory<DATA_BITS, ADDRESS_BITS>* instruction_memory;
   Adder<ADDRESS_BITS>* pc_adder;

   sc_signal<sc_uint<ADDRESS_BITS>> pc_output;
   sc_signal<sc_uint<ADDRESS_BITS>> adder_output;

   sc_signal<sc_uint<ADDRESS_BITS>> adder_constant;
   sc_signal<bool> pc_write_enable_constant;
   sc_signal<bool> memory_write_enable_constant;
   sc_signal<bool> memory_read_enable_constant;

   void process() { next_pc.write(adder_output.read()); }

   void initialize_memory(std::vector<sc_uint<DATA_BITS>> data) {
      instruction_memory->initialize(data);
   }

   void reset_memory() { instruction_memory->reset_memory(); }

   SC_CTOR(InstructionFetch) {
      /// Inicializando constantes
      adder_constant.write(1);
      pc_write_enable_constant.write(true);
      memory_write_enable_constant.write(false);
      memory_read_enable_constant.write(true);

      /// Inicializando o PC
      pc_register = new Register<ADDRESS_BITS>(sc_module_name("pc_register"));
      pc_register->clock(clock);
      pc_register->reset(reset);
      pc_register->write_enable(pc_write_enable_constant);
      pc_register->input(adder_output);
      pc_register->output(pc_output);

      /// Inicializando a memória
      instruction_memory = new Memory<DATA_BITS, ADDRESS_BITS>(
        sc_module_name("instruction_memory"));
      instruction_memory->clock(clock);
      instruction_memory->reset(reset);
      instruction_memory->write_enable(memory_write_enable_constant);
      instruction_memory->read_enable(memory_read_enable_constant);
      instruction_memory->address(pc_output);
      instruction_memory->output(instruction);

      /// Incializando o somador
      pc_adder = new Adder<ADDRESS_BITS>(sc_module_name("pc_adder"));
      pc_adder->clock(clock);
      pc_adder->reset(reset);
      pc_adder->input_1(pc_output);
      pc_adder->input_2(adder_constant);
      pc_adder->output(adder_output);

      SC_METHOD(process);
      dont_initialize();
      sensitive << clock.pos() << reset.pos();
   }
};

#endif // !INSTRUCTION_FETCH_HPP
