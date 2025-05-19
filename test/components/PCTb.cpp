#include "pc/PC.hpp"
#include <systemc>
#include <iostream>
#include <iomanip>

SC_MODULE(PCTb) {
    sc_clock clk;
    sc_signal<bool> reset, enable;
    sc_signal<sc_uint<32>> pc_in;
    sc_signal<sc_uint<32>> pc_out;

    PC* pc;

    void print_state() {
        std::cout << "[" << sc_time_stamp() << "] "
                  << "Reset: " << reset.read() 
                  << " | Enable: " << enable.read()
                  << " | Input: 0x" << std::hex << std::setw(8) << std::setfill('0') << pc_in.read()
                  << " | Output: 0x" << std::hex << std::setw(8) << std::setfill('0') << pc_out.read()
                  << std::dec << std::endl;
    }

    void test_sequence() {
        std::cout << "\n=== INICIANDO TESTES DO PC ===" << std::endl;

        // 1. Teste de reset
        std::cout << "\n[TESTE 1] Reset inicial" << std::endl;
        reset.write(true);
        enable.write(false);
        pc_in.write(0x00000000);
        wait(10, SC_NS);
        print_state();
        
        // 2. Liberar reset e testar comportamento normal
        std::cout << "\n[TESTE 2] Operação normal" << std::endl;
        reset.write(false);
        enable.write(true);
        
        // Sequência de valores de entrada
        const sc_uint<32> test_values[] = {
            0x00000000, 0x00000004, 0x00000008, 
            0xFFFFFFFC, 0xFFFFFFFF, 0x12345678
        };
        
        for (const auto& val : test_values) {
            pc_in.write(val);
            wait(10, SC_NS);
            print_state();
        }

        // 3. Testar enable/disable
        std::cout << "\n[TESTE 3] Controle por enable" << std::endl;
        pc_in.write(0xAAAAAAAA);
        
        enable.write(false);
        wait(10, SC_NS);
        print_state();
        
        enable.write(true);
        wait(10, SC_NS);
        print_state();
        
        enable.write(false);
        wait(10, SC_NS);
        print_state();

        // 4. Testar reset durante operação
        std::cout << "\n[TESTE 4] Reset durante operação" << std::endl;
        pc_in.write(0xBBBBBBBB);
        wait(5, SC_NS);
        reset.write(true);
        wait(5, SC_NS);
        print_state();
        
        // 5. Restaurar operação normal
        reset.write(false);
        enable.write(true);
        pc_in.write(0xCCCCCCCC);
        wait(10, SC_NS);
        print_state();

        std::cout << "\n=== TESTES CONCLUÍDOS ===" << std::endl;
        sc_stop();
    }

    SC_CTOR(PCTb) : clk("clk", 10, SC_NS) {
        pc = new PC("pc");
        pc->clk(clk);
        pc->reset(reset);
        pc->enable(enable);
        pc->in(pc_in);
        pc->out(pc_out);

        SC_THREAD(test_sequence);
        sensitive << clk.posedge_event();  // Correção aplicada aqui
    }

    ~PCTb() {
        delete pc;
    }
};

int sc_main(int argc, char* argv[]) {
    PCTb tb("tb");
    sc_start(200, SC_NS);
    return 0;
}