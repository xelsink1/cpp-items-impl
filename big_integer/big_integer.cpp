#include "../big_integer.h"

#include <iostream>
#include <cstring>
#include <ostream>

BigInteger::BigInteger() : negative(false), arr{} { }

BigInteger::BigInteger(const char *str) : negative(false), arr{} {
  const size_t len_str = strlen(str);
  const size_t len = (len_str + 3) / 4;
  if (len > kN) {
    throw BigIntegerOverflow{};
  }
  for (size_t i = len_str - 1, _ = 0; _ < len; _++, i -= 4) {
    for (size_t j = (i < 3 ? 0 : i - 3); j <= i; ++j) {
      if (str[j] == '-' || str[j] == '+') {
        negative = (str[j] == '-');
      } else {
        arr[(len_str - j - 1) / 4] = arr[(len_str - j - 1) / 4] * 10 + static_cast<DigitType>(str[j] - '0');
      }
    }
  }
}


bool BigInteger::IsNegative() const {
  return negative;
}

void BigInteger::ChangeSign() {
  negative = !negative;
}

BigInteger BigInteger::operator+() const {
  return *this;
}

BigInteger BigInteger::operator-() const {
  auto x = *this;
  x.negative = !x.IsNegative();
  return x;
}

BigInteger &BigInteger::operator+=(const BigInteger &x) {
  if (negative == x.IsNegative()) {
    DigitType ex = 0;
    for (size_t i = 0; i < kN; ++i) {
      DoubleDigitType res = static_cast<DoubleDigitType>(arr[i]) + x.arr[i] + ex;
      ex = res / kDigit;
      arr[i] = res % kDigit;
    }
    if (ex != 0) {
      throw BigIntegerOverflow{};
    }
    return *this;
  }
  if (x.IsNegative()) {
    return *this -= -x;
  }
  this->ChangeSign();
  *this -= x;
  if (*this != 0) {
    this->ChangeSign();
  }
  return *this;
}

BigInteger &BigInteger::operator-=(const BigInteger &x) {
  if (negative == x.IsNegative()) {
    if (negative) {
      this->ChangeSign();
      *this -= -x;
      if (*this != 0) {
        this->ChangeSign();
      }
      return *this;
    }
    if (x > *this) {
      auto x1 = x;
      x1 -= *this;
      *this = -x1;
      return *this;
    }
    DigitType ex = 0;
    for (size_t i = 0; i < kN; ++i) {
      DoubleDigitType res = 0;
      if (static_cast<DoubleDigitType>(arr[i]) < x.arr[i] + ex) {
        res = static_cast<DoubleDigitType>(kDigit) + arr[i] - x.arr[i] - ex;
        ex = 1;
      } else {
        res = arr[i] - x.arr[i] - ex;
        ex = 0;
      }
      arr[i] = res % kDigit;
    }
    return *this;
  }
  *this += -x;
  return *this;
}

BigInteger &BigInteger::operator++() {
  *this += 1;
  return *this;
}

BigInteger BigInteger::operator++(int) {
  const auto old = *this;
  ++(*this);
  return old;
}

BigInteger &BigInteger::operator--() {
  *this -= 1;
  return *this;
}

BigInteger BigInteger::operator--(int) {
  const auto old = *this;
  --(*this);
  return old;
}

BigInteger::operator bool() const {
  return *this != 0;
}

BigInteger &BigInteger::operator*=(const BigInteger &x) {
  BigInteger res = 0;
  for (size_t i = 0; i < kN; ++i) {
    if (arr[i] == 0) {
      continue;
    }
    BigInteger cur = 0;
    DigitType ex = 0;
    for (size_t j = 0; j + i < kN; ++j) {
      DoubleDigitType add = static_cast<DoubleDigitType>(arr[i]) * x.arr[j] + ex;
      ex = add / kDigit;
      cur.arr[j + i] = add % kDigit;
    }
    if (ex != 0) {
      throw BigIntegerOverflow{};
    }
    res += cur;
  }
  if (res != 0) {
    res.negative = this->negative != x.negative;
  }
  *this = res;
  return *this;
}

