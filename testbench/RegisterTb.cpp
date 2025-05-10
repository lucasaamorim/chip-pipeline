#include "register/Register.hpp"
#include <systemc.h>

SC_MODULE(Testbench) {
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

  SC_CTOR(Testbench)
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
    sensitive << clock; // Sensível à borda positiva do clock
  }

  // Geração de estímulos para o teste
  void stimulus() {
    // Inicializando sinais
    reset.write(true);
    write.write(false);
    input.write(0);

    // Aplicando reset inicial
    wait(2, SC_NS); // Espera 2 ciclos de clock
    reset.write(false);

    // Testando escrita no registrador
    wait(2, SC_NS);    // Espera mais 2 ciclos
    write.write(true); // Habilita a escrita
    input.write(123);  // Escreve o valor 123
    wait(2, SC_NS);    // Espera 2 ciclos para registrar o valor

    write.write(false); // Desabilita a escrita

    // Verificando o valor no registrador
    wait(2, SC_NS);
    cout << "Valor após escrita: " << output.read() << endl;

    // Testando o reset
    wait(2, SC_NS);
    reset.write(true); // Habilita o reset
    wait(2, SC_NS);    // Espera o reset acontecer

    // Verificando o valor após o reset
    cout << "Valor após reset: " << output.read() << endl;

    // Testando nova escrita após reset
    reset.write(false); // Desabilita o reset
    wait(2, SC_NS);
    write.write(true); // Habilita a escrita
    input.write(456);  // Escreve o valor 456
    wait(2, SC_NS);    // Espera 2 ciclos

    write.write(false); // Desabilita a escrita

    // Verificando o valor após nova escrita
    wait(2, SC_NS);
    cout << "Valor após nova escrita: " << output.read() << endl;

    sc_stop(); // Para a simulação
  }
};

int sc_main(int argc, char *argv[]) {
  Testbench tb("Testbench");

  // Inicia a simulação
  sc_start();
  return 0;
}
