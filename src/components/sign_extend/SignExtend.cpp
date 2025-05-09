#include "SignExtend.hpp"

template <unsigned int INPUT_WIDTH, unsigned int OUTPUT_WIDTH>
SignExtend<INPUT_WIDTH, OUTPUT_WIDTH>::SignExtend(sc_module_name name)
    : sc_module(name)
{
    SC_METHOD(extend);
    sensitive << immediate_in;
}

template <unsigned int INPUT_WIDTH, unsigned int OUTPUT_WIDTH>
void SignExtend<INPUT_WIDTH, OUTPUT_WIDTH>::extend() {
    sc_int<INPUT_WIDTH> input = immediate_in.read();
    sc_int<OUTPUT_WIDTH> output = input;
    extended_out.write(output);
}

// Instanciações explícitas
template class SignExtend<16, 32>;
template class SignExtend<8, 16>; 
