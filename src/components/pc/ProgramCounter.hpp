#ifndef PROGRAM_COUNTER_HPP
#define PROGRAM_COUNTER_HPP

#include <systemc.h>

template <unsigned int ADDR_BITS = 32>
SC_MODULE(ProgramCounter) {
public:
    // Entradas
    sc_in<sc_uint<ADDR_BITS>> next_addr_in;
    sc_in<bool> clock_in;
    sc_in<bool> reset_in;
    sc_in<bool> enable_in;

    // Saída
    sc_out<sc_uint<ADDR_BITS>> addr_out;

    // Construtor
    SC_CTOR(ProgramCounter);

private:
    void update();
};

#endif // PROGRAM_COUNTER_HPP
