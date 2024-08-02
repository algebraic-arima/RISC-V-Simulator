#ifndef RISCV_SIMULATOR_HPP
#define RISCV_SIMULATOR_HPP

#include <fstream>
#include "decoder.h"

namespace arima {
    class Simulator {
    public:
      Decoder dec;
      RegFile reg;
      LoadStoreBuffer lsb;
      ReservationStation rss;
      ReorderBuffer rob;
      Bus cd_bus{}, mem_bus{}, br_bus{};
      Bus new_cd_bus{}, new_mem_bus{}, new_br_bus{};
      word br_pc = 0, new_br_pc = 0;
      word pc = 0;
      unsigned seed = 0;

      using FuncPtr = void (Simulator::*)();
      FuncPtr flushFuncs[5] = {
              &Simulator::lsbFlush,
              &Simulator::decFlush,
              &Simulator::regFlush,
              &Simulator::robFlush,
              &Simulator::rssFlush,
      };
      FuncPtr executeFuncs[4] = {
              &Simulator::lsbExecute,
              &Simulator::decExecute,
              &Simulator::robExecute,
              &Simulator::rssExecute,
      };
      std::ofstream of;


      Simulator();

      explicit Simulator(const char *filename);

      ~Simulator();

      void lsbFlush() { lsb.flush(); }

      void decFlush() { dec.flush(); }

      void regFlush() { reg.flush(); }

      void robFlush() { rob.flush(); }

      void rssFlush() { rss.flush(); }

      void lsbExecute() { lsb.execute(); }

      void decExecute() { dec.execute(reg, rob, lsb, rss); }

      void robExecute() { rob.execute(dec, reg, lsb); }

      void rssExecute() { rss.execute(); }

      void init();

      void run();

      void flush();

      void execute();

    };
}

#endif //RISCV_SIMULATOR_HPP
