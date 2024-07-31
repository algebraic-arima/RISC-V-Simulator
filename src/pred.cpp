#include "pred.h"

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
        correct_num++;
      } else {
        if (counter > 0) {
          counter--;
        }
      }
      branch_num++;
    }

    double Predictor::get_succ_rate() const {
      return (double) correct_num / (double) branch_num;
      return 0;
    }
}