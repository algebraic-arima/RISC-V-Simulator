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
      Bus *cd_bus{}, *new_cd_bus{};
      Bus *mem_bus{}, *new_mem_bus{};
      Bus *br_bus{}, *new_br_bus{};
      MemoryController mem;
    private:
      cir_queue<std::pair<LsbEntry,bool>, LSB_SIZE> lsb;
      cir_queue<std::pair<LsbEntry,bool>, LSB_SIZE> new_lsb;

    public:

      void display() {
        if (lsb.empty()) {
          std::cout << "LSB is empty" << std::endl;
          return;
        }

        std::cout << "Load Store Buffer" << std::endl;
        std::cout << std::left << std::setw(10) << "Ready"
                  << std::setw(10) << "Code"
                  << std::setw(10) << "VJ"
                  << std::setw(10) << "VK"
                  << std::setw(10) << "QJ"
                  << std::setw(10) << "QK"
                  << std::setw(10) << "A"
                  << std::setw(10) << "ROB Dest" << std::endl;

        // Print table rows
        for (auto &f: lsb) {
          auto e = f.first;
          std::cout << std::left << std::setw(10) << e.ready
                    << std::setw(10) << opCodeStr[e.code]
                    << std::setw(10) << e.vj
                    << std::setw(10) << e.vk
                    << std::setw(10) << e.qj
                    << std::setw(10) << e.qk
                    << std::setw(10) << e.a
                    << std::setw(10) << e.rob_dest << std::endl;
        }
      }

      LoadStoreBuffer() : mem() {}

      explicit LoadStoreBuffer(const char *filename) : mem(filename) {}

      bool empty() { return lsb.empty(); }

      bool full() const { return lsb.full(); }

      void add(const LsbEntry &entry);

      void flush();

      void execute();

    };
}

#endif //RISCV_LSB_H
