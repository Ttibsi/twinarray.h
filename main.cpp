#include "twin_array.h"

#define BOOST_UT_DISABLE_MODULE
#include "ut.hpp"

namespace ut = boost::ut;

ut::suite<"constructors"> constructors = [] {
    using namespace ut;

    "empty_constructor"_test = [] {
        auto buf = TwinArray<int>();
        expect(true) << "hello";
    };
};

int main() {}
