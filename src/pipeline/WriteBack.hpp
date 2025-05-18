#ifndef WRITE_BACK_HPP
#define WRITE_BACK_HPP

#include <systemc>
#include "mux/Multiplexer.hpp" // Assuming Multiplexer is in this path
#include "register/bank/RegisterBank.hpp" // Assuming RegisterBank is in this path

using namespace sc_core;
using namespace sc_dt;

/**
 * @brief Módulo do estágio Write Back (WB) do pipeline
 *
 * @tparam DATA_BITS Tamanho em bits dos dados
 * @tparam REGISTER_BITS Tamanho em bits do endereço do registrador
 *
 * Este módulo implementa o estágio final do pipeline, responsável por escrever
 * o resultado da operação (da ALU ou da memória) de volta no banco de registradores.
 */
template <unsigned int DATA_BITS = 32, unsigned int REGISTER_BITS = 5>
SC_MODULE(WriteBack) {

    // Clock e reset
    sc_in_clk clock;
    sc_in<bool> reset;

    // ----- Entradas vindas do registrador MEM-WB -----

    sc_in<sc_int<DATA_BITS>> alu_result_in;       // Resultado da ALU
    sc_in<sc_int<DATA_BITS>> read_data_in;        // Dados lidos da memória
    sc_in<sc_uint<REGISTER_BITS>> write_register_in; // Endereço do registrador de destino

    // Sinais de controle
    sc_in<bool> reg_write_in;     // Habilita escrita no banco de registradores
    sc_in<bool> mem_to_reg_in;    // Seleciona a fonte de dados para escrita (ALU ou Memória)
    sc_in<bool> branch_taken_in;  // Sinal de branch tomado (pode ser usado para lógica de controle global)
    sc_in<sc_int<DATA_BITS>> branch_target_in; // Endereço de destino do branch/jump (passado para controle)


    // ----- Saídas para o Banco de Registradores e Controle Global -----

    // Saídas para o banco de registradores
    sc_out<sc_int<DATA_BITS>> write_data_out;      // Dados a serem escritos no banco de registradores
    sc_out<sc_uint<REGISTER_BITS>> write_address_out; // Endereço do registrador de destino
    sc_out<bool> reg_write_enable_out; // Habilita escrita no banco de registradores

    // Saídas para controle global (por exemplo, para lógica de branch/jump)
    sc_out<bool> branch_taken_out;
    sc_out<sc_int<DATA_BITS>> branch_target_out;


    // Componentes internos
    // Multiplexer para selecionar entre ALU result e Memory read data
    Multiplexer<DATA_BITS, 1, sc_int<DATA_BITS>> *write_data_mux;

    sc_signal<sc_uint<1>> mem_to_reg_mux; // Sinal de controle para o multiplexador

    /**
     * @brief Processo do estágio Write Back
     *
     * Este método é sensível às entradas do registrador MEM-WB e ao clock.
     */
    void process_wb_stage() {
        mem_to_reg_mux.write((sc_uint<1>)mem_to_reg_in.read()); // Passa o sinal de controle para o multiplexador
        // Passa o endereço do registrador de destino
        write_address_out.write(write_register_in.read());

        // O sinal de habilitação de escrita no registrador vem diretamente do estágio anterior
        reg_write_enable_out.write(reg_write_in.read());

        // Passa os sinais de branch para o controle global
        branch_taken_out.write(branch_taken_in.read());
        branch_target_out.write(branch_target_in.read());
        // O multiplexador seleciona o dado a ser escrito com base em mem_to_reg_in
        // A saída do multiplexador (write_data_out) já está conectada.
    }


    SC_CTOR(WriteBack) {
        // Instancia o multiplexador para selecionar os dados de escrita
        write_data_mux = new Multiplexer<DATA_BITS, 1, sc_int<DATA_BITS>>("write_data_mux");

        // Conecta os sinais ao multiplexador
        write_data_mux->clock(clock); // Mux combinacional, clock não é estritamente necessário, mas boa prática
        write_data_mux->reset(reset); // Reset para zerar a saída no reset
        write_data_mux->inputs[0](alu_result_in);  // Entrada 0: Resultado da ALU
        write_data_mux->inputs[1](read_data_in);   // Entrada 1: Dados lidos da memória
        write_data_mux->select(mem_to_reg_mux);     // Sinal de seleção (0 para ALU, 1 para Memória)
        write_data_mux->output(write_data_out);    // Conecta a saída do mux à saída do estágio WB

        // Método para processar os outros sinais e controlar a escrita no registrador
        SC_METHOD(process_wb_stage);
        sensitive << clock.pos() << reset
                  << alu_result_in << read_data_in << write_register_in
                  << reg_write_in << mem_to_reg_in
                  << branch_taken_in << branch_target_in;

        dont_initialize(); // Não inicializa este método na simulação
    }

    // Destrutor para liberar a memória alocada
    ~WriteBack() {
        delete write_data_mux;
    }
};

#endif // WRITE_BACK_HPP
