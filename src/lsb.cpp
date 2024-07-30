#include "lsb.h"

namespace arima {
    void LoadStoreBuffer::add(const LsbEntry &entry) {
      new_lsb.push(entry);
    }

    void LoadStoreBuffer::set_ready() {
      new_lsb.front().ready = true;
    }

    void LoadStoreBuffer::flush() {
      if (br_bus->get_type() == BusType::PC) {
        auto e = br_bus->read();
        if (e.first) {
          new_lsb.clear();
        }
      }
      lsb = new_lsb;
    }

    void LoadStoreBuffer::execute() {
      if (lsb.empty()) return;
      if (reset) {
        new_lsb.clear();
        new_reset = false;
        return;
      }
      if (mem_bus->get_type() == BusType::Reg) {
        auto e = mem_bus->read();
        if (e.first != -1) {
          int reg = e.first;
          word val = e.second;
          for (auto &entry: new_lsb) {
            if (entry.qj == reg) {
              entry.vj = val;
              entry.qj = -1;
            }
            if (entry.qk == reg) {
              entry.vk = val;
              entry.qk = -1;
            }
          }
        }
      } else if (mem_bus->get_type() == BusType::STen) {
        if (lsb.front().code == SB || lsb.front().code == SH || lsb.front().code == SW) {
          if (mem_bus->read().first == lsb.front().rob_dest) {
            new_lsb.front().ready = true;
          }
        }
      }

      if (cd_bus->get_type() == BusType::Reg) {
        auto e = cd_bus->read();
        if (e.first != -1) {
          int reg = e.first;
          word val = e.second;
          for (auto &entry: new_lsb) {
            if (entry.qj == reg) {
              entry.vj = val;
              entry.qj = -1;
            }
            if (entry.qk == reg) {
              entry.vk = val;
              entry.qk = -1;
            }
          }
        }
      }

      LsbEntry entry = lsb.front();
      word result;
      if (!entry.ready || entry.qj != -1 || entry.qk != -1) {
        return;
      } else {
        entry.a += entry.vj;
        switch (entry.code) {
          case LB:
            result = mem.ldb(entry.a);
            new_mem_bus->write(BusType::Reg, entry.rob_dest, result);
            break;
          case LH:
            result = mem.ldh(entry.a);
            new_mem_bus->write(BusType::Reg, entry.rob_dest, result);
            break;
          case LW:
            result = mem.ldw(entry.a);
            new_mem_bus->write(BusType::Reg, entry.rob_dest, result);
            break;
          case LBU:
            result = mem.ldbu(entry.a);
            new_mem_bus->write(BusType::Reg, entry.rob_dest, result);
            break;
          case LHU:
            result = mem.ldhu(entry.a);
            new_mem_bus->write(BusType::Reg, entry.rob_dest, result);
            break;
          case SB:
            mem.stb(entry.a, entry.vk);
            new_mem_bus->write(BusType::Mem, entry.rob_dest, 1);
            // when store is prepared, write a success to mem_bus
            // then rob will know the store is ready and set WRITE
            // when ST comes to the head, it will be committed and
            // simultaneously write to memory
            break;
          case SH:
            mem.sth(entry.a, entry.vk);
            new_mem_bus->write(BusType::Mem, entry.rob_dest, 1);
            break;
          case SW:
            mem.stw(entry.a, entry.vk);
            new_mem_bus->write(BusType::Mem, entry.rob_dest, 1);
            break;
          default:
            break;
        }
        new_lsb.pop();
      }

    }
}