#include <systemc>
#include "register_bank/RegisterBank.hpp"

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(Testbench) {
   sc_clock clk;
   sc_signal<bool> reset, write_enable;
   sc_signal<sc_uint<4>> read_address_1, read_address_2, write_address;
   sc_signal<sc_uint<32>> data_in;
   sc_signal<sc_uint<32>> data_out_1, data_out_2;

   RegisterBank reg_bank;

   SC_CTOR(Testbench)
       : clk("clk", 10, SC_NS)
       , reg_bank("reg_bank") {
      // Conectando os sinais
      reg_bank.clk(clk);
      reg_bank.reset(reset);
      reg_bank.write_enable(write_enable);
      reg_bank.read_address_1(read_address_1);
      reg_bank.read_address_2(read_address_2);
      reg_bank.write_address(write_address);
      reg_bank.data_in(data_in);
      reg_bank.data_out_1(data_out_1);
      reg_bank.data_out_2(data_out_2);

      SC_THREAD(run);
   }

   void run() {
      // Reset inicial
      reset.write(true);
      write_enable.write(false);
      wait(10, SC_NS);
      reset.write(false);

      // Escreve 100 em R1
      write_enable.write(true);
      write_address.write(1);
      data_in.write(100);
      wait(10, SC_NS);

      // Escreve 200 em R2
      write_address.write(2);
      data_in.write(200);
      wait(10, SC_NS);

      // Tenta escrever em R0 (deve ser ignorado)
      write_address.write(0);
      data_in.write(999);
      wait(10, SC_NS);

      // Leitura dos registradores R1 e R2
      write_enable.write(false);
      read_address_1.write(1);
      read_address_2.write(2);
      wait(10, SC_NS);

      std::cout << "Tempo: " << sc_time_stamp() << " | R1: " << data_out_1.read()
                << " | R2: " << data_out_2.read() << std::endl;

      // Leitura do registrador R0 (deve ser 0)
      read_address_1.write(0);
      read_address_2.write(0);
      wait(10, SC_NS);

      std::cout << "Tempo: " << sc_time_stamp() << " | R0: " << data_out_1.read()
                << std::endl;

      sc_stop();
   }
};

int sc_main(int argc, char* argv[]) {
   Testbench tb("tb");
   sc_start();
   return 0;
}
