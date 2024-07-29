#include "utils.h"

namespace arima{
    word signExt(word input, int size) {
      word mask = 1 << (size - 1);
      return (input ^ mask) - mask;
    }
}