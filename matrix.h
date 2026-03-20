#ifndef MATRIX_H
#define MATRIX_H
#define MATRIX_SQUARE_MATRIX_IMPLEMENTED

#include <fstream>
#include <stdexcept>

class MatrixIsDegenerateError : public std::runtime_error {
public:
  MatrixIsDegenerateError() : std::runtime_error("MatrixIsDegenerateError") {
  }
};

class MatrixOutOfRange : public std::out_of_range {
public:
  MatrixOutOfRange() : std::out_of_range("MatrixOutOfRange") {
  }
};

template<class T, size_t CountRows, size_t CountColumns>
class Matrix {
public:

  T arr[CountRows][CountColumns];

  constexpr size_t RowsNumber() const {
    return CountRows;
  }

  constexpr size_t ColumnsNumber() const {
    return CountColumns;
  }

  T &operator()(size_t i, size_t j) {
    return arr[i][j];
  }

  const T &operator()(size_t i, size_t j) const {
    return arr[i][j];
  }

  T &At(size_t i, size_t j) {
    if (i >= CountRows || j >= CountColumns) {
      throw MatrixOutOfRange{};
    }
    return arr[i][j];
  }

  const T &At(size_t i, size_t j) const {
    if (i >= CountRows || j >= CountRows) {
      throw MatrixOutOfRange{};
    }
    return arr[i][j];
  }

  Matrix &operator+=(const Matrix &other) {
    for (size_t i = 0; i < CountRows; i++) {
      for (size_t j = 0; j < CountColumns; j++) {
        arr[i][j] += other(i, j);
      }
    }
    return *this;
  }

  Matrix &operator-=(const Matrix &other) {
    for (size_t i = 0; i < CountRows; i++) {
      for (size_t j = 0; j < CountColumns; j++) {
        arr[i][j] -= other(i, j);
      }
    }
    return *this;
  }

  Matrix &operator*=(const T &value) {
    for (size_t i = 0; i < CountRows; i++) {
      for (size_t j = 0; j < CountColumns; j++) {
        arr[i][j] *= value;
      }
    }
    return *this;
  }

  Matrix &operator*=(const Matrix<T, CountColumns, CountColumns> &other) {
    Matrix res;
    for (size_t i = 0; i < CountRows; ++i) {
      for (size_t j = 0; j < CountColumns; ++j) {
        for (size_t r = 0; r < CountColumns; ++r) {
          res(i, j) += arr[i][r] * other(r, j);
        }
      }
    }
    for (size_t i = 0; i < CountRows; ++i) {
      for (size_t j = 0; j < CountColumns; ++j) {
        arr[i][j] = res(i, j);
      }
    }
    return *this;
  }

  Matrix &operator/=(const T &value) {
    for (size_t i = 0; i < CountRows; i++) {
      for (size_t j = 0; j < CountColumns; j++) {
        arr[i][j] /= value;
      }
    }
    return *this;
  }

  bool operator==(const Matrix &other) {
    for (size_t i = 0; i < CountRows; ++i) {
      for (size_t j = 0; j < CountColumns; ++j) {
        if (arr[i][j] != other(i, j)) {
          return false;
        }
      }
    }
    return true;
  }

  bool operator==(const Matrix &other) const {
    for (size_t i = 0; i < CountRows; ++i) {
      for (size_t j = 0; j < CountColumns; ++j) {
        if (arr[i][j] != other(i, j)) {
          return false;
        }
      }
    }
    return true;
  }

  bool operator!=(const Matrix other) {
    return !(*this == other);
  }

  bool operator!=(const Matrix &other) const {
    return !(*this == other);
  }
};

template<class T, size_t CountRows, size_t CountColumns>
Matrix<T, CountColumns, CountRows> GetTransposed(const Matrix<T, CountRows, CountColumns> &mat) {
  Matrix<T, CountColumns, CountRows> res{};
  for (size_t i = 0; i < CountRows; i++) {
    for (size_t j = 0; j < CountColumns; j++) {
      res(j, i) = mat(i, j);
    }
  }
  return res;
}

template<class T, size_t Count>
void Transpose(Matrix<T, Count, Count> &mat) {
  for (size_t i = 0; i < Count; ++i) {
    for (size_t j = i; j < Count; ++j) {
      std::swap(mat(i, j), mat(j, i));
    }
  }
}

template<class T, size_t Count>
T Trace(const Matrix<T, Count, Count> &mat) {
  T res = 0;
  for (size_t i = 0; i < Count; i++) {
    res += mat(i, i);
  }
  return res;
}

template<class T, size_t Count>
Matrix<T, Count - 1, Count - 1> Slice(const Matrix<T, Count, Count> &mat, size_t indi, size_t indj) {
  size_t f1 = 0;
  Matrix<T, Count - 1, Count - 1> new_mat{};
  for (size_t i = 0; i < Count; ++i) {
    if (i == indi) {
      f1 = 1;
      continue;
    }
    size_t f2 = 0;
    for (size_t j = 0; j < Count; ++j) {
      if (j == indj) {
        f2 = 1;
        continue;
      }
      new_mat(i - f1, j - f2) = mat(i, j);
    }
  }
  return new_mat;
}

template<class T, size_t Count>
T Determinant(const Matrix<T, Count, Count> &mat) {
  if constexpr (Count == 1) {
    return static_cast<T>(mat(0, 0));
  } else {
    T res = 0;
    for (size_t i = 0; i < Count; ++i) {
      res += mat(i, 0) * Determinant(Slice(mat, i, 0)) * static_cast<T>((i & 1) ? -1 : 1);
    }
    return res;
  }
}

template<class T, size_t Count>
Matrix<T, Count, Count> &Inverse(Matrix<T, Count, Count> &mat) {
  T det = Determinant(mat);
  if (det == 0) {
    throw MatrixIsDegenerateError{};
  }
  Matrix<T, Count, Count> mat_dop{};
  for (size_t i = 0; i < Count; ++i) {
    for (size_t j = 0; j < Count; ++j) {
      if constexpr (Count == 1) {
        mat_dop(i, j) = static_cast<T>((i + j) & 1 ? -1 : 1);
      } else {
        mat_dop(i, j) = Determinant(Slice(mat, i, j)) * static_cast<T>((i + j) & 1 ? -1 : 1);
      }
    }
  }
  mat = mat_dop / det;
  Transpose(mat);
  return mat;
}

template<class T, size_t Count>
Matrix<T, Count, Count> GetInversed(const Matrix<T, Count, Count> &mat) {
  Matrix<T, Count, Count> res = mat;
  Inverse(res);
  return res;
}

template<class T, size_t CountRows, size_t CountColumns>
Matrix<T, CountRows, CountColumns> operator+(const Matrix<T, CountRows, CountColumns> &a, const Matrix<T, CountRows, CountColumns> &b) {
  Matrix<T, CountRows, CountColumns> c = a;
  c += b;
  return c;
}

template<class T, size_t CountRows, size_t CountColumns>
Matrix<T, CountRows, CountColumns> operator-(const Matrix<T, CountRows, CountColumns> &a, const Matrix<T, CountRows, CountColumns> &b) {
  Matrix<T, CountRows, CountColumns> c = a;
  c -= b;
  return c;
}

template<class T, class U, size_t CountRows, size_t CountColumns>
Matrix<T, CountRows, CountColumns> operator*(const Matrix<T, CountRows, CountColumns> &mat, const U &value) {
  Matrix<T, CountRows, CountColumns> res = mat;
  res *= value;
  return res;
}

template<class T, class U, size_t CountRows, size_t CountColumns>
Matrix<T, CountRows, CountColumns> operator*(const U &value, const Matrix<T, CountRows, CountColumns> &mat) {
  Matrix<T, CountRows, CountColumns> res = mat;
  res *= value;
  return res;
}

template<class T, size_t CountRows, size_t CountColumns, size_t CountColumns2>
Matrix<T, CountRows, CountColumns2> operator*(const Matrix<T, CountRows, CountColumns> &mat1, const Matrix<T, CountColumns, CountColumns2> &mat2) {
  Matrix<T, CountRows, CountColumns2> res{};
  for (size_t i = 0; i < CountRows; ++i) {
    for (size_t j = 0; j < CountColumns2; ++j) {
      for (size_t r = 0; r < CountColumns; ++r) {
        res(i, j) += mat1(i, r) * mat2(r, j);
      }
    }
  }
  return res;
}

template<class T, class U, size_t CountRows, size_t CountColumns>
Matrix<T, CountRows, CountColumns> operator/(const Matrix<T, CountRows, CountColumns> &mat, const U &value) {
  Matrix<T, CountRows, CountColumns> res = mat;
  res /= value;
  return res;
}

template<class T, size_t CountRows, size_t CountColumns>
std::istream& operator>>(std::istream &in, Matrix<T, CountRows, CountColumns>& mat) {
  for (size_t i = 0; i < CountRows; ++i) {
    for (size_t j = 0; j < CountColumns; ++j) {
      in >> mat(i, j);
    }
  }
  return in;
}

template<class T, size_t CountRows, size_t CountColumns>
std::ostream& operator<<(std::ostream &out, Matrix<T, CountRows, CountColumns> mat) {
  for (size_t i = 0; i < CountRows; ++i) {
    for (size_t j = 0; j < CountColumns; ++j) {
      out << mat(i, j);
      if (j < CountColumns - 1) {
        out << ' ';
      }
    }
    out << '\n';
  }
  return out;
}

#endif