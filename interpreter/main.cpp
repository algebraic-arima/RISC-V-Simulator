#include <iostream>
#include <fstream>
#include <csignal>

#define vis

typedef uint8_t byte;
typedef uint16_t hword;
typedef uint32_t word;

const int RAM_SIZE = 1 << 20;

const int REG_SIZE = 32;

enum opType {
  U,
  J,
  B,
  I,
  S,
  R,
};

enum opCode {
  // U type
  LUI,                       // load upper immediate
  AUIPC,                     // add upper immediate to pc
  // J type
  JAL,                       // jump and link
  // I type
  JALR,                      // jump and link register
  // B type
  BEQ,                       // branch equal
  BNE,                       // branch not equal
  BLT,                       // branch less than
  BGE,                       // branch greater or equal
  BLTU,                      // branch less than unsigned
  BGEU,                      // branch greater or equal unsigned
  // I type
  LB,                        // load byte
  LH,                        // load half word
  LW,                        // load word
  LBU,                       // load byte unsigned
  LHU,                       // load half word unsigned
  // S type
  SB,                        // store byte
  SH,                        // store half word
  SW,                        // store word
  // I type
  ADDI,                      // add immediate
  SLTI,                      // set less than immediate
  SLTIU,                     // set less than immediate unsigned
  XORI,                      // xor immediate
  ORI,                       // or immediate
  ANDI,                      // and immediate
  // I type
  SLLI,                      // shift left logical immediate
  SRLI,                      // shift right logical immediate
  SRAI,                      // shift right arithmetic immediate
  // R type
  ADD,                       // add
  SUB,                       // subtract
  SLL,                       // shift left logical
  SLT,                       // set less than
  SLTU,                      // set less than unsigned
  XOR,                       // xor
  SRL,                       // shift right logical
  SRA,                       // shift right arithmetic
  OR,                        // or
  AND,                       // and
};

word signExt(word input, int size) {
  word mask = (word) 1 << (size - 1);
  return (input ^ mask) - mask;
}

class RegFile {
  word reg[REG_SIZE]{};
public:
  RegFile() = default;

  word &operator[](std::size_t idx) {
    return reg[idx];
  }
};

struct Instruction {
  opType type{};
  opCode code{};
  word imm = -1;
  word rd = -1, rs1 = -1, rs2 = -1;
};

class State {
public:
  word pc = -1;
  RegFile reg;
  bool terminate = false;
};

class Memory {
  byte ram[RAM_SIZE]{};
public:

  Memory() {
    std::string str;
    word cur_addr;
    while (std::cin >> str) {
      if (str[0] == '@') {
        cur_addr = std::stoul(str.substr(1), nullptr, 16);
      } else {
        ram[cur_addr++] = std::stoul(str, nullptr, 16);
      }
    }
  }

  explicit Memory(const char *filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
      std::cerr << "Error: cannot open file " << filename << std::endl;
      Memory();
      return;
    }
    std::string str;
    word cur_addr;
    while (file >> str) {
      if (str[0] == '@') {
        cur_addr = std::stoul(str.substr(1), nullptr, 16);
      } else {
        ram[cur_addr++] = std::stoul(str, nullptr, 16);
      }
    }
    file.close();
  }

  byte &operator[](word addr) {
    return ram[addr];
  }

};

class MemoryController {
  Memory ram;
public:
  MemoryController() = default;

  explicit MemoryController(const char *filename) : ram(filename) {}

  byte &operator[](word addr) {
    return ram[addr];
  }

  byte ldb(word addr) {
    return ram[addr];
  }

  hword ldh(word addr) {
    return ram[addr] | (ram[addr + 1] << 8);
  }

  word ldw(word addr) {
    return ram[addr] | (ram[addr + 1] << 8) | (ram[addr + 2] << 16) | (ram[addr + 3] << 24);
  }

  void stb(word addr, byte data) {
    ram[addr] = data;
  }

  void sth(word addr, hword data) {
    ram[addr] = data & 0xFF;
    ram[addr + 1] = data >> 8;
  }

  void stw(word addr, word data) {
    ram[addr] = data & 0xFF;
    ram[addr + 1] = data >> 8 & 0xFF;
    ram[addr + 2] = data >> 16 & 0xFF;
    ram[addr + 3] = data >> 24;
  }

};

class Decoder {

public:

  static word fetch(State *st, MemoryController *mem) {
    return mem->ldw(st->pc);
  }

