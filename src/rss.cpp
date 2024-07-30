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
      new_rss[idx] = entry;
    }

    void ReservationStation::flush() {
      if (br_bus->get_type() == BusType::PC) {
        auto e = br_bus->read();
        if (e.first) {
          for (int i = 0; i < RS_SIZE; i++)
            new_rss[i].busy = false;
        }
      }
      rss = new_rss;
    }

    void ReservationStation::update() {
      auto e = cd_bus->read();
      auto f = mem_bus->read();
      for (int i = 0; i < RS_SIZE; i++) {
        if (e.first != -1) {
          if (rss[i].qj == e.first) {
            new_rss[i].qj = -1;
            new_rss[i].vj = e.second;
          }
          if (rss[i].qk == e.first) {
            new_rss[i].qk = -1;
            new_rss[i].vk = e.second;
          }
        }
        if (f.first != -1) {
          if (rss[i].qj == f.first) {
            new_rss[i].qj = -1;
            new_rss[i].vj = f.second;
          }
          if (rss[i].qk == f.first) {
            new_rss[i].qk = -1;
            new_rss[i].vk = f.second;
          }
        }
      }
    }

    void ReservationStation::clear() {
      for (auto &entry: new_rss) {
        entry.busy = false;
      }
    }

    void ReservationStation::execute() {
      update();
      for (int i = 0; i < RS_SIZE; i++) {
        if (rss[i].busy && rss[i].qj == -1 && rss[i].qk == -1) {
          auto en = new_rss[i];
          switch (en.ins.code) {
            case ADDI:
              new_rss[i].a = new_rss[i].vj + en.ins.imm;
              break;
            case SLTI:
              new_rss[i].a = new_rss[i].vj < en.ins.imm;
              break;
            case SLTIU:
              new_rss[i].a = (unsigned) new_rss[i].vj < (unsigned) en.ins.imm;
              break;
            case XORI:
              new_rss[i].a = new_rss[i].vj ^ en.ins.imm;
              break;
            case ORI:
              new_rss[i].a = new_rss[i].vj | en.ins.imm;
              break;
            case ANDI:
              new_rss[i].a = new_rss[i].vj & en.ins.imm;
              break;
            case SLLI:
              new_rss[i].a = new_rss[i].vj << en.ins.imm;
              break;
            case SRLI:
              new_rss[i].a = (unsigned) new_rss[i].vj >> en.ins.imm;
              break;
            case SRAI:
              new_rss[i].a = (int) new_rss[i].vj >> en.ins.imm;
              break;
            case ADD:
              new_rss[i].a = new_rss[i].vj + new_rss[i].vk;
              break;
            case SUB:
              new_rss[i].a = new_rss[i].vj - new_rss[i].vk;
              break;
            case SLL:
              new_rss[i].a = new_rss[i].vj << new_rss[i].vk;
              break;
            case SLT:
              new_rss[i].a = new_rss[i].vj < new_rss[i].vk;
              break;
            case SLTU:
              new_rss[i].a = (unsigned) new_rss[i].vj < (unsigned) new_rss[i].vk;
              break;
            case XOR:
              new_rss[i].a = new_rss[i].vj ^ new_rss[i].vk;
              break;
            case SRL:
              new_rss[i].a = (unsigned) new_rss[i].vj >> new_rss[i].vk;
              break;
            case SRA:
              new_rss[i].a = new_rss[i].vj >> new_rss[i].vk;
              break;
            case OR:
              new_rss[i].a = new_rss[i].vj | new_rss[i].vk;
              break;
            case AND:
              new_rss[i].a = new_rss[i].vj & new_rss[i].vk;
              break;
            case JALR:
              new_rss[i].a = new_rss[i].vj + en.ins.imm;
              break;
            case BEQ:
              new_rss[i].a = new_rss[i].vj == new_rss[i].vk;
              break;
            case BNE:
              new_rss[i].a = new_rss[i].vj != new_rss[i].vk;
              break;
            case BLT:
              new_rss[i].a = new_rss[i].vj < new_rss[i].vk;
              break;
            case BGE:
              new_rss[i].a = new_rss[i].vj >= new_rss[i].vk;
              break;
            case BLTU:
              new_rss[i].a = (unsigned) new_rss[i].vj < (unsigned) new_rss[i].vk;
              break;
            case BGEU:
              new_rss[i].a = (unsigned) new_rss[i].vj >= (unsigned) new_rss[i].vk;
              break;
            case JAL:
            case LUI:
            case AUIPC:
            default:
              break;
          }

          if (en.ins.type != B)
            new_cd_bus->write(BusType::Reg, new_rss[i].rob_dest, new_rss[i].a);
          else {
            // for branch, a = 1 for branch taken, 0 for branch not taken
            new_cd_bus->write(BusType::PC, new_rss[i].rob_dest, new_rss[i].a);
          }
          new_rss[i].busy = false;
          break; // complete a calculation in one cycle

        }
      }

    }
}