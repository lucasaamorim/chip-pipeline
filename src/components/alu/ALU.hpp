#ifndef ALU_HPP
#define ALU_HPP

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

constexpr int CODE_OPERATION_LENGTH = 4;


enum AluOperation {
    ALU_NOT = 0b0000,
    ALU_AND = 0b0011,
    ALU_OR  = 0b0010,
    ALU_XOR = 0b0001,
    ALU_ADD = 0b0111,
    ALU_SUB = 0b1000,
    ALU_CMP = 0b0100,
};



template <unsigned int DATA_WIDTH = 32>
class ALU : public sc_module {
public:
    sc_in<sc_uint<DATA_WIDTH>> operand_a_in;
    sc_in<sc_uint<DATA_WIDTH>> operand_b_in;
    sc_in<sc_uint<CODE_OPERATION_LENGTH>> operation_select_in;
    sc_out<sc_uint<DATA_WIDTH>> result_out;
    sc_out<bool>              zero_flag_out;
    sc_out<bool>              negative_flag_out;

    SC_CTOR(ALU);
    
private:
    void process_operation();
};

// Definição do construtor e do método de operação
template <unsigned int DATA_BITS>
ALU<DATA_BITS>::ALU(sc_module_name name) : sc_module(name) {
    SC_METHOD(process_operation);
    sensitive << operand_a_in << operand_b_in << operation_select_in;
}

template <unsigned int DATA_BITS>
void ALU<DATA_BITS>::process_operation() {
    const sc_uint<DATA_BITS> operand_a = operand_a_in.read();
    const sc_uint<DATA_BITS> operand_b = operand_b_in.read();
    sc_uint<DATA_BITS> operation_result = 0;

    switch (operation_select_in.read()) {
        case ALU_AND: operation_result = operand_a & operand_b; break;
        case ALU_OR:  operation_result = operand_a | operand_b; break;
        case ALU_XOR: operation_result = operand_a ^ operand_b; break;
        case ALU_NOT: operation_result = ~operand_a; break;
        case ALU_ADD: operation_result = operand_a + operand_b; break;
        case ALU_SUB: operation_result = operand_a - operand_b; break;
        case ALU_CMP:
            if (operand_a == operand_b) {
                operation_result = 0; // Zero se iguais
            } else if (operand_a < operand_b) {
                operation_result = -1; // Dois em complemento para -1
            } else {
                operation_result = 1;
            }
            break;
        default: 
            operation_result = 0; // Caso padrão, não faz nada
    }

    result_out.write(operation_result);
    zero_flag_out.write(operation_result == 0);  // Se o resultado for zero
    negative_flag_out.write(operation_result[DATA_BITS - 1]); // Se o bit MSB (bit mais significativo) for 1, é negativo
}

// Instanciando para 32 bits
template class ALU<32>;

#endif // ALU_HPP
