#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H
#define BIG_INTEGER_DIVISION_IMPLEMENTED

#include <array>
#include <iostream>
#include <stdexcept>
#include <cinttypes>

class BigIntegerOverflow : public std::runtime_error {
public:
  BigIntegerOverflow() : std::runtime_error("BigIntegerOverflow") {
  }
};

class BigIntegerDivisionByZero : public std::runtime_error {
public:
  BigIntegerDivisionByZero() : std::runtime_error("BigIntegerDivisionByZero") {
  }
};

using DigitType = uint16_t;
using DoubleDigitType = uint32_t;

class BigInteger {

public:
  static constexpr size_t kN = 7500 + 1;
  static constexpr DigitType kDigit = 10000;

  bool negative;

  std::array<DigitType, kN> arr;


  BigInteger();

  explicit BigInteger(const char *);

  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInteger(T value) : negative(false), arr{} { //NOLINT
    if (value < 0) {
      negative = true;
      value = -value;
    }
    for (size_t i = 0; i < kN && value != 0; ++i, value /= kDigit) {
      arr[i] = value % kDigit;
    }
  }

  bool IsNegative() const;

  void ChangeSign();

  BigInteger operator+() const;

  BigInteger operator-() const;

  BigInteger &operator+=(const BigInteger &);

  BigInteger &operator-=(const BigInteger &);

  BigInteger &operator*=(const BigInteger &);

  BigInteger &operator/=(const BigInteger &);

  BigInteger &operator%=(const BigInteger &);

  BigInteger &operator++();

  BigInteger operator++(int);

  BigInteger &operator--();

  BigInteger operator--(int);

  explicit operator bool() const;

  friend std::istream& operator>>(std::istream &, BigInteger &);

  friend std::ostream& operator<<(std::ostream &, BigInteger);
};

BigInteger operator+(const BigInteger &, const BigInteger &);

BigInteger operator-(const BigInteger &, const BigInteger &);

BigInteger operator*(const BigInteger &, const BigInteger &);

BigInteger operator/(const BigInteger &, const BigInteger &);

BigInteger operator%(const BigInteger &, const BigInteger &);

bool operator>(const BigInteger &, const BigInteger &);

bool operator<(const BigInteger &, const BigInteger &);

bool operator>=(const BigInteger &, const BigInteger &);

bool operator<=(const BigInteger &, const BigInteger &);

bool operator==(const BigInteger &, const BigInteger &);

bool operator!=(const BigInteger &, const BigInteger &);

#endif