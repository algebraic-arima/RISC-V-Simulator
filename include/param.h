#ifndef RISCV_PARAM_H
#define RISCV_PARAM_H

#include <cstdint>

namespace arima {
    typedef uint8_t byte;
    typedef uint16_t hword;
    typedef uint32_t word;

    const int RAM_SIZE = 1 << 20;

    const int REG_SIZE = 32;

    const int RS_SIZE = 32;

    const int ROB_SIZE = 32;

    const int LSB_SIZE = 32;

    enum opType {
      U,
      J,
      B,
      I,
      S,
      R,
    };

    enum opCode {
      // U type
      LUI,                       // load upper immediate
      AUIPC,                     // add upper immediate to pc
      // J type
      JAL,                       // jump and link
      // I type
      JALR,                      // jump and link register
      // B type
      BEQ,                       // branch equal
      BNE,                       // branch not equal
      BLT,                       // branch less than
      BGE,                       // branch greater or equal
      BLTU,                      // branch less than unsigned
      BGEU,                      // branch greater or equal unsigned
      // I type
      LB,                        // load byte
      LH,                        // load half word
      LW,                        // load word
      LBU,                       // load byte unsigned
      LHU,                       // load half word unsigned
      // S type
      SB,                        // store byte
      SH,                        // store half word
      SW,                        // store word
      // I type
      ADDI,                      // add immediate
      SLTI,                      // set less than immediate
      SLTIU,                     // set less than immediate unsigned
      XORI,                      // xor immediate
      ORI,                       // or immediate
      ANDI,                      // and immediate
      // I type
      SLLI,                      // shift left logical immediate
      SRLI,                      // shift right logical immediate
      SRAI,                      // shift right arithmetic immediate
      // R type
      ADD,                       // add
      SUB,                       // subtract
      SLL,                       // shift left logical
      SLT,                       // set less than
      SLTU,                      // set less than unsigned
      XOR,                       // xor
      SRL,                       // shift right logical
      SRA,                       // shift right arithmetic
      OR,                        // or
      AND,                       // and
    };

    struct Instruction {
      opType type;
      opCode code;
      int imm;
      word rd, rs1, rs2;
    };

}

#endif //RISCV_PARAM_H
