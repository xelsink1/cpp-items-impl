#include <complex>
#include "rational.h"


int Gcd(const int x, const int y) {
  return (y == 0 ? x : Gcd(y, x % y));
}

void Rational::Reduction() {
  if (denominator_ == 0) {
    throw RationalDivisionByZero{};
  }
  if (denominator_ < 0) {
    numerator_ = -numerator_;
    denominator_ = -denominator_;
  }
  if (const int g = Gcd(std::abs(numerator_), denominator_); g != 1) {
    numerator_ /= g;
    denominator_ /= g;
  }
}

Rational Rational::operator+() const {
  return {numerator_, denominator_};
}

Rational Rational::operator-() const {
  return {-numerator_, denominator_};
}

Rational& Rational::operator+=(const Rational& other) {
  numerator_ = numerator_ * other.GetDenominator() + denominator_ * other.GetNumerator();
  denominator_ = denominator_ * other.GetDenominator();
  Reduction();
  return *this;
}

Rational& Rational::operator-=(const Rational& other) {
  numerator_ = numerator_ * other.GetDenominator() - denominator_ * other.GetNumerator();
  denominator_ = denominator_ * other.GetDenominator();
  Reduction();
  return *this;
}

Rational& Rational::operator*=(const Rational& other) {
  numerator_ = numerator_ * other.GetNumerator();
  denominator_ = denominator_ * other.GetDenominator();
  Reduction();
  return *this;
}

Rational& Rational::operator/=(const Rational& other) {
  numerator_ = numerator_ * other.GetDenominator();
  denominator_ = denominator_ * other.GetNumerator();
  Reduction();
  return *this;
}

Rational& Rational::operator++() {
  numerator_ += denominator_;
  Reduction();
  return *this;
}

Rational& Rational::operator--() {
  numerator_ -= denominator_;
  Reduction();
  return *this;
}

Rational Rational::operator++(int) {
  auto old = *this;
  numerator_ += denominator_;
  Reduction();
  return old;
}

Rational Rational::operator--(int) {
  auto old = *this;
  numerator_ -= denominator_;
  Reduction();
  return old;
}

Rational& Rational::operator=(const Rational& other) {
  if (this != &other) {
    numerator_ = other.GetNumerator();
    denominator_ = other.GetDenominator();
  }
  return *this;
}

void Rational::SetNumerator(const int x) {
  numerator_ = x;
  Reduction();
}

void Rational::SetDenominator(const int y) {
  denominator_ = y;
  Reduction();
}

int Rational::GetNumerator() const {
  return numerator_;
}

int Rational::GetDenominator() const {
  return denominator_;
}

Rational::Rational(const Rational& other) = default;

Rational::Rational() : numerator_(0), denominator_(1) {}

Rational::Rational(int x) : numerator_(x), denominator_(1) {}  //NOLINT

Rational::Rational(const int x, const int y) : numerator_(x), denominator_(y) {
  Reduction();
}


Rational operator+(const Rational& a, const Rational& b) {
  return {
    a.GetNumerator() * b.GetDenominator() + b.GetNumerator() * a.GetDenominator(),
    a.GetDenominator() * b.GetDenominator()
    };
}


Rational operator-(const Rational& a, const Rational& b) {
  return {
    a.GetNumerator() * b.GetDenominator() - b.GetNumerator() * a.GetDenominator(),
    a.GetDenominator() * b.GetDenominator()
    };
}

Rational operator*(const Rational& a, const Rational& b) {
  return {
    a.GetNumerator() * b.GetNumerator(),
    a.GetDenominator() * b.GetDenominator()
    };
}

Rational operator/(const Rational& a, const Rational& b) {
  return {
    a.GetNumerator() * b.GetDenominator(),
    a.GetDenominator() * b.GetNumerator()
    };
}

bool operator>(const Rational& a, const Rational& b) {
  return a.GetNumerator() * b.GetDenominator() > b.GetNumerator() * a.GetDenominator();
}

bool operator<(const Rational& a, const Rational& b) {
  return a.GetNumerator() * b.GetDenominator() < b.GetNumerator() * a.GetDenominator();
}

bool operator==(const Rational& a, const Rational& b) {
  return a.GetNumerator() == b.GetNumerator() && a.GetDenominator() == b.GetDenominator();
}

bool operator>=(const Rational& a, const Rational& b) {
  return a > b || a == b;
}

bool operator<=(const Rational& a, const Rational& b) {
  return a < b || a == b;
}

std::istream& operator>>(std::istream& in, Rational& r) {
  std::string s;
  in >> s;
  const size_t pos = s.find('/');
  if (pos < s.size()) {
    r.numerator_ = std::stoi(s.substr(0, pos));
    r.denominator_ = std::stoi(s.substr(pos + 1));
  } else {
    r.numerator_ = std::stoi(s);
    r.denominator_ = 1;
  }
  r.Reduction();
  return in;
}

std::ostream& operator<<(std::ostream& out, Rational r) {
  if (r.denominator_ == 1) {
    out << r.numerator_;
  } else {
    out << r.numerator_ << '/' << r.denominator_;
  }
  return out;
}