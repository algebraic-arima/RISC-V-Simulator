#ifndef RISCV_BUS_H
#define RISCV_BUS_H

#include "param.h"
#include <iostream>

namespace arima {

    enum class BusType {
      Reg,
      Mem,
      PC,
    };


    class Bus {
      BusType type;
      int reg;
      word val;
    public:
      Bus() {
        type = BusType::Reg;
        reg = -1;
        val = 0;
      }

      BusType get_type() const { return type; }

      void write(BusType t, int re, word va) {
        type = t;
        reg = re;
        val = va;
      }

      std::pair<int, word> read() { return {reg, val}; }
    };
}

#endif //RISCV_BUS_H
