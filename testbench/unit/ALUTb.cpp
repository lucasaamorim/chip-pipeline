#include "alu/ALU.hpp"
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(TestbenchALU) {
  sc_signal<sc_uint<32>> operand_a, operand_b;
  sc_signal<sc_uint<4>> op;
  sc_signal<sc_uint<32>> result;
  sc_signal<bool> zero_flag, negative_flag;

  ALU<32>* alu;

  SC_CTOR(TestbenchALU) {
    alu = new ALU<32>("ALU");

    alu->operand_a_in(operand_a);
    alu->operand_b_in(operand_b);
    alu->operation_select_in(op);
    alu->result_out(result);
    alu->zero_flag_out(zero_flag);
    alu->negative_flag_out(negative_flag);

    SC_THREAD(stimulus);
  }

  void stimulus() {
    operand_a.write(10);
    operand_b.write(20);
    op.write(ALU_ADD);
    wait(10, SC_NS);
    std::cout << "ADD: " << result.read() << std::endl;

    operand_a.write(40);
    operand_b.write(15);
    op.write(ALU_SUB);
    wait(10, SC_NS);
    std::cout << "SUB: " << result.read() << std::endl;

    operand_a.write(0b1100);
    operand_b.write(0b1010);
    op.write(ALU_AND);
    wait(10, SC_NS);
    std::cout << "AND: " << result.read() << std::endl;

    operand_a.write(10);
    operand_b.write(10);
    op.write(ALU_CMP);
    wait(10, SC_NS);
    std::cout << "CMP (equal): " << result.read() << std::endl;

  operand_a.write(5);
  operand_b.write(10);
  op.write(ALU_CMP);
  wait(10, SC_NS);
  std::cout << "CMP (less): " << result.read() << " (esperado: -1)" << std::endl;

  operand_a.write(20);
  operand_b.write(10);
  op.write(ALU_CMP);
  wait(10, SC_NS);
  std::cout << "CMP (greater): " << result.read() << " (esperado: 1)" << std::endl;


    sc_stop();
  }
};

int sc_main(int argc, char* argv[]) {
  TestbenchALU tb("TestbenchALU");
  sc_start();
  return 0;
}
