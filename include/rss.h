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
      Bus *cd_bus{}, *mem_bus{}, *br_bus{};
      Bus *new_cd_bus{}, *new_mem_bus{}, *new_br_bus{};
    private:

      arr<RssEntry, RS_SIZE> new_rss;

      void update();

    public:

      void display() {
        std::cout << "Reservation Station" << std::endl;
        std::cout << std::left << std::setw(10) << "Busy"
                  << std::setw(10) << "Code"
                  << std::setw(10) << "VJ"
                  << std::setw(10) << "VK"
                  << std::setw(10) << "QJ"
                  << std::setw(10) << "QK"
                  << std::setw(10) << "A"
                  << std::setw(10) << "ROB Dest" << std::endl;

        // Print table rows
        for (int i = 0; i < RS_SIZE; i++) {
          std::cout << std::left << std::setw(10) << rss[i].busy
                    << std::setw(10) << opCodeStr[rss[i].ins.code]
                    << std::setw(10) << rss[i].vj
                    << std::setw(10) << rss[i].vk
                    << std::setw(10) << rss[i].qj
                    << std::setw(10) << rss[i].qk
                    << std::setw(10) << rss[i].a
                    << std::setw(10) << rss[i].rob_dest << std::endl;
        }
      }

      int find_empty();

      bool full();

      void add(const RssEntry &entry);

      void flush();

      void clear();

      void execute();
    };
}

#endif //RISCV_RSS_H
