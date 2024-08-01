#include "simulator.h"

int main() {
  arima::Simulator sim("../testcases/bulgarian.data");
  sim.run();
  return 0;
}