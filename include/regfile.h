#ifndef RISCV_REGFILE_HPP
#define RISCV_REGFILE_HPP

#include "param.h"
#include "bus.h"

namespace arima {

    struct Register {
      word val = 0;
      int dep = -1; // -1 for no dependency
    };

    class RegFile {
      Register new_reg[REG_SIZE];
    public:
      Register reg[REG_SIZE];
      Bus *br_bus{}, *new_br_bus{};

      RegFile();


      void display() {
        int column = 0;
        for (int i = 0; i < REG_SIZE; i++) {
          std::cout << std::left << std::setw(20) <<
                    (regStr[i] + "=" + std::to_string(reg[i].val) + " #" + std::to_string(reg[i].dep));
          column++;
          if (column % 4 == 0) {
            std::cout << std::endl;
          }
        }
        if (column % 4 != 0) {
          std::cout << std::endl;
        }
        std::cout << std::dec;
      }

      word operator[](std::size_t) const;

      void set_dep(std::size_t, int);

      void set_val(std::size_t, word);

      int get_dep(std::size_t);

      int get_new_dep(std::size_t);

      void flush();

      void execute() {}
    };
}

#endif //RISCV_REGFILE_HPP
