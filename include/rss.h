#ifndef RISCV_RSS_H
#define RISCV_RSS_H

#include "decoder.h"
#include "utils.h"


namespace arima {
    struct RssEntry {
      Instruction ins{};
      int vj = 0, vk = 0;
      int qj = -1, qk = -1;
      word a = 0;
      int rob_dest = -1;
    };

    class ReservationStation {
    public:
      cir_queue<RssEntry, RS_SIZE> rss;
    private:
      cir_queue<RssEntry, RS_SIZE> next_rss;
    public:
      bool empty() { return rss.empty(); }
      bool full() { return rss.full(); }
    };
}

#endif //RISCV_RSS_H
