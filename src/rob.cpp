#include "rob.h"
#include "param.h"
#include "utils.h"
#include "regfile.h"
#include "lsb.h"

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

    void ReorderBuffer::execute() {
      if (cd_bus->get_type() == BusType::Reg) {
        auto e = cd_bus->read();
        for (auto &entry: rob) {
          if (entry.dest == e.first) {
            entry.status = Exec;
            entry.value = e.value;
          }
        }
      }
    }
}