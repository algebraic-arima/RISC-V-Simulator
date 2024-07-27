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

    enum RegName {
      zero = 0,
      ra = 1,
      sp = 2,
      gp = 3,
      tp = 4,
      t0 = 5,
      t1 = 6,
      t2 = 7,
      s0 = 8,
      s1 = 9,
      a0 = 10,
      a1 = 11,
      a2 = 12,
      a3 = 13,
      a4 = 14,
      a5 = 15,
      a6 = 16,
      a7 = 17,
      s2 = 18,
      s3 = 19,
      s4 = 20,
      s5 = 21,
      s6 = 22,
      s7 = 23,
      s8 = 24,
      s9 = 25,
      s10 = 26,
      s11 = 27,
      t3 = 28,
      t4 = 29,
      t5 = 30,
      t6 = 31,
    };

    struct Instruction {
      opType type;
      opCode code;
      int imm;
      word rd, rs1, rs2;
    };

}

#endif //RISCV_PARAM_H
