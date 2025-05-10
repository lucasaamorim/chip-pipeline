#include "register/Register.hpp"
#include <systemc.h>

SC_MODULE(TestbenchReg) {
  // Sinais de entrada
  sc_signal<bool> reset;
  sc_signal<bool> write;
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
    reg->write(write);
    reg->input(input);
    reg->output(output);

    // Método de geração de estímulos
    SC_THREAD(stimulus);
    sensitive << clock.posedge_event(); // Sensível à borda positiva do clock
  }

  // Geração de estímulos para o teste
  void stimulus() {
    reset.write(true);
    write.write(false);
    input.write(0);
    wait(5, SC_NS); // Descida do clock

    std::cout << "Valor inicial do registrador: " << output.read() << std::endl;

    reset.write(false); // Desativa o reset

    wait(5, SC_NS); // Subida do clock
    write.write(true);
    input.write(42);
    wait(5, SC_NS); // Descida do clock

    std::cout << "Valor do registrador depois de escrita: " << output.read()
              << std::endl;

    write.write(false);
    input.write(0);

    wait(5, SC_NS); // Subida do clock

    std::cout << "Valor do registrador depois de mais um clock: "
              << output.read() << std::endl;

    wait(5, SC_NS); // Descida do clock
    reset.write(true);
    wait(5, SC_NS); // Subida do clock

    std::cout << "Valor do registrador depois do reset: " << output.read()
              << std::endl;

    wait(5, SC_NS); // Descida do clock
    reset.write(false);
    wait(5, SC_NS); // Subida do clock
    write.write(true);
    input.write(120);
    wait(5, SC_NS); // Descida do clock

    std::cout << "Valor do registrador depois de escrita: " << output.read()
              << std::endl;

    wait(5, SC_NS); // Subida do clock
    input.write(756);
    wait(5, SC_NS); // Descida do clock

    std::cout << "Valor do registrador depois de mais um clock: "
              << output.read() << std::endl;

    sc_stop(); // Para a simulação
  }
};

int sc_main(int argc, char *argv[]) {
  TestbenchReg tb("TestbenchReg");

  // Inicia a simulação
  sc_start();
  return 0;
}
