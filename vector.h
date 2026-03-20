#ifndef VECTOR_H
#define VECTOR_H
#define VECTOR_MEMORY_IMPLEMENTED
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <type_traits>

template<class T>
class Vector {
  size_t size_ = 0;
  size_t capacity_ = 0;
  T *data_ = nullptr;

  void Destroy(T *deleted_data, size_t first, size_t last) {
    for (; first < last; ++first) {
      deleted_data[first].~T();
    }
  }

  template<class Constructor>
  void Allocate(const size_t count, Constructor&& constructor) {
    if (count > 0) {
      data_ = static_cast<T*>(operator new(size_ * sizeof(T)));
      size_t i = 0;
      try {
        for (; i < size_; ++i) {
          constructor(data_ + i);
        }
      } catch (...) {
        Destroy(data_, 0, i);
        operator delete(data_);
        data_ = nullptr;
        size_ = capacity_ = 0;
        throw;
      }
    }
  }

public:
  using ValueType = T;
  using Pointer = T *;
  using ConstPointer = const T *;
  using Reference = T &;
  using ConstReference = const T &;
  using SizeType = size_t;
  using Iterator = T *;
  using ConstIterator = const T *;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

  Vector() = default;

  explicit Vector(const size_t size) : size_(size), capacity_(size) {
    try {
      Allocate(size_, [](T *ptr){ new(ptr) T(); });
    } catch (...) {
      throw;
    }
  }

  Vector(const size_t size, const T &value) : size_(size), capacity_(size) {
    try {
      Allocate(size_, [&value](T *ptr){ new(ptr) T(value); });
    } catch (...) {
      throw;
    }
  }

  template <class IteratorType,
            class = std::enable_if_t<std::is_base_of_v<std::forward_iterator_tag,
                typename std::iterator_traits<IteratorType>::iterator_category>>>
  Vector(IteratorType first, IteratorType last) {
    size_ = static_cast<size_t>(last - first);
    capacity_ = size_;
    if (size_ > 0) {
      data_ = static_cast<T*>(operator new(size_ * sizeof(T)));
      size_t i = 0;
      try {
        for (; i < size_; ++i, ++first) {
          new(data_ + i) T(*first);
        }
      } catch (...) {
        Destroy(data_, 0, i);
        operator delete(data_);
        data_ = nullptr;
        size_ = capacity_ = 0;
        throw;
      }
    }
  }

  Vector(std::initializer_list<T> list) {
    auto first = list.begin();
    auto last = list.end();
    size_ = static_cast<size_t>(last - first);
    capacity_ = size_;
    if (size_ > 0) {
      data_ = static_cast<T*>(operator new(size_ * sizeof(T)));
      size_t i = 0;
      try {
        for (; i < size_; ++i, ++first) {
          new(data_ + i) T(*first);
        }
      } catch (...) {
        Destroy(data_, 0, i);
        operator delete(data_);
        data_ = nullptr;
        size_ = capacity_ = 0;
        throw;
      }
    }
  }

  Vector(const Vector &other) : size_(other.size_), capacity_(other.capacity_) {
    if (size_ > 0) {
      data_ = static_cast<T*>(operator new(size_ * sizeof(T)));
      size_t i = 0;
      try {
        for (; i < size_; ++i) {
          new(data_ + i) T(other.data_[i]);
        }
      } catch (...) {
        Destroy(data_, 0, i);
        operator delete(data_);
        data_ = nullptr;
        size_ = capacity_ = 0;
        throw;
      }
    }
  }

  Vector(Vector &&other) noexcept {
    size_ = other.size_;
    capacity_ = other.capacity_;
    data_ = other.data_;
    other.size_ = 0;
    other.capacity_ = 0;
    other.data_ = nullptr;
  }

  Vector &operator=(const Vector &other) {
    if (this != &other) {
      T *new_data = nullptr;
      if (other.size_ > 0) {
        new_data = static_cast<T*>(operator new(other.capacity_ * sizeof(T)));
        size_t i = 0;
        try {
          for (; i < other.capacity_; ++i) {
            new(new_data + i) T(other.data_[i]);
          }
        } catch (...) {
          Destroy(new_data, 0, i);
          operator delete(new_data);
          throw;
        }
      }
      Destroy(data_, 0, size_);
      operator delete(data_);
      data_ = new_data;
      size_ = other.size_;
      capacity_ = other.capacity_;
    }
    return *this;
  }

  Vector &operator=(Vector &&other) noexcept {
    if (this != &other) {
      Destroy(data_, 0, size_);
      operator delete(data_);
      size_ = other.size_;
      capacity_ = other.capacity_;
      data_ = other.data_;
      other.size_ = 0;
      other.capacity_ = 0;
      other.data_ = nullptr;
    }
    return *this;
  }

  size_t Size() const noexcept { return size_; }  // NOLINT
  size_t Capacity() const noexcept { return capacity_; }  // NOLINT
  bool Empty() const noexcept { return size_ == 0; }  // NOLINT

  T &operator[](size_t ind) { return data_[ind]; }
  const T &operator[](size_t ind) const { return data_[ind]; }

  T &At(size_t ind) {
    if (ind >= size_) {
      throw std::out_of_range("Index out of range");
    }
    return data_[ind];
  }

  const T &At(size_t ind) const {
    if (ind >= size_) {
      throw std::out_of_range("Index out of range");
    }
    return data_[ind];
  }

  Reference Front() { return data_[0]; }
  ConstReference Front() const { return data_[0]; }
  Reference Back() { return data_[size_ - 1]; }
  ConstReference Back() const { return data_[size_ - 1]; }

  Iterator Data() { return data_; }
  ConstIterator Data() const { return data_; }

  Iterator begin() { return data_; }  // NOLINT
  ConstIterator begin() const { return data_; }  // NOLINT
  ConstIterator cbegin() const { return data_; }  // NOLINT
  ReverseIterator rbegin() { return ReverseIterator(end()); }  // NOLINT
  ConstReverseIterator rbegin() const { return ConstReverseIterator(end()); }  // NOLINT
  ConstReverseIterator crbegin() const { return ConstReverseIterator(end()); }  // NOLINT

  Iterator end() { return data_ + size_; }  // NOLINT
  ConstIterator end() const { return data_ + size_; }  // NOLINT
  ConstIterator cend() const { return data_ + size_; }  // NOLINT
  ReverseIterator rend() { return ReverseIterator(begin()); }  // NOLINT
  ConstReverseIterator rend() const { return ConstReverseIterator(begin()); }  // NOLINT
  ConstReverseIterator crend() const { return ConstReverseIterator(begin()); }  // NOLINT

  bool operator==(const Vector &other) const {
    if (size_ != other.size_) {
      return false;
    }
    for (size_t i = 0; i < size_; ++i) {
      if (!(data_[i] == other.data_[i])) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const Vector &other) const {
    return !(*this == other);
  }

  bool operator<(const Vector &other) const {
    for (size_t i = 0; ; ++i) {
      if (i == other.size_) {
        return false;
      }
      if (i == size_) {
        return true;
      }
      if (data_[i] < other.data_[i]) {
        return true;
      }
      if (data_[i] > other.data_[i]) {
        return false;
      }
    }
  }

  bool operator>(const Vector &other) const {
    for (size_t i = 0; ; ++i) {
      if (i == size_) {
        return false;
      }
      if (i == other.size_) {
        return true;
      }
      if (data_[i] > other.data_[i]) {
        return true;
      }
      if (data_[i] < other.data_[i]) {
        return false;
      }
    }
  }

  bool operator<=(const Vector &other) const {
    return *this == other || *this < other;
  }

  bool operator>=(const Vector &other) const {
    return *this == other || *this > other;
  }

  void Swap(Vector &other) {
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    std::swap(data_, other.data_);
  }

  void Resize(const size_t new_size) {
    Resize(new_size, [](T *ptr) { new(ptr) T(); }, 0);
  }

  void Resize(const size_t new_size, const T& value) {
    Resize(new_size, [&value](T *ptr) { new(ptr) T(value); }, 0);
  }

  template<class Constructor>
  void Resize(const size_t new_size, Constructor&& constructor, int) {
    if (new_size <= capacity_) {
      if (size_ > new_size) {
        Destroy(data_, new_size, size_);
      } else if (size_ < new_size) {
        size_t i = size_;
        try {
          for (; i < new_size; ++i) {
            constructor(data_ + i);
          }
        } catch (...) {
          Destroy(data_, size_, i);
          throw;
        }
      }
      size_ = new_size;
    } else {
      size_t new_capacity = std::max(new_size, capacity_ * 2);
      T *new_data = nullptr;
      new_data = static_cast<T*>(operator new(new_capacity * sizeof(T)));
      size_t i = 0;
      try {
        for (; i < size_; ++i) {
          new(new_data + i) T(std::move(data_[i]));
        }
      } catch (...) {
        Destroy(new_data, 0, i);
        operator delete(new_data);
        throw;
      }
      size_t j = size_;
      try {
        for (; j < new_size; ++j) {
          constructor(new_data + j);
        }
      } catch (...) {
        Destroy(new_data, 0, j);
        operator delete(new_data);
        throw;
      }
      Destroy(data_, 0, size_);
      operator delete(data_);
      data_ = new_data;
      size_ = new_size;
      capacity_ = new_capacity;
    }
  }

  void Reserve(const size_t new_capacity) {
    if (new_capacity == 0) {
      Destroy(data_, 0, size_);
      operator delete(data_);
      size_ = capacity_ = 0;
      data_ = nullptr;
    } else if (capacity_ < new_capacity) {
      T *new_data = static_cast<T*>(operator new(new_capacity * sizeof(T)));
      size_t i = 0;
      try {
        for (; i < size_; ++i) {
          new(new_data + i) T(std::move(data_[i]));
        }
      } catch (...) {
        Destroy(new_data, 0, i);
        operator delete(new_data);
        throw;
      }
      Destroy(data_, 0, size_);
      operator delete(data_);
      data_ = new_data;
      capacity_ = new_capacity;
    }
  }

  void ShrinkToFit() {
    if (size_ < capacity_) {
      T *new_data = nullptr;
      if (size_ > 0) {
        new_data = static_cast<T*>(operator new(size_ * sizeof(T)));
        size_t i = 0;
        try {
          for (; i < size_; ++i) {
            new(new_data + i) T(std::move(data_[i]));
          }
        } catch (...) {
          Destroy(new_data, 0, i);
          operator delete(new_data);
          throw;
        }
      }
      Destroy(data_, 0, size_);
      operator delete(data_);
      data_ = new_data;
      capacity_ = size_;
    }
  }

  void Clear() {
    for (size_t i = 0; i < size_; ++i) {
      data_[i].~T();
    }
    size_ = 0;
  }

  void PushBack(const T &value) {
    if (size_ == capacity_) {
      size_t new_capacity = std::max(static_cast<size_t>(1), capacity_ * 2);
      T* new_data = static_cast<T*>(operator new(new_capacity * sizeof(T)));

      size_t i = 0;
      try {
        for (; i < size_; ++i) {
          new(new_data + i) T(std::move(data_[i]));
        }
        ++i;
        new(new_data + size_) T(value);
      } catch (...) {
        Destroy(new_data, 0, i);
        operator delete(new_data);
        throw;
      }

      Destroy(data_, 0, size_);
      operator delete(data_);
      data_ = new_data;
      capacity_ = new_capacity;
      size_++;
    } else {
      new(data_ + size_) T(value);
      ++size_;
    }
  }

  void PushBack(T &&value) {
    if (size_ == capacity_) {
      size_t new_capacity = std::max(static_cast<size_t>(1), capacity_ * 2);
      T* new_data = static_cast<T*>(operator new(new_capacity * sizeof(T)));

      size_t i = 0;
      try {
        for (; i < size_; ++i) {
          new(new_data + i) T(std::move(data_[i]));
        }
        ++i;
        new(new_data + size_) T(std::move(value));
      } catch (...) {
        Destroy(new_data, 0, i);
        operator delete(new_data);
        throw;
      }

      Destroy(data_, 0, size_);
      operator delete(data_);
      data_ = new_data;
      capacity_ = new_capacity;
      size_++;
    } else {
      new(data_ + size_) T(std::move(value));
      ++size_;
    }
  }

  void PopBack() {
    if (size_ == 0) {
      return;
    }
    data_[size_ - 1].~T();
    --size_;
  }

  template <typename... Args>
  void EmplaceBack(Args&&... args) {
    if (size_ == capacity_) {
      Reserve(std::max(static_cast<size_t>(1), capacity_ * 2));
    }
    new (data_ + size_) T(std::forward<Args>(args)...);
    ++size_;
  }

  ~Vector() {
    if (data_) {
      Destroy(data_, 0, size_);
      operator delete(data_);
    }
    data_ = nullptr;
    size_ = capacity_ = 0;
  }
};

#endif
