#ifndef MEM_WB_REGISTER_HPP
#define MEM_WB_REGISTER_HPP

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

/**
 * @brief Módulo do registrador MEM/WB do pipeline
 *
 * @tparam DATA_BITS Tamanho em bits dos dados
 * @tparam REGISTER_BITS Tamanho em bits do endereço do registrador
 *
 * Este módulo implementa o registrador de pipeline entre os estágios de Acesso à Memória (MEM)
 * e Write Back (WB). Ele armazena todos os sinais necessários para o estágio WB.
 */
template <unsigned int DATA_BITS = 32, unsigned int REGISTER_BITS = 5>
SC_MODULE(MEM_WB_Register) {

    sc_in_clk clock;
    sc_in<bool> reset;

    // ----- Entradas vindas do estágio MEM -----

    // Dados processados
    sc_in<sc_int<DATA_BITS>> alu_result_in;       // Resultado da ALU (passado direto para WB se não for load)
    sc_in<sc_int<DATA_BITS>> read_data_in;        // Dados lidos da memória (para instruções load)
    sc_in<sc_uint<REGISTER_BITS>> write_register_in; // Endereço do registrador de destino

    // Sinais de controle
    sc_in<bool> reg_write_in;     // Habilita escrita no banco de registradores
    //sc_in<bool> mem_to_reg_in;    // Seleciona entre memória ou ALU para o dado de escrita no registrador
    sc_in<bool> branch_taken_in;  // Sinal de branch tomado (para tratamento de hazards de controle)
    sc_in<sc_int<DATA_BITS>> branch_target_in; // Endereço de destino do branch/jump (passado para controle)
    sc_in<bool> mem_to_reg_in; // Sinal de controle para selecionar entre ALU e memória

    // ----- Saídas para o estágio WB -----

    // Dados a serem escritos no banco de registradores
    sc_out<sc_int<DATA_BITS>> alu_result_out;
    sc_out<sc_int<DATA_BITS>> read_data_out;
    sc_out<sc_uint<REGISTER_BITS>> write_register_out;

    // Sinais de controle para WB
    sc_out<bool> reg_write_out;
    sc_out<bool> mem_to_reg_out;
    sc_out<bool> branch_taken_out;
    sc_out<sc_int<DATA_BITS>> branch_target_out;


    /**
     * @brief Atualiza os registradores de pipeline
     *
     * Este método é sensível à borda de subida do clock e ao reset.
     */
    void update() {
        if (reset.read()) {
            // Reset assíncrono - limpa todos os registradores
            alu_result_out.write(0);
            read_data_out.write(0);
            write_register_out.write(0);
            reg_write_out.write(false);
            mem_to_reg_out.write(false);
            branch_taken_out.write(false);
            branch_target_out.write(0);
        }
        else if (clock.posedge()) { // Podia ser só um else também, mas não custa nada checar
            // Na borda de subida do clock, registra todos os sinais
            alu_result_out.write(alu_result_in.read());
            read_data_out.write(read_data_in.read());
            write_register_out.write(write_register_in.read());
            reg_write_out.write(reg_write_in.read());
            mem_to_reg_out.write(mem_to_reg_in.read());
            branch_taken_out.write(branch_taken_in.read());
            branch_target_out.write(branch_target_in.read());
        }
    }


    SC_CTOR(MEM_WB_Register) {
        SC_METHOD(update);
        sensitive << clock.pos() << reset;

        dont_initialize();
    }
};

#endif // MEM_WB_REGISTER_HPP
