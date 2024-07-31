#include "simulator.h"

int main() {
  arima::Simulator sim("../testcases/hanoi.data");
  sim.run();
  return 0;
}