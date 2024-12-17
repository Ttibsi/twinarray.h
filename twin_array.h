#ifndef TWIN_ARRAY_H
#define TWIN_ARRAY_H

#include <memory>
#include <ranges>

template <typename T>
class TwinArray {
    private:
        std::unique_ptr<T[]> lhs;
    std::unique_ptr<T[]> rhs;
        std::size_t lhs_size;
        std::size_t rhs_size;
        std::size_t capacity;

    public:
        TwinArray(const std::size_t len = 32):
            lhs(std::make_unique<T[]>(len)),
            rhs(std::make_unique<T[]>(len)), // rhs is going to be backwards
            lhs_size(0),
            rhs_size(0),
            capacity(len)
        {}

        void push(const T& val) {
            if ( size() == capacity) { resize(capacity * 2); }
            lhs[lhs_size] = val;
            lhs_size++;
        }

        T pop() {
            if (size() == 0) {} // TODO
            lhs[lhs_size] = T();
            lhs_size--;
        }

        void move_left() {
            if (lhs_size == 0) { return; }

            rhs[rhs_size] = lhs[lhs_size];
            lhs[lhs_size] = T();
            lhs_size--;
            rhs_size++;
        }

        void move_right() {
            if (rhs_size == 0) { return; }

            lhs[lhs_size] = rhs[rhs_size];
            rhs[rhs_size] = T();
            lhs_size++;
            rhs_size--;
        }

        T at(const std::size_t idx) {
            // TODO: Throw
            if (idx <= lhs_size) { return lhs[idx]; }

            if (idx <= size()) {
                auto rhs_view = std::views::reverse(
                    std::ranges::subrange(rhs.get(), rhs.get() + rhs_size)
                );

                return rhs_view[idx - lhs_size];
            }
        }

        void resize(const std::size_t new_size) {}

        int size() { return lhs_size + rhs_size; }
        bool empty() { return size() == 0; }
};

#endif // TWIN_ARRAY_H
