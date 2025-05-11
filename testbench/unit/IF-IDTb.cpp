#include "register/bank/intermediary/IF-ID.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(TestbenchIfId) {
  // Sinais de entrada
  sc_signal<bool> reset;
  sc_signal<bool> write_enable;

  // Sinais de entrada
  sc_signal<sc_uint<32>> instruction_in;
  sc_signal<sc_uint<8>> next_pc_in;

  // Sinal de saída
  sc_signal<sc_uint<32>> instruction_out;
  sc_signal<sc_uint<8>> next_pc_out;

  // Instância do registrador
  IF_ID<32, 8> *if_id;

  // Clock
  sc_clock clock; // Usando sc_clock em vez de sc_signal<bool>

  SC_CTOR(TestbenchIfId)
      : clock("clock", 10, SC_NS) { // Definindo um clock de 10 ns

    /// Inicializando a constante
    write_enable.write(true);

    // Instanciando o módulo Register
    if_id = new IF_ID<32, 8>("IF_ID");

    // Conectando sinais
    if_id->clock(clock);
    if_id->reset(reset);
    if_id->write_enable(write_enable);
    if_id->instruction_in(instruction_in);
    if_id->next_pc_in(next_pc_in);
    if_id->instruction_out(instruction_out);
    if_id->next_pc_out(next_pc_out);

    // Método de geração de estímulos
    SC_THREAD(stimulus);
    sensitive << clock.posedge_event(); // Sensível à borda positiva do clock
  }

  // Geração de estímulos para o teste
  void stimulus() {
    // Inicializa os sinais
    instruction_in.write(0xDEADBEEF);
    next_pc_in.write(0x10);
    wait(10, SC_NS); // Subida do clock (reset aplicado)

    std::cout << "Valor inicial instruction_out: " << instruction_out.read()
              << " (esperado: 0)" << std::endl;
    std::cout << "Valor inicial next_pc_out: " << next_pc_out.read()
              << " (esperado: 0)" << std::endl;

    // Primeiro ciclo de escrita
    instruction_in.write(0x12345678);
    next_pc_in.write(0x20);
    write_enable.write(true);
    wait(10, SC_NS); // Subida do clock

    std::cout << "Após 1º escrita - instruction_out: " << instruction_out.read()
              << " (esperado: 0xDEADBEEF)" << std::endl;
    std::cout << "Após 1º escrita - next_pc_out: " << next_pc_out.read()
              << " (esperado: 16)" << std::endl;

    // Segundo ciclo de escrita
    instruction_in.write(0xCAFEBABE);
    next_pc_in.write(0x30);
    wait(10, SC_NS); // Subida do clock

    std::cout << "Após 2º escrita - instruction_out: " << instruction_out.read()
              << " (esperado: 0x12345678)" << std::endl;
    std::cout << "Após 2º escrita - next_pc_out: " << next_pc_out.read()
              << " (esperado: 32)" << std::endl;

    // Desabilita escrita
    write_enable.write(false);
    instruction_in.write(0);
    next_pc_in.write(0);
    wait(10, SC_NS); // Subida do clock

    std::cout << "Após desabilitar escrita - instruction_out: "
              << instruction_out.read() << " (esperado: 0xCAFEBABE)"
              << std::endl;
    std::cout << "Após desabilitar escrita - next_pc_out: "
              << next_pc_out.read() << " (esperado: 48)" << std::endl;

    // Aplica reset novamente
    reset.write(true);
    wait(10, SC_NS); // Subida do clock
    wait(10, SC_NS); // Subida do clock

    std::cout << "Após reset - instruction_out: " << instruction_out.read()
              << " (esperado: 0)" << std::endl;
    std::cout << "Após reset - next_pc_out: " << next_pc_out.read()
              << " (esperado: 0)" << std::endl;

    sc_stop(); // Encerra simulação
  }
};

int sc_main(int argc, char *argv[]) {
  TestbenchIfId tb("TestbenchIfId");

  // Inicia a simulação
  sc_start();
  return 0;
}
