#ifndef RISCV_ALU_H
#define RISCV_ALU_H

#include "param.h"

namespace arima {
    struct Compute {
      opCode operand;
      word l, r;
      word rd;
    };

    class ALU {
    private:
      word l_operand, r_operand;
      opCode operand;
      word result;

    public:

      void set_params(word l, word r, opCode op) {
        l_operand = l;
        r_operand = r;
        operand = op;
      }

      word compute() {
        switch (operand) {
          case ADD:
          case ADDI:
            result = l_operand + r_operand;
            break;
          case SUB:
            result = l_operand - r_operand;
            break;
          case AND:
          case ANDI:
            result = l_operand & r_operand;
            break;
          case OR:
          case ORI:
            result = l_operand | r_operand;
            break;
          case XOR:
          case XORI:
            result = l_operand ^ r_operand;
            break;
          case SLL:
          case SLLI:
            result = l_operand << r_operand;
            break;
          case SRL:
          case SRLI:
            result = l_operand >> r_operand;
            break;
          case SRA:
          case SRAI:
            result = (int) l_operand >> r_operand;
            break;
          case SLT:
          case SLTI:
            result = (int)l_operand < (int)r_operand;
            break;
          case SLTU:
          case SLTIU:
            result = (unsigned) l_operand < (unsigned) r_operand;
            break;
          case BEQ:
            result = l_operand == r_operand;
            break;
          case BNE:
            result = l_operand != r_operand;
            break;
          case BLT:
            result = (int)l_operand < (int)r_operand;
            break;
          case BGE:
            result = (int)l_operand >= (int)r_operand;
            break;
          case BLTU:
            result = (unsigned) l_operand < (unsigned) r_operand;
            break;
          case BGEU:
            result = (unsigned) l_operand >= (unsigned) r_operand;
            break;
          default:
            result = 0;
        }
        return result;
      }
    };
}

#endif //RISCV_ALU_H
