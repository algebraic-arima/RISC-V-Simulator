#include "simulator.h"

//#define vis

namespace arima {
    Simulator::Simulator(const char *filename)
            : lsb(filename) {
      init();
    }

    void Simulator::init() {
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
          execute();
        }
      } catch (word &res) {
        std::cout << (res & 0xff) << std::endl;
//        std::cout << "Fail rate: " << dec.pred.get_mispredict_rate() << std::endl;
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
      dec.flush();
      reg.flush();
      lsb.flush();
      rob.flush();
      rss.flush();


#ifdef vis
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

    }

    void Simulator::execute() {
      lsb.execute();rob.execute(dec, reg, lsb);
      rss.execute();
      dec.execute(reg, rob, lsb, rss);

    }
}