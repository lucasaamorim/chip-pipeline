#ifndef IF_ID_HPP
#define IF_ID_HPP

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(IF_ID) {
  sc_in_clk clock;
  sc_in<bool> reset;
  sc_in<bool> flush;          // Novo sinal para flushar instruções
  sc_in<bool> stall;          // Novo sinal para stall do pipeline
  
  sc_in<sc_uint<32>> pc_in;
  sc_out<sc_uint<32>> pc_out;
  sc_in<sc_uint<32>> instruction_in;
  sc_out<sc_uint<32>> instruction_out;

  void latch() {
    if (reset.read()) {
      pc_out.write(0);
      instruction_out.write(0);
    } 
    else if (flush.read()) {
      // Flush: substitui por NOP (instrução 0) quando um jump é tomado
      pc_out.write(pc_in.read());
      instruction_out.write(0);  // NOP
    }
    else if (stall.read()) {
      // Stall: mantém os valores atuais
      // Não faz nada (registros mantêm seus valores)
    }
    else {
      // Comportamento normal
      pc_out.write(pc_in.read());
      instruction_out.write(instruction_in.read());
    }
  }

  SC_CTOR(IF_ID) {
    SC_METHOD(latch);
    sensitive << clock.pos();
  }
};

#endif // !IF_ID_HPP