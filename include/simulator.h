#ifndef RISCV_SIMULATOR_HPP
#define RISCV_SIMULATOR_HPP

#include "decoder.h"

namespace arima {
    class Simulator {
    public:
      Decoder dec;
      RegFile reg;
      LoadStoreBuffer lsb;
      ReservationStation rss;
      ReorderBuffer rob;
      Bus cd_bus{}, mem_bus{};
      Bus new_cd_bus{}, new_mem_bus{};

      Simulator();

      void run();

      void flush();

      void execute();


    };
}

#endif //RISCV_SIMULATOR_HPP
