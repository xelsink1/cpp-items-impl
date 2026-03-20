#ifndef RANGE_H
#define RANGE_H
#define REVERSE_RANGE_IMPLEMENTED

#include <vector>
#include <cinttypes>
#include <iostream>

class Iterator {
public:
  int64_t beginIt;
  int64_t endIt;
  int64_t step;

  Iterator(int64_t, int64_t, int64_t);

  Iterator &operator++();

  Iterator begin() const; //NOLINT

  Iterator end() const; //NOLINT

  Iterator rbegin() const; //NOLINT

  Iterator rend() const; //NOLINT

  bool operator!=(const Iterator&) const;

  int64_t operator*() const;
};

Iterator Range(int64_t);

Iterator Range(int64_t, int64_t, int64_t = 1);

#endif