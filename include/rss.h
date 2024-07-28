#ifndef RISCV_RSS_H
#define RISCV_RSS_H

#include "decoder.h"
#include "utils.h"

namespace arima {
    struct RssEntry {
      bool busy = false;
      Instruction ins{};
      int vj = 0, vk = 0;
      int qj = -1, qk = -1;
      int a = 0;
      int rob_dest = -1;
    };

    class ReservationStation {
    public:
      RssEntry rss[RS_SIZE];
    private:
      RssEntry next_rss[RS_SIZE];
    public:
      int find_empty() {
        for (int i = 0; i < RS_SIZE; i++)
          if (!rss[i].busy) return i;
        return -1;
      }

      bool full(){
        for (int i = 0; i < RS_SIZE; i++)
          if (!rss[i].busy) return false;
        return true;
      }
    };
}

#endif //RISCV_RSS_H
