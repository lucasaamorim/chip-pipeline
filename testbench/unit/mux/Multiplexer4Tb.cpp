#include "mux/Multiplexer.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(TestbenchMux4) {
  // Sinais de entrada
  sc_signal<bool> reset;
  sc_signal<sc_uint<32>> input_0, input_1, input_2, input_3;
  sc_signal<sc_uint<2>> select;

  // Sinal de saída
  sc_signal<sc_uint<32>> output;

  // Instância do multiplexador
  Multiplexer<32, 2, sc_uint<32>> *mux;

  // Clock
  sc_clock clock;

  SC_CTOR(TestbenchMux4) : clock("clock", 10, SC_NS) {
    mux = new Multiplexer<32, 2, sc_uint<32>>("Multiplexer");

    // Conectando sinais
    mux->clock(clock);
    mux->reset(reset);
    mux->select(select);
    mux->output(output);

    // Conectando os 4 sinais de entrada ao vetor
    mux->inputs[0](input_0);
    mux->inputs[1](input_1);
    mux->inputs[2](input_2);
    mux->inputs[3](input_3);

    SC_THREAD(stimulus);
    sensitive << clock.posedge_event();
  }

  void stimulus() {
    reset.write(true);
    input_0.write(100);
    input_1.write(200);
    input_2.write(300);
    input_3.write(400);
    select.write(0);
    wait(10, SC_NS);

    std::cout << "Reset ativo -> Saída: " << output.read()
              << " (esperado: 0)\n";

    wait(10, SC_NS);
    reset.write(false);

    for (int i = 0; i < 4; ++i) {
      select.write(i);
      wait(10, SC_NS);
      std::cout << "select = " << i << " -> saída: " << output.read() << '\n';
    }

    sc_stop();
  }
};

int sc_main(int argc, char *argv[]) {
  TestbenchMux4 tb("TestbenchMux4");
  sc_start();
  return 0;
}
