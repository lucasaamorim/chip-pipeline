#include "mux/Multiplexer.hpp"
#include <systemc.h>

SC_MODULE(TestbenchMux) {
  // Sinais de entrada
  sc_signal<bool> reset;
  sc_signal<sc_uint<32>> input_0;
  sc_signal<sc_uint<32>> input_1;
  sc_signal<sc_uint<1>> select;

  // Sinal de saída
  sc_signal<sc_uint<32>> output;

  // Instância do multiplexador
  Multiplexer<32, 1> *mux;

  // Clock
  sc_clock clock;

  SC_CTOR(TestbenchMux) : clock("clock", 10, SC_NS) {
    mux = new Multiplexer<32, 1>("Multiplexer");

    // Conectando sinais
    mux->clock(clock);
    mux->reset(reset);
    mux->select(select);
    mux->output(output);
    mux->inputs[0](input_0);
    mux->inputs[1](input_1);

    SC_THREAD(stimulus);
    sensitive << clock.posedge_event();
  }

  void stimulus() {
    reset.write(true);
    input_0.write(0);
    input_1.write(0);
    select.write(0);

    wait(10, SC_NS);
    std::cout << "Valor inicial do mux: " << output.read()
              << " (esperado: 0)\n";

    wait(10, SC_NS);
    reset.write(false);
    input_0.write(10);
    input_1.write(20);
    select.write(0);
    wait(10, SC_NS);
    std::cout << "Mux select=0 -> " << output.read() << " (esperado: 10)\n";

    wait(10, SC_NS);
    input_0.write(15);
    input_1.write(25);
    select.write(1);
    wait(10, SC_NS);
    std::cout << "Mux select=1 -> " << output.read() << " (esperado: 25)\n";

    wait(10, SC_NS);
    input_0.write(1);
    input_1.write(3);
    wait(10, SC_NS);
    std::cout << "Mux select=1 -> " << output.read() << " (esperado: 3)\n";

    sc_stop();
  }
};

int sc_main(int argc, char *argv[]) {
  TestbenchMux tb("TestbenchMux");
  sc_start();
  return 0;
}
