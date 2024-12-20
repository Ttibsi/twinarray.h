#!/bin/bash

compile_cmd="clang++ -std=c++20 -Wall -Wextra -g main.cpp -o test"

if eval "$compile_cmd"; then
    ./test ${1:+$1}
else
    echo "Failed with return code $?"
fi
