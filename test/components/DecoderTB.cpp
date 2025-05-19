#include "decoder/Decoder.hpp"
#include <systemc>
#include <iostream>
#include <iomanip>

SC_MODULE(DecoderTestBench) {
    sc_clock clk;
    sc_signal<bool> reset;
    sc_signal<sc_uint<32>> instruction;
    sc_signal<sc_uint<4>> rs, rt, rd;
    sc_signal<sc_uint<32>> immediate_i, immediate_j;
    sc_signal<bool> reg_dst, alu_src, mem_to_reg, reg_write, mem_read, mem_write, jump, jump_n, jump_z;
    sc_signal<sc_uint<3>> alu_op;

    Decoder* decoder;

    void print_control_signals() {
        std::cout << "  Controle: [";
        std::cout << "reg_dst=" << reg_dst.read();
        std::cout << " alu_src=" << alu_src.read();
        std::cout << " mem_to_reg=" << mem_to_reg.read();
        std::cout << " reg_write=" << reg_write.read();
        std::cout << " mem_read=" << mem_read.read();
        std::cout << " mem_write=" << mem_write.read();
        std::cout << " jump=" << jump.read();
        std::cout << " jump_n=" << jump_n.read();
        std::cout << " jump_z=" << jump_z.read();
        std::cout << " alu_op=" << alu_op.read();
        std::cout << "]" << std::endl;
    }

    void print_instruction_fields() {
        std::cout << "  Campos: [";
        std::cout << "rs=" << rs.read();
        std::cout << " rt=" << rt.read();
        std::cout << " rd=" << rd.read();
        std::cout << " imm_i=0x" << std::hex << immediate_i.read();
        std::cout << " imm_j=0x" << std::hex << immediate_j.read() << std::dec;
        std::cout << "]" << std::endl;
    }

    void test_sequence() {
        std::cout << "\n=== INICIANDO TESTES DO DECODER ===" << std::endl;

        // 1. Teste de reset
        std::cout << "\n[TESTE 1] Reset inicial" << std::endl;
        reset.write(true);
        instruction.write(0);
        wait(10, SC_NS);
        std::cout << "[" << sc_time_stamp() << "] Reset ativo" << std::endl;
        print_control_signals();
        print_instruction_fields();

        // 2. Teste instruções tipo R
        std::cout << "\n[TESTE 2] Instruções Tipo R" << std::endl;
        reset.write(false);

        // AND rd, rs, rt
        instruction.write(0b00001100011000100010000000000000);
        wait(10, SC_NS);
        std::cout << "[" << sc_time_stamp() << "] AND rd, rs, rt" << std::endl;
        print_control_signals();
        print_instruction_fields();

        // OR rd, rs, rt
        instruction.write(0b00001000011000100010000000000000);
        wait(10, SC_NS);
        std::cout << "[" << sc_time_stamp() << "] OR rd, rs, rt" << std::endl;
        print_control_signals();
        print_instruction_fields();

        // ADD rd, rs, rt
        instruction.write(0b00001110011000100010000000000000);
        wait(10, SC_NS);
        std::cout << "[" << sc_time_stamp() << "] ADD rd, rs, rt" << std::endl;
        print_control_signals();
        print_instruction_fields();

        // 3. Teste instruções tipo I
        std::cout << "\n[TESTE 3] Instruções Tipo I" << std::endl;
        
        // LD rt, rs, imm
        instruction.write(0b10011010011000100000000000000000);
        wait(10, SC_NS);
        std::cout << "[" << sc_time_stamp() << "] LD rt, rs, imm" << std::endl;
        print_control_signals();
        print_instruction_fields();

        // ADDI rt, rs, imm
        instruction.write(0b10001110011000100000000000000000);
        wait(10, SC_NS);
        std::cout << "[" << sc_time_stamp() << "] ADDI rt, rs, imm" << std::endl;
        print_control_signals();
        print_instruction_fields();

        // NOT rt, rs
        instruction.write(0b10000000011000100000000000000000);
        wait(10, SC_NS);
        std::cout << "[" << sc_time_stamp() << "] NOT rt, rs" << std::endl;
        print_control_signals();
        print_instruction_fields();

        // 4. Teste instruções tipo J
        std::cout << "\n[TESTE 4] Instruções Tipo J" << std::endl;
        
        // JUMP endereço
        instruction.write(0b11111110011000100010000000000000);
        wait(10, SC_NS);
        std::cout << "[" << sc_time_stamp() << "] JUMP endereço" << std::endl;
        print_control_signals();
        print_instruction_fields();

        // JUMPN endereço
        instruction.write(0b11110010011000100010000000000000);
        wait(10, SC_NS);
        std::cout << "[" << sc_time_stamp() << "] JUMPN endereço" << std::endl;
        print_control_signals();
        print_instruction_fields();

        // 5. Teste reset durante operação
        std::cout << "\n[TESTE 5] Reset durante operação" << std::endl;
        instruction.write(0b00001110011000100010000000000000); // ADD
        wait(5, SC_NS);
        reset.write(true);
        wait(5, SC_NS);
        std::cout << "[" << sc_time_stamp() << "] Reset ativo durante ADD" << std::endl;
        print_control_signals();
        print_instruction_fields();

        std::cout << "\n=== TESTES CONCLUÍDOS ===" << std::endl;
        sc_stop();
    }

    SC_CTOR(DecoderTestBench) : clk("clk", 10, SC_NS) {
        decoder = new Decoder("decoder");
        decoder->clk(clk);
        decoder->reset(reset);
        decoder->instruction(instruction);
        decoder->rs(rs);
        decoder->rt(rt);
        decoder->rd(rd);
        decoder->immediate_i(immediate_i);
        decoder->immediate_j(immediate_j);
        decoder->reg_dst(reg_dst);
        decoder->alu_src(alu_src);
        decoder->mem_to_reg(mem_to_reg);
        decoder->reg_write(reg_write);
        decoder->mem_read(mem_read);
        decoder->mem_write(mem_write);
        decoder->jump(jump);
        decoder->jump_n(jump_n);
        decoder->jump_z(jump_z);
        decoder->alu_op(alu_op);

        SC_THREAD(test_sequence);
        sensitive << clk.posedge_event();
    }

    ~DecoderTestBench() {
        delete decoder;
    }
};

int sc_main(int argc, char* argv[]) {
    DecoderTestBench tb("tb");
    sc_start(200, SC_NS);
    return 0;
}