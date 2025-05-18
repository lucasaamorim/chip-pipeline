#include "MemoryAccess.hpp" // Inclui o módulo MEM
#include <systemc>
#include <iostream>
#include <vector>
#include <iomanip> // Para formatação de saída

using namespace sc_core;
using namespace sc_dt;
using namespace std;

// Definições de parâmetros (devem corresponder aos do módulo MEM)
constexpr unsigned int DATA_BITS = 32;
constexpr unsigned int ADDRESS_BITS = 8;
constexpr unsigned int REGISTER_BITS = 4;

SC_MODULE(MEMTb) {
    // Clock
    sc_clock clock;

    // Sinais para conectar ao módulo MEM
    sc_signal<bool> reset;

    // Entradas (do registrador EX-MEM)
    sc_signal<sc_int<DATA_BITS>> alu_result_in;       // Resultado da ALU (endereço de memória)
    sc_signal<sc_int<DATA_BITS>> write_data_in;       // Dados a serem escritos na memória (para ST)
    sc_signal<sc_uint<REGISTER_BITS>> write_register_in; // Endereço do registrador de destino (para WB)
    sc_signal<bool> zero_flag_in;                     // Flag zero da ALU
    sc_signal<bool> negative_flag_in;                 // Flag negativo da ALU

    // Sinais de controle (do registrador EX-MEM)
    sc_signal<bool> mem_read_in;                      // Habilita leitura da memória
    sc_signal<bool> mem_write_in;                     // Habilita escrita na memória
    sc_signal<bool> reg_write_in;                     // Habilita escrita no banco de registradores (para WB)
    sc_signal<bool> branch_taken_in;                  // Sinal de branch tomado
    sc_signal<sc_int<DATA_BITS>> branch_target_in;    // Endereço de destino do branch/jump

    // Saídas (para o próximo registrador MEM-WB)
    sc_signal<sc_int<DATA_BITS>> alu_result_out;     // Resultado da ALU (passa para WB)
    sc_signal<sc_int<DATA_BITS>> read_data_out;      // Dados lidos da memória (para LD e WB)
    sc_signal<sc_uint<REGISTER_BITS>> write_register_out; // Endereço do registrador de destino (para WB)

    // Sinais de controle (passam para o próximo estágio)
    sc_signal<bool> mem_read_out;                    // Passa habilitação de leitura
    sc_signal<bool> mem_write_out;                   // Passa habilitação de escrita
    sc_signal<bool> reg_write_out;                   // Habilita escrita no banco de registradores (para WB)
    sc_signal<bool> branch_taken_out;                // Passa sinal de branch tomado
    sc_signal<sc_int<DATA_BITS>> branch_target_out;  // Passa endereço de destino do branch/jump

    // Instância do módulo MEM sob teste
    MEM<DATA_BITS, ADDRESS_BITS, REGISTER_BITS> *mem_stage;

    SC_CTOR(MEMTb) : clock("clock", 10, SC_NS) {
        mem_stage = new MEM<DATA_BITS, ADDRESS_BITS, REGISTER_BITS>("MEM_Stage");

        // Conectando os sinais ao módulo MEM
        mem_stage->clock(clock);
        mem_stage->reset(reset);

        mem_stage->alu_result_in(alu_result_in);
        mem_stage->write_data_in(write_data_in);
        mem_stage->write_register_in(write_register_in);
        mem_stage->zero_flag_in(zero_flag_in);
        mem_stage->negative_flag_in(negative_flag_in);

        mem_stage->mem_read_in(mem_read_in);
        mem_stage->mem_write_in(mem_write_in);
        mem_stage->reg_write_in(reg_write_in);
        mem_stage->branch_taken_in(branch_taken_in);
        mem_stage->branch_target_in(branch_target_in);

        mem_stage->alu_result_out(alu_result_out);
        mem_stage->read_data_out(read_data_out);
        mem_stage->write_register_out(write_register_out);

        mem_stage->reg_write_out(reg_write_out);
        mem_stage->branch_taken_out(branch_taken_out);
        mem_stage->branch_target_out(branch_target_out);

        // Registrar o método de estímulo
        SC_THREAD(stimulus);
        sensitive << clock.posedge_event();
    }

    // Destrutor
    ~MEMTb() {
        delete mem_stage;
    }

    // Função auxiliar para esperar ciclos de clock
    void wait_clock_cycles(int cycles) {
        for (int i = 0; i < cycles; ++i) wait(clock.posedge_event());
    }

    // Função auxiliar para imprimir resultados
    void print_output_signals() {
        cout << "----------------------------------------" << endl;
        cout << "Cycle: " << sc_time_stamp() << endl;
        cout << "  alu_result_out    : " << alu_result_out.read() << endl;
        cout << "  read_data_out     : " << read_data_out.read() << endl;
        cout << "  write_register_out: " << write_register_out.read() << endl;
        //cout << "  zero_flag_out     : " << zero_flag_out.read() << endl;
        //cout << "  negative_flag_out : " << negative_flag_out.read() << endl;
        cout << "  mem_read_out      : " << mem_read_out.read() << endl;
        cout << "  mem_write_out     : " << mem_write_out.read() << endl;
        cout << "  reg_write_out     : " << (reg_write_out.read() ? "True" : "False") << endl;
        cout << "  branch_taken_out  : " << (branch_taken_out.read() ? "True" : "False") << endl;
        cout << "  branch_target_out : " << branch_target_out.read() << endl;
        cout << "----------------------------------------" << endl;
    }

    // Método de estímulo
    void stimulus() {
        // Inicialização
        cout << "Iniciando testbench para o estágio MEM..." << endl;
        reset.write(true);
        alu_result_in.write(0);
        write_data_in.write(0);
        write_register_in.write(0);
        zero_flag_in.write(false);
        negative_flag_in.write(false);
        mem_read_in.write(false);
        mem_write_in.write(false);
        reg_write_in.write(false);
        branch_taken_in.write(false);
        branch_target_in.write(0);

        wait_clock_cycles(2); // Espera 2 ciclos para reset

        reset.write(false);
        cout << "Reset desativado." << endl;
        wait_clock_cycles(1); // Espera 1 ciclo para sair do reset

        // --- Teste 1: Instrução sem acesso à memória (ex: ADD, SUB) ---
        cout << "\n--- Teste 1: Instrução sem acesso à memória ---" << endl;
        alu_result_in.write(100);         // Resultado da ALU (não usado pela memória neste caso)
        write_register_in.write(5);       // Registrador de destino
        reg_write_in.write(true);         // Habilita escrita no registrador (no estágio WB)
        zero_flag_in.write(false);
        negative_flag_in.write(false);
        mem_read_in.write(false);
        mem_write_in.write(false);
        branch_taken_in.write(false);
        branch_target_in.write(0);

        wait_clock_cycles(1); // Executa o estágio MEM

        // Verifica as saídas (devem ser as mesmas das entradas, exceto read_data_out)
        print_output_signals();
        if (alu_result_out.read() == alu_result_in.read()) {
            cout << "\033[1;32mTESTE 1 OK\033[0m" << endl;
        } else {
            cout << "\033[1;31mTESTE 1 ERRO\033[0m" << endl;
        }
        // Adicionar asserções ou verificações mais detalhadas aqui

        // --- Teste 2: Instrução ST (Store) ---
        cout << "\n--- Teste 2: Instrução ST ---" << endl;
        alu_result_in.write(20);          // Endereço de memória (resultado da ALU)
        write_data_in.write(0xabcd00);  // Dados a serem escritos
        write_register_in.write(0);       // Não relevante para ST, mas passa adiante
        reg_write_in.write(false);        // ST não escreve no registrador
        zero_flag_in.write(false);
        negative_flag_in.write(false);
        mem_read_in.write(false);
        mem_write_in.write(true);         // Habilita escrita na memória
        branch_taken_in.write(false);
        branch_target_in.write(0);

        
        wait_clock_cycles(1); // Executa o estágio MEM (escrita na memória)
        wait(SC_ZERO_TIME); // dá um pouco mais de tempo para a memória responder
        print_output_signals();
        if (mem_stage->data_memory->memory[20] == 0xabcd00) {
            cout << "\033[1;32mTESTE 2 OK\033[0m" << endl;
        } else {
            cout << "\033[1;31mTESTE 2 ERRO\033[0m" << endl;
        }
        
        // --- Teste 3: Instrução LD (Load) ---
        cout << "\n--- Teste 3: Instrução LD ---" << endl;
        // -----------------------------------------------------------

        // Inicializa a memória de dados para o teste de leitura
        vector<sc_int<DATA_BITS>> initial_data(1 << ADDRESS_BITS, 0); // Cria um vetor zerado do tamanho da memória
        initial_data[30] = 0xf23611b; // Coloca um valor conhecido na posição 30
        mem_stage->initialize_data_memory(initial_data); // Chame o novo método

        // Simula a leitura
        alu_result_in.write(30);          // Endereço de memória a ser lido
        write_data_in.write(0);           // Não relevante para LD
        write_register_in.write(7);       // Registrador de destino para o valor lido
        reg_write_in.write(true);         // Habilita escrita no registrador (no estágio WB)
        zero_flag_in.write(false);
        negative_flag_in.write(false);
        mem_read_in.write(true);          // Habilita leitura da memória
        mem_write_in.write(false);
        branch_taken_in.write(false);
        branch_target_in.write(0);

        wait_clock_cycles(1); // Executa o estágio MEM (leitura da memória)
        wait(SC_ZERO_TIME); // dá um pouco mais de tempo para a memória responder
        
        print_output_signals();
        if (read_data_out.read() == 0xf23611b) {
            cout << "\033[1;32mTESTE 3 OK\033[0m" << endl;
        } else {
            cout << "\033[1;31mTESTE 3 ERRO\033[0m" << endl;
        }
        // Verifica se o valor lido é o esperado
        cout << "  Valor lido esperado: " << 0xf23611b << dec << endl;

        // --- Teste 4: Branch tomado (passa sinais de branch) ---
        cout << "\n--- Teste 4: Branch tomado ---" << endl;
        alu_result_in.write(0);
        write_data_in.write(0);
        write_register_in.write(0);
        reg_write_in.write(false);
        zero_flag_in.write(true); // Exemplo: branch se zero
        negative_flag_in.write(false);
        mem_read_in.write(false);
        mem_write_in.write(false);
        branch_taken_in.write(true);      // Sinal de branch tomado
        branch_target_in.write(0x50);     // Endereço de destino do branch
    
        wait_clock_cycles(1); // Executa o estágio MEM

        print_output_signals();
        // Verifica se os sinais de branch foram passados
        if (branch_taken_out.read() == true && branch_target_out.read() == 0x50) {
            cout << "\033[1;32mTESTE 4 OK\033[0m" << endl;
        } else {
            cout << "\033[1;31mTESTE 4 ERRO\033[0m" << endl;
        }

        // --- Reset final ---
        cout << "\n--- Reset Final ---" << endl;
        reset.write(true);
        alu_result_in.write(0);
        write_data_in.write(0);
        write_register_in.write(0);
        reg_write_in.write(false);
        zero_flag_in.write(false);
        negative_flag_in.write(false);
        mem_read_in.write(false);
        mem_write_in.write(false);
        branch_taken_in.write(false);
        branch_target_in.write(0);
        mem_stage->reset_memory();
        wait_clock_cycles(2);
        reset.write(false);
        wait_clock_cycles(1);
        wait(SC_ZERO_TIME);
        print_output_signals(); // Saídas devem estar zeradas após reset
        if (alu_result_out.read() == 0 && write_register_out.read() == 0 && reg_write_out == false && branch_taken_out.read() == false && branch_target_out.read() == 0) {
            cout << "\033[1;32mRESET FINAL OK\033[0m" << endl;
        } else {
            cout << "\033[1;31mRESET FINAL ERRO\033[0m" << endl;
        }

        cout << "\nTestbench para o estágio MEM concluído." << endl;
        sc_stop(); // Finaliza a simulação
    }
};

int sc_main(int argc, char* argv[]) {
    MEMTb tb("MEM_Testbench");
    sc_start();
    return 0;
}