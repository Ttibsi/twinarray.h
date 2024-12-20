#ifndef TWIN_ARRAY_H
#define TWIN_ARRAY_H

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>

// TODO: Iterator?
// TODO: `using`
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
    constexpr explicit TwinArray(const std::size_t len = 32)
        : lhs(std::make_unique<T[]>(len)),
          rhs(std::make_unique<T[]>(len)),
          lhs_size(0),
          rhs_size(0),
          capacity(len) {}

    template <typename InputIt>
    constexpr explicit TwinArray(InputIt begin, InputIt end)
        : TwinArray(std::distance(begin, end) + 8) {
        lhs_size = std::distance(begin, end);
        std::copy(begin, end, lhs.get());
    }

    constexpr TwinArray(std::initializer_list<T> lst) : TwinArray(lst.size() + 8) {
        lhs_size = lst.size();
        std::copy(lst.begin(), lst.end(), lhs.get());
    }

    constexpr explicit TwinArray(std::string_view str)
        requires(std::is_same_v<T, char>)
        : TwinArray(str.size() + 8) {
        lhs_size = str.size();
        std::copy(str.begin(), str.end(), lhs.get());
    }

    // Copy Constructor
    TwinArray(const TwinArray& other)
        : lhs(std::make_unique<T[]>(other.capacity)),
          rhs(std::make_unique<T[]>(other.capacity)),
          lhs_size(other.lhs_size),
          rhs_size(other.rhs_size),
          capacity(other.capacity) {
        for (std::size_t i = 0; i < lhs_size; ++i) {
            lhs[i] = other.lhs[i];
        }

        for (std::size_t i = 0; i < rhs_size; ++i) {
            rhs[i] = other.rhs[i];
        }
    }

    // Copy Assignment Operator
    TwinArray& operator=(const TwinArray& other) {
        if (this != &other) {
            auto new_lhs = std::make_unique<T[]>(other.capacity);
            auto new_rhs = std::make_unique<T[]>(other.capacity);

            // Copy data from other
            for (std::size_t i = 0; i < other.lhs_size; ++i) {
                new_lhs[i] = other.lhs[i];
            }
            for (std::size_t i = 0; i < other.rhs_size; ++i) {
                new_rhs[i] = other.rhs[i];
            }

            lhs = std::move(new_lhs);
            rhs = std::move(new_rhs);
            lhs_size = other.lhs_size;
            rhs_size = other.rhs_size;
            capacity = other.capacity;
        }
        return *this;
    }

    // Move Constructor
    TwinArray(TwinArray&& other) noexcept
        : lhs(std::move(other.lhs)),
          rhs(std::move(other.rhs)),
          lhs_size(other.lhs_size),
          rhs_size(other.rhs_size),
          capacity(other.capacity) {
        // Reset other's state
        other.lhs_size = 0;
        other.rhs_size = 0;
        other.capacity = 0;
    }

    // Move Assignment Operator
    TwinArray& operator=(TwinArray&& other) noexcept {
        if (this != &other) {
            // Move resources
            lhs = std::move(other.lhs);
            rhs = std::move(other.rhs);
            lhs_size = other.lhs_size;
            rhs_size = other.rhs_size;
            capacity = other.capacity;

            other.lhs_size = 0;
            other.rhs_size = 0;
            other.capacity = 0;
        }
        return *this;
    }

    // Destructor
    ~TwinArray() = default;

    // Operator overloads
    friend std::ostream& operator<<(std::ostream& os, const TwinArray& buf) {
        os << "[";
        for (auto i = 0; i < buf.lhs_size; i++) {
            os << buf.lhs[i] << " ";
        }
        os << "]";

        os << "[";
        for (auto i = 0; i < buf.rhs_size; i++) {
            os << buf.rhs[i] << " ";
        }
        os << "]";

        return os;
    }

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

        T ret = lhs[lhs_size - 1];
        lhs[lhs_size - 1] = T();
        lhs_size--;

        return ret;
    }

    void move_left() {
        if (lhs_size == 0) {
            return;
        }

        rhs[rhs_size] = lhs[lhs_size - 1];
        lhs[lhs_size - 1] = T();
        lhs_size--;
        rhs_size++;
    }

    void move_right() {
        if (rhs_size == 0) {
            return;
        }

        lhs[lhs_size] = rhs[rhs_size - 1];
        rhs[rhs_size - 1] = T();
        lhs_size++;
        rhs_size--;
    }

    // Element Access
    [[nodiscard]] T at(const std::size_t idx) const {
        if (idx >= size()) {
            throw std::out_of_range("index out of range");
        }

        if (idx < lhs_size) {
            return lhs[idx];
        } else if (idx < size()) {
            auto rhs_view =
                std::views::reverse(std::ranges::subrange(rhs.get(), rhs.get() + rhs_size));

            return rhs_view[idx - lhs_size];
        }
    }

    [[nodiscard]] T peek() const { return lhs[lhs_size - 1]; }

    // Capacity
    [[nodiscard]] int size() const noexcept { return lhs_size + rhs_size; }
    [[nodiscard]] int total_capacity() const noexcept { return capacity; }
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
        std::for_each(lhs.get(), lhs.get() + lhs_size, [&](const char& c) { ret.push_back(c); });

        auto rhs_view = std::views::reverse(std::ranges::subrange(rhs.get(), rhs.get() + rhs_size));

        std::for_each(rhs_view.begin(), rhs_view.end(), [&](const char& c) { ret.push_back(c); });

        return ret;
    }

    [[nodiscard]] std::string get_current_line() const noexcept
        requires(std::is_same_v<T, char>)
    {
        std::string ret;
        int last_idx = std::string_view(lhs.get()).find_last_of('\n');
        if (last_idx > lhs_size) {
            last_idx = 0;
        }

        for (int i = last_idx; i < lhs_size; i++) {
            ret.push_back(lhs[i]);
        }

        if (lhs[lhs_size - 1] != '\n') {
            int rhs_idx = rhs_size - 1;

            while (rhs[rhs_idx] != '\n' || rhs_idx > 0) {
                ret.push_back(rhs[rhs_idx]);
                rhs_idx--;
            }
        }

        return ret;
    }

    [[nodiscard]] char get_current_char() const noexcept
        requires(std::is_same_v<T, char>)
    {
        return lhs[lhs_size - 1];
    }

    [[nodiscard]] int curr_line_index() const noexcept
        requires(std::is_same_v<T, char>)
    {
        return std::count(lhs.get(), lhs.get() + lhs_size, '\n') + 1;
    }

    [[nodiscard]] int curr_char_index() const noexcept
        requires(std::is_same_v<T, char>)
    {
        auto last_idx = std::string_view(lhs.get()).find_last_of('\n');
        if (last_idx > lhs_size) {
            last_idx = 0;
        }
        return (lhs_size - last_idx) - 1;
    }
};

#endif  // TWIN_ARRAY_H
