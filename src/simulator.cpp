#include "simulator.h"

namespace arima {
    Simulator::Simulator() {
      init();
    }

    void Simulator::init() {
      dec.cd_bus = &cd_bus;
      dec.mem_bus = &mem_bus;
      rob.cd_bus = &cd_bus;
      rob.mem_bus = &mem_bus;
      rss.cd_bus = &cd_bus;
      rss.mem_bus = &mem_bus;
      lsb.cd_bus = &cd_bus;
      lsb.mem_bus = &mem_bus;
      dec.new_cd_bus = &new_cd_bus;
      dec.new_mem_bus = &new_mem_bus;
      rob.new_cd_bus = &new_cd_bus;
      rob.new_mem_bus = &new_mem_bus;
      rss.new_cd_bus = &new_cd_bus;
      rss.new_mem_bus = &new_mem_bus;
      lsb.new_mem_bus = &new_mem_bus;
    }

    void Simulator::run() {
      try {
        while (true) {
          flush();
          execute();
        }
      } catch (word &res) {
        std::cout << (res & 0xff) << std::endl;
        std::cout << "Fail rate: " << dec.pred.get_mispredict_rate() << std::endl;
        return;
      }
    }

    void Simulator::flush() {
      mem_bus = new_mem_bus;
      cd_bus = new_cd_bus;
      dec.flush();
      reg.flush();
      reg.display();
      lsb.flush();
      lsb.display();
      rss.flush();
      rss.display();
      rob.flush();
      rob.display();
      std::cout << "mem_bus: " << mem_bus << std::endl;
      std::cout << "cd_bus: " << cd_bus << std::endl;
    }

    void Simulator::execute() {
      dec.execute(reg, rob, lsb, rss);
      rss.execute();
      rob.execute(dec, reg, lsb);
      lsb.execute();
    }
}