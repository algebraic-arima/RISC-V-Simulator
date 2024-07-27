#include "rob.h"
#include "param.h"
#include "utils.h"
#include "regfile.h"
#include "lsb.h"

namespace arima {
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
}