#ifndef DECODER_HPP
#define DECODER_HPP

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

#define AND_OP 0
#define OR_OP 1
#define XOR_OP 2
#define CMP_OP 3
#define ADD_OP 4
#define SUB_OP 5
#define NOT_OP 6

SC_MODULE(Decoder) {
   sc_in_clk clk;
   sc_in<bool> reset;
   sc_in<sc_uint<32>> instruction;
   sc_out<sc_uint<4>> rs, rt, rd;
   sc_out<sc_uint<32>> immediate_i, immediate_j;
   sc_out<bool> reg_dst, alu_src, mem_to_reg, reg_write, mem_read, mem_write,
     jump, jump_n, jump_z;
   sc_out<sc_uint<3>> alu_op;

   void decode() {
      if (reset.read()) {
         rs.write(0);
         rt.write(0);
         rd.write(0);
         immediate_i.write(0);
         immediate_j.write(0);
         reg_dst.write(false);
         alu_src.write(false);
         mem_to_reg.write(false);
         reg_write.write(false);
         mem_read.write(false);
         mem_write.write(false);
         jump.write(false);
         jump_n.write(false);
         jump_z.write(false);
         alu_op.write(0);
      } else {
         rs.write(instruction.read().range(25, 22));
         rt.write(instruction.read().range(21, 18));
         rd.write(instruction.read().range(17, 14));
         immediate_i.write(instruction.read().range(17, 0));
         immediate_j.write(instruction.read().range(25, 0) << 2);

         sc_uint<6> op = instruction.read().range(31, 26);
         sc_uint<2> opcode_type = op.range(5, 4);
         sc_uint<4> opcode_function = op.range(3, 0);

         if (opcode_type == 0b00) {
            reg_dst.write(true);
            alu_src.write(false);
            mem_to_reg.write(false);
            reg_write.write(true);
            mem_read.write(false);
            mem_write.write(false);
            jump.write(false);
            jump_n.write(false);
            jump_z.write(false);

            switch (opcode_function) {
               case 0b0011:
                  alu_op.write(AND_OP);
                  break;
               case 0b0010:
                  alu_op.write(OR_OP);
                  break;
               case 0b0001:
                  alu_op.write(XOR_OP);
                  break;
               case 0b0100:
                  alu_op.write(CMP_OP);
                  break;
               case 0b0111:
                  alu_op.write(ADD_OP);
                  break;
               case 0b1000:
                  alu_op.write(SUB_OP);
                  break;
               default:
                  break;
            }
         } else if (opcode_type == 0b10) {
            // Tipo I
            reg_dst.write(false);
            jump.write(false);
            jump_n.write(false);
            jump_z.write(false);

            switch (opcode_function) {
               case 0b0000: // NOT
                  alu_src.write(false);
                  reg_write.write(true);
                  mem_read.write(false);
                  mem_write.write(false);
                  mem_to_reg.write(false);
                  alu_op.write(NOT_OP);
                  break;

               case 0b0110: // LD
                  alu_src.write(true);
                  reg_write.write(true);
                  mem_read.write(true);
                  mem_write.write(false);
                  mem_to_reg.write(true);
                  alu_op.write(ADD_OP); // calcula endereço base + offset
                  break;

               case 0b1001: // ST
                  alu_src.write(true);
                  reg_write.write(false);
                  mem_read.write(false);
                  mem_write.write(true);
                  mem_to_reg.write(false);
                  alu_op.write(ADD_OP);
                  break;

               case 0b0011: // ANDI
                  alu_src.write(true);
                  reg_write.write(true);
                  mem_read.write(false);
                  mem_write.write(false);
                  mem_to_reg.write(false);
                  alu_op.write(AND_OP);
                  break;

               case 0b0010: // ORI
                  alu_src.write(true);
                  reg_write.write(true);
                  mem_read.write(false);
                  mem_write.write(false);
                  mem_to_reg.write(false);
                  alu_op.write(OR_OP);
                  break;

               case 0b0001: // XORI
                  alu_src.write(true);
                  reg_write.write(true);
                  mem_read.write(false);
                  mem_write.write(false);
                  mem_to_reg.write(false);
                  alu_op.write(XOR_OP);
                  break;

               case 0b0100: // CMPI
                  alu_src.write(true);
                  reg_write.write(true);
                  mem_read.write(false);
                  mem_write.write(false);
                  mem_to_reg.write(false);
                  alu_op.write(CMP_OP);
                  break;

               case 0b0111: // ADDI
                  alu_src.write(true);
                  reg_write.write(true);
                  mem_read.write(false);
                  mem_write.write(false);
                  mem_to_reg.write(false);
                  alu_op.write(ADD_OP);
                  break;

               case 0b1000: // SUBI
                  alu_src.write(true);
                  reg_write.write(true);
                  mem_read.write(false);
                  mem_write.write(false);
                  mem_to_reg.write(false);
                  alu_op.write(SUB_OP);
                  break;

               default:
                  alu_src.write(false);
                  reg_write.write(false);
                  mem_read.write(false);
                  mem_write.write(false);
                  mem_to_reg.write(false);
                  alu_op.write(ADD_OP);
                  break;
            }
         } else if (opcode_type == 0b11) {
            // Tipo J
            reg_dst.write(false);
            alu_src.write(false);
            mem_to_reg.write(false);
            reg_write.write(false);
            mem_read.write(false);
            mem_write.write(false);
            alu_op.write(ADD_OP);

            switch (opcode_function) {
               case 0b1111:
                  jump.write(true);
                  jump_n.write(false);
                  jump_z.write(false);
                  break;

               case 0b1000:
                  jump.write(false);
                  jump_n.write(true);
                  jump_z.write(false);
                  break;

               case 0b0000:
                  jump.write(false);
                  jump_n.write(false);
                  jump_z.write(true);
                  break;
               default:
                  jump.write(false);
                  jump_n.write(false);
                  jump_z.write(false);
                  break;
            }
         } else {
            reg_dst.write(false);
            alu_src.write(false);
            mem_to_reg.write(false);
            reg_write.write(false);
            mem_read.write(false);
            mem_write.write(false);
            alu_op.write(ADD_OP);
            jump.write(false);
            jump_n.write(false);
            jump_z.write(false);
         }
      }
   }

   SC_CTOR(Decoder) {
      SC_METHOD(decode);
      sensitive << instruction << clk.pos();
   }
};

#endif // !DECODER_HPP