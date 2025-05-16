#ifndef INSTRUCTION_FETCH_HPP
#define INSTRUCTION_FETCH_HPP

#include "arith/Adder.hpp"
#include "memory/Memory.hpp"
#include "mux/Multiplexer.hpp"
#include "register/Register.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

template <unsigned int DATA_BITS = 32, unsigned int ADDRESS_BITS = 8>
SC_MODULE(InstructionFetch) {
  sc_in<bool> clock;
  sc_in<bool> reset;

  sc_in<sc_uint<1>> jump_pc_enable;
  sc_in<sc_uint<ADDRESS_BITS>> jump_pc;

  sc_out<sc_uint<ADDRESS_BITS>> next_pc;
  sc_out<sc_uint<DATA_BITS>> instruction;

  Multiplexer<ADDRESS_BITS, 1, sc_uint<ADDRESS_BITS>> *pc_mux;
  Register<ADDRESS_BITS, false, sc_uint<ADDRESS_BITS>> *pc_register;
  Memory<DATA_BITS, ADDRESS_BITS, sc_uint<DATA_BITS>> *instruction_memory;

  sc_signal<sc_uint<ADDRESS_BITS>> pc_output;

  sc_signal<sc_uint<ADDRESS_BITS>> select_pc;
  sc_signal<sc_uint<ADDRESS_BITS>> adder_constant;
  sc_signal<bool> pc_write_enable_constant;
  sc_signal<bool> memory_write_enable_constant;
  sc_signal<bool> memory_read_enable_constant;
  sc_signal<sc_uint<DATA_BITS>> memory_input_constant;

  void process() { next_pc.write(pc_output.read() + adder_constant.read()); }

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
    memory_input_constant.write(0);

    /// Inicializando o MUX
    pc_mux = new Multiplexer<ADDRESS_BITS, 1, sc_uint<ADDRESS_BITS>>(
        sc_module_name("pc_mux"));
    pc_mux->clock(clock);
    pc_mux->reset(reset);

    pc_mux->inputs[0](next_pc);
    pc_mux->inputs[1](jump_pc);
    pc_mux->select(jump_pc_enable);
    pc_mux->output(select_pc);

    /// Inicializando o PC
    pc_register = new Register<ADDRESS_BITS, false, sc_uint<ADDRESS_BITS>>(
        sc_module_name("pc_register"));
    pc_register->clock(clock);
    pc_register->reset(reset);
    pc_register->write_enable(pc_write_enable_constant);
    pc_register->input(select_pc);
    pc_register->output(pc_output);

    /// Inicializando a memória
    instruction_memory =
        new Memory<DATA_BITS, ADDRESS_BITS, sc_uint<DATA_BITS>>(
            sc_module_name("instruction_memory"));
    instruction_memory->clock(clock);
    instruction_memory->reset(reset);
    instruction_memory->write_enable(memory_write_enable_constant);
    instruction_memory->read_enable(memory_read_enable_constant);
    instruction_memory->address(pc_output);
    instruction_memory->input(memory_input_constant);
    instruction_memory->output(instruction);

    SC_METHOD(process);
    dont_initialize();
    sensitive << clock.pos() << reset.pos();
  }
};

#endif // !INSTRUCTION_FETCH_HPP
