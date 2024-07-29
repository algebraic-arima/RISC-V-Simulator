#pragma once
#ifndef RISCV_BUS_H
#define RISCV_BUS_H

#include "param.h"
#include <iostream>

namespace arima {

    enum class BusType {
      Reg,
      Mem,
      PC,
      STen,
    };

    const std::string busTypeStr[4] = {
            "Reg",
            "Mem",
            "PC",
            "STen",
    };



    class Bus {
    public:
      BusType type;
      int reg;
      word val;

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

      friend std::ostream &operator<<(std::ostream &os, const Bus &bus) {
        os << busTypeStr[static_cast<int>(bus.type)] << " " << bus.reg << " " << bus.val;
        return os;
      }
    };


}

#endif //RISCV_BUS_H
