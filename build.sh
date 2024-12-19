
time clang++ -std=c++20 -Wall -Wextra -g main.cpp -o test

if [ $? -eq 0 ]; then
    ./test
else
    echo "Failed with return code $?"
fi
