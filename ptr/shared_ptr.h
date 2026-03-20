#ifndef SHARED_PTR_H
#define SHARED_PTR_H
#include <iostream>
#define WEAK_PTR_IMPLEMENTED

class BadWeakPtr : public std::runtime_error {
public:
  BadWeakPtr() : std::runtime_error("BadWeakPtr") {
  }
};

template<class T>
class SharedPtr;

class ControlBlock {
public:
  int strong_count;
  int weak_count;

  ControlBlock(): strong_count(0), weak_count(0) {}
};

template<class T>
class WeakPtr {
  T *ptr_ = nullptr;
  ControlBlock *control_block_ = nullptr;

  void DeletePtr() {
    if (control_block_) {
      --control_block_->weak_count;
      if (control_block_->strong_count == 0 && control_block_->weak_count == 0) {
        delete control_block_;
        control_block_ = nullptr;
        ptr_ = nullptr;
      } else if (control_block_->strong_count == 0) {
        ptr_ = nullptr;
      }
    }
  }

public:
  friend class SharedPtr<T>;

  WeakPtr() = default;

  explicit WeakPtr(T *raw_ptr) {
    if (raw_ptr) {
      ptr_ = raw_ptr;
      control_block_ = new ControlBlock();
      ++control_block_->weak_count;
    }
  }

  WeakPtr(const WeakPtr &other_weak_ptr) noexcept{ //NOLINT
    ptr_ = other_weak_ptr.ptr_;
    control_block_ = other_weak_ptr.control_block_;
    if (control_block_) {
      ++control_block_->weak_count;
    }
  }

  WeakPtr(const SharedPtr<T> &other_shared_ptr) { //NOLINT
    ptr_ = other_shared_ptr.ptr_;
    control_block_ = other_shared_ptr.control_block_;
    if (control_block_) {
      ++control_block_->weak_count;
    }
  }

  WeakPtr &operator=(const WeakPtr &other_weak_ptr) {
    if (this != &other_weak_ptr) {
      DeletePtr();
      ptr_ = other_weak_ptr.ptr_;
      control_block_ = other_weak_ptr.control_block_;
      if (control_block_) {
        ++control_block_->weak_count;
      }
    }
    return *this;
  }

  WeakPtr (WeakPtr &&other_weak_ptr) noexcept {
    ptr_ = other_weak_ptr.ptr_;
    control_block_ = other_weak_ptr.control_block_;
    other_weak_ptr.control_block_ = nullptr;
    other_weak_ptr.ptr_ = nullptr;
  }

  WeakPtr &operator=(WeakPtr &&other_weak_ptr) noexcept {
    if (this != &other_weak_ptr) {
      DeletePtr();
      ptr_ = other_weak_ptr.ptr_;
      control_block_ = other_weak_ptr.control_block_;
      other_weak_ptr.control_block_ = nullptr;
      other_weak_ptr.ptr_ = nullptr;
    }
    return *this;
  }

  void Swap(WeakPtr &other_weak_ptr) {
    std::swap(ptr_, other_weak_ptr.ptr_);
    std::swap(control_block_, other_weak_ptr.control_block_);
  }

  void Reset() {
    DeletePtr();
    ptr_ = nullptr;
    control_block_ = nullptr;
  }

  int UseCount() const { //NOLINT
    return (control_block_ ? control_block_->strong_count : 0);
  }

  bool Expired() const { //NOLINT
    return (control_block_ ? control_block_->strong_count == 0 : true);
  }

  SharedPtr<T> Lock() const;

  ~WeakPtr() {
    DeletePtr();
  }
};

template<class T>
class SharedPtr {
  T *ptr_ = nullptr;
  ControlBlock *control_block_ = nullptr;

  void DeletePtr() {
    if (control_block_) {
      --control_block_->strong_count;
      if (control_block_->strong_count == 0 && control_block_->weak_count == 0) {
        delete control_block_;
        control_block_ = nullptr;
        delete ptr_;
        ptr_ = nullptr;
      } else if (control_block_->strong_count == 0) {
        delete ptr_;
        ptr_ = nullptr;
      }
    }
  }

public:
  friend class WeakPtr<T>;

  SharedPtr(): control_block_(nullptr) {}

  SharedPtr (T *raw_ptr) { //NOLINT
    if (raw_ptr) {
      ptr_ = raw_ptr;
      control_block_ = new ControlBlock();
      ++control_block_->strong_count;
    }
  }

  SharedPtr (const WeakPtr<T> &other_weak_ptr) { //NOLINT
    if (other_weak_ptr.Expired()) {
      throw BadWeakPtr{};
    }
    ptr_ = other_weak_ptr.ptr_;
    control_block_ = other_weak_ptr.control_block_;
    if (control_block_) {
      ++control_block_->strong_count;
    }
  }

  SharedPtr (const SharedPtr &other_shared_ptr) { //NOLINT
    ptr_ = other_shared_ptr.ptr_;
    control_block_ = other_shared_ptr.control_block_;
    if (control_block_) {
      ++control_block_->strong_count;
    }
  }

  SharedPtr &operator=(const SharedPtr &other_shared_ptr) {
    if (this != &other_shared_ptr) {
      DeletePtr();
      ptr_ = other_shared_ptr.ptr_;
      control_block_ = other_shared_ptr.control_block_;
      if (control_block_) {
        ++control_block_->strong_count;
      }
    }
    return *this;
  }

  SharedPtr (SharedPtr &&other_shared_ptr) noexcept {
    ptr_ = other_shared_ptr.ptr_;
    control_block_ = other_shared_ptr.control_block_;
    other_shared_ptr.control_block_ = nullptr;
    other_shared_ptr.ptr_ = nullptr;
  }

  SharedPtr &operator=(SharedPtr &&other_shared_ptr) noexcept {
    if (this != &other_shared_ptr) {
      DeletePtr();
      ptr_ = other_shared_ptr.ptr_;
      control_block_ = other_shared_ptr.control_block_;
      other_shared_ptr.control_block_ = nullptr;
      other_shared_ptr.ptr_ = nullptr;
    }
    return *this;
  }

  void Reset(T *new_ptr = nullptr) {
    DeletePtr();
    ptr_ = nullptr;
    control_block_ = nullptr;
    if (new_ptr) {
      ptr_ = new_ptr;
      control_block_ = new ControlBlock();
      ++control_block_->strong_count;
    }
  }

  void Swap(SharedPtr &other_shared_ptr) {
    std::swap(ptr_, other_shared_ptr.ptr_);
    std::swap(control_block_, other_shared_ptr.control_block_);
  }

  T *Get() const {
    return (control_block_ ? ptr_ : nullptr);
  }

  int UseCount() const {  // NOLINT
    return control_block_ ? control_block_->strong_count : 0;
  }

  T &operator*() const {
    return *ptr_;
  }

  T *operator->() const {
    return (control_block_ ? ptr_ : nullptr);
  }

  explicit operator bool() const {
    return (control_block_ ? ptr_ != nullptr : false);
  }

  ~SharedPtr() {
    DeletePtr();
  }
};

template<class T>
SharedPtr<T> WeakPtr<T>::Lock() const {
  if (!control_block_ || control_block_->strong_count == 0) {
    return SharedPtr<T>(nullptr);
  }
  SharedPtr<T> res;
  res.ptr_ = ptr_;
  res.control_block_ = control_block_;
  ++res.control_block_->strong_count;
  return res;
}

template<class T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
  return SharedPtr<T>(new T(std::forward<Args>(args)...));
}


#endif