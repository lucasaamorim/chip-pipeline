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

  // Novos sinais de controle
  sc_signal<bool> reg_write_enable;
  sc_signal<bool> mem_read_enable;
  sc_signal<bool> mem_write_enable;
  sc_signal<bool> alu_src;
  sc_signal<bool> jump;
  sc_signal<bool> branch_z;
  sc_signal<bool> branch_n;
  sc_signal<bool> reg_dst;
  sc_signal<sc_uint<4>> alu_op;

  sc_signal<sc_uint<8>> next_pc_out;

  InstructionDecoder<32, 4, 8, 32> *decoder;

  SC_CTOR(TestbenchInstructionDecoder) : clock("clock", 10, SC_NS) {
    decoder = new InstructionDecoder<32, 4, 8, 32>("Decoder");

    decoder->clock(clock);
    decoder->reset(reset);
    decoder->instruction(instruction);
    decoder->next_pc(next_pc);
    decoder->write_address_in(write_address_in);
    decoder->write_enable(write_enable);
    decoder->write_data(write_data);

    decoder->read_data_1(read_data_1);
    decoder->read_data_2(read_data_2);
    decoder->reg_address_1(reg_address_1);
    decoder->reg_address_2(reg_address_2);
    decoder->reg_address_3(reg_address_3);
    decoder->immediate_i(immediate_i);
    decoder->immediate_j(immediate_j);

    decoder->reg_write_enable(reg_write_enable);
    decoder->mem_read_enable(mem_read_enable);
    decoder->mem_write_enable(mem_write_enable);
    decoder->alu_src(alu_src);
    decoder->jump(jump);
    decoder->branch_z(branch_z);
    decoder->branch_n(branch_n);
    decoder->reg_dst(reg_dst);
    decoder->alu_op(alu_op);

    decoder->next_pc_out(next_pc_out);

    SC_THREAD(stimulus);
    sensitive << clock.posedge_event();
  }

  void write_register(unsigned int addr, unsigned int value) {
    write_enable.write(true);
    write_data.write(value);
    write_address_in.write(addr);
    wait(10, SC_NS);
    write_enable.write(false);
    wait(10, SC_NS);
  }

  void stimulus() {
    reset.write(false);
    write_enable.write(false);
    next_pc.write(10); // PC atual
    wait(10, SC_NS);

    // Etapa 1: Escreve valores em registradores R1 e R2
    write_register(1, 10);
    write_register(2, 40);
    write_register(3, 20);
    write_register(4, 30);

    // === Teste Tipo R: ADD ===

    wait(10, SC_NS);

    {
      sc_uint<32> instr = 0;
      instr.range(31, 26) = 0b000111; // opcode ADD
      instr.range(25, 22) = 1;        // rs = R1
      instr.range(21, 18) = 2;        // rt = R2
      instr.range(17, 14) = 3;        // rd = R3

      instruction.write(instr);

      wait(10, SC_NS);

      std::cout << "\n===== Teste Tipo R: ADD =====\n";

      std::cout << "Reg1 (R1) valor esperado: 10 => " << read_data_1.read()
                << "\n";
      std::cout << "Reg2 (R2) valor esperado: 40 => " << read_data_2.read()
                << "\n";
      std::cout << "Reg3 (R3): " << reg_address_3.read() << " (esperado: 3)\n";
      std::cout << "ALU_OP esperado: 7 -> lido: " << alu_op.read() << "\n";
      std::cout << "reg_write_enable esperado: 1 -> lido: "
                << reg_write_enable.read() << "\n";
      std::cout << "alu_src esperado: 0 -> lido: " << alu_src.read() << "\n";
      std::cout << "reg_dst esperado: 1 -> lido: " << reg_dst.read() << "\n";
      std::cout << "next_pc_out esperado: 10 -> lido: " << next_pc_out.read()
                << "\n";
    }

    // === Teste Tipo I: SUBI ===

    {
      sc_uint<32> instr = 0;
      instr.range(31, 26) = 0b101000; // opcode SUBI
      instr.range(25, 22) = 1;        // rs = R1
      instr.range(21, 18) = 4;        // rt = R4 (destino)
      instr.range(17, 0) = 1294;      // imediato 1294

      instruction.write(instr);
      wait(10, SC_NS);

      std::cout << "\n===== Teste Tipo I: SUBI =====\n";

      std::cout << "Immediate_i esperado: 1294 -> lido: " << immediate_i.read()
                << "\n";
      std::cout << "ALU_OP esperado: 8 -> lido: " << alu_op.read() << "\n";
      std::cout << "reg_write_enable esperado: 1 -> lido: "
                << reg_write_enable.read() << "\n";
      std::cout << "alu_src esperado: 1 -> lido: " << alu_src.read() << "\n";
      std::cout << "reg_dst esperado: 0 -> lido: " << reg_dst.read() << "\n";
      std::cout << "Reg1 (R1) valor esperado: 10 => " << read_data_1.read()
                << "\n";
      std::cout << "Reg2 (R2) => " << reg_address_2.read() << " (esperado 4)\n";
    }

    // === Teste Tipo J: JUMP ===
    {
      sc_uint<32> instr = 0;
      instr.range(31, 26) = 0b111111; // opcode JUMP
      instr.range(25, 18) = 32;       // immediate_j = 0x21

      instruction.write(instr);
      wait(10, SC_NS);

      std::cout << "\n===== Teste Tipo J: JUMP =====\n";
      std::cout << "Jump esperado: 1 -> lido: " << jump.read() << "\n";
      std::cout << "Immediate_j esperado: 32 -> lido: " << immediate_j.read()
                << "\n";

      // next_pc era 0x10, esperado: 0x10 + 0x21 = 0x31
      std::cout << "Jump PC esperado: 42 -> lido: " << next_pc_out.read()
                << "\n";
    }

    std::cout << "\n===== Fim dos testes =====\n";
    sc_stop();
  }
};

int sc_main(int argc, char *argv[]) {
  TestbenchInstructionDecoder tb("TB_Decoder");
  sc_start();
  return 0;
}
