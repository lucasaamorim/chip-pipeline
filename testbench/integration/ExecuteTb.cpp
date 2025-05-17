#include "Execute.hpp"
#include <systemc>
#include <iostream>
#include <iomanip>

using namespace sc_core;
using namespace sc_dt;

// enum AluOperation {
//     ALU_NOT = 0b0000,
//     ALU_AND = 0b0011,
//     ALU_OR  = 0b0010,
//     ALU_XOR = 0b0001,
//     ALU_ADD = 0b0111,
//     ALU_SUB = 0b1000,
//     ALU_CMP = 0b0100,
// };


SC_MODULE(ExecuteTb) {
    // Clock e reset
    sc_clock clock{"clock", 10, SC_NS};
    sc_signal<bool> reset;

    // Entradas de dados
    sc_signal<sc_int<32>> read_data_1;
    sc_signal<sc_int<32>> read_data_2;
    sc_signal<sc_int<32>> immediate_i;

    // Registradores da instrução
    sc_signal<sc_uint<4>> reg_address_2;
    sc_signal<sc_uint<4>> reg_address_3;

    // Controle
    sc_signal<sc_uint<1>> alu_src;
    sc_signal<sc_uint<1>> reg_dst;
    sc_signal<sc_uint<4>> alu_op;

    // Saídas
    sc_signal<sc_int<32>> alu_result;
    sc_signal<sc_uint<4>> write_register;


    Execute<32, 4> *execute;

    SC_CTOR(ExecuteTb) {
        execute = new Execute<32, 4>("Execute");
        execute->clock(clock);
        execute->reset(reset);
        execute->read_data_1(read_data_1);
        execute->read_data_2(read_data_2);
        execute->immediate_i(immediate_i);
        execute->reg_address_2(reg_address_2);
        execute->reg_address_3(reg_address_3);
        execute->alu_src(alu_src);
        execute->reg_dst(reg_dst);
        execute->alu_op(alu_op);
        execute->alu_result(alu_result);
        execute->write_register(write_register);

        SC_THREAD(run_tests);
    }

    void wait_clock_cycles(int cycles) {
        for (int i = 0; i < cycles; ++i) wait(clock.posedge_event());
    }

    void print_test_header(const std::string& title) {
        std::cout << "\n===== " << title << " =====\n";
    }

    template <typename T>
    void print_signal(const std::string& name, T value, T expected) {
        std::cout << std::left << std::setw(20) << name
                  << " | Esperado: " << std::setw(10) << expected
                  << " | Obtido: " << std::setw(10) << value;
        if (value == expected) {
            std::cout << " [OK]\n";
        } else {
            std::cout << " [ERRO]\n";
        }
    }

    void run_tests() {
        auto print_check = [](const std::string& name, auto value, auto expected) {
            std::cout << std::left << std::setw(20) << name
                    << " | Esperado: " << std::setw(10) << expected
                    << " | Obtido: " << std::setw(10) << value
                    << (value == expected ? " [OK]" : " [ERRO]") << std::endl;
        };

        reset = true;
        wait_clock_cycles(2);
        reset = false;
        wait_clock_cycles(1);

        // --- Teste ADD
        print_test_header("ADD");
        read_data_1 = 10;
        read_data_2 = 20;
        immediate_i = 0;
        alu_src = false;
        reg_dst = true;
        reg_address_2 = 1;
        reg_address_3 = 2;
        alu_op = ALU_ADD;
        wait_clock_cycles(1);

        print_check("ALU Result", alu_result.read(), sc_int<32>(30));
        print_check("Write Register", write_register.read(), sc_uint<4>(2));

        // --- Teste SUB
        print_test_header("SUB");
        read_data_1 = 50;
        read_data_2 = 30;
        alu_op = ALU_SUB;
        wait_clock_cycles(1);
        print_check("ALU Result", alu_result.read(), sc_int<32>(20));

        // --- Teste SUB negativo
        print_test_header("SUB negativo");
        read_data_1 = 30;
        read_data_2 = 50;
        wait_clock_cycles(1);
        print_check("ALU Result", alu_result.read(), sc_int<32>(-20));

        // --- Teste SUB zero
        print_test_header("SUB zero");
        read_data_1 = 50;
        read_data_2 = 50;
        wait_clock_cycles(1);
        print_check("ALU Result", alu_result.read(), sc_int<32>(0));

        // --- Teste AND
        print_test_header("AND");
        read_data_1 = 0b1010;
        read_data_2 = 0b1100;
        alu_op = ALU_AND;
        wait_clock_cycles(1);
        print_check("ALU Result", alu_result.read(), sc_int<32>(0b1000));

        // --- Teste OR
        print_test_header("OR");
        alu_op = ALU_OR;
        wait_clock_cycles(1);
        print_check("ALU Result", alu_result.read(), sc_int<32>(0b1110));

        // --- Teste XOR
        print_test_header("XOR");
        alu_op = ALU_XOR;
        wait_clock_cycles(1);
        print_check("ALU Result", alu_result.read(), sc_int<32>(0b0110));


        // --- Teste ADDI
        print_test_header("ADDI");
        read_data_1 = 100;
        immediate_i = 50;
        alu_src = true;
        reg_dst = false;
        alu_op = ALU_ADD;
        reg_address_2 = 1;
        wait_clock_cycles(1);
        print_check("ALU Result", alu_result.read(), sc_int<32>(150));
        print_check("Write Register", write_register.read(), sc_uint<4>(1));

        // --- Reset final
        print_test_header("RESET");
        reset = true;
        wait_clock_cycles(1);
        print_check("ALU Result", alu_result.read(), sc_int<32>(0));
        reset = false;

        std::cout << "\n--- Todos os testes concluídos ---\n";
        sc_stop();
    }

};

int sc_main(int argc, char* argv[]) {
    ExecuteTb tb("ExecuteTb");
    sc_start();
    return 0;
}
