#include "rss.h"

namespace arima {
    int ReservationStation::find_empty() {
      for (int i = 0; i < RS_SIZE; i++)
        if (!rss[i].busy) return i;
      return -1;
    }

    bool ReservationStation::full() {
      for (int i = 0; i < RS_SIZE; i++)
        if (!rss[i].busy) return false;
      return true;
    }

    void ReservationStation::add(const RssEntry &entry) {
      int idx = find_empty();
      rss[idx] = entry;
    }

    void ReservationStation::flush() {
      rss = new_rss;
      reset = new_reset;
    }

    void ReservationStation::update() {
      auto e = cd_bus->read();
      auto f = mem_bus->read();
      for (int i = 0; i < RS_SIZE; i++) {
        if (rss[i].qj == e.first) {
          rss[i].qj = -1;
          rss[i].vj = e.second;
        } else if (rss[i].qj == f.first) {
          rss[i].qj = -1;
          rss[i].vj = f.second;
        }
        if (rss[i].qk == e.first) {
          rss[i].qk = -1;
          rss[i].vk = e.second;
        } else if (rss[i].qk == f.first) {
          rss[i].qk = -1;
          rss[i].vk = f.second;
        }
      }
    }

    void ReservationStation::clear() {
      for (auto &entry: new_rss) {
        entry.busy = false;
      }
    }


    void ReservationStation::execute() {
      if (reset) {
        clear();
        new_reset = false;
        return;
      }
      update();
      for (int i = 0; i < RS_SIZE; i++) {
        if (rss[i].busy && rss[i].qj == -1 && rss[i].qk == -1) {
          auto en = rss[i];
          switch (en.ins.code) {
            case ADDI:
              rss[i].a = rss[i].vj + en.ins.imm;
              break;
            case SLTI:
              rss[i].a = rss[i].vj < en.ins.imm;
              break;
            case SLTIU:
              rss[i].a = (unsigned) rss[i].vj < (unsigned) en.ins.imm;
              break;
            case XORI:
              rss[i].a = rss[i].vj ^ en.ins.imm;
              break;
            case ORI:
              rss[i].a = rss[i].vj | en.ins.imm;
              break;
            case ANDI:
              rss[i].a = rss[i].vj & en.ins.imm;
              break;
            case SLLI:
              rss[i].a = rss[i].vj << en.ins.imm;
              break;
            case SRLI:
              rss[i].a = (unsigned) rss[i].vj >> en.ins.imm;
              break;
            case SRAI:
              rss[i].a = (int) rss[i].vj >> en.ins.imm;
              break;
            case ADD:
              rss[i].a = rss[i].vj + rss[i].vk;
              break;
            case SUB:
              rss[i].a = rss[i].vj - rss[i].vk;
              break;
            case SLL:
              rss[i].a = rss[i].vj << rss[i].vk;
              break;
            case SLT:
              rss[i].a = rss[i].vj < rss[i].vk;
              break;
            case SLTU:
              rss[i].a = (unsigned) rss[i].vj < (unsigned) rss[i].vk;
              break;
            case XOR:
              rss[i].a = rss[i].vj ^ rss[i].vk;
              break;
            case SRL:
              rss[i].a = (unsigned) rss[i].vj >> rss[i].vk;
              break;
            case SRA:
              rss[i].a = rss[i].vj >> rss[i].vk;
              break;
            case OR:
              rss[i].a = rss[i].vj | rss[i].vk;
              break;
            case AND:
              rss[i].a = rss[i].vj & rss[i].vk;
              break;
            case JALR:
              rss[i].a = rss[i].vj + en.ins.imm;
              break;
            case BEQ:
              rss[i].a = rss[i].vj == rss[i].vk;
              break;

            case BNE:
              rss[i].a = rss[i].vj != rss[i].vk;
              break;
            case BLT:
              rss[i].a = rss[i].vj < rss[i].vk;
              break;
            case BGE:
              rss[i].a = rss[i].vj >= rss[i].vk;
              break;
            case BLTU:
              rss[i].a = (unsigned) rss[i].vj < (unsigned) rss[i].vk;
              break;
            case BGEU:
              rss[i].a = (unsigned) rss[i].vj >= (unsigned) rss[i].vk;
              break;
            case JAL:
              rss[i].a = rss[i].ins.imm;
              break;
            default:
              break;
          }

          new_cd_bus->write(BusType::Reg, rss[i].rob_dest, rss[i].a);

          rss[i].busy = false;
          break; // complete a calculation in one cycle

        }
      }

    }
}