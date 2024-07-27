#ifndef RISCV_MEM_HPP
#define RISCV_MEM_HPP

#include "param.h"

namespace arima {
    class Memory {
      byte ram[RAM_SIZE]{};
    public:
      Memory();

      explicit Memory(const char *);

      byte &operator[](word);

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

}

#endif //RISCV_MEM_HPP
