#ifndef SIGN_EXTEND_HPP
#define SIGN_EXTEND_HPP

#include <systemc.h>

template <unsigned int INPUT_WIDTH = 16, unsigned int OUTPUT_WIDTH = 32>
SC_MODULE(SignExtend) {
public:
    // Entrada: valor imediato de 16 bits (para instruções como lw, sw, beq)
    sc_in<sc_int<INPUT_WIDTH>> immediate_in;
    
    // Saída: valor estendido para 32 bits (com extensão de sinal)
    sc_out<sc_int<OUTPUT_WIDTH>> extended_out;

    SC_CTOR(SignExtend);

private:
    void extend();
};


#endif // SIGN_EXTEND_HPP