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
      int a = 0; // a temporary address after vj and vk are available or an imm
      int rob_dest = -1;
    };

    class LoadStoreBuffer {
    public:
      cir_queue<LsbEntry, LSB_SIZE> lsb;
      Bus *mem_bus{},*new_mem_bus{};
      bool reset = false, new_reset = false;
      MemoryController mem;
    private:
      cir_queue<LsbEntry, LSB_SIZE> new_lsb;

    public:

      LoadStoreBuffer(){
        mem = MemoryController("../testcases/sample.data");
      }

      bool empty() { return lsb.empty(); }

      bool full() { return lsb.full(); }

      void add(const LsbEntry &entry);

      void set_ready(int rob_dest);

      void flush();

      void execute();

    };
}

#endif //RISCV_LSB_H
