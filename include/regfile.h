#ifndef RISCV_REGFILE_HPP
#define RISCV_REGFILE_HPP

#include "param.h"

namespace arima {

    struct Register {
      word val = 0;
      int dep = 0;
    };

    class RegFile {
      Register reg[REG_SIZE];
    public:
      RegFile();

      word &operator[](std::size_t);

      void set_dep(std::size_t, int);
    };
}

#endif //RISCV_REGFILE_HPP
