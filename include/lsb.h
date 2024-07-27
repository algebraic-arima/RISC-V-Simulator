#ifndef RISCV_LSB_H
#define RISCV_LSB_H

#include "param.h"
#include "utils.h"
#include "memory.h"
#include "bus.h"

namespace arima {

    struct LsbEntry {
      bool ready = false;
      opCode code{}; // LB, LH, LW, LBU, LHU, SB, SH, SW
      int vj = 0, vk = 0;
      int qj = -1, qk = -1;
      int a = 0; // a temporary address after vj and vk are available
      int rob_dest = -1;
    };

    class LoadStoreBuffer {
      cir_queue<LsbEntry, LSB_SIZE> lsb;
      Bus *cd_bus;
    private:
      cir_queue<LsbEntry, LSB_SIZE> new_lsb;
      MemoryController mem;

    public:

      bool empty() { return lsb.empty(); }

      bool full() { return lsb.full(); }

      void add(const LsbEntry &entry);

      void flush();

      void execute();

    };
}

#endif //RISCV_LSB_H
