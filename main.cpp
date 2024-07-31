#include "simulator.h"

int main() {
  arima::Simulator sim("../testcases/pi.data");
  sim.run();
  return 0;
}