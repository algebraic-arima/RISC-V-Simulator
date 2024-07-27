#include "memory.h"
#include <iostream>
#include <fstream>

namespace arima {
    Memory::Memory() {
      std::string str;
      word cur_addr;
      while (std::cin >> str) {
        if (str[0] == '@') {
          cur_addr = std::stoul(str, nullptr, 16);
        } else {
          ram[cur_addr++] = std::stoul(str, nullptr, 16);
        }
      }
    }

    Memory::Memory(const char *filename) {
      std::ifstream file(filename);
      if (!file.is_open()) {
        std::cerr << "Error: cannot open file " << filename << std::endl;
        Memory();
      }
      std::string str;
      word cur_addr;
      while (file >> str) {
        if (str[0] == '@') {
          cur_addr = std::stoul(str, nullptr, 16);
        } else {
          ram[cur_addr++] = std::stoul(str, nullptr, 16);
        }
      }
      file.close();
    }

    byte &Memory::operator[](word addr) {
      return ram[addr];
    }
}