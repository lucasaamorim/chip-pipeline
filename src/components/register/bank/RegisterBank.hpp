#ifndef REGISTER_BANK_HPP
#define REGISTER_BANK_HPP

#include "register/Register.hpp"
#include <string>
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

template <unsigned int DATA_BITS = 32, unsigned int ADDRESS_BITS = 5>
SC_MODULE(RegisterBank) {
  static const unsigned int REGISTERS = 1 << ADDRESS_BITS;

  sc_in<bool> clock;
  sc_in<bool> reset;
  sc_in<bool> write_enable;

  sc_in<sc_uint<ADDRESS_BITS>> read_address_1;
  sc_in<sc_uint<ADDRESS_BITS>> read_address_2;
  sc_in<sc_uint<ADDRESS_BITS>> write_address;

  sc_in<sc_uint<DATA_BITS>> input;
  sc_out<sc_uint<DATA_BITS>> output_1;
  sc_out<sc_uint<DATA_BITS>> output_2;

  Register<DATA_BITS> *registers[REGISTERS];

  sc_signal<bool> register_writes[REGISTERS];
  sc_signal<sc_uint<DATA_BITS>> register_inputs[REGISTERS];
  sc_signal<sc_uint<DATA_BITS>> register_outputs[REGISTERS];

  void process() {
    /// Percorre todos os registradores atualizando os seus sinais
    if (write_enable.read()) {
      /// Não escreve no primeiro registrador
      for (int i = 1; i < REGISTERS; i++) {
        if (i == write_address.read()) {
          register_writes[i].write(true);
          register_inputs[i].write(input.read());
        } else {
          register_writes[i].write(false);
        }
      }
    }

    /// Leitura dos registradores
    output_1.write(register_outputs[read_address_1.read()]);
    output_2.write(register_outputs[read_address_2.read()]);
  }

  SC_CTOR(RegisterBank) {
    for (int i = 0; i < REGISTERS; i++) {
      std::string register_name = "register_" + std::to_string(i);
      registers[i] =
          new Register<DATA_BITS>(sc_module_name(register_name.c_str()));
      registers[i]->clock(clock);
      registers[i]->reset(reset);
      registers[i]->write_enable(register_writes[i]);
      registers[i]->input(register_inputs[i]);
      registers[i]->output(register_outputs[i]);
    }

    SC_METHOD(process);
    dont_initialize();
    sensitive << clock.pos() << reset.pos();
  }
};

#endif ///!REGISTER_BANK_HPP