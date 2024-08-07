#include "decoder.h"

//#define vis

namespace arima {

    word Decoder::fetch(MemoryController &mem) const {
      return mem.ldw(instr_addr);
    }

    void decode_branch(word input, Instruction &ins) {
      byte opn = input >> 12 & 0b111;
      switch (opn) {
        case 0b00000000:
          ins.code = opCode::BEQ;
          break;
        case 0b00000001:
          ins.code = opCode::BNE;
          break;
        case 0b00000100:
          ins.code = opCode::BLT;
          break;
        case 0b00000101:
          ins.code = opCode::BGE;
          break;
        case 0b00000110:
          ins.code = opCode::BLTU;
          break;
        case 0b00000111:
          ins.code = opCode::BGEU;
          break;
        default:
          break;
      }
    }

    void decode_load(word input, Instruction &ins) {
      byte opn = input >> 12 & 0b111;
      switch (opn) {
        case 0b00000000:
          ins.code = opCode::LB;
          break;
        case 0b00000001:
          ins.code = opCode::LH;
          break;
        case 0b00000010:
          ins.code = opCode::LW;
          break;
        case 0b00000100:
          ins.code = opCode::LBU;
          break;
        case 0b00000101:
          ins.code = opCode::LHU;
          break;
        default:
          break;
      }
    }

    void decode_store(word input, Instruction &ins) {
      byte opn = input >> 12 & 0b111;
      switch (opn) {
        case 0b00000000:
          ins.code = opCode::SB;
          break;
        case 0b00000001:
          ins.code = opCode::SH;
          break;
        case 0b00000010:
          ins.code = opCode::SW;
          break;
        default:
          break;
      }
    }

    void decode_arithi(word input, Instruction &ins) {
      byte opn = input >> 12 & 0b111;
      switch (opn) {
        case 0b000:
          ins.code = opCode::ADDI;
          break;
        case 0b010:
          ins.code = opCode::SLTI;
          break;
        case 0b011:
          ins.code = opCode::SLTIU;
          break;
        case 0b100:
          ins.code = opCode::XORI;
          break;
        case 0b110:
          ins.code = opCode::ORI;
          break;
        case 0b111:
          ins.code = opCode::ANDI;
          break;
        case 0b001:
          ins.code = opCode::SLLI;
          break;
        case 0b101:
          if (input >> 30 & 0b1) {
            ins.code = opCode::SRAI;
          } else {
            ins.code = opCode::SRLI;
          }
          break;
        default:
          break;
      }
    }

    void decode_arithr(word input, Instruction &ins) {
      byte opn = input >> 12 & 0b111;
      switch (opn) {
        case 0b000:
          if (input >> 30 & 0b1)
            ins.code = opCode::SUB;
          else
            ins.code = opCode::ADD;
          break;
        case 0b001:
          ins.code = opCode::SLL;
          break;
        case 0b010:
          ins.code = opCode::SLT;
          break;
        case 0b011:
          ins.code = opCode::SLTU;
          break;
        case 0b100:
          ins.code = opCode::XOR;
          break;
        case 0b101:
          if (input >> 30 & 0b1)
            ins.code = opCode::SRA;
          else
            ins.code = opCode::SRL;
          break;
        case 0b110:
          ins.code = opCode::OR;
          break;
        case 0b111:
          ins.code = opCode::AND;
          break;
        default:
          break;
      }
    }

    void Decoder::decode(word input, Instruction &ins) {
      byte opn = input & 0b100;
      if (opn != 0) {
        if (input & 0b0010000) {
          ins.type = opType::U;
          ins.rd = (input >> 7) & 0b11111;
          ins.imm = input >> 12 << 12;
          if (input & 0b0100000) {
            ins.code = opCode::LUI;
          } else {
            ins.code = opCode::AUIPC;
          }
        } else {
          if (input & 0b0001000) {
            ins.type = opType::J;
            ins.code = opCode::JAL;
            ins.rd = (input >> 7) & 0b11111;
            ins.imm = ((input >> 31) << 20)
                      | ((input >> 12 & 0b11111111) << 12)
                      | ((input >> 20 & 0b1) << 11)
                      | (input >> 21 & 0b1111111111) << 1;
            ins.imm = signExt(ins.imm, 21);
          } else {
            ins.type = opType::I;
            ins.code = opCode::JALR;
            ins.rd = (input >> 7) & 0b11111;
            ins.imm = input >> 20;
            ins.rs1 = (input >> 15) & 0b11111;
            ins.imm = signExt(ins.imm, 12);
          }
        }
        return;
      }
      opn = input >> 3 & 0b1111;
      if (opn == 0b1100) {
        ins.type = opType::B;
        ins.rs1 = (input >> 15) & 0b11111;
        ins.rs2 = (input >> 20) & 0b11111;
        ins.imm = (input >> 31 & 0b1) << 12
                  | (input >> 25 & 0b111111) << 5
                  | (input >> 8 & 0b1111) << 1
                  | (input >> 7 & 0b1) << 11;
        ins.imm = signExt(ins.imm, 13);
        decode_branch(input, ins);
        return;
      } else if (opn == 0b0000) {
        ins.type = opType::I;
        ins.rs1 = (input >> 15) & 0b11111;
        ins.rd = (input >> 7) & 0b11111;
        ins.imm = signExt(input >> 20, 12);
        decode_load(input, ins);
      } else if (opn == 0b0100) {
        ins.type = opType::S;
        ins.rs1 = (input >> 15) & 0b11111;
        ins.rs2 = (input >> 20) & 0b11111;
        ins.imm = input >> 25 << 5 | ((input >> 7) & 0b11111);
        ins.imm = signExt(ins.imm, 12);
        decode_store(input, ins);
      } else if (opn == 0b0010) {
        ins.type = opType::I;
        ins.rs1 = (input >> 15) & 0b11111;
        ins.rd = (input >> 7) & 0b11111;
        ins.imm = signExt(input >> 20, 12);
        decode_arithi(input, ins);
      } else if (opn == 0b0110) {
        ins.type = opType::R;
        ins.rs1 = (input >> 15) & 0b11111;
        ins.rs2 = (input >> 20) & 0b11111;
        ins.rd = (input >> 7) & 0b11111;
        decode_arithr(input, ins);
      }
    }

