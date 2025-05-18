#include "WriteBack.hpp"
#include "mux/Multiplexer.hpp" 
#include "register/bank/RegisterBank.hpp"
#include <systemc>
#include <iostream>
#include <iomanip>

using namespace sc_core;
using namespace sc_dt;
using namespace std;

constexpr unsigned int DATA_BITS = 32;
constexpr unsigned int REGISTER_BITS = 5; // Assuming 5 bits for register address based on RegisterBank

SC_MODULE(WriteBackTb) {
    // Clock e Reset
    sc_clock clock;
    sc_signal<bool> reset;


    // Inputs (do registrador MEM-WB)
    sc_signal<sc_int<DATA_BITS>> alu_result_in;       // Resultado da ALU
    sc_signal<sc_int<DATA_BITS>> read_data_in;        // Dados lidos da memória
    sc_signal<sc_uint<REGISTER_BITS>> write_register_in; // Endereço do registrador de destino

    // Sinais de Controle (Entradas)
    sc_signal<bool> reg_write_in;     // Habilitar escrita no arquivo de registradores
    sc_signal<bool> mem_to_reg_in;    // Selecionar fonte de dados (resultado da ALU ou dados da memória)
    sc_signal<bool> branch_taken_in;  // Sinal de branch tomado
    sc_signal<sc_int<DATA_BITS>> branch_target_in; // Endereço de destino do branch

    // Outputs
    sc_signal<sc_int<DATA_BITS>> write_data_out;      // Dados a serem escritos no arquivo de registradores
    sc_signal<sc_uint<REGISTER_BITS>> write_address_out; // Endereço do registrador de destino
    sc_signal<bool> reg_write_enable_out; // Habilitar escrita no arquivo de registradores

    // Sinais de Controle (Saída)
    sc_signal<bool> branch_taken_out;
    sc_signal<sc_int<DATA_BITS>> branch_target_out;

    // Módulo a ser testado
    WriteBack<DATA_BITS, REGISTER_BITS> *wb_stage;

    SC_CTOR(WriteBackTb) : clock("clock", 10, SC_NS) {
        wb_stage = new WriteBack<DATA_BITS, REGISTER_BITS>("WB_Stage");

        // Conectando os sinais ao módulo WriteBack
        wb_stage->clock(clock);
        wb_stage->reset(reset);

        wb_stage->alu_result_in(alu_result_in);
        wb_stage->read_data_in(read_data_in);
        wb_stage->write_register_in(write_register_in);

        wb_stage->reg_write_in(reg_write_in);
        wb_stage->mem_to_reg_in(mem_to_reg_in);
        wb_stage->branch_taken_in(branch_taken_in);
        wb_stage->branch_target_in(branch_target_in);

        wb_stage->write_data_out(write_data_out);
        wb_stage->write_address_out(write_address_out);
        wb_stage->reg_write_enable_out(reg_write_enable_out);

        wb_stage->branch_taken_out(branch_taken_out);
        wb_stage->branch_target_out(branch_target_out);

        SC_THREAD(stimulus);
        sensitive << clock.posedge_event(); 
    }

    // Destructor
    ~WriteBackTb() {
        delete wb_stage;
    }

    // Função auxiliar para esperar ciclos de clock
    void wait_clock_cycles(int cycles) {
        for (int i = 0; i < cycles; ++i) wait(clock.posedge_event());
    }

    // Printar os sinais de saída
    void print_output_signals(const string& test_name) {
        cout << "\n--- " << test_name << " ---" << endl;
        cout << "Cycle: " << sc_time_stamp() << endl;
        cout << "  write_data_out      : " << write_data_out.read() << endl;
        cout << "  write_address_out   : " << write_address_out.read() << endl;
        cout << "  reg_write_enable_out: " << (reg_write_enable_out.read() ? "True" : "False") << endl;
        cout << "  branch_taken_out    : " << (branch_taken_out.read() ? "True" : "False") << endl;
        cout << "  branch_target_out   : " << branch_target_out.read() << endl;
        cout << "----------------------------------------" << endl;
    }

    void stimulus() {
        cout << "Iniciando testbench para o estágio Write Back..." << endl;

        // Reset inicial
        reset.write(true);
        alu_result_in.write(0);
        read_data_in.write(0);
        write_register_in.write(0);
        reg_write_in.write(false);
        mem_to_reg_in.write(false);
        branch_taken_in.write(false);
        branch_target_in.write(0);

        wait_clock_cycles(2); // Esperar 2 ciclos para o reset propagar

        reset.write(false);
        cout << "Reset desativado." << endl;
        wait_clock_cycles(1); // Esperar um ciclo após o reset
        cout << "Iniciando os testes..." << endl;

        // --- Test Case 1: Escrever resultado da ALU (e.g., ADD, SUB) ---
        cout << "\n--- Teste 1: Escrever resultado da ALU ---" << endl;
        alu_result_in.write(150);         // resultado da ALU
        read_data_in.write(999);
        write_register_in.write(8);       // Destino R8
        reg_write_in.write(true);         // Escrever no registrador
        mem_to_reg_in.write(false);       // Selecionar resultado da ALU
        branch_taken_in.write(false);
        branch_target_in.write(0);

        wait_clock_cycles(1); // Executar WB

        print_output_signals("Teste 1 Resultados");
        // Add assertions here to check expected outputs
        if (write_data_out.read() == 150 && 
            write_address_out.read() == 8 &&
            reg_write_enable_out.read() == true) {
            cout << "\033[1;32mTESTE 1 OK\033[0m" << endl;
        } else {
            cout << "\033[1;31mTESTE 1 ERRO\033[0m" << endl;
        }

        // --- Test Case 2: Write back Memory data (e.g., LD) ---
        cout << "\n--- Teste 2: Escrever dados da Memória ---" << endl;
        alu_result_in.write(777);         // ALU result (should be ignored)
        read_data_in.write(0xFACA);       // Data read from memory
        write_register_in.write(12);      // Destination register R12
        reg_write_in.write(true);         // Enable write back
        mem_to_reg_in.write(true);        // Select Memory data
        branch_taken_in.write(false); 
        branch_target_in.write(0);

        wait_clock_cycles(1); // Executar WB

        print_output_signals("Teste 2 Resultados");
        if (write_data_out.read() == 0xFACA && 
            write_address_out.read() == 12 &&
            reg_write_enable_out.read() == true) {
            cout << "\033[1;32mTESTE 2 OK\033[0m" << endl;
        } else {
            cout << "\033[1;31mTESTE 2 ERRO\033[0m" << endl;
        }

        // --- Test Case 3: Instruction that does NOT write back (e.g., ST, J, Branch) ---
        cout << "\n--- Teste 3: Nao escrever no Banco de Registradores ---" << endl;
        alu_result_in.write(123);
        read_data_in.write(456);
        write_register_in.write(1);       // Endereço do registrador (deve ser ignorado pelo RegFile)
        reg_write_in.write(false);        // Desativar escrita no registrador
        mem_to_reg_in.write(false);       // Não será usado (reg_write_in = false)
        branch_taken_in.write(true);      // Exemplo: uma instrução de branch
        branch_target_in.write(0x100);    // Endereço de destino do branch

        wait_clock_cycles(1); // Executar WB

        print_output_signals("Teste 3 Resultados");
        if (reg_write_enable_out.read() == false && 
            branch_taken_out.read() == true && 
            branch_target_out.read() == 0x100) {
            cout << "\033[1;32mTESTE 3 OK\033[0m" << endl;
        } else {
            cout << "\033[1;31mTESTE 3 ERRO\033[0m" << endl;
        }

        // --- Test Case 4: Reset ---
        cout << "\n--- Teste 4: Reset ---" << endl;
        reset.write(true);
        alu_result_in.write(0);
        read_data_in.write(0);
        write_register_in.write(0);
        reg_write_in.write(false);
        mem_to_reg_in.write(false);
        branch_taken_in.write(false);
        branch_target_in.write(0);

        wait_clock_cycles(1); // Executar WB

        print_output_signals("Teste 4 Resultados (Reset)");
        if (write_data_out.read() == 0 &&
            write_address_out.read() == 0 &&
            reg_write_enable_out.read() == false &&
            branch_taken_out.read() == false &&
            branch_target_out.read() == 0) {
            cout << "\033[1;32mTESTE 4 OK\033[0m" << endl;
        } else {
            cout << "\033[1;31mTESTE 4 ERRO\033[0m" << endl;
        }

        cout << "\nTestbench para o estágio Write Back concluído." << endl;
        sc_stop(); // Encerrar a simulação
    }
};

int sc_main(int argc, char* argv[]) {
    WriteBackTb tb("WB_Testbench");
    sc_start();
    return 0;
}
