#include "arith/Adder.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(TestbenchAdder) {
  // Sinais de entrada
  sc_signal<bool> reset;
  sc_signal<sc_uint<32>> input_1;
  sc_signal<sc_uint<32>> input_2;

  // Sinal de saída
  sc_signal<sc_uint<32>> output;

  // Instância do somador
  Adder<32> *adder;

  // Clock
  sc_clock clock;

  SC_CTOR(TestbenchAdder)
      : clock("clock", 10, SC_NS) { // Definindo um clock de 10 ns
    // Instanciando o módulo Adder
    adder = new Adder<32>("Adder");

    // Conectando sinais
    adder->clock(clock);
    adder->reset(reset);
    adder->input_1(input_1);
    adder->input_2(input_2);
    adder->output(output);

    // Método de geração de estímulos
    SC_THREAD(stimulus);
    sensitive << clock.posedge_event(); // Sensível à borda positiva do clock
  }

  // Geração de estímulos para o teste
  void stimulus() {
    reset.write(false);
    input_1.write(0);
    input_2.write(0);

    wait(10, SC_NS); // Descida do clock

    std::cout << "Valor inicial do somador: " << output.read()
              << " (esperado: 0)" << std::endl;

    wait(10, SC_NS); // Subida do clock
    input_1.write(10);
    input_2.write(20);
    wait(10, SC_NS); // Descida do clock

    std::cout << "Valor esperado do somador: " << output.read()
              << " (esperado: 30)" << std::endl;

    wait(10, SC_NS); // Subida do clock
    input_1.write(15);
    input_2.write(25);
    wait(10, SC_NS); // Descida do clock

    std::cout << "Valor esperado do somador: " << output.read()
              << " (esperado: 40)" << std::endl;

    wait(10, SC_NS); // Subida do clock
    input_1.write(1);
    input_2.write(3);
    wait(10, SC_NS); // Descida do clock

    std::cout << "Valor esperado do somador: " << output.read()
              << " (esperado: 4)" << std::endl;

    sc_stop(); // Para a simulação
  }
};

int sc_main(int argc, char *argv[]) {
  TestbenchAdder tb("TestbenchAdder");

  // Inicia a simulação
  sc_start();
  return 0;
}
