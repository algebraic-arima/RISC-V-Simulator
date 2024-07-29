#include "decoder.h"

namespace arima {

    bool Predictor::predict() const {
      return counter >= 2; // jump or not
    }

    word Predictor::next_front(word front, word imm) const {
      return (predict()) ? imm : front + 4;
    }

    void Predictor::update(bool correct) {
      if (correct) {
        if (counter < 3) {
          counter++;
        }
      } else {
        if (counter > 0) {
          counter--;
        }
      }
      branch_num++;
      if (correct) {
        correct_num++;
      }
    }

    int Predictor::get_mispredict_rate() const {
      return (branch_num - correct_num) * 100 / branch_num;
    }

    word Decoder::fetch(MemoryController &mem) {
      return mem.ldw(instrAddr);
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
      if (rss.full() || rob.full() || lsb.full()) {
        return;
      }
      if (ins.code == AUIPC) {
        push_rob = true;
        new_rob = {Issue, ins, ins.rd, static_cast<int>((instrAddr + ins.imm))};
      } else if (ins.code == opCode::LUI) {
        push_rob = true;
        new_rob = {Write, ins, ins.rd, ins.imm};
      } else if (ins.code == JAL) {
        push_rob = true;
        new_rob = {Write, ins, ins.rd, static_cast<int>(instrAddr + 4)};
        new_instrAddr = pred.next_front(instrAddr, instrAddr + ins.imm);
        return;
      } else if (ins.code == JALR) {
        push_rss = push_rob = true;
        new_rob = {Issue, ins, ins.rd, static_cast<int>(instrAddr + 4)};
        new_rss = {true, ins, 0, ins.imm, -1, -1, 0, rob.get_empty()};
        if (int r_i = reg.get_dep(ins.rs1) != -1) {
          if (rob.get_state(r_i) != Write)
            new_rss.qj = r_i;
          else new_rss.vj = rob.get_value(r_i);
        } else {
          new_rss.vj = reg[ins.rs1];
        }
        new_freeze = true;
      } else if (ins.type == R) {
        push_rss = push_rob = true;
        new_rob = {Issue, ins, ins.rd, 0};
        new_rss = {true, ins, 0, 0, -1, -1, 0, rob.get_empty()};
        if (int r_i = reg.get_dep(ins.rs1) != -1) {
          if (rob.get_state(r_i) != Write)
            new_rss.qj = r_i;
          else new_rss.vj = rob.get_value(r_i);
        } else {
          new_rss.vj = reg[ins.rs1];
        }
        if (int r_i = reg.get_dep(ins.rs2) != -1) {
          if (rob.get_state(r_i) != Write)
            new_rss.qk = r_i;
          else new_rss.vk = rob.get_value(r_i);
        } else {
          new_rss.vk = reg[ins.rs2];
        }
      } else if (ins.type == I) {
        push_rob = true;
        new_rob = {Issue, ins, ins.rd, 0};
        if (ins.code == LB || ins.code == LH || ins.code == LW || ins.code == LBU || ins.code == LHU) {
          push_lsb = true;
          new_lsb = {true, ins.code, 0, 0, -1, -1, ins.imm, rob.get_empty()};
          if (int r_i = reg.get_dep(ins.rs1) != -1) {
            if (rob.get_state(r_i) != Write)
              new_lsb.qj = r_i;
            else new_lsb.vj = rob.get_value(r_i);
          } else {
            new_lsb.vj = reg[ins.rs1];
          }
        } else {
          push_rss = true;
          new_rss = {true, ins, 0, 0, -1, -1, ins.imm, rob.get_empty()};
          if (int r_i = reg.get_dep(ins.rs1) != -1) {
            if (rob.get_state(r_i) != Write)
              new_rss.qj = r_i;
            else new_rss.vj = rob.get_value(r_i);
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
          new_lsb.qj = r_i;
        } else {
          new_lsb.vj = reg[ins.rs1];
        }
        r_i = reg.get_dep(ins.rs2);
        if (r_i != -1) {
          new_lsb.qk = r_i;
        } else {
          new_lsb.vk = reg[ins.rs2];
        }
      } else if (ins.type == B) {
        push_rob = push_rss = true;
        new_rob = {Issue, ins, static_cast<word>(-1), 0};
        new_rss = {true, ins, 0, 0, -1, -1, ins.imm, rob.get_empty()};
        if (int r_i = reg.get_dep(ins.rs1) != -1) {
          new_rss.qj = r_i;
        } else {
          new_rss.vj = reg[ins.rs1];
        }
        if (int r_i = reg.get_dep(ins.rs2) != -1) {
          new_rss.qk = r_i;
        } else {
          new_rss.vk = reg[ins.rs2];
        }
        new_instrAddr = pred.next_front(instrAddr, instrAddr + ins.imm);
        new_rob.value = pred.predict();// 1 for jump, 0 for not
        return;
      }
      new_instrAddr = instrAddr + 4;
    }

    void Decoder::flush() {
      instrAddr = new_instrAddr;
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
      for (auto &e: rob.rob) {
        if (e.ins.code == JALR) {
          new_freeze = false;
        }
      }
      if (freeze) {
        return;
      }

      push_rss = false, push_lsb = false, push_rob = false;
      // bug: decoder has to query the mem_bus and cd_bus

      Instruction ins;
      word instr = fetch(lsb.mem);
      decode(instr, ins);
      parse(ins, reg, rob, lsb, rss);
      std::cout << ins << std::endl;
      if (push_rob) {
        if (new_rob.dest >= 0 && new_rob.dest < 32)
          reg.set_dep(new_rob.dest, rob.get_empty()); // set dependence
        rob.add(new_rob);
      }
      if (push_rss) {
        rss.add(new_rss);
      }
      if (push_lsb) {
        lsb.add(new_lsb);
      }

    }

    void Decoder::melt() {
      freeze = false;
    }


}