  static void decode_branch(word input, Instruction &ins) {
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

  static void decode_load(word input, Instruction &ins) {
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

  static void decode_store(word input, Instruction &ins) {
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

  static void decode_arithi(word input, Instruction &ins) {
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

  static void decode_arithr(word input, Instruction &ins) {
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

  static void decode(word input, Instruction &ins) {
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

};

class ALU {
public:
  static void execute(const Instruction &op, State *state, MemoryController *mem) {
    switch (op.code) {
      case opCode::ADD:
        state->reg[op.rd] = state->reg[op.rs1] + state->reg[op.rs2];
        return;
      case opCode::SUB:
        state->reg[op.rd] = state->reg[op.rs1] - state->reg[op.rs2];
        return;
      case opCode::SLL:
        state->reg[op.rd] = state->reg[op.rs1] << (state->reg[op.rs2] & 0b11111);
        return;
      case opCode::SLTU:
        state->reg[op.rd] = (word) state->reg[op.rs1] < (word) state->reg[op.rs2];
        return;
      case opCode::SLT:
        state->reg[op.rd] = (int) state->reg[op.rs1] < (int) state->reg[op.rs2];
        return;
      case opCode::XOR:
        state->reg[op.rd] = state->reg[op.rs1] ^ state->reg[op.rs2];
        return;
      case opCode::SRL:
        state->reg[op.rd] = state->reg[op.rs1] >> (state->reg[op.rs2] & 0b11111);
        return;
      case opCode::SRA:
        state->reg[op.rd] = (int) state->reg[op.rs1] >> (state->reg[op.rs2] & 0b11111);
        return;
      case opCode::OR:
        state->reg[op.rd] = state->reg[op.rs1] | state->reg[op.rs2];
        return;
      case opCode::AND:
        state->reg[op.rd] = state->reg[op.rs1] & state->reg[op.rs2];
        return;
      case opCode::ADDI:
        if (op.rd == 10 && op.rs1 == 0 && op.imm == 255) {
          state->terminate = true;
          return;
        }
        state->reg[op.rd] = state->reg[op.rs1] + op.imm;
        return;
      case opCode::SLTI:
        state->reg[op.rd] = (int) state->reg[op.rs1] < (int) op.imm;
        return;
      case opCode::SLTIU:
        state->reg[op.rd] = (word) state->reg[op.rs1] < (word) op.imm;
        return;
      case opCode::XORI:
        state->reg[op.rd] = state->reg[op.rs1] ^ op.imm;
        return;
      case opCode::ORI:
        state->reg[op.rd] = state->reg[op.rs1] | op.imm;
        return;
      case opCode::ANDI:
        state->reg[op.rd] = state->reg[op.rs1] & op.imm;
        return;
      case opCode::SLLI:
        state->reg[op.rd] = state->reg[op.rs1] << (op.imm & 0b11111);
        return;
      case opCode::SRLI:
        state->reg[op.rd] = state->reg[op.rs1] >> (op.imm & 0b11111);
        return;
      case opCode::SRAI:
        state->reg[op.rd] = (int) state->reg[op.rs1] >> (op.imm & 0b11111);
        return;
      case opCode::JALR:
        state->reg[op.rd] = state->pc + 4;
        state->pc = state->reg[op.rs1] + op.imm - 4;
        return;
      case opCode::JAL:
        state->reg[op.rd] = state->pc + 4;
        state->pc += op.imm - 4;
        return;
      case opCode::BEQ:
        if (state->reg[op.rs1] == state->reg[op.rs2]) {
          state->pc += op.imm - 4;
        }
        return;
      case opCode::BNE:
        if (state->reg[op.rs1] != state->reg[op.rs2]) {
          state->pc += op.imm - 4;
        }
        return;
      case opCode::BLT:
        if ((int) state->reg[op.rs1] < (int) state->reg[op.rs2]) {
          state->pc += op.imm - 4;
        }
        return;
      case opCode::BGE:
        if ((int) state->reg[op.rs1] >= (int) state->reg[op.rs2]) {
          state->pc += op.imm - 4;
        }
        return;
      case opCode::BLTU:
        if ((word) state->reg[op.rs1] < (word) state->reg[op.rs2]) {
          state->pc += op.imm - 4;
        }
        return;
      case opCode::BGEU:
        if ((word) state->reg[op.rs1] >= (word) state->reg[op.rs2]) {
          state->pc += op.imm - 4;
        }
        return;
      case opCode::AUIPC:
        state->reg[op.rd] = state->pc + op.imm;
        return;
      case opCode::LUI:
        state->reg[op.rd] = op.imm;
        return;
      case opCode::LB:
        state->reg[op.rd] = signExt(mem->ldb(state->reg[op.rs1] + op.imm), 8);
        return;
      case opCode::LH:
        state->reg[op.rd] = signExt(mem->ldh(state->reg[op.rs1] + op.imm), 16);
        return;
      case opCode::LW:
        state->reg[op.rd] = mem->ldw(state->reg[op.rs1] + op.imm);
        return;
      case opCode::LBU:
        state->reg[op.rd] = mem->ldb(state->reg[op.rs1] + op.imm);
        return;
      case opCode::LHU:
        state->reg[op.rd] = mem->ldh(state->reg[op.rs1] + op.imm);
        return;
      case opCode::SB:
        mem->stb(state->reg[op.rs1] + op.imm, state->reg[op.rs2] & 0xFF);
        return;
      case opCode::SH:
        mem->sth(state->reg[op.rs1] + op.imm, state->reg[op.rs2] & 0xFFFF);
        return;
      case opCode::SW:
        mem->stw(state->reg[op.rs1] + op.imm, state->reg[op.rs2]);
        return;
      default:
        return;
    }
  }
};

class Simulator {
  State state;
  MemoryController mem;
  Decoder decoder;
  ALU alu;
public:
  Simulator() = default;

  explicit Simulator(const char *filename) : mem(filename) {}

  void display() {
    std::cout << "pc = " << std::hex << state.pc << std::endl;
    for (int i = 0; i < 32; i++) {
      std::cout << "reg[" << i << "] = " << state.reg[i] << "\n";
    }
    std::cout << std::endl;

  }

  void run() {
    state.pc = 0;
    while (true) {
      word ins = Decoder::fetch(&state, &mem);
      Instruction decoded;
      Decoder::decode(ins, decoded);
      ALU::execute(decoded, &state, &mem);
      state.reg[0] = 0;
      if (state.terminate) {
        std::cout << (state.reg[10] & 0xff) << std::endl;
        break;
      }
#ifdef vis
      display();
#endif
      state.pc += 4;
#ifdef vis
      sleep(1);
#endif
    }
  }

};

int main() {
  Simulator s;
  s.run();
  return 0;
}
