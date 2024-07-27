#ifndef RISCV_BUS_H
#define RISCV_BUS_H

#include "param.h"

namespace arima{

    enum class BusType {
        INST,
        DATA
    };


    class Bus {
      BusType type;
      word val;
    public:
        static void write(word addr, word data);
        static word read(word addr);
    };
}

#endif //RISCV_BUS_H
