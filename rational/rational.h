#ifndef RATIONAL_H
#define RATIONAL_H

#include <stdexcept>

class RationalDivisionByZero : public std::runtime_error {
public:
  RationalDivisionByZero() : std::runtime_error("RationalDivisionByZero") {
  }
};

class Rational {
  int numerator_;
  int denominator_;

public:
  Rational operator-() const;

  Rational operator+() const;

  Rational& operator++();

  Rational& operator--();

  Rational operator++(int);

  Rational operator--(int);

  Rational& operator=(const Rational&);

  Rational& operator+=(const Rational&);

  Rational& operator-=(const Rational&);

  Rational& operator*=(const Rational&);

  Rational& operator/=(const Rational&);

  void SetNumerator(int);

  void SetDenominator(int);

  int GetNumerator() const;

  int GetDenominator() const;

  friend std::istream& operator>>(std::istream&, Rational&);

  friend std::ostream& operator<<(std::ostream&, Rational);

  void Reduction();

  Rational(const Rational&);

  Rational();

  Rational(int); //NOLINT

  Rational(int, int);
};

Rational operator+(const Rational&, const Rational&);

Rational operator-(const Rational&, const Rational&);

Rational operator*(const Rational&, const Rational&);

Rational operator/(const Rational&, const Rational&);

bool operator>(const Rational&, const Rational&);

bool operator<(const Rational&, const Rational&);

bool operator>=(const Rational&, const Rational&);

bool operator<=(const Rational&, const Rational&);

bool operator==(const Rational& a, const Rational& b);

#endif // RATIONAL_H