    void Decoder::parse(const Instruction &ins,
                        RegFile &reg,
                        ReorderBuffer &rob,
                        LoadStoreBuffer &lsb,
                        ReservationStation &rss) {
      push_rss = false, push_lsb = false, push_rob = false;

      if (ins.code == AUIPC) {
        push_rob = push_rss = true;
        new_rob = {Issue, ins, ins.rd, static_cast<int>((instr_addr + ins.imm))};
        new_rss = {true, ins, 0, 0, -1, -1, static_cast<int>(instr_addr + ins.imm), rob.get_empty()};
      } else if (ins.code == opCode::LUI) {
        push_rob = push_rss = true;
        new_rob = {Issue, ins, ins.rd, ins.imm};
        new_rss = {true, ins, 0, 0, -1, -1, ins.imm, rob.get_empty()};
      } else if (ins.code == JAL) {
        push_rob = push_rss = true;
        new_rob = {Issue, ins, ins.rd, static_cast<int>(instr_addr + 4)};
        new_rss = {true, ins, 0, 0, -1, -1, static_cast<int>(instr_addr + 4), rob.get_empty()};
        new_instr_addr = instr_addr + ins.imm;
        return;
      } else if (ins.code == JALR) {
        push_rss = push_rob = true;
        new_rob = {Issue, ins, ins.rd, static_cast<int>(instr_addr + 4)};
        new_rss = {true, ins, 0, ins.imm, -1, -1, 0, rob.get_empty()};
        int r_i = reg.get_dep(ins.rs1);
        if (r_i != -1) {
          if (rob.get_state(r_i) != Write)
            new_rss.qj = r_i;
          else
            new_rss.vj = rob.get_value(r_i);
        } else {
          new_rss.vj = reg[ins.rs1];
        }
        new_freeze = true;
      } else if (ins.type == R) {
        push_rss = push_rob = true;
        new_rob = {Issue, ins, ins.rd, 0};
        new_rss = {true, ins, 0, 0, -1, -1, 0, rob.get_empty()};
        int r_i = reg.get_dep(ins.rs1);
        if (r_i != -1) {
          if (rob.get_state(r_i) != Write)
            new_rss.qj = r_i;
          else
            new_rss.vj = rob.get_value(r_i);
        } else {
          new_rss.vj = reg[ins.rs1];
        }
        r_i = reg.get_dep(ins.rs2);
        if (r_i != -1) {
          if (rob.get_state(r_i) != Write)
            new_rss.qk = r_i;
          else
            new_rss.vk = rob.get_value(r_i);
        } else {
          new_rss.vk = reg[ins.rs2];
        }
      } else if (ins.type == I) {
        push_rob = true;
        new_rob = {Issue, ins, ins.rd, 0};
        if (ins.code == LB || ins.code == LH || ins.code == LW || ins.code == LBU || ins.code == LHU) {
          push_lsb = true;
          new_lsb = {true, ins.code, 0, 0, -1, -1, ins.imm, rob.get_empty()};
          int r_i = reg.get_dep(ins.rs1);
          if (r_i != -1) {
            if (rob.get_state(r_i) != Write)
              new_lsb.qj = r_i;
            else
              new_lsb.vj = rob.get_value(r_i);
          } else {
            new_lsb.vj = reg[ins.rs1];
          }
        } else {
          push_rss = true;
          new_rss = {true, ins, 0, 0, -1, -1, ins.imm, rob.get_empty()};
          int r_i = reg.get_dep(ins.rs1);
          if (r_i != -1) {
            if (rob.get_state(r_i) != Write)
              new_rss.qj = r_i;
            else
              new_rss.vj = rob.get_value(r_i);
          } else {
            new_rss.vj = reg[ins.rs1];
          }
        }
      } else if (ins.type == S) {
        push_rob = push_lsb = true;
        new_rob = {Issue, ins, static_cast<word>(-1), 0};
        new_lsb = {false, ins.code, 0, 0, -1, -1, ins.imm, rob.get_empty()};
        int r_i = reg.get_dep(ins.rs1);
        if (r_i != -1) {
          if (rob.get_state(r_i) != Write)
            new_lsb.qj = r_i;
          else
            new_lsb.vj = rob.get_value(r_i);
        } else {
          new_lsb.vj = reg[ins.rs1];
        }
        r_i = reg.get_dep(ins.rs2);
        if (r_i != -1) {
          if (rob.get_state(r_i) != Write)
            new_lsb.qk = r_i;
          else
            new_lsb.vk = rob.get_value(r_i);
        } else {
          new_lsb.vk = reg[ins.rs2];
        }
      } else if (ins.type == B) {
        push_rob = push_rss = true;
        new_rob = {Issue, ins, 0, 0};
        new_rss = {true, ins, 0, 0, -1, -1, ins.imm, rob.get_empty()};
        int r_i = reg.get_dep(ins.rs1);
        if (r_i != -1) {
          if (rob.get_state(r_i) != Write)
            new_rss.qj = r_i;
          else
            new_rss.vj = rob.get_value(r_i);
        } else {
          new_rss.vj = reg[ins.rs1];
        }
        r_i = reg.get_dep(ins.rs2);
        if (r_i != -1) {
          if (rob.get_state(r_i) != Write)
            new_rss.qk = r_i;
          else
            new_rss.vk = rob.get_value(r_i);
        } else {
          new_rss.vk = reg[ins.rs2];
        }
        new_instr_addr = pred.next_front(instr_addr, instr_addr + ins.imm);
        new_rob.value = pred.predict(instr_addr); // 1 for jump, 0 for not
        if (new_rob.value) new_rob.dest = instr_addr + 4;
        else new_rob.dest = instr_addr + ins.imm;
        // dest is the PC after a prediction fail
        return;
      }
      new_instr_addr = instr_addr + 4;
    }

