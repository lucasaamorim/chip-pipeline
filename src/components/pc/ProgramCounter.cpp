#include "ProgramCounter.hpp"

template <unsigned int ADDR_BITS>
ProgramCounter<ADDR_BITS>::ProgramCounter(sc_module_name name) : sc_module(name) {
    SC_METHOD(update);
    sensitive << clock_in.pos() << reset_in;
    dont_initialize();
}

template <unsigned int ADDR_BITS>
void ProgramCounter<ADDR_BITS>::update() {
    if (reset_in.read()) {
        addr_out.write(0);  // Reset para o endereço 0
    } else if (enable_in.read()) {
        addr_out.write(next_addr_in.read());  // Atualiza o PC
    }
}

// Instanciação explícita
template class ProgramCounter<32>;
