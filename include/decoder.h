#ifndef RISCV_DECODER_H
#define RISCV_DECODER_H

#include "param.h"
#include "memory.h"
#include "state.h"
#include "rob.h"
#include "rss.h"

namespace arima {

    class Predictor {
      int branch_num = 0;
      int correct_num = 0;
      int counter = 1;
    public:
      Predictor() = default;

      word next_front(word front, word offset) const;

      bool predict() const;

      void update(bool);

      int get_mispredict_rate() const;
    };

    class Decoder {
      RssEntry new_rss;
      RobEntry new_rob;
      LsbEntry new_lsb;

    public:
      word instrAddr = 0;
      Predictor pred;

      void decode(word, Instruction &);

      word fetch(MemoryController *mem);

      void parse(const Instruction &ins, RegFile &reg, ReorderBuffer &rob, LoadStoreBuffer &lsb,ReservationStation &rss);
    };

}

#endif //RISCV_DECODER_H