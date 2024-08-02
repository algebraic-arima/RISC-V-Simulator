#ifndef RISCV_PREDICTOR_H
#define RISCV_PREDICTOR_H

#include "param.h"

namespace arima {

    class Predictor {
      int branch_num = 0;
      int correct_num = 0;
      word counter[2048] = {0}; // 16 2-bit saturating counters
      byte hist[2048] = {0}; // high 4 bits for jumps, low 4 bits for predictions
      /*
       * +----------------+-----------------------+----------------------+
       * | latest 4 jumps | finite state machines | latest 4 predictions |
       * +----------------+-----------------------+----------------------+
       * |       (4)      |        (16)*2         |         (4)          |
       * +----------------+-----------------------+----------------------+
       * |                |    0000000000000000   |                      |
       * |                |    0000000000000000   |                      |
       * +----------------+-----------------------+----------------------+
       * |                |    fedcba9876543210   |                      |
       * +----------------+-----------------------+----------------------+
       */
      // enough for 4 history jumps and predicts
    public:
      std::string histoire[2048];

      Predictor() = default;

      word next_front(word, word) ;

      bool predict(word);

      void update(word, bool, bool);

      double get_succ_rate() const;

      bool predict_ada(word);

      void update_ada(word, bool, bool);
    };
}

#endif //RISCV_PREDICTOR_H
