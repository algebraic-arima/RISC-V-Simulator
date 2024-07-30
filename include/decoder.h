#ifndef RISCV_DECODER_H
#define RISCV_DECODER_H

#include "param.h"
#include "lsb.h"
#include "rob.h"
#include "rss.h"

namespace arima {

    struct RobEntry;

    class Predictor {
      int branch_num = 0;
      int correct_num = 0;
      int counter = 2;
    public:
      Predictor() = default;

      word next_front(word front, word offset) const;

      bool predict() const;

      void update(bool);

      int get_mispredict_rate() const;
    };

    class Decoder {
      bool push_rss = false, push_rob = false, push_lsb = false;
      RssEntry new_rss;
      RobEntry new_rob;
      LsbEntry new_lsb;

    public:
      word instrAddr = 0;
      word new_instrAddr = 0;
      bool freeze = false, new_freeze = false;
      bool fail = false, new_fail = false;
      Bus *cd_bus{}, *mem_bus{}, *br_bus{};
      Bus *new_cd_bus{}, *new_mem_bus{},*new_br_bus{};
      Predictor pred;

      void decode(word, Instruction &);

      word fetch(MemoryController &mem);

      void parse(const Instruction &ins,
                 RegFile &reg,
                 ReorderBuffer &rob,
                 LoadStoreBuffer &lsb,
                 ReservationStation &rss);

      void execute(RegFile &reg,
                   ReorderBuffer &rob,
                   LoadStoreBuffer &lsb,
                   ReservationStation &rss);

      void flush();

      void melt();
    };

}

#endif //RISCV_DECODER_H