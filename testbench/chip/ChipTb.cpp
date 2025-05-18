#include "Chip.hpp"
#include <iostream>
#include <systemc>

using namespace sc_core;
using namespace sc_dt;

SC_MODULE(ChipTb) {
  sc_clock clock;
  sc_signal<bool> reset;

  Chip *chip;

  SC_CTOR(ChipTb) : clock("clock", 10, SC_NS) {
    chip = new Chip("chip");
    chip->clock(clock);
    chip->reset(reset);

    SC_THREAD(run_tests);
  }

  void reset_chip() {
    reset = 1;
    reset = 1;
    wait(10, SC_NS);
    reset = 0;
    wait(10, SC_NS);
  }

  void initialize_instruction_memory() {
    std::vector<sc_uint<32>> instructions = {
        0b10111000001000000000000000001010, // LDI R1, 10
        0b10111000010000000000000000000101, // LDI R2, 5
        0b00011100001000100001100000000000, // ADD R1, R2 → R3
        0b00100000001000100010000000000000, // SUB R1, R2 → R4
        0b00001100001000100010100000000000, // AND R1, R2 → R5
        0b00001000001000100011000000000000, // OR R1, R2 → R6
        0b00010000001000100011100000000000, // CMP R1, R2 → R7
        0b10010000001000000000000000001010, // CMPI R1, 10
        0b11000000000000000001000000000000, // JZ 4
        0b00000100001000100100000000000000, // XOR R1, R2 → R8
        0b10000000000000000001100100000000, // NOT R3 → R9
        0b11111100000000000000000000000000  // J 0
    };

    chip->initialize_instruction_memory(instructions);
  }

  void run_tests() {
    reset_chip();
    initialize_instruction_memory();

    /// RUN first instruction
    wait(10, SC_NS);

    std::cout << "Instruction Fetch" << std::endl;
    std::cout << "Instruction (0b10111000001000000000000000001010): "
              << chip->instruction_fetch->instruction.read() << std::endl;
    std::cout << "Next PC (1): " << chip->instruction_fetch->next_pc.read()
              << std::endl;
    std::cout << "Jump PC (0): " << chip->instruction_fetch->jump_pc.read()
              << std::endl;
    std::cout << "Jump PC Enable (0): "
              << chip->instruction_fetch->jump_pc_enable.read() << std::endl;

    sc_stop();
  }
};

int sc_main(int argc, char *argv[]) {
  ChipTb chipTb("chipTb");
  sc_start();
  return 0;
}
