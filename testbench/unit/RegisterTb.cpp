#include "register/Register.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(TestbenchReg) {
  // Sinais de entrada
  sc_signal<bool> reset;
  sc_signal<bool> write_enable;
  sc_signal<sc_uint<32>> input;

  // Sinal de saída
  sc_signal<sc_uint<32>> output;

  // Instância do registrador
  Register<32> *reg;

  // Clock
  sc_clock clock; // Usando sc_clock em vez de sc_signal<bool>

  SC_CTOR(TestbenchReg)
      : clock("clock", 10, SC_NS) { // Definindo um clock de 10 ns
    // Instanciando o módulo Register
    reg = new Register<32>("Register");

    // Conectando sinais
    reg->clock(clock);
    reg->reset(reset);
    reg->write_enable(write_enable);
    reg->input(input);
    reg->output(output);

    // Método de geração de estímulos
    SC_THREAD(stimulus);
    sensitive << clock.posedge_event(); // Sensível à borda positiva do clock
  }

  // Geração de estímulos para o teste
  void stimulus() {
    reset.write(true);
    write_enable.write(false);
    input.write(0);
    wait(10, SC_NS); // Descida do clock

    std::cout << "Valor inicial do registrador: " << output.read()
              << " (esperado: 0)" << std::endl;

    reset.write(false); // Desativa o reset

    wait(10, SC_NS); // Subida do clock
    write_enable.write(true);
    input.write(42);
    wait(10, SC_NS); // Descida do clock

    std::cout << "Valor do registrador depois de escrita: " << output.read()
              << " (esperado: 42)" << std::endl;

    write_enable.write(false);
    input.write(0);

    wait(10, SC_NS); // Subida do clock

    std::cout << "Valor do registrador depois de mais um clock: "
              << output.read() << " (esperado: 42)" << std::endl;

    wait(10, SC_NS); // Descida do clock
    reset.write(true);
    wait(10, SC_NS); // Subida do clock

    std::cout << "Valor do registrador depois do reset: " << output.read()
              << " (esperado: 0)" << std::endl;

    wait(10, SC_NS); // Descida do clock
    reset.write(false);
    wait(10, SC_NS); // Subida do clock
    write_enable.write(true);
    input.write(120);
    wait(10, SC_NS); // Descida do clock

    std::cout << "Valor do registrador depois de escrita: " << output.read()
              << " (esperado: 120)" << std::endl;

    wait(10, SC_NS); // Subida do clock
    input.write(756);
    wait(10, SC_NS); // Descida do clock

    std::cout << "Valor do registrador depois de mais um clock: "
              << output.read() << " (esperado: 756)" << std::endl;

    sc_stop(); // Para a simulação
  }
};

int sc_main(int argc, char *argv[]) {
  TestbenchReg tb("TestbenchReg");

  // Inicia a simulação
  sc_start();
  return 0;
}