    void Decoder::flush() {
      if (br_bus->get_type() == BusType::PC) {
        auto e = br_bus->read();
        auto f = *br_pc;
        if (e.first) {
          new_instr_addr = e.second;
          new_freeze = false;
        }
        if (new_instr_addr == f + 4) {
          pred.update(f, false, !e.first);
        } else {
          pred.update(f, true, !e.first);
        }
      }
      instr_addr = new_instr_addr;
      freeze = new_freeze;
    }

    std::ostream &operator<<(std::ostream &os, const Instruction &ins) {
      os << "Instruction{code: " << opCodeStr[ins.code] << ", imm: " << std::hex << ins.imm;
      if (ins.rd < 37 && ins.rd >= 0) os << ", rd: " << regStr[ins.rd];
      if (ins.rs1 < 37 && ins.rs1 >= 0) os << ", rs1: " << regStr[ins.rs1];
      if (ins.rs2 < 37 && ins.rs2 >= 0) os << ", rs2: " << regStr[ins.rs2];
      os << "}" << std::dec;
      return os;
    }

    void Decoder::execute(RegFile &reg,
                          ReorderBuffer &rob,
                          LoadStoreBuffer &lsb,
                          ReservationStation &rss) {
      new_freeze = false;
      for (auto &e: rob.rob) {
        if (e.ins.code == JALR) {
          new_freeze = true;
          new_instr_addr = e.value;
        }
      }
      if (freeze) {
        return;
      }
      if (rss.full() || rob.full() || lsb.full()) {
        return;
      }

      push_rss = false, push_lsb = false, push_rob = false;
      Instruction ins;
      word instr = fetch(lsb.mem);
      decode(instr, ins);
      parse(ins, reg, rob, lsb, rss);
      new_rob.addr = instr_addr;

#ifdef vis
      std::cout << ins << std::endl;
#endif
      auto e = cd_bus->read();
      auto f = mem_bus->read();
      if (push_rob) {
        if (new_rob.dest >= 0 && new_rob.dest < 32)
          reg.set_dep(new_rob.dest, rob.get_empty()); // set dependence
        rob.add(new_rob);
      }
      if (push_rss) {
        if (e.first != -1) {
          if (new_rss.qj == e.first) {
            new_rss.qj = -1;
            new_rss.vj = e.second;
          }
          if (new_rss.qk == e.first) {
            new_rss.qk = -1;
            new_rss.vk = e.second;
          }
        }
        if (f.first != -1) {
          if (new_rss.qj == f.first) {
            new_rss.qj = -1;
            new_rss.vj = f.second;
          }
          if (new_rss.qk == f.first) {
            new_rss.qk = -1;
            new_rss.vk = f.second;
          }
        }
        rss.add(new_rss);
      }
      if (push_lsb) {
        if (f.first != -1) {
          if (new_lsb.qj == f.first) {
            new_lsb.qj = -1;
            new_lsb.vj = f.second;
          }
          if (new_lsb.qk == f.first) {
            new_lsb.qk = -1;
            new_lsb.vk = f.second;
          }
        }
        if (e.first != -1) {
          if (new_lsb.qj == e.first) {
            new_lsb.qj = -1;
            new_lsb.vj = e.second;
          }
          if (new_lsb.qk == e.first) {
            new_lsb.qk = -1;
            new_lsb.vk = e.second;
          }
        }
        lsb.add(new_lsb);
      }
      // decoder has to query the mem_bus and cd_bus
    }

}

