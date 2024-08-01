#include "simulator.h"
#include <algorithm>
#include <random>
#include <chrono>

//#define vis
//#define log

namespace arima {
    Simulator::Simulator() : lsb() {
      init();
    }

    Simulator::Simulator(const char *filename)
            : lsb(filename) {
      init();
#ifdef log
      of.open("F:/vscode/RISC-V-Simulator/log/output_fake.log", std::ios::out);
      if(!of.is_open()) {
        std::cerr << "Failed to open output file" << std::endl;
        exit(1);
      }
#endif
    }

    Simulator::~Simulator() {
#ifdef log
      of.close();
#endif
    }

    void Simulator::init() {
      seed = std::chrono::system_clock::now().time_since_epoch().count();

      reg.br_bus = &br_bus;
      reg.new_br_bus = &new_br_bus;

      dec.cd_bus = &cd_bus;
      dec.mem_bus = &mem_bus;
      dec.br_bus = &br_bus;
      dec.new_cd_bus = &new_cd_bus;
      dec.new_mem_bus = &new_mem_bus;
      dec.new_br_bus = &new_br_bus;

      rob.cd_bus = &cd_bus;
      rob.mem_bus = &mem_bus;
      rob.br_bus = &br_bus;
      rob.new_cd_bus = &new_cd_bus;
      rob.new_mem_bus = &new_mem_bus;
      rob.new_br_bus = &new_br_bus;

      rob.pc = &pc;

      rss.cd_bus = &cd_bus;
      rss.mem_bus = &mem_bus;
      rss.br_bus = &br_bus;
      rss.new_cd_bus = &new_cd_bus;
      rss.new_mem_bus = &new_mem_bus;
      rss.new_br_bus = &new_br_bus;

      lsb.cd_bus = &cd_bus;
      lsb.mem_bus = &mem_bus;
      lsb.br_bus = &br_bus;
      lsb.new_cd_bus = &new_cd_bus;
      lsb.new_mem_bus = &new_mem_bus;
      lsb.new_br_bus = &new_br_bus;
    }

    void Simulator::run() {
      try {
        while (true) {
          flush();
//          if (pc == 0x1348 && dec.instrAddr == 0x1358) {
//            std::cout << "やっとついた" << std::endl;
//          }
          execute();
        }
      } catch (word &res) {
        std::cout << (res & 0xff) << std::endl;
        std::cout << "Success rate: " << dec.pred.get_succ_rate() << std::endl;
        return;
      }
    }

    void Simulator::flush() {
      mem_bus = new_mem_bus;
      new_mem_bus.reset();
      cd_bus = new_cd_bus;
      new_cd_bus.reset();
      br_bus = new_br_bus;
      new_br_bus.reset();

      std::shuffle(std::begin(flushFuncs), std::end(flushFuncs), std::default_random_engine(seed));

      for (auto &flushFunc: flushFuncs) {
        (this->*flushFunc)();
      }

#ifdef vis
      if (pc == 0x1144 || pc == 0x113c || pc == 0x1140) {
        std::cout << "start" << std::endl;
        reg.display();
        lsb.display();
        rss.display();
        rob.display();
        std::cout << "mem_bus: " << mem_bus << std::endl;
        std::cout << "cd_bus: " << cd_bus << std::endl;
        std::cout << "br_bus: " << br_bus << std::endl;
      }
      std::cout << "\033[32m" << "PC: " << std::hex << pc << " ";
      std::cout << "INS: " << dec.instrAddr << std::dec << "\033[0m" << std::endl;
      reg.display();
      lsb.display();
      rss.display();
      rob.display();
      std::cout << "mem_bus: " << mem_bus << std::endl;
      std::cout << "cd_bus: " << cd_bus << std::endl;
      std::cout << "br_bus: " << br_bus << std::endl;
#endif
//      std::cout << "PC: " << std::hex << pc << " ";
//      std::cout << "INS: " << dec.instrAddr << std::dec << std::endl;

    }

    void Simulator::execute() {
      std::shuffle(std::begin(executeFuncs), std::end(executeFuncs), std::default_random_engine(seed));

      for (auto &executeFunc: executeFuncs) {
        (this->*executeFunc)();
      }
    }
}