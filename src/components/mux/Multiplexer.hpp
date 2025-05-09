#ifndef MULTIPLEXER_HPP
#define MULTIPLEXER_HPP

#include <systemc.h>

template <unsigned int DATA_BITS = 32>
SC_MODULE(Multiplexer) {
public:
    // Entradas
    sc_in<sc_uint<DATA_BITS>> input0_in;
    sc_in<sc_uint<DATA_BITS>> input1_in;
    sc_in<bool> select_in;

    // Saída
    sc_out<sc_uint<DATA_BITS>> data_out;

    SC_CTOR(Multiplexer);

private:
    void process();
};

#endif // MULTIPLEXER_HPP
