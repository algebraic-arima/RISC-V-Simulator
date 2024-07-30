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
      Bus *cd_bus{}, *mem_bus{}, *br_bus{};
      Bus *new_cd_bus{}, *new_mem_bus{}, *new_br_bus{};

      void add(const RobEntry &entry);


      void display() {
        if (rob.empty()) {
          std::cout << "ROB is empty" << std::endl;
          return;
        }

        std::cout << "Reorder Buffer" << std::endl;
        std::cout << std::left << std::setw(10) << "Status"
                  << std::setw(10) << "Code"
                  << std::setw(10) << "Dest"
                  << std::setw(10) << "Value" << std::endl;

        // Print table rows
        for (auto &e: rob) {
          std::cout << std::left << std::setw(10) << e.status
                    << std::setw(10) << opCodeStr[e.ins.code]
                    << std::setw(10) << e.dest
                    << std::setw(10) << e.value << std::endl;
        }
      }

      void update(RegFile &reg, LoadStoreBuffer &lsb);

      void commit(RegFile &reg, LoadStoreBuffer &lsb);

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
