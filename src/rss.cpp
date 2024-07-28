#include "rss.h"

namespace arima {
    int ReservationStation::find_empty() {
      for (int i = 0; i < RS_SIZE; i++)
        if (!rss[i].busy) return i;
      return -1;
    }

    bool ReservationStation::full() {
      for (int i = 0; i < RS_SIZE; i++)
        if (!rss[i].busy) return false;
      return true;
    }

    void ReservationStation::add(const RssEntry &entry) {
      int idx = find_empty();
      rss[idx] = entry;
    }

    void ReservationStation::flush() {
      rss = new_rss;
    }

    void ReservationStation::update() {
      auto e = cd_bus->read();
      auto f = mem_bus->read();
      for (int i = 0; i < RS_SIZE; i++) {
        if (rss[i].qj == e.first) {
          rss[i].qj = -1;
          rss[i].vj = e.second;
        } else if (rss[i].qj == f.first) {
          rss[i].qj = -1;
          rss[i].vj = f.second;
        }
        if (rss[i].qk == e.first) {
          rss[i].qk = -1;
          rss[i].vk = e.second;
        } else if (rss[i].qk == f.first) {
          rss[i].qk = -1;
          rss[i].vk = f.second;
        }
      }
    }

    void ReservationStation::execute(Decoder &dec, RegFile &reg, LoadStoreBuffer &lsb) {
      update();
      for (int i = 0; i < RS_SIZE; i++) {
        if (rss[i].busy) {
          if (rss[i].qj == -1 && rss[i].qk == -1) {
            rss[i].busy = false;
            // todo: make ALU compute and then write to cdb
            // pay attention to the data structure
            break;// complete a calculation in one cycle
          }
        }
      }
    }

}