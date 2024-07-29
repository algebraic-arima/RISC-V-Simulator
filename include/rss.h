#ifndef RISCV_RSS_H
#define RISCV_RSS_H

#include "utils.h"
#include "alu.h"
#include "bus.h"

namespace arima {

    class Decoder;

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
      arr<RssEntry, RS_SIZE> rss;
      ALU alu;
      bool reset = false, new_reset = false;
      Bus *cd_bus, *mem_bus;
      Bus *new_cd_bus, *new_mem_bus;
    private:

      arr<RssEntry, RS_SIZE> new_rss;

      void update();

    public:
      int find_empty();

      bool full();

      void add(const RssEntry &entry);

      void flush();

      void clear();

      void execute();
    };
}

#endif //RISCV_RSS_H
