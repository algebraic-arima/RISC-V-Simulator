#include "pred.h"

//#define adv

namespace arima {
    int bias = 3; // -bias is the initial value

    inline word hash(word pos) {
      return pos & 0x0000003f;
    }



    word Predictor::next_front(word front, word imm) {
      return (predict(hash(front))) ? imm : front + 4;
    }

    double Predictor::get_succ_rate() const {
      return (double) correct_num / (double) branch_num;
    }

#ifndef adv
    bool Predictor::predict(word pos) {
      return counter[hash(pos)] >= bias + 2; // jump or not
    }

    void Predictor::update(word pos, bool jump, bool correct) {
      if (jump) {
        histoire[hash(pos)] += "1";
        if (counter[hash(pos)] < bias + 3) {
          ++counter[hash(pos)];
        }
      } else {
        histoire[hash(pos)] += "0";
        if (counter[hash(pos)] > bias) {
          counter[hash(pos)]--;
        }
      }
      if (correct) correct_num++;
      branch_num++;
    }
#endif

#ifdef adv
    bool Predictor::predict(word pc) {
      word f = counter[hash(pc)];
      byte &p = hist[hash(pc)];
      byte pred_hist = p & 0x0f;
      bool pred = f & (1 << ((pred_hist << 1) | 1));
      // the higher 1 bit determines the prediction
      p = (p & 0xf0) | (((pred_hist << 1) | (pred ? 0x01 : 0x00)) & 0x0f);
      // update the prediction history
      return pred;
    }

    void Predictor::update(word pc, bool jump, bool correct) {
      word &f = counter[hash(pc)];
      byte &p = hist[hash(pc)];

      // add into the counter
      int hist = p >> 4;
      bool high = f & (1 << (hist << 1 | 1));
      bool low = f & (1 << (hist << 1));
      if (jump) {
        if (high) f |= 1 << (hist << 1);
        else {
          if (low) f ^= (3 << (hist << 1));
          else f ^= (1 << (hist << 1));
        }
      } else {
        if (!high) f &= ~(1 << (hist << 1));
        else {
          if (!low) f ^= (3 << (hist << 1));
          else f ^= (1 << (hist << 1));
        }
      }

      p = (p & 0x0f) | ((p & 0xf0) << 1 | (jump ? 0x10 : 0x00));
      // take down the new taken history
      if (correct) correct_num++;
      else p = (p & 0xf0) | ((p & 0xf0) >> 4);
      // if incorrect, replace the prediction history with the taken history
      branch_num++;
    }
#endif
}