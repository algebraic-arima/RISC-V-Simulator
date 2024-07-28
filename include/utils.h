#ifndef RISCV_UTILS_H
#define RISCV_UTILS_H

#include "param.h"

namespace arima {
    word signExt(word input, int size) {
      word mask = 1 << (size - 1);
      return (input ^ mask) - mask;
    }

    template<class T, int size>
    class arr {
    public:
      T data[size]{};

      arr() = default;

      T &operator[](int index) {
        return data[index];
      }

      T operator[](int index) const {
        return data[index];
      }

      arr &operator=(const arr &other) {
        if (this == &other) return *this;
        for (int i = 0; i < size; i++) {
          data[i] = other.data[i];
        }
        return *this;
      }

      arr(const arr &other) {
        for (int i = 0; i < size; i++) {
          data[i] = other.data[i];
        }
      }

      struct iterator {
        int index;
        T *data;

        iterator(int index, T *data) : index(index), data(data) {}

        T &operator*() {
          return data[index];
        }

        iterator &operator++() {
          index++;
          return *this;
        }

        bool operator!=(const iterator &other) {
          return index != other.index || data != other.data;
        }

        bool operator==(const iterator &other) {
          return index == other.index && data == other.data;
        }
      };

      iterator begin() {
        return iterator(0, data);
      }

      iterator end() {
        return iterator(size, data);
      }
    };

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

      cir_queue &operator=(const cir_queue &other) {
        if (this == &other) return *this;
        head = other.head;
        tail = other.tail;
        for (int i = 0; i < size; i++) {
          data[i] = other.data[i];
        }
        return *this;
      }

      cir_queue(const cir_queue &other) {
        head = other.head;
        tail = other.tail;
        data = new T[size];
        for (int i = 0; i < size; i++) {
          data[i] = other.data[i];
        }
      }

      void push(T val) {
        data[tail] = val;
        tail = (tail + 1) % size;
      }

      T &front() {
        return data[head];
      }

      int get_front() const {
        return head;
      }

      int get_tail() const {
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
