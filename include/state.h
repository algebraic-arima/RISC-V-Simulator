#ifndef RISCV_STATE_H
#define RISCV_STATE_H

#include "param.h"
#include "regfile.h"

namespace arima{
    class State{
    public:
      word pc{};
      RegFile reg{};
    };
}

#endif //RISCV_STATE_H
