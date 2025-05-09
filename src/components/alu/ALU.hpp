#ifndef ALU_HPP
#define ALU_HPP

#include <systemc.h>

constexpr int CODE_OPERATION_LENGTH = 3;

enum AluOperation {
    ALU_AND = 0,
    ALU_OR,
    ALU_XOR,
    ALU_NOT,
    ALU_ADD,
    ALU_SUB,
    ALU_CMP
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

#include "alu.cpp"

#endif // ALU_HPP