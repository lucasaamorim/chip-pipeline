#include "memory/Memory.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(TestbenchMemoryInt) {
  // Clock
  sc_clock clock;

  // Sinais
  sc_signal<bool> reset;
  sc_signal<bool> write_enable;
  sc_signal<bool> read_enable;

  sc_signal<sc_uint<8>> address; // 8 bits = 256 posições
  sc_signal<sc_int<32>> input;
  sc_signal<sc_int<32>> output;

  // Instância da memória
  Memory<32, 8, sc_int<32>> *mem;

  SC_CTOR(TestbenchMemoryInt) : clock("clock", 10, SC_NS) {
    mem = new Memory<32, 8, sc_int<32>>("Memory");

    mem->clock(clock);
    mem->reset(reset);
    mem->write_enable(write_enable);
    mem->read_enable(read_enable);
    mem->address(address);
    mem->input(input);
    mem->output(output);

    SC_THREAD(stimulus);
    sensitive << clock.posedge_event();
  }

  void stimulus() {
    // Inicialização
    reset.write(false);
    write_enable.write(false);
    read_enable.write(false);
    input.write(0);
    address.write(0);
    wait(10, SC_NS);

    // Escreve valor 42 na posição 10
    address.write(10);
    input.write(42);
    write_enable.write(true);
    wait(10, SC_NS);

    write_enable.write(false);
    wait(10, SC_NS); // estabilização

    // Lê da posição 10
    read_enable.write(true);
    wait(10, SC_NS);

    std::cout << "Valor na posição 10: " << output.read() << " (esperado: 42)"
              << std::endl;
    read_enable.write(false);

    // Escreve valor -20 na posição 15
    address.write(15);
    input.write(-20);
    write_enable.write(true);
    wait(10, SC_NS);

    write_enable.write(false);
    wait(10, SC_NS); // estabilização

    // Lê da posição 10
    read_enable.write(true);
    wait(10, SC_NS);

    std::cout << "Valor na posição 15: " << output.read() << " (esperado: -20)"
              << std::endl;
    read_enable.write(false);

    // Reseta a memória chamando a função manualmente
    mem->reset_memory(); // <<<< CHAMADA MANUAL DE RESET

    // Lê novamente da posição 10 após "reset"
    read_enable.write(true);
    wait(10, SC_NS);

    std::cout << "Após reset, valor na posição 10: " << output.read()
              << " (esperado: 0)" << std::endl;
    read_enable.write(false);

    sc_stop();
  }
};

int sc_main(int argc, char *argv[]) {
  TestbenchMemoryInt tb("TestbenchMemoryInt");
  sc_start();
  return 0;
}
