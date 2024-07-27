#ifndef RISCV_ROB_H
#define RISCV_ROB_H

#include "param.h"
#include "utils.h"
#include "regfile.h"
#include "lsb.h"

namespace arima {

    enum RobState {
      Issue,
      Exec, // for arithmetic
      Exec1,
      Exec2,
      Exec3, // for load and store
      Write,
      Commit
    };

    struct RobEntry {
      RobState status{};
      Instruction ins{};
      word dest = -1;
      int value = 0;
    };

    class ReorderBuffer {
      cir_queue<RobEntry, ROB_SIZE> rob;
    public:
      ReorderBuffer() = default;

      void issue(const Instruction &, RegFile &, int);

      void write(const RobEntry &, bus *);

      void update();

      void commit(RegFile &reg, LoadStoreBuffer &lsb);

      bool empty() { return rob.empty(); }

      bool full() { return rob.full(); }

      int get_empty() { return rob.full() ? -1 : rob.tail; }
    };
}

#endif //RISCV_ROB_H
