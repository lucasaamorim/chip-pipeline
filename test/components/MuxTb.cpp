#include <systemc>
#include "mux/Mux.hpp"

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(Testbench) {
   sc_clock clk;
   sc_signal<bool> reset;
   sc_vector<sc_signal<sc_uint<8>>> inputs;
   sc_signal<sc_uint<2>> sel;
   sc_signal<sc_uint<8>> out;

   Mux<8, 2> mux_inst;

   SC_CTOR(Testbench)
       : clk("clk", 10, SC_NS)
       , inputs("inputs", 4)  // 2^2 entradas
       , mux_inst("mux_inst") {

      // Ligação de sinais
      mux_inst.clk(clk);
      mux_inst.reset(reset);
      mux_inst.sel(sel);
      mux_inst.out(out);
      for (int i = 0; i < 4; ++i) {
         mux_inst.in[i](inputs[i]);
      }

      SC_THREAD(run);
   }

   void run() {
      // Inicialização
      reset.write(true);
      wait(1, SC_NS);
      reset.write(false);

      // Valores para entradas
      inputs[0].write(10);
      inputs[1].write(20);
      inputs[2].write(30);
      inputs[3].write(40);

      for (int i = 0; i < 4; ++i) {
         sel.write(i);
         wait(10, SC_NS);  // Esperar um ciclo de clock
         std::cout << "Tempo: " << sc_time_stamp()
                   << " | sel: " << i
                   << " | out: " << out.read()
                   << " (esperado: " << inputs[i].read() << ")"
                   << std::endl;
      }

      sc_stop();  // Finalizar simulação
   }
};

int sc_main(int argc, char* argv[]) {
   Testbench tb("tb");
   sc_start();
   return 0;
}
