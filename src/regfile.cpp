#include "regfile.h"

namespace arima {
    RegFile::RegFile() {
        for (int i = 0; i < REG_SIZE; i++) {
            reg[i].val = 0;
            reg[i].dep = 0;
        }
    }

    word &RegFile::operator[](std::size_t idx) {
        return reg[idx].val;
    }

    void RegFile::set_dep(std::size_t idx, int dep) {
        reg[idx].dep = dep;
    }

    int RegFile::get_dep(std::size_t idx) {
        return reg[idx].dep;
    }
}