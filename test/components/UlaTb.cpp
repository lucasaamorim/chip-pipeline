#include "ula/ULA.hpp"
#include <systemc>
#include <iostream>
#include <string>

SC_MODULE(UlaTb) {
    sc_clock clk;
    sc_signal<bool> reset;
    sc_signal<sc_uint<32>> in1, in2;
    sc_signal<sc_uint<3>> func;
    sc_signal<sc_uint<32>> out;

    ULA *ula;

    // Função para obter o nome da operação
    std::string get_op_name(int op) {
        switch(op) {
            case 0: return "AND";
            case 1: return "OR";
            case 2: return "XOR";
            case 3: return "GREATER_THAN";
            case 4: return "ADD";
            case 5: return "SUB";
            case 6: return "NOT";
            default: return "UNKNOWN";
        }
    }

    void print_test(int op, sc_uint<32> a, sc_uint<32> b, sc_uint<32> result) {
        std::cout << "--------------------------------------------------" << std::endl;
        std::cout << "Time: " << sc_time_stamp() 
                  << " | Operation: " << get_op_name(op) << " (" << op << ")"
                  << std::endl;
        std::cout << "Inputs: A = " << a << " (0x" << std::hex << a << std::dec << ")"
                  << ", B = " << b << " (0x" << std::hex << b << std::dec << ")"
                  << std::endl;
        std::cout << "Result: " << result << " (0x" << std::hex << result << std::dec << ")"
                  << std::endl;
    }

    void test() {
        std::cout << "\n\n=== INICIANDO TESTES DA ULA ===\n" << std::endl;

        // Teste de reset
        reset.write(true);
        wait(2, SC_NS);
        reset.write(false);
        wait(2, SC_NS);
        std::cout << "Reset testado - Saída deve ser 0: " << out.read() << "\n" << std::endl;

        // Teste cada operação com valores específicos que mostram o comportamento
        sc_uint<32> a, b;
        
        // AND (op=0)
        func.write(0);
        a = 0xAAAAAAAA; b = 0x55555555;
        in1.write(a); in2.write(b); wait(10, SC_NS);
        print_test(0, a, b, out.read());
        
        // OR (op=1)
        func.write(1);
        a = 0xAAAAAAAA; b = 0x55555555;
        in1.write(a); in2.write(b); wait(10, SC_NS);
        print_test(1, a, b, out.read());
        
        // XOR (op=2)
        func.write(2);
        a = 0xAAAAAAAA; b = 0x55555555;
        in1.write(a); in2.write(b); wait(10, SC_NS);
        print_test(2, a, b, out.read());
        
        // GREATER_THAN (op=3)
        func.write(3);
        a = 100; b = 50;
        in1.write(a); in2.write(b); wait(10, SC_NS);
        print_test(3, a, b, out.read());
        
        a = 50; b = 100;
        in1.write(a); in2.write(b); wait(10, SC_NS);
        print_test(3, a, b, out.read());
        
        // ADD (op=4)
        func.write(4);
        a = 50; b = 25;
        in1.write(a); in2.write(b); wait(10, SC_NS);
        print_test(4, a, b, out.read());
        
        // SUB (op=5)
        func.write(5);
        a = 50; b = 25;
        in1.write(a); in2.write(b); wait(10, SC_NS);
        print_test(5, a, b, out.read());
        
        // NOT (op=6)
        func.write(6);
        a = 0xAAAAAAAA; b = 0; // b é ignorado para NOT
        in1.write(a); in2.write(b); wait(10, SC_NS);
        print_test(6, a, b, out.read());

        // Casos especiais
        std::cout << "\n=== TESTANDO CASOS ESPECIAIS ===" << std::endl;
        
        // Overflow na soma
        func.write(4);
        a = 0xFFFFFFFF; b = 1;
        in1.write(a); in2.write(b); wait(10, SC_NS);
        print_test(4, a, b, out.read());
        
        // Underflow na subtração
        func.write(5);
        a = 0; b = 1;
        in1.write(a); in2.write(b); wait(10, SC_NS);
        print_test(5, a, b, out.read());
        
        // Teste com zero
        func.write(4);
        a = 0; b = 0;
        in1.write(a); in2.write(b); wait(10, SC_NS);
        print_test(4, a, b, out.read());

        std::cout << "\n=== TESTES COMPLETADOS COM SUCESSO ===" << std::endl;
        sc_stop();
    }

    SC_CTOR(UlaTb) : clk("clk", 10, SC_NS) {
        ula = new ULA("ula");
        ula->clk(clk);
        ula->reset(reset);
        ula->in1(in1);
        ula->in2(in2);
        ula->func(func);
        ula->out(out);

        SC_THREAD(test);
    }

    ~UlaTb() {
        delete ula;
    }
};

int sc_main(int argc, char* argv[]) {
    UlaTb tb("tb");
    sc_start();
    return 0;
}