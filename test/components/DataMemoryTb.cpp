#include "data_memory/DataMemory.hpp"
#include <systemc>
#include <iostream>
#include <iomanip>
#include <cstdlib>

SC_MODULE(DataMemoryTestBench) {
    sc_clock clk;
    sc_signal<bool> reset, mem_write, mem_read;
    sc_signal<sc_uint<32>> address, write_data;
    sc_signal<sc_uint<32>> read_data;

    DataMemory* data_mem;

    void print_state() {
        std::cout << "[" << sc_time_stamp() << "] "
                  << "Reset: " << reset.read()
                  << " | Write: " << mem_write.read()
                  << " | Read: " << mem_read.read()
                  << " | Addr: 0x" << std::hex << std::setw(8) << std::setfill('0') << address.read()
                  << " | WrData: 0x" << std::hex << std::setw(8) << std::setfill('0') << write_data.read()
                  << " | RdData: 0x" << std::hex << std::setw(8) << std::setfill('0') << read_data.read()
                  << std::dec << std::endl;
    }

    void test_sequence() {
        std::cout << "\n=== INICIANDO TESTES DA MEMÓRIA DE DADOS ===" << std::endl;

        // 1. Teste de reset inicial
        std::cout << "\n[TESTE 1] Reset inicial" << std::endl;
        reset.write(true);
        mem_write.write(false);
        mem_read.write(false);
        address.write(0x00000000);
        write_data.write(0x00000000);
        wait(10, SC_NS);
        print_state();

        // 2. Operação normal - Escrita
        std::cout << "\n[TESTE 2] Operações de escrita" << std::endl;
        reset.write(false);
        mem_write.write(true);
        mem_read.write(false);

        // Escrever em diferentes posições de memória
        address.write(0x00000000);
        write_data.write(0x12345678);
        wait(10, SC_NS);
        print_state();

        address.write(0x00000004);
        write_data.write(0x9ABCDEF0);
        wait(10, SC_NS);
        print_state();

        address.write(0x00000100);  // Último endereço válido (256*4 - 4)
        write_data.write(0xFFFFFFFF);
        wait(10, SC_NS);
        print_state();

        // 3. Operação normal - Leitura
        std::cout << "\n[TESTE 3] Operações de leitura" << std::endl;
        mem_write.write(false);
        mem_read.write(true);

        // Ler os valores escritos anteriormente
        address.write(0x00000000);
        wait(10, SC_NS);
        print_state();

        address.write(0x00000004);
        wait(10, SC_NS);
        print_state();

        address.write(0x00000100);
        wait(10, SC_NS);
        print_state();

        // 4. Teste de leitura/escrita simultânea
        std::cout << "\n[TESTE 4] Leitura e escrita simultânea" << std::endl;
        mem_write.write(true);
        mem_read.write(true);
        address.write(0x00000008);
        write_data.write(0x55555555);
        wait(10, SC_NS);
        print_state();

        // 5. Teste de reset durante operação
        std::cout << "\n[TESTE 5] Reset durante operação" << std::endl;
        reset.write(false);
        mem_write.write(true);
        mem_read.write(false);
        address.write(0x0000000C);
        write_data.write(0xAAAAAAAA);
        wait(5, SC_NS);
        reset.write(true);
        wait(5, SC_NS);
        print_state();

        // 6. Teste de endereçamento não alinhado
        std::cout << "\n[TESTE 6] Endereçamento não alinhado (potencial problema)" << std::endl;
        reset.write(false);
        mem_write.write(true);
        mem_read.write(false);
        address.write(0x00000001);  // Endereço não múltiplo de 4
        write_data.write(0x11111111);
        wait(10, SC_NS);
        print_state();

        // Verificar se escreveu no endereço correto (deveria escrever em 0x00000000)
        mem_write.write(false);
        mem_read.write(true);
        address.write(0x00000000);
        wait(10, SC_NS);
        print_state();

        std::cout << "\n=== TESTES CONCLUÍDOS ===" << std::endl;
        sc_stop();
    }

    SC_CTOR(DataMemoryTestBench) : clk("clk", 10, SC_NS) {
        data_mem = new DataMemory("data_mem");
        data_mem->clk(clk);
        data_mem->reset(reset);
        data_mem->mem_write(mem_write);
        data_mem->mem_read(mem_read);
        data_mem->address(address);
        data_mem->write_data(write_data);
        data_mem->data(read_data);

        SC_THREAD(test_sequence);
        sensitive << clk.posedge_event();  // Correção aplicada aqui
    }

    ~DataMemoryTestBench() {
        delete data_mem;
    }
};

int sc_main(int argc, char* argv[]) {
    DataMemoryTestBench tb("tb");
    sc_start(200, SC_NS);
    return 0;
}