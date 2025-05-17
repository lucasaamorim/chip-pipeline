#ifndef EX_MEM_HPP
#define EX_MEM_HPP

#include <systemc.h>

/**
 * @brief Módulo do registrador EX/MEM do pipeline MIPS
 * 
 * @tparam DATA_BITS Tamanho em bits dos dados (padrão 32 para MIPS)
 * @tparam REGISTER_BITS Tamanho em bits do endereço do registrador (padrão 5 para MIPS)
 * 
 * Este módulo implementa o registrador de pipeline entre os estágios de Execução (EX)
 * e Acesso à Memória (MEM). Ele armazena todos os sinais necessários para o estágio
 * MEM e para o estágio de Write Back (WB) subsequente.
 */
template <unsigned int DATA_BITS = 32, unsigned int REGISTER_BITS = 5>
SC_MODULE(EX_MEM) {

  sc_in_clk clock;      
  sc_in<bool> reset;   
  
  // ----- Entradas vindas do estágio EX -----
  
  // Resultados da execução
  sc_in<sc_int<DATA_BITS>> alu_result_in;       // Resultado calculado pela ALU
  sc_in<sc_int<DATA_BITS>> write_data_in;       // Dado a ser escrito na memória (para instruções store)
  sc_in<sc_uint<REGISTER_BITS>> write_register_in; // Registrador destino (rt ou rd)
  
  // Sinais de controle
  sc_in<bool> reg_write_in;     // Indica se a instrução escreve no banco de registradores
  sc_in<bool> mem_to_reg_in;    // Seleciona entre memória ou ALU para o dado de escrita no registrador
  sc_in<bool> mem_read_in;      // Habilita leitura da memória (para load)
  sc_in<bool> mem_write_in;     // Habilita escrita na memória (para store)
  sc_in<bool> branch_in;        // Indica se é uma instrução de branch
  sc_in<bool> zero_flag_in;     // Flag zero da ALU (usada para branches)

  // ----- Saídas para o estágio MEM -----
  
  // Dados processados
  sc_out<sc_int<DATA_BITS>> alu_result_out;     // Resultado da ALU para estágio MEM
  sc_out<sc_int<DATA_BITS>> write_data_out;     // Dado para escrita na memória
  sc_out<sc_uint<REGISTER_BITS>> write_register_out; // Registrador destino
  
  // Sinais de controle
  sc_out<bool> reg_write_out;   // Passa o sinal de escrita no registrador
  sc_out<bool> mem_to_reg_out;  // Passa o sinal de seleção de fonte para WB
  sc_out<bool> mem_read_out;    // Passa o sinal de leitura da memória
  sc_out<bool> mem_write_out;   // Passa o sinal de escrita na memória
  sc_out<bool> branch_out;      // Passa o sinal de branch
  sc_out<bool> zero_flag_out;   // Passa a flag zero da ALU
  sc_out<sc_int<DATA_BITS>> branch_target_out;  // Alvo calculado do branch (vindo da ALU)

    
  /**
   * @brief Atualiza os registradores de pipeline
   * 
   * Este método é sensível à borda de subida do clock e ao reset.
   */
  void update() {
    if (reset.read()) {
      // Reset assíncrono - limpa todos os registradores
      alu_result_out.write(0);
      write_data_out.write(0);
      write_register_out.write(0);
      reg_write_out.write(false);
      mem_to_reg_out.write(false);
      mem_read_out.write(false);
      mem_write_out.write(false);
      branch_out.write(false);
      zero_flag_out.write(false);
      branch_target_out.write(0);
    } 
    else if (clock.posedge()) {
      // Na borda de subida do clock, registra todos os sinais
      
      // Dados processados
      alu_result_out.write(alu_result_in.read());
      write_data_out.write(write_data_in.read());
      write_register_out.write(write_register_in.read());
      
      // Sinais de controle
      reg_write_out.write(reg_write_in.read());
      mem_to_reg_out.write(mem_to_reg_in.read());
      mem_read_out.write(mem_read_in.read());
      mem_write_out.write(mem_write_in.read());
      branch_out.write(branch_in.read());
      zero_flag_out.write(zero_flag_in.read());
      
      // O alvo do branch é o resultado da ALU (calculado no estágio EX)
      branch_target_out.write(alu_result_in.read());
    }
  }

    

  SC_CTOR(EX_MEM) {
    SC_METHOD(update);
    sensitive << clock.pos() << reset;
    
    dont_initialize();
  }
};

#endif // EX_MEM_HPP