#ifndef TWIN_ARRAY_H
#define TWIN_ARRAY_H

#include <memory>
#include <optional>
#include <ranges>
#include <stdexcept>

// TODO: Iterator?
// TODO: `using`
// TODO: Char-only methods
// TODO: Static asserts and exceptions

template <typename T> class TwinArray {
private:
  std::unique_ptr<T[]> lhs;
  std::unique_ptr<T[]> rhs; // NOTE: rhs is stored backwards
  std::size_t lhs_size;
  std::size_t rhs_size;
  std::size_t capacity;

public:
  // Constructors
  TwinArray(const std::size_t len = 32)
      : lhs(std::make_unique<T[]>(len)), rhs(std::make_unique<T[]>(len)),
        lhs_size(0), rhs_size(0), capacity(len) {}

  // Modifiers
  void push(const T &val) {
    if (size() == capacity) {
      resize(capacity * 2);
    }
    lhs[lhs_size] = val;
    lhs_size++;
  }

  [[nodiscard]] std::optional<T> pop() {
    if (size() == 0) {
      return {};
    }

    T ret = lhs[lhs_size];
    lhs[lhs_size] = T();
    lhs_size--;

    return ret;
  }

  void move_left() {
    if (lhs_size == 0) {
      return;
    }

    rhs[rhs_size] = lhs[lhs_size];
    lhs[lhs_size] = T();
    lhs_size--;
    rhs_size++;
  }

  void move_right() {
    if (rhs_size == 0) {
      return;
    }

    lhs[lhs_size] = rhs[rhs_size];
    rhs[rhs_size] = T();
    lhs_size++;
    rhs_size--;
  }

  // Element Access
  [[nodiscard]] T at(const std::size_t idx) const {
    if (idx > size()) {
      throw std::out_of_range("index out of range");
    }
    if (idx <= lhs_size) {
      return lhs[idx];
    }

    if (idx <= size()) {
      auto rhs_view = std::views::reverse(
          std::ranges::subrange(rhs.get(), rhs.get() + rhs_size));

      return rhs_view[idx - lhs_size];
    }
  }

  // Capacity
  [[nodiscard]] int size() const noexcept { return lhs_size + rhs_size; }
  [[nodiscard]] bool empty() const noexcept { return size() == 0; }

  void resize(const std::size_t new_cap) {
    auto new_lhs = std::make_unique<T[]>(new_cap);
    auto new_rhs = std::make_unique<T[]>(new_cap);

    for (std::size_t i = 0; i < lhs_size; i++) {
      new_lhs[i] = lhs[i];
    }

    for (std::size_t i = 0; i < rhs_size; i++) {
      new_rhs[i] = rhs[i];
    }

    lhs = std::move(new_lhs);
    rhs = std::move(new_rhs);
    capacity = new_cap;
  }

  // Char-only methods
};

#endif // TWIN_ARRAY_H
