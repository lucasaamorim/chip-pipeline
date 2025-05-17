#ifndef EX_MEM_REGISTER_HPP
#define EX_MEM_REGISTER_HPP

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

template <unsigned int DATA_BITS = 32, unsigned int REGISTER_BITS = 4>
SC_MODULE(EX_MEM_Register) {
    // Clock e reset
    sc_in_clk clock;
    sc_in<bool> reset;
    
    // Entradas da fase EX
    sc_in<sc_int<DATA_BITS>> alu_result_in;
    sc_in<sc_int<DATA_BITS>> write_data_in;
    sc_in<sc_uint<REGISTER_BITS>> write_register_in;
    sc_in<bool> zero_flag_in;
    sc_in<bool> negative_flag_in;
    
    // Sinais de controle
    sc_in<bool> mem_read_in;
    sc_in<bool> mem_write_in;
    sc_in<bool> reg_write_in;
    sc_in<bool> branch_taken_in;
    sc_in<sc_int<DATA_BITS>> branch_target_in;
    
    // Saídas para fase MEM
    sc_out<sc_int<DATA_BITS>> alu_result_out;
    sc_out<sc_int<DATA_BITS>> write_data_out;
    sc_out<sc_uint<REGISTER_BITS>> write_register_out;
    sc_out<bool> zero_flag_out;
    sc_out<bool> negative_flag_out;
    
    // Sinais de controle
    sc_out<bool> mem_read_out;
    sc_out<bool> mem_write_out;
    sc_out<bool> reg_write_out;
    sc_out<bool> branch_taken_out;
    sc_out<sc_int<DATA_BITS>> branch_target_out;

    void update() {
        if (reset.read()) {
            // Zera todas as saídas no reset
            alu_result_out.write(0);
            write_data_out.write(0);
            write_register_out.write(0);
            zero_flag_out.write(false);
            negative_flag_out.write(false);
            mem_read_out.write(false);
            mem_write_out.write(false);
            reg_write_out.write(false);
            branch_taken_out.write(false);
            branch_target_out.write(0);
        } else {
            // Passa os valores para o próximo estágio
            alu_result_out.write(alu_result_in.read());
            write_data_out.write(write_data_in.read());
            write_register_out.write(write_register_in.read());
            zero_flag_out.write(zero_flag_in.read());
            negative_flag_out.write(negative_flag_in.read());
            mem_read_out.write(mem_read_in.read());
            mem_write_out.write(mem_write_in.read());
            reg_write_out.write(reg_write_in.read());
            branch_taken_out.write(branch_taken_in.read());
            branch_target_out.write(branch_target_in.read());
        }
    }

    SC_CTOR(EX_MEM_Register) {
        SC_METHOD(update);
        sensitive << clock.pos() << reset;
    }
};

#endif // EX_MEM_REGISTER_HPP