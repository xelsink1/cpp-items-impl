#ifndef UNORDERED_SET_H
#define UNORDERED_SET_H

#include <list>
#include <vector>
#include <iostream>

template <class KeyT>
class UnorderedSet {
  std::vector<std::list<KeyT>> ht_;
  size_t count_buckets_ = 0;
  size_t count_elements_ = 0;

 public:
  UnorderedSet() : ht_() {
  }

  explicit UnorderedSet(size_t count) : ht_(count), count_buckets_(count) {
  }

  UnorderedSet(const UnorderedSet &other_unorderd_set) {
    ht_ = other_unorderd_set.ht_;
    count_buckets_ = other_unorderd_set.count_buckets_;
    count_elements_ = other_unorderd_set.count_elements_;
  }

  UnorderedSet &operator=(const UnorderedSet &other_unorderd_set) {
    ht_ = other_unorderd_set.ht_;
    count_buckets_ = other_unorderd_set.count_buckets_;
    count_elements_ = other_unorderd_set.count_elements_;
    return *this;
  }

  UnorderedSet(UnorderedSet &&other_unorderd_set) noexcept {
    ht_ = other_unorderd_set.ht_;
    count_buckets_ = other_unorderd_set.count_buckets_;
    count_elements_ = other_unorderd_set.count_elements_;
    other_unorderd_set.ht_.clear();
    other_unorderd_set.count_buckets_ = other_unorderd_set.count_elements_ = 0;
  }

  UnorderedSet &operator=(UnorderedSet &&other_unorderd_set) noexcept {
    ht_ = other_unorderd_set.ht_;
    count_buckets_ = other_unorderd_set.count_buckets_;
    count_elements_ = other_unorderd_set.count_elements_;
    other_unorderd_set.ht_.clear();
    other_unorderd_set.count_buckets_ = other_unorderd_set.count_elements_ = 0;
    return *this;
  }

  template <class Iter>
  UnorderedSet(Iter first, Iter last) {
    std::size_t count = std::distance(first, last);
    ht_.resize(count);
    count_buckets_ = count;
    for (auto it = first; it != last; ++it) {
      Insert(*it);
    }
  }

  size_t Size() const {  // NOLINT
    return count_elements_;
  }

  void Clear() {
    ht_.clear();
    ht_.assign(count_buckets_, {});
    count_elements_ = 0;
  }

  bool Empty() const {  // NOLINT
    return count_elements_ == 0;
  }

  void Insert(const KeyT &key) {
    if (count_buckets_ == 0 || count_elements_ + 1 > count_buckets_) {
      Rehash(std::max(static_cast<size_t>(1), count_buckets_ * 2));
    }
    size_t bucket = std::hash<KeyT>{}(key) % count_buckets_;
    for (auto it : ht_[bucket]) {
      if (it == key) {
        return;
      }
    }
    ht_[bucket].push_back(key);
    ++count_elements_;
  }

  void Insert(const KeyT &&key) {
    if (count_buckets_ == 0 || count_elements_ + 1 > count_buckets_) {
      Rehash(std::max(static_cast<size_t>(1), count_buckets_ * 2));
    }
    size_t bucket = std::hash<KeyT>{}(key) % count_buckets_;
    for (auto it : ht_[bucket]) {
      if (it == key) {
        return;
      }
    }
    ht_[bucket].push_back(key);
    ++count_elements_;
  }

  void Erase(const KeyT &key) {
    size_t bucket = std::hash<KeyT>{}(key) % count_buckets_;
    for (auto it = ht_[bucket].begin(); it != ht_[bucket].end(); ++it) {
      if (*it == key) {
        ht_[bucket].erase(it);
        --count_elements_;
        return;
      }
    }
  }

  void Rehash(size_t new_count_buckets) {
    if (new_count_buckets == count_buckets_ || new_count_buckets < count_elements_) {
      return;
    }
    if (new_count_buckets > count_buckets_) {
      ht_.resize(new_count_buckets);
    }
    for (size_t i = 0; i < count_buckets_; ++i) {
      if (ht_[i].empty()) {
        continue;
      }
      for (auto it = ht_[i].begin(); it != ht_[i].end();) {
        KeyT key = *it;
        size_t bucket = std::hash<KeyT>{}(key) % new_count_buckets;
        if (bucket == i) {
          ++it;
        } else {
          it = ht_[i].erase(it);
          --count_elements_;
          ht_[bucket].push_back(key);
          ++count_elements_;
        }
      }
    }
    if (new_count_buckets < count_buckets_) {
      ht_.resize(new_count_buckets);
    }
    count_buckets_ = new_count_buckets;
  }

  void Reserve(size_t new_count_buckets) {
    if (new_count_buckets <= count_buckets_) {
      return;
    }
    count_buckets_ = new_count_buckets;
    ht_.resize(count_buckets_);
  }

  size_t BucketCount() const {  // NOLINT
    return count_buckets_;
  }

  size_t BucketSize(size_t id) const {  // NOLINT
    return count_buckets_ <= id ? 0 : ht_[id].size();
  }

  size_t Bucket(KeyT key) const {
    return count_buckets_ ? std::hash<KeyT>{}(key) % count_buckets_ : 0;
  }

  double LoadFactor() const {  // NOLINT
    return count_buckets_ == 0 ? 0 : static_cast<double>(count_elements_) / count_buckets_;
  }

  bool Find(const KeyT &key) const {
    if (ht_.empty()) {
      return false;
    }
    for (auto e : ht_[Bucket(key)]) {
      if (e == key) {
        return true;
      }
    }
    return false;
  }
};

#endif