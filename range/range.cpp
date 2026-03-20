#include "../range.h"

#include <vector>
#include <cinttypes>

Iterator::Iterator(int64_t begin, int64_t end, int64_t step): beginIt(begin), endIt(end), step(step) {
  if (step > 0) {
    beginIt = std::min(beginIt, endIt);
  } else {
    beginIt = std::max(beginIt, endIt);
  }
}

Iterator Iterator::begin() const { //NOLINT
  if (beginIt == endIt) {
    return {0, 0, 0};
  }
  return {beginIt, endIt, step};
}

Iterator Iterator::end() const { //NOLINT
  if (beginIt == endIt) {
    return {0, 0, 0};
  }
  return {endIt, endIt, step};
}

Iterator Iterator::rbegin() const{ //NOLINT
  if (beginIt == endIt) {
    return {0, 0, 0};
  }
  return {(endIt - beginIt - (endIt > beginIt ? 1 : -1)) / step * step + beginIt, beginIt - step, -step};
}

Iterator Iterator::rend() const { //NOLINT
  if (beginIt == endIt) {
    return {0, 0, 0};
  }
  return {beginIt - step, beginIt - step, -step};
}

Iterator &Iterator::operator++() {
  beginIt += step;
  if (step > 0) {
    beginIt = std::min(beginIt, endIt);
  } else {
    beginIt = std::max(beginIt, endIt);
  }
  return *this;
}

int64_t Iterator::operator*() const {
  return beginIt;
}

bool Iterator::operator!=(const Iterator &other) const{
  return beginIt != other.endIt;
}


Iterator Range(int64_t end) {
  if (end > static_cast<int64_t>(1e8)) {
    return {0, 0, 1};
  }
  return {0, end, 1};
}

Iterator Range(int64_t begin, int64_t end, int64_t step) {
  if (step == 0 || begin == end || (end - begin) / step > static_cast<int64_t>(1e8)) {
    return {0, 0, 1};
  }
  return {begin, end, step};
}

