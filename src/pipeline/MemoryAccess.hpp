#ifndef MEM_HPP
#define MEM_HPP

#include <systemc>
#include "memory/Memory.hpp"

using namespace sc_core;
using namespace sc_dt;


template <unsigned int DATA_BITS = 32, unsigned int ADDRESS_BITS = 8,
          unsigned int REGISTER_BITS = 4>
SC_MODULE(MEM) {
    // Clock e reset
    sc_in_clk clock;
    sc_in<bool> reset;

    // Entradas
    sc_in<sc_int<DATA_BITS>> alu_result_in;       // Resultado da ALU (endereço de memória)
    sc_in<sc_int<DATA_BITS>> write_data_in;       // Dados a serem escritos na memória (para ST)
    sc_in<sc_uint<REGISTER_BITS>> write_register_in; // Endereço do registrador de destino (para WB)
    sc_in<bool> zero_flag_in;                     // Flag zero da ALU
    sc_in<bool> negative_flag_in;                 // Flag negativo da ALU

    // Sinais de controle
    sc_in<bool> mem_read_in;                      // Habilita leitura da memória
    sc_in<bool> mem_write_in;                     // Habilita escrita na memória
    sc_in<bool> reg_write_in;                     // Habilita escrita no banco de registradores (para WB)
    sc_in<bool> branch_taken_in;                  // Sinal de branch tomado (para tratamento de hazards de controle)
    sc_in<sc_int<DATA_BITS>> branch_target_in;    // Endereço de destino do branch/jump

    // Saídas
    sc_out<sc_int<DATA_BITS>> alu_result_out;     // Resultado da ALU (passa para WB)
    sc_out<sc_int<DATA_BITS>> read_data_out;      // Dados lidos da memória (para LD e WB)
    sc_out<sc_uint<REGISTER_BITS>> write_register_out; // Endereço do registrador de destino (para WB)

    // Sinais de controle
    sc_out<bool> reg_write_out;                   // Habilita escrita no banco de registradores (para WB)
    sc_out<bool> branch_taken_out;                // Passa sinal de branch tomado
    sc_out<sc_int<DATA_BITS>> branch_target_out;  // Passa endereço de destino do branch/jump

    // Componente de memória de dados
    Memory<DATA_BITS, ADDRESS_BITS, sc_int<DATA_BITS>> *data_memory;

    // Sinal intermediário para o endereço da memória
    sc_signal<sc_uint<ADDRESS_BITS>> memory_address_sig;

    // Sinal intermediário para a saída da memória
    sc_signal<sc_int<DATA_BITS>> data_memory_output;

    void reset_memory() {
        data_memory->reset_memory();
    }

    void process_mem_stage() {
        memory_address_sig.write( (sc_uint<ADDRESS_BITS>) alu_result_in.read() );

        // Passa os sinais de controle e outros dados diretamente para o próximo estágio
        // A lógica de escrita/leitura na memória é tratada pelo módulo Memory
        alu_result_out.write(alu_result_in.read());
        write_register_out.write(write_register_in.read());
        reg_write_out.write(reg_write_in.read());
        branch_taken_out.write(branch_taken_in.read());
        branch_target_out.write(branch_target_in.read());

        // A saída da memória (data_memory_output) já está conectada à saída read_data_out
        // dentro do SC_CTOR, então não precisa escrever aqui.
    }

    void initialize_data_memory(std::vector<sc_int<DATA_BITS>> data) {
        data_memory->initialize(data);
    }

    SC_CTOR(MEM) {
        // Instancia a memória de dados
        data_memory = new Memory<DATA_BITS, ADDRESS_BITS, sc_int<DATA_BITS>>("data_memory");

        // Conecta os sinais à memória de dados
        data_memory->clock(clock);
        data_memory->reset(reset);
        data_memory->write_enable(mem_write_in);
        data_memory->read_enable(mem_read_in);
        data_memory->address(memory_address_sig);
        data_memory->input(write_data_in);
        data_memory->output(read_data_out); // Conecta a saída da memória a um sinal intermediário


        // Método para passar os outros sinais
        SC_METHOD(process_mem_stage);
        sensitive << clock.pos() << reset << alu_result_in << write_data_in
                  << write_register_in << zero_flag_in << negative_flag_in
                  << mem_read_in << mem_write_in << reg_write_in
                  << branch_taken_in << branch_target_in;
        dont_initialize(); // Não inicializa este método na simulação
    }

    // Destrutor para liberar a memória alocada
    ~MEM() {
        delete data_memory;
    }
};

#endif // MEM_HPP