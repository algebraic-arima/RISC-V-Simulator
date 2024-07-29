#include "rob.h"
#include "param.h"
#include "utils.h"
#include "regfile.h"
#include "lsb.h"
#include "decoder.h"

namespace arima {
    void ReorderBuffer::add(const RobEntry &entry) {
      new_rob.push(entry);
    }

    void ReorderBuffer::flush() {
      rob = new_rob;
    }

    void ReorderBuffer::update(RegFile &reg, LoadStoreBuffer &lsb) {
      if (new_rob.empty()) return;
      if (cd_bus->get_type() == BusType::Reg) {
        // rss says with CDB that a reg is ready
        auto e = cd_bus->read();
        int rob_id = e.first;
        rob[rob_id].value = e.second;
        rob[rob_id].status = Write;
        if (rob[rob_id].ins.type == S) {
          lsb.set_ready(rob_id);
        }// when ST changes to write, it is ready to be written to memory
        // when ST is written to memory, it is ready to be committed
      } else if (cd_bus->get_type() == BusType::PC) {
        // rss says with CDB that a branch is ready
        auto e = cd_bus->read();// e.second = 1 for branch taken, 0 for branch not taken
        int rob_id = e.first;
        rob[rob_id].value ^= e.second; // 1 for predict fail, 0 for predict success
        rob[rob_id].status = Write;
        // in general, we can know the result of branch at this stage
        // however, we wait the branch instruction to be committed
        // and then decide whether to clear rob, rs, etc.
      }

      if (mem_bus->get_type() == BusType::Reg) {
        // lsb says with memB that a reg is ready
        // from issue to write
        auto e = mem_bus->read();
        int rob_id = e.first;
        rob[rob_id].value = e.second;
        rob[rob_id].status = Write;
      } else if (mem_bus->get_type() == BusType::Mem) {
        // lsb says with memB that a store is written successfully
        // then we can commit the store instruction
        auto e = mem_bus->read();
        int rob_id = e.first;
        if (e.second) rob[rob_id].status = Commit;
      }
    }

    void ReorderBuffer::commit(RegFile &reg) {
      if (new_rob.empty()) return;
      auto &e = new_rob.front();
      if (e.status == Write || e.status == Commit) {
        e.status = Commit;
        auto &entry = new_rob.front();
        int rob_id = new_rob.get_front();
        if (entry.ins.type == opType::I
            || entry.ins.type == opType::R
            || entry.ins.type == opType::U) {
          int dest = entry.dest;
          if (reg.get_dep(dest) == rob_id) {
            reg.set_dep(dest, -1);
            reg[dest] = entry.value;
          }
        } else if (entry.ins.type == opType::S) {
          // set store ready = 1 in lsb
        } else if (entry.ins.type == opType::B) {
          if (entry.value) {

          }
        } else if (entry.ins.type == opType::J) {

        }
        new_rob.pop();
      }

    }

    // issue--(result broadcast received(for ST, when it's all ready))->write
    // --(ST ? stored successfully : head)->commit
    void ReorderBuffer::execute(Decoder &dec, RegFile &reg, LoadStoreBuffer &lsb) {
      update(reg, lsb);
      commit(reg);
    }
}