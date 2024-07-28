#ifndef RISCV_REGFILE_HPP
#define RISCV_REGFILE_HPP

#include "param.h"

namespace arima {

    struct Register {
      word val = 0;
      int dep = 0; // -1 for no dependency
    };

    class RegFile {
      Register new_reg[REG_SIZE];
    public:
      Register reg[REG_SIZE];

      RegFile();

      word &operator[](std::size_t);

      word operator[](std::size_t) const;

      void set_dep(std::size_t, int);

      int get_dep(std::size_t);

      void flush();

      void execute(){}
    };
}

#endif //RISCV_REGFILE_HPP
