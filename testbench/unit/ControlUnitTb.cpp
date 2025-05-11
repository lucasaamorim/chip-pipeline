#include "control/ControlUnit.hpp"
#include <systemc>
#include <iostream>

using namespace sc_core;
using namespace sc_dt;
using namespace std;

SC_MODULE(ControlUnitTestbench) {
  sc_clock clock;
  // Sinais
  sc_signal<sc_uint<6>> opcode;
  sc_signal<bool> reg_write_enable, mem_read_enable, mem_write_enable;
  sc_signal<bool> alu_src, jump, branch_z, branch_n, reg_dst;
  sc_signal<sc_uint<4>> alu_op;

  // Instância do módulo sob teste
  ControlUnit* control_unit;

  void run() {
    // Vetor com pares {opcode, descrição}
    struct TestCase {
      sc_uint<6> opcode;
      std::string name;
    };

    TestCase tests[] = {
      {0b000111, "ADD"}, {0b001000, "SUB"}, {0b000011, "AND"},
      {0b000010, "OR"},  {0b000001, "XOR"}, {0b000100, "CMP"}, {0b100000, "NOT"},
      {0b100111, "ADDI"}, {0b101000, "SUBI"}, {0b100011, "ANDI"},
      {0b100010, "ORI"}, {0b100001, "XORI"}, {0b100100, "CMPI"},
      {0b100110, "LD"}, {0b101001, "ST"}, {0b111111, "J"},
      {0b110000, "JZ"}, {0b111000, "JN"}
    };

    for (const auto& test : tests) {
      opcode.write(test.opcode);
      wait(10, SC_NS); // Espera atualização da lógica combinacional

      cout << "Opcode: " << test.opcode.to_string(SC_BIN)
           << " (" << test.name << ")\n";
      cout << "  reg_write_enable: " << reg_write_enable.read() << "\n";
      cout << "  mem_read_enable : " << mem_read_enable.read() << "\n";
      cout << "  mem_write_enable: " << mem_write_enable.read() << "\n";
      cout << "  alu_src         : " << alu_src.read() << "\n";
      cout << "  jump            : " << jump.read() << "\n";
      cout << "  branch_z        : " << branch_z.read() << "\n";
      cout << "  branch_n        : " << branch_n.read() << "\n";
      cout << "  reg_dst         : " << reg_dst.read() << "\n";
      cout << "  alu_op          : " << alu_op.read().to_string(SC_BIN) << "\n";
      cout << "------------------------------------------\n";
    }

    sc_stop(); // Finaliza a simulação
  }

  SC_CTOR(ControlUnitTestbench) {
    control_unit = new ControlUnit("control_unit");
    control_unit->opcode(opcode);
    control_unit->reg_write_enable(reg_write_enable);
    control_unit->mem_read_enable(mem_read_enable);
    control_unit->mem_write_enable(mem_write_enable);
    control_unit->alu_src(alu_src);
    control_unit->jump(jump);
    control_unit->branch_z(branch_z);
    control_unit->branch_n(branch_n);
    control_unit->reg_dst(reg_dst);
    control_unit->alu_op(alu_op);
    control_unit->clk(clock);

    SC_THREAD(run);
  }

  ~ControlUnitTestbench() {
    delete control_unit;
  }
};

int sc_main(int argc, char* argv[]) {
  ControlUnitTestbench tb("tb");
  sc_start();
  return 0;
}
