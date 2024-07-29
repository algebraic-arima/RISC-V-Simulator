#include "regfile.h"

namespace arima {
    RegFile::RegFile() {
      for (int i = 0; i < REG_SIZE; i++) {
        reg[i].val = 0;
        reg[i].dep = -1;
      }
    }

    word &RegFile::operator[](std::size_t idx) {
      return reg[idx].val;
    }

    word RegFile::operator[](std::size_t idx) const {
      return reg[idx].val;
    }

    void RegFile::set_dep(std::size_t idx, int dep) {
      new_reg[idx].dep = dep;
    }

    void RegFile::set_val(std::size_t idx, word val) {
      new_reg[idx].val = val;
    }

    int RegFile::get_dep(std::size_t idx) {
      return reg[idx].dep;
    }

    void RegFile::flush() {
      for (int i = 1; i < REG_SIZE; i++) {
        reg[i] = new_reg[i];
      }
      reg[0].val = 0;
      reg[0].dep = -1;
    }
}