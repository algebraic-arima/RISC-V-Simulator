#ifndef RISCV_ROB_H
#define RISCV_ROB_H

#include "param.h"
#include "utils.h"
#include "rss.h"
#include "lsb.h"
#include "regfile.h"

namespace arima {

    enum RobState {
      Issue,
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
      cir_queue<RobEntry, ROB_SIZE> new_rob;

      bool reset = false, new_reset = false;
    public:
      cir_queue<RobEntry, ROB_SIZE> rob;
      Bus *cd_bus, *mem_bus;
      Bus *new_cd_bus, *new_mem_bus;

      void add(const RobEntry &entry);

      void issue(const Instruction &, RegFile &, int);

      void update(RegFile &reg, LoadStoreBuffer &lsb);

      void commit(RegFile &reg);

      bool empty() { return rob.empty(); }

      bool full() const { return rob.full(); }

      int get_empty() const { return rob.full() ? -1 : rob.tail; }

      int get_front() const { return rob.head; }

      int get_value(int i) { return rob[i].value; }

      RobState get_state(int i) { return rob[i].status; }

      void flush();

      void execute(Decoder &dec, RegFile &reg, LoadStoreBuffer &lsb);


    };
}

#endif //RISCV_ROB_H