BigInteger &BigInteger::operator/=(const BigInteger &x) {
  if (x == BigInteger(0)) {
    throw BigIntegerDivisionByZero{};
  }
  BigInteger res = 0;
  BigInteger d = x;
  d.negative = false;
  bool f1 = this->negative;
  this->negative = false;
  while (*this != 0) {
    int l = 0;
    int r = 30001;
    while (r - l > 1) {
      int mid = (r + l) / 2;
      if (d * BigInteger(("1" + std::string(mid, '0')).c_str()) <= *this) {
        l = mid;
      } else {
        r = mid;
      }
    }
    if (l > 0 || d <= *this) {
      int j = 1;
      auto buf = BigInteger(("1" + std::string(l, '0')).c_str());
      while ((j + 1) * d * buf <= *this) {
        ++j;
      }
      res += j * buf;
      *this -= j * buf * d;
    } else {
      break;
    }
  }
  *this = res;
  if (*this != 0 && (f1 != x.negative)) {
    this->negative = true;
  }
  return *this;
}

BigInteger &BigInteger::operator%=(const BigInteger &x) {
  *this = *this - *this / x * x;
  return *this;
}

bool operator>(const BigInteger &x, const BigInteger &y) {
  if (x.IsNegative() && !y.IsNegative()) {
    return false;
  }
  if (!x.IsNegative() && y.IsNegative()) {
    return true;
  }
  for (size_t i = BigInteger::kN - 1, _ = 0; _ < BigInteger::kN; --i, ++_) {
    if (x.arr[i] > y.arr[i]) {
      return !x.IsNegative();
    }
    if (x.arr[i] < y.arr[i]) {
      return x.IsNegative();
    }
  }
  return false;
}

bool operator==(const BigInteger &x, const BigInteger &y) {
  if (x.IsNegative() != y.IsNegative()) {
    return false;
  }
  for (size_t i = 0; i < BigInteger::kN; i++) {
    if (x.arr[i] != y.arr[i]) {
      return false;
    }
  }
  return true;
}

bool operator!=(const BigInteger &x, const BigInteger &y) {
  return !(x == y);
}

bool operator>=(const BigInteger &x, const BigInteger &y) {
  return x > y || x == y;
}

bool operator<(const BigInteger &x, const BigInteger &y) {
  return !(x >= y);
}

bool operator<=(const BigInteger &x, const BigInteger &y) {
  return x < y || x == y;
}

BigInteger operator+(const BigInteger &x, const BigInteger &y) {
  auto x1 = x;
  x1 += y;
  return x1;
}

BigInteger operator-(const BigInteger &x, const BigInteger &y) {
  auto x1 = x;
  x1 -= y;
  return x1;
}

BigInteger operator*(const BigInteger &x, const BigInteger &y) {
  auto x1 = x;
  x1 *= y;
  return x1;
}

BigInteger operator/(const BigInteger &x, const BigInteger &y) {
  auto x1 = x;
  x1 /= y;
  return x1;
}

BigInteger operator%(const BigInteger &x, const BigInteger &y) {
  auto x1 = x;
  x1 %= y;
  return x1;
}

std::istream &operator>>(std::istream &in, BigInteger &x) {
  std::string s;
  in >> s;
  x = BigInteger(s.c_str());
  return in;
}

std::ostream &operator<<(std::ostream &out, BigInteger x) {
  if (x.negative) {
    out << '-';
  }
  bool flag = false;
  for (size_t i = 0; i < BigInteger::kN; ++i) {
    DigitType output = x.arr[BigInteger::kN - i - 1];
    if (1000 > output && flag) {
      out << '0';
    }
    if (100 > output && flag) {
      out << '0';
    }
    if (10 > output && flag) {
      out << '0';
    }
    if (output > 0 || i == BigInteger::kN - 1) {
      flag = true;
    }
    if (flag) {
      out << output;
    }
  }
  return out;
}