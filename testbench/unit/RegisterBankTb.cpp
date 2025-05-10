#include "register/Register.hpp"
#include "register/bank/RegisterBank.hpp"
#include <systemc.h>

SC_MODULE(TestbenchRegBank) {
  // Clock
  sc_clock clock;

  // Sinais de controle
  sc_signal<bool> reset;
  sc_signal<bool> write_enable;

  // Sinais de endereço
  sc_signal<sc_uint<5>> read_address_1;
  sc_signal<sc_uint<5>> read_address_2;
  sc_signal<sc_uint<5>> write_address;

  // Sinais de dados
  sc_signal<sc_uint<32>> input;
  sc_signal<sc_uint<32>> output_1;
  sc_signal<sc_uint<32>> output_2;

  // Instância do banco de registradores
  RegisterBank<32, 5> *bank;

  SC_CTOR(TestbenchRegBank) : clock("clock", 10, SC_NS) {
    bank = new RegisterBank<32, 5>("RegisterBank");

    bank->clock(clock);
    bank->reset(reset);
    bank->write_enable(write_enable);
    bank->read_address_1(read_address_1);
    bank->read_address_2(read_address_2);
    bank->write_address(write_address);
    bank->input(input);
    bank->output_1(output_1);
    bank->output_2(output_2);

    SC_THREAD(stimulus);
    sensitive << clock.posedge_event();
  }

  void stimulus() {
    // Inicialização
    reset.write(true);
    write_enable.write(false);
    input.write(0);
    wait(10, SC_NS); // espera 1 ciclo de clock

    reset.write(false);
    wait(10, SC_NS);

    // Escreve valor 123 no registrador 5
    write_address.write(5);
    input.write(123);
    write_enable.write(true);
    wait(10, SC_NS); // Subida do clock para escrita

    write_enable.write(false);
    wait(10, SC_NS); // Clock de estabilidade

    // Lê registradores 5 e 0
    read_address_1.write(5);
    read_address_2.write(0);
    wait(10, SC_NS); // Clock de leitura

    std::cout << "Lendo R5: " << output_1.read() << " (esperado: 123)"
              << std::endl;
    std::cout << "Lendo R0: " << output_2.read() << " (esperado: 0)"
              << std::endl;

    // Escreve valor 888 no registrador 10
    write_address.write(10);
    input.write(888);
    write_enable.write(true);
    wait(10, SC_NS);

    write_enable.write(false);
    wait(10, SC_NS);

    // Lê registradores 10 e 5
    read_address_1.write(10);
    read_address_2.write(5);
    wait(10, SC_NS);

    std::cout << "Lendo R10: " << output_1.read() << " (esperado: 888)"
              << std::endl;
    std::cout << "Lendo R5: " << output_2.read() << " (esperado: 123)"
              << std::endl;

    // Reset final
    reset.write(true);
    wait(10, SC_NS);
    reset.write(false);

    read_address_1.write(10);
    read_address_2.write(5);
    wait(10, SC_NS);

    std::cout << "Após reset, R10: " << output_1.read() << " (esperado: 0)"
              << std::endl;
    std::cout << "Após reset, R5: " << output_2.read() << " (esperado: 0)"
              << std::endl;

    sc_stop(); // Finaliza a simulação
  }
};

int sc_main(int argc, char *argv[]) {
  TestbenchRegBank tb("TestbenchRegBank");
  sc_start();
  return 0;
}
