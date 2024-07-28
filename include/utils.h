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

      int get_front() const{
        return head;
      }

      int get_tail() const{
        return tail;
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

      bool full() const {
        return (tail + 1) % size == head;
      }

      struct iterator {
        int index;
        T *data;

        iterator(int index, T *data) : index(index), data(data) {}

        T &operator*() {
          return data[index];
        }

        iterator &operator++() {
          index = (index + 1) % size;
          return *this;
        }

        bool operator!=(const iterator &other) {
          return index != other.index;
        }

        bool operator==(const iterator &other) {
          return index == other.index;
        }


      };

      iterator begin() {
        return iterator(head, data);
      }

      iterator end() {
        return iterator(tail, data);
      }

    };


}

#endif //RISCV_UTILS_H
