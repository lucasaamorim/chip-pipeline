#include "Multiplexer.hpp"

template <unsigned int DATA_BITS>
Multiplexer<DATA_BITS>::Multiplexer(sc_module_name name)
    : sc_module(name)
{
    SC_METHOD(process);
    sensitive << input0_in << input1_in << select_in;
}

template <unsigned int DATA_BITS>
void Multiplexer<DATA_BITS>::process() {
    data_out.write(select_in.read() ? input1_in.read() : input0_in.read());
}

template class Multiplexer<32>;
template class Multiplexer<8>;  