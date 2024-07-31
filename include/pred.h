#ifndef RISCV_PREDICTOR_H
#define RISCV_PREDICTOR_H

#include "param.h"

namespace arima{
    class Predictor {
      int branch_num = 0;
      int correct_num = 0;
      int counter = 2;
    public:
      Predictor() = default;

      word next_front(word front, word offset) const;

      bool predict() const;

      void update(bool);

      double get_succ_rate() const;
    };
}

#endif //RISCV_PREDICTOR_H
