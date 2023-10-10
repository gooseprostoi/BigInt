#include <iostream>
#include "BigInt.hpp"


int main() {
    BigInt a = 0;

    BigInt b = -1; 
    BigInt c = 1;
    b += c;
    std::cout << (a > b) << '\n';
}