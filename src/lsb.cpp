#include "lsb.h"
#include "bus.h"

namespace arima {
    void LoadStoreBuffer::add(const LsbEntry &entry) {
      new_lsb.push(entry);
    }

    void LoadStoreBuffer::flush() {
      lsb = new_lsb;
    }

    void LoadStoreBuffer::execute() {
      if (cd_bus->get_type() == BusType::Reg) {
        auto e = cd_bus->read();
        int reg = e.first;
        word val = e.second;
        for (auto &entry: lsb) {
          if (entry.qj == reg) {
            entry.vj = val;
            entry.qj = -1;
          }
          if (entry.qk == reg) {
            entry.vk = val;
            entry.qk = -1;
          }
        }
      } // read from cdb to fetch dependency
      if (new_lsb.empty()) return;
      LsbEntry entry = new_lsb.front();
      word result;
      if (!entry.ready || entry.qj != -1 || entry.qk != -1) {
        return;
      } else {
        switch (entry.code) {
          case LB:
            result = mem.ldb(entry.a);
            break;
          case LH:
            result = mem.ldh(entry.a);
            break;
          case LW:
            result = mem.ldw(entry.a);
            break;
          case LBU:
            result = mem.ldbu(entry.a);
            break;
          case LHU:
            result = mem.ldhu(entry.a);
            break;
          case SB:
            mem.stb(entry.a, entry.vj);
            break;
          case SH:
            mem.sth(entry.a, entry.vj);
            break;
          case SW:
            mem.stw(entry.a, entry.vj);
            break;
          default:
            break;
        }
        new_lsb.pop();
        lsb.push(entry);
      }

    }
}