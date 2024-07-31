#include "rob.h"
#include "param.h"
#include "regfile.h"
#include "lsb.h"
#include "decoder.h"

namespace arima {
    void ReorderBuffer::add(const RobEntry &entry) {
      if (new_rob.empty() && entry.ins.type == S) {
        new_mem_bus->write(BusType::STen, 0, 0);
      }
      new_rob.push(entry);
    }

    void ReorderBuffer::flush() {
      if (br_bus->get_type() == BusType::PC) {
        auto e = br_bus->read();
        if (e.first) {
          new_rob.clear();
        }
      }
      rob = new_rob;
    }

    void ReorderBuffer::update(RegFile &reg, LoadStoreBuffer &lsb) {
      if (rob.empty()) return;
      if (cd_bus->get_type() == BusType::Reg) {
        // rss says with CDB that a reg is ready
        auto e = cd_bus->read();
        if (e.first != -1) {
          int rob_id = e.first;
          new_rob[rob_id].value = e.second;
          new_rob[rob_id].status = Write;
          // when ST changes to write, it is ready to be written to memory
          // when ST is written to memory, it is ready to be committed
        }
      } else if (cd_bus->get_type() == BusType::PC) {
        // rss says with CDB that a branch is ready
        auto e = cd_bus->read();// e.second = 1 for branch taken, 0 for branch not taken
        if (e.first != -1) {
          int rob_id = e.first;
          new_rob[rob_id].value ^= e.second; // 1 for predict fail, 0 for predict success
          new_rob[rob_id].status = Write;
          // in general, we can know the result of branch at this stage
          // however, we wait the branch instruction to be committed
          // and then decide whether to clear new_rob, rs, etc.
        }
      }

      if (mem_bus->get_type() == BusType::Reg) {
        // lsb says with memB that a reg is ready
        // from issue to write
        auto e = mem_bus->read();
        if (e.first != -1) {
          int rob_id = e.first;
          new_rob[rob_id].value = e.second;
          new_rob[rob_id].status = Write;
        }
      } else if (mem_bus->get_type() == BusType::Mem) {
        // lsb says with memB that a store is written successfully
        // then we can commit the store instruction
        auto e = mem_bus->read();
        int rob_id = e.first;
        if (e.second) new_rob[rob_id].status = Commit;
      }
    }


    void ReorderBuffer::commit(RegFile &reg, LoadStoreBuffer &lsb) {
      if (rob.empty()) return;
      auto &entry = rob.front();
      auto &new_entry = new_rob.front();
      // if S is ready to be committed, then commit it
      // and let lsb write
      if (entry.status == Write || entry.status == Commit) {
        new_entry.status = Commit;
        if (entry.ins.code == ADDI && entry.ins.rd == 10
            && entry.ins.rs1 == 0 && entry.ins.imm == 255) {
          throw reg[10];
        }
        int rob_id = rob.get_front();
        if (entry.ins.type == opType::I
            || entry.ins.type == opType::R
            || entry.ins.type == opType::U) {
          int dest = entry.dest;
          reg.set_val(dest, entry.value);
          if (reg.get_new_dep(dest) == rob_id) {
            reg.set_dep(dest, -1);
          }
        } else if (entry.ins.type == opType::S) {
          // nothing
        } else if (entry.ins.type == opType::B) {
          // type, predict succeed/fail, next PC offset
          // if predict failed, reset instrAddr to next PC
          // if succeeded, do nothing
          new_br_bus->write(BusType::PC, entry.value, entry.dest);
          if (entry.value) {
//            std::cout << "h" << std::endl;
          }
        } else if (entry.ins.type == opType::J) {
          if (entry.ins.code == JAL) {
            int dest = entry.dest;
            reg.set_val(dest, entry.value);
            if (reg.get_dep(dest) == rob_id) {
              reg.set_dep(dest, -1);
            }
          } else if (entry.ins.code == JALR) {
            int dest = entry.dest;
            reg.set_val(dest, entry.value);
            if (reg.get_dep(dest) == rob_id) {
              reg.set_dep(dest, -1);
            }

          }

        }
        new_rob.pop();
        *pc = entry.addr;
        if (new_rob.front().ins.type == S) {
          new_mem_bus->write(BusType::STen, new_rob.get_front(), 0);
        }
      }
    }

// issue--(result broadcast received(for ST, when it's all ready))->write
// --(ST ? stored successfully : head)->commit
    void ReorderBuffer::execute(Decoder &dec, RegFile &reg, LoadStoreBuffer &lsb) {
      update(reg, lsb);
      commit(reg, lsb);
    }

}