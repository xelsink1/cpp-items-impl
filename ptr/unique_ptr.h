#ifndef UNIQUE_PTR_H
#define UNIQUE_PTR_H
#include <fstream>

template<class T>
class UniquePtr {
  T *ptr_ = nullptr;

public:

  UniquePtr(): ptr_(nullptr) {}

  explicit UniquePtr(T *raw_ptr): ptr_(raw_ptr) {}

  UniquePtr(const UniquePtr&) = delete;

  UniquePtr& operator=(const UniquePtr&) = delete;

  UniquePtr(UniquePtr &&other_unique_ptr) noexcept {
    delete ptr_;
    ptr_ = other_unique_ptr.ptr_;
    other_unique_ptr.ptr_ = nullptr;
  }

  UniquePtr &operator=(UniquePtr &&other_unique_ptr) noexcept {
    if (this != &other_unique_ptr) {
      delete ptr_;
      ptr_ = other_unique_ptr.ptr_;
      other_unique_ptr.ptr_ = nullptr;
    }
    return *this;
  }

  T *Release() {
    auto old = ptr_;
    ptr_ = nullptr;
    return old;
  }

  void Reset(T *raw_ptr = nullptr) {
    delete ptr_;
    ptr_ = raw_ptr;
  }

  void Swap(UniquePtr &other_unique_ptr) {
    std::swap(ptr_, other_unique_ptr.ptr_);
  }

  T *Get() const {
    return ptr_;
  }

  T &operator*() const {
    return *ptr_;
  }

  T *operator->() const {
    return ptr_;
  }

  explicit operator bool() const {
    return ptr_ != nullptr;
  }

  ~UniquePtr() {
    delete ptr_;
  }
};

#endif