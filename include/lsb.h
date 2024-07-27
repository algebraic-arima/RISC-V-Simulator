#ifndef RISCV_LSB_H
#define RISCV_LSB_H

#include "param.h"
#include "utils.h"

namespace arima{

    struct lsEntry{
      opCode code{}; // LB, LH, LW, LBU, LHU, SB, SH, SW
      int vj = 0, vk = 0;
      int qj = -1, qk = -1;
      word a = 0; // a temporary address after vj and vk are available
      int rob_dest = -1;
    };

    class LoadStoreBuffer{
      cir_queue<lsEntry, LSB_SIZE> lsb;
    private:
      cir_queue<lsEntry, LSB_SIZE> new_lsb;

      void flush();

      void execute();

    public:

      bool empty() { return lsb.empty(); }
      bool full() { return lsb.full(); }

      void step();

    };
}

#endif //RISCV_LSB_H
