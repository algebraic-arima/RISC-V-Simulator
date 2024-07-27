#ifndef RISCV_UTILS_H
#define RISCV_UTILS_H

#include "param.h"

namespace arima {
    word signExt(word input, int size) {
      word mask = 1 << (size - 1);
      return (input ^ mask) - mask;
    }

    template<class T, int size>
    class cir_queue {
    public:
      T *data;
      int head, tail;

      cir_queue() {
        data = new T[size];
        head = tail = 0;
      }

      ~cir_queue() {
        delete[] data;
      }

      void push(T val) {
        data[tail] = val;
        tail = (tail + 1) % size;
      }

      T &front() {
        return data[head];
      }

      T &back() {
        return data[(tail - 1 + size) % size];
      }

      T &operator[](int index) {
        return data[(head + index) % size];
      }

      T operator[](int index) const {
        return data[(head + index) % size];
      }

      T pop() {
        T val = data[head];
        head = (head + 1) % size;
        return val;
      }

      bool empty() {
        return head == tail;
      }

      bool full() {
        return (tail + 1) % size == head;
      }
    };
}

#endif //RISCV_UTILS_H
