#ifndef TWIN_ARRAY_H
#define TWIN_ARRAY_H

#include <algorithm>
#include <memory>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>

// TODO: Iterator?
// TODO: `using`
// TODO: Char-only methods
// TODO: Static asserts and exceptions

template <typename T>
class TwinArray {
   public:
    // member types
    using value_type = T;
    using allocator_type = std::allocator<T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = std::allocator_traits<std::allocator<T>>::pointer;
    using const_pointer = std::allocator_traits<std::allocator<T>>::const_pointer;

   private:
    std::unique_ptr<T[]> lhs;
    std::unique_ptr<T[]> rhs;  // NOTE: rhs is stored backwards
    std::size_t lhs_size;
    std::size_t rhs_size;
    std::size_t capacity;

   public:
    // Constructors
    TwinArray(const std::size_t len = 32)
        : lhs(std::make_unique<T[]>(len)),
          rhs(std::make_unique<T[]>(len)),
          lhs_size(0),
          rhs_size(0),
          capacity(len) {}

    // Modifiers
    void push(const T& val) {
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
            auto rhs_view =
                std::views::reverse(std::ranges::subrange(rhs.get(), rhs.get() + rhs_size));

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
    [[nodiscard]] std::string to_str() const noexcept
        requires(std::is_same_v<T, char>)
    {
        std::string ret;
        std::for_each(
            std::begin(lhs.get()), std::end(lhs.get()), [&](const char& c) { ret.push_back(c); });

        auto rhs_view = std::views::reverse(std::ranges::subrange(rhs.get(), rhs.get() + rhs_size));

        std::for_each(rhs_view.begin(), rhs_view.end(), [&](const char& c) { ret.push_back(c); });

        return ret;
    }

    [[nodiscard]] std::string get_current_line() const noexcept
        requires(std::is_same_v<T, char>)
    {
        auto last_idx = std::string_view(lhs.get()).find_last_of('\n');
        std::string ret;

        for (int i = last_idx; i < lhs_size; i++) {
            ret.push_back(lhs[i]);
        }

        return ret;
    }

    [[nodiscard]] char get_current_char() const noexcept
        requires(std::is_same_v<T, char>)
    {
        return lhs[lhs_size];
    }

    [[nodiscard]] int curr_line_count() const noexcept
        requires(std::is_same_v<T, char>)
    {
        return std::count(std::begin(lhs.get()), std::end(lhs.get()), '\n');
    }

    [[nodiscard]] int curr_char_count() const noexcept
        requires(std::is_same_v<T, char>)
    {
        auto last_idx = std::string_view(lhs.get()).find_last_of('\n');
        return lhs_size - last_idx;
    }
};

#endif  // TWIN_ARRAY_H
