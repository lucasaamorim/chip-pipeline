#include "InstructionDecoder.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(TestbenchInstructionDecoder) {
  sc_clock clock;

  sc_signal<bool> reset;
  sc_signal<bool> write_enable;

  sc_signal<sc_uint<32>> instruction;
  sc_signal<sc_uint<8>> next_pc;
  sc_signal<sc_uint<4>> write_address_in;
  sc_signal<sc_uint<32>> write_data;

  sc_signal<sc_uint<32>> read_data_1;
  sc_signal<sc_uint<32>> read_data_2;
  sc_signal<sc_uint<4>> reg_address_1;
  sc_signal<sc_uint<4>> reg_address_2;
  sc_signal<sc_uint<4>> reg_address_3;
  sc_signal<sc_uint<32>> immediate_i;
  sc_signal<sc_uint<8>> immediate_j;

  InstructionDecoder<32, 4, 8, 32> *decoder;

  SC_CTOR(TestbenchInstructionDecoder) : clock("clock", 10, SC_NS) {
    decoder = new InstructionDecoder<32, 4, 8, 32>("Decoder");

    decoder->clock(clock);
    decoder->reset(reset);
    decoder->write_enable(write_enable);
    decoder->instruction(instruction);
    decoder->next_pc(next_pc);
    decoder->write_address_in(write_address_in);
    decoder->write_data(write_data);

    decoder->read_data_1(read_data_1);
    decoder->read_data_2(read_data_2);
    decoder->reg_address_1(reg_address_1);
    decoder->reg_address_2(reg_address_2);
    decoder->reg_address_3(reg_address_3);
    decoder->immediate_i(immediate_i);
    decoder->immediate_j(immediate_j);

    SC_THREAD(stimulus);
    sensitive << clock.posedge_event();
  }

  void write_register(unsigned int addr, unsigned int value) {
    write_enable.write(true);
    write_address_in.write(addr);
    write_data.write(value);
    wait(10, SC_NS);
    write_enable.write(false);
    wait(10, SC_NS);
  }

  void stimulus() {
    reset.write(false);
    write_enable.write(false);
    wait(10, SC_NS);

    // Etapa 1: Escreve valores em registradores R0-R3
    write_register(1, 10);
    write_register(2, 20);
    write_register(3, 30);
    write_register(4, 40);

    // Aguarde um ciclo de clock para garantir que a escrita nos registradores
    // foi concluída
    wait(10, SC_NS);

    // Etapa 2: Envia instrução fictícia para testar leitura
    sc_uint<32> instr = 0;
    instr.range(31, 26) = 20;    // opcode
    instr.range(25, 22) = 1;     // reg1
    instr.range(21, 18) = 2;     // reg2
    instr.range(17, 14) = 3;     // reg3
    instr.range(13, 0) = 0x00AB; // immediate_i fictício (14 bits apenas)

    instruction.write(instr);
    wait(10, SC_NS);

    std::cout << "Instrução enviada: " << instr.to_string(SC_BIN) << "\n";

    // Checa se os registradores foram lidos corretamente
    std::cout << "Reg1 (R1) valor esperado: 10 => " << read_data_1.read()
              << "\n";
    std::cout << "Reg2 (R2) valor esperado: 20 => " << read_data_2.read()
              << "\n";
    std::cout << "Reg3 (R3): " << reg_address_3.read() << " (esperado: 3)\n";

    std::cout << "Immediate I: " << immediate_i.read()
              << " (esperado: 0x00AB)\n";

    std::cout << "Immediate J: " << immediate_j.read()
              << " (esperado: parte alta da instrução como bits 25-18 com "
                 "tamanho 8)\n";

    sc_stop();
  }
};

int sc_main(int argc, char *argv[]) {
  TestbenchInstructionDecoder tb("TB_Decoder");
  sc_start();
  return 0;
}