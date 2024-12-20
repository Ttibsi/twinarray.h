#include <string>
#include <vector>

#include "twin_array.h"
#include "ut.hpp"

namespace ut = boost::ut;

ut::suite<"Constructors"> constructors = [] {
    using namespace ut;

    "Default Constructor"_test = [] {
        auto buf = TwinArray<int>();

        expect(buf.size() == 0);
        expect(buf.total_capacity() == 32);
    };

    "Constructor With Size"_test = [] {
        auto buf = TwinArray<int>(8);

        expect(buf.size() == 0);
        expect(buf.total_capacity() == 8);
    };

    "Constructor From Vec"_test = [] {
        std::vector<int> v = {1, 2, 3};
        auto buf = TwinArray<int>(v.begin(), v.end());

        expect(buf.size() == 3);
        expect(buf.size() == v.size());
        expect(buf.total_capacity() == 11);
    };

    "Constructor From Init List"_test = [] {
        TwinArray<int> buf = {1, 2, 3};

        expect(buf.size() == 3);
        expect(buf.total_capacity() == 11);
    };

    "Constructor From String_view"_test = [] {
        std::string s = "hello world";
        auto buf = TwinArray<char>(s);

        expect(buf.size() == s.size());
        expect(buf.total_capacity() == s.size() + 8);
    };

    // TODO: RUle of 5
};

ut::suite<"Modifiers"> modifiers = [] {
    using namespace ut;

    "Push"_test = [] {
        should("with int") = [=]() {
            auto buf = TwinArray<int>(8);
            buf.push(1);

            expect(buf.size() == 1);
            expect(buf.at(0) == 1);
        };

        should("with char") = [=]() {
            auto buf = TwinArray<char>(8);
            buf.push('_');

            expect(buf.size() == 1);
            expect(buf.at(0) == '_');
        };
    };

    "Pop"_test = [] {
        should("Standard") = [] {
            TwinArray<int> buf = {1, 2, 3};
            auto p = buf.pop();

            expect(p.has_value());
            expect(p.value() == 3) << p.value();
            expect(buf.size() == 2);
        };

        should("Empty buffer") = [] {
            auto buf = TwinArray<int>(8);
            auto p = buf.pop();

            expect(!(p.has_value()));
        };
    };

    "Move Left"_test = [] {
        should("Standard") = [] {
            TwinArray<int> buf = {1, 2, 3};
            expect(buf.peek() == 3);

            buf.move_left();
            expect(buf.peek() == 2);
        };

        should("Cant move left") = [] {
            auto buf = TwinArray<int>(8);
            buf.push(1);

            expect(buf.peek() == 1);
            buf.move_left();
            expect(buf.peek() == 0);
        };
    };

    "Move Right"_test = [] {
        should("Standard") = [] {
            TwinArray<int> buf = {1, 2, 3};
            expect(buf.peek() == 3);

            buf.move_left();
            expect(buf.peek() == 2);

            buf.move_right();
            expect(buf.peek() == 3);
        };

        should("Cant move right") = [] {
            auto buf = TwinArray<int>(8);
            buf.push(1);

            expect(buf.peek() == 1);
            buf.move_right();
            expect(buf.peek() == 1);
        };
    };
};

ut::suite<"Element Access"> element_access = [] {
    using namespace ut;

    "At"_test = [] {
        TwinArray<int> buf = {1, 2, 3};
        expect(buf.at(0) == 1);
        expect(buf.at(1) == 2);
        expect(buf.at(2) == 3);
        expect(throws<std::out_of_range>([=] { buf.at(3); }));

        buf.move_left();
        buf.move_left();

        expect(buf.at(0) == 1);
        expect(buf.at(1) == 2) << buf.at(1);
        expect(buf.at(2) == 3);
        expect(throws<std::out_of_range>([=] { buf.at(3); }));
    };

    "Peek"_test = [] {
        TwinArray<int> buf = {1, 2, 3};
        expect(buf.peek() == 3);
    };
};

ut::suite<"Capacity"> capacity = [] {
    using namespace ut;

    "Size"_test = [] {
        should("All on left") = [] {
            TwinArray<int> buf = {1, 2, 3};
            expect(buf.size() == 3);
        };

        should("Mixed lhs/rhs") = [] {
            TwinArray<int> buf = {1, 2, 3};
            buf.move_left();
            expect(buf.size() == 3);
        };

        should("All on right") = [] {
            TwinArray<int> buf = {1, 2, 3};
            buf.move_left();
            buf.move_left();
            buf.move_left();
            expect(buf.size() == 3);
        };

        should("No value") = [] {
            auto buf = TwinArray<int>(8);
            expect(buf.size() == 0);
        };
    };

    "Total Capacity"_test = [] {
        should("All on left") = [] {
            TwinArray<int> buf = {1, 2, 3};
            expect(buf.total_capacity() == 11);
        };

        should("Mixed lhs/rhs") = [] {
            TwinArray<int> buf = {1, 2, 3};
            buf.move_left();
            expect(buf.total_capacity() == 11);
        };

        should("All on right") = [] {
            TwinArray<int> buf = {1, 2, 3};
            buf.move_left();
            buf.move_left();
            buf.move_left();
            expect(buf.total_capacity() == 11);
        };

        should("No value") = [] {
            auto buf = TwinArray<int>(8);
            expect(buf.total_capacity() == 8);
        };
    };

    "Empty"_test = [] {
        should("Is empty") = [] {
            auto buf = TwinArray<int>(8);
            expect(buf.empty() == true);
        };

        should("Is not empty") = [] {
            TwinArray<int> buf = {1, 2, 3};
            expect(buf.empty() == false);
        };
    };

    "Resize"_test = [] {
        auto buf = TwinArray<int>(2);
        expect(buf.total_capacity() == 2);

        buf.push(1);
        buf.push(2);
        buf.push(3);
        expect(buf.total_capacity() == 4);
        expect(buf.size() == 3);

        buf.move_left();
        buf.move_left();
        buf.move_left();
        expect(buf.total_capacity() == 4);
        expect(buf.size() == 3);
    };
};

int main() {}
