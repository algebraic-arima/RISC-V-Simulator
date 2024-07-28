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

    void ReorderBuffer::issue(const Instruction &ins, RegFile &reg, int value) {
      RobEntry entry;
      entry.status = Issue;
      entry.ins = ins;
      entry.dest = ins.rd;
      reg.set_dep(ins.rd, rob.tail);
      entry.value = value;
      rob.push(entry);
    }

    void ReorderBuffer::commit(RegFile &reg, LoadStoreBuffer &lsb) {
      if (rob.empty()) return;
      while (rob.front().status == Commit) {
        rob.pop();
      }
      if (rob.front().status != Write) {
        return;
      }
      RobEntry entry = rob.pop();

    }

    // issue--(dep removed)->exec
    // --(result broadcast)->write
    // --(ST ? stored successfully  :head)->commit
    void ReorderBuffer::execute(Decoder &dec, RegFile &reg, LoadStoreBuffer &lsb, MemoryController &mem) {
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
        auto e = mem_bus->read();
        int rob_id = e.first;
        rob[rob_id].value = e.second;
        rob[rob_id].status = Write;
      } else if (mem_bus->get_type() == BusType::Mem) {
        // lsb says with memB that a store is written successfully
        auto e = mem_bus->read();
        int rob_id = e.first;
        if (e.second) rob[rob_id].status = Write;
      }

      // commit
      if (new_rob.front().status == Write) {
        new_rob.front().status = Commit;
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
            dec.set_pc(entry.ins.imm);
          }
        } else if (entry.ins.type == opType::J) {
          dec.set_pc(entry.value);
        }
      }
    }
}