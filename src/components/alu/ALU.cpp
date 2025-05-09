#include "ALU.hpp"

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
                operation_result = 0;
            } else if (operand_a < operand_b) {
                operation_result = -1; // Using two's complement representation
            } else {
                operation_result = 1;
            }
            break;
        default: 
            operation_result = 0; // Default case for undefined operations
    }

    result_out.write(operation_result);
    zero_flag_out.write(operation_result == 0);
    negative_flag_out.write(operation_result[DATA_BITS - 1]); // MSB indicates sign

}
template class ALU<32>;