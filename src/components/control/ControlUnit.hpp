#ifndef CONTROL_UNIT_HPP
#define CONTROL_UNIT_HPP

#include <systemc>
using namespace sc_core;
using namespace sc_dt;

enum AluOperation {
    ALU_AND = 0b0011,
    ALU_OR  = 0b0010,
    ALU_XOR = 0b0001,
    ALU_NOT = 0b0000,
    ALU_ADD = 0b0111,
    ALU_SUB = 0b1000,
    ALU_CMP = 0b0100,
};


// Módulo da Unidade de Controle (Control Unit)
SC_MODULE(ControlUnit) {

  sc_in<bool> clk;  

  // Entrada: opcode da instrução (bits [31:26])
  sc_in<sc_uint<6>> opcode;

  // Sinais de controle que serão gerados
  sc_out<bool> reg_write_enable;    // Habilita escrita no banco de registradores
  sc_out<bool> mem_read_enable;     // Habilita leitura da memória
  sc_out<bool> mem_write_enable;    // Habilita escrita na memória
  sc_out<bool> alu_src;             // Seleciona origem do segundo operando da ALU (registrador ou imediato)
  sc_out<bool> jump;                // Instrução de salto incondicional (J)
  sc_out<bool> branch_z;            // Salto condicional se zero (JZ)
  sc_out<bool> branch_n;            // Salto condicional se negativo (JN)
  sc_out<bool> reg_dst;             // Define se o registrador destino vem de `rd` (R-type) ou `rt` (I-type)
  sc_out<sc_uint<4>> alu_op;        // Operação a ser realizada pela ALU

  // Método que decodifica o opcode e gera os sinais de controle
  void decode() {
    // Inicialização padrão: todos sinais desligados
    reg_write_enable.write(false);
    mem_read_enable.write(false);
    mem_write_enable.write(false);
    alu_src.write(false);
    jump.write(false);
    branch_z.write(false);
    branch_n.write(false);
    reg_dst.write(true); // padrão: registrador destino é rd
    alu_op.write(0);     // operação default da ALU

    // Decodificação por opcode
    switch (opcode.read()) {
      case 0b000111: // ADD
        alu_op.write(ALU_ADD);         
        reg_write_enable.write(true); // Escreve resultado no registrador
        break;

      case 0b001000: // SUB
        alu_op.write(ALU_SUB);        
        reg_write_enable.write(true);
        break;

      case 0b000011: // AND
        alu_op.write(ALU_AND);        
        reg_write_enable.write(true);
        break;

      case 0b000010: // OR
        alu_op.write(ALU_OR);         
        reg_write_enable.write(true);
        break;

      case 0b000001: // XOR
        alu_op.write(ALU_XOR);         
        reg_write_enable.write(true);
        break;

      case 0b000100: // CMP
        alu_op.write(ALU_CMP);         
        reg_write_enable.write(true);
        break;
    
        case 0b100000: // NOT 
        alu_op.write(ALU_NOT);
        reg_write_enable.write(true);
        break;

      case 0b100111: // ADDI (imediato)
        alu_op.write(ALU_ADD);         
        reg_write_enable.write(true);
        alu_src.write(true);          // Segundo operando é imediato
        reg_dst.write(false);         // Registrador destino é `rt`
        break;

      case 0b101000: // SUBI
        alu_op.write(ALU_SUB);         
        reg_write_enable.write(true);
        alu_src.write(true);
        reg_dst.write(false);
        break;

      case 0b100011: // ANDI
        alu_op.write(ALU_AND);
        reg_write_enable.write(true);
        alu_src.write(true);
        reg_dst.write(false);
        break;

      case 0b100010: // ORI
        alu_op.write(ALU_OR);
        reg_write_enable.write(true);
        alu_src.write(true);
        reg_dst.write(false);
        break;

      case 0b100001: // XORI
        alu_op.write(ALU_XOR);
        reg_write_enable.write(true);
        alu_src.write(true);
        reg_dst.write(false);
        break;

      case 0b100100: // CMPI
        alu_op.write(ALU_CMP);
        reg_write_enable.write(true);
        alu_src.write(true);
        reg_dst.write(false);
        break;

      case 0b100110: // LD (leitura de memória)
        alu_op.write(ALU_ADD);         // Endereço = reg + imediato
        mem_read_enable.write(true);  // Ativa leitura da memória
        reg_write_enable.write(true); // Escreve valor lido no registrador
        alu_src.write(true);
        reg_dst.write(false);
        break;

      case 0b101001: // ST (escrita na memória)
        alu_op.write(ALU_ADD);
        mem_write_enable.write(true); // Ativa escrita na memória
        alu_src.write(true);
        break;

      case 0b111111: // J (salto incondicional)
        jump.write(true);
        break;

      case 0b110000: // JZ (salta se resultado zero)
        branch_z.write(true);
        break;

      case 0b111000: // JN (salta se negativo)
        branch_n.write(true);
        break;

      default:
        // Caso nenhuma instrução válida, mantém sinais desligados
        break;
    }
  }

  SC_CTOR(ControlUnit) {
    SC_METHOD(decode);
    sensitive << clk.pos();
    dont_initialize();

  }
};

#endif // CONTROL_UNIT_HPP
