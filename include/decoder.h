#ifndef RISCV_DECODER_H
#define RISCV_DECODER_H

#include "param.h"
#include "lsb.h"
#include "rob.h"
#include "rss.h"
#include "pred.h"

namespace arima {

    class Decoder {
      bool push_rss = false, push_rob = false, push_lsb = false;
      RssEntry new_rss;
      RobEntry new_rob;
      LsbEntry new_lsb;

    public:
      word instr_addr = 0;
      word new_instr_addr = 0;
      bool freeze = false, new_freeze = false;
      Bus *cd_bus{}, *mem_bus{}, *br_bus{};
      Bus *new_cd_bus{}, *new_mem_bus{}, *new_br_bus{};
      word *br_pc{}, *new_br_pc{};
      Predictor pred;

      void decode(word, Instruction &);

      word fetch(MemoryController &mem) const;

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

    };

}

#endif //RISCV_DECODER_H