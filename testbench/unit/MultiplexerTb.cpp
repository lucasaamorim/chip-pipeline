#include "mux/Multiplexer.hpp"
#include <systemc.h>

SC_MODULE(TestbenchMux) {
  // Sinais de entrada
  sc_signal<bool> reset;
  sc_signal<sc_uint<32>> input_1;
  sc_signal<sc_uint<32>> input_2;
  sc_signal<bool> select;

  // Sinal de saída
  sc_signal<sc_uint<32>> output;

  // Instância do somador
  Multiplexer<32> *mux;

  // Clock
  sc_clock clock;

  SC_CTOR(TestbenchMux)
      : clock("clock", 10, SC_NS) { // Definindo um clock de 10 ns
    // Instanciando o módulo Adder
    mux = new Multiplexer<32>("Multiplexer");

    // Conectando sinais
    mux->clock(clock);
    mux->reset(reset);
    mux->input_1(input_1);
    mux->input_2(input_2);
    mux->select(select);
    mux->output(output);

    // Método de geração de estímulos
    SC_THREAD(stimulus);
    sensitive << clock.posedge_event(); // Sensível à borda positiva do clock
  }

  // Geração de estímulos para o teste
  void stimulus() {
    reset.write(true);
    input_1.write(0);
    input_2.write(0);
    select.write(false);

    wait(10, SC_NS); // Descida do clock

    std::cout << "Valor inicial do mux: " << output.read() << " (esperado: 0)"
              << std::endl;

    wait(10, SC_NS); // Subida do clock
    reset.write(false);
    input_1.write(10);
    input_2.write(20);
    select.write(false);
    wait(10, SC_NS); // Descida do clock

    std::cout << "Valor esperado do mux: " << output.read() << " (esperado: 10)"
              << std::endl;

    wait(10, SC_NS); // Subida do clock
    input_1.write(15);
    input_2.write(25);
    select.write(true);
    wait(10, SC_NS); // Descida do clock

    std::cout << "Valor esperado do mux: " << output.read() << " (esperado: 25)"
              << std::endl;

    wait(10, SC_NS); // Subida do clock
    input_1.write(1);
    input_2.write(3);
    wait(10, SC_NS); // Descida do clock

    std::cout << "Valor esperado do mux: " << output.read() << " (esperado: 3)"
              << std::endl;

    sc_stop(); // Para a simulação
  }
};

int sc_main(int argc, char *argv[]) {
  TestbenchMux tb("TestbenchMux");

  // Inicia a simulação
  sc_start();
  return 0;
}
