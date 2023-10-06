#include <iostream>
#include <string>
#include <cstdint>
#include <vector>

using u32 = u_int32_t;
using u64 = u_int64_t;
const u64 U32MAX = static_cast<u64>(static_cast<u32>(-1)) + 1;

/*
Errors:
1 - In USubtract func *this < rhs
*/


class BigInt {
private:
    std::vector<u64> digits;
    bool sign = 0; // 0 - plus, 1 - minus


public:
    BigInt() : digits(std::vector<u64>(0)) {}
    BigInt(const std::vector<u64>& v) : digits(v) {}


    void ChangeSign () {sign = !sign;}

    void PrintPow() const {
        if (sign) std::cout << "-(";
        std::cout << digits[0];
        for (u64 i = 1; i < digits.size(); ++i) {
            std::cout << " + " << digits[i] << " * 2^(" << i * 32 << ")";
        }
        if (sign) std::cout << ")";
        std::cout << '\n';
    }




private:
// Add and Subtract for unsigned
    BigInt& UAdd (const BigInt& rhs) {
        const u64 DIGITSIZE = digits.size();
        const u64 RHSSIZE = rhs.digits.size();

        if (DIGITSIZE < RHSSIZE) {
            digits.resize(RHSSIZE + 1);
        }
        else {
            digits.resize(DIGITSIZE + 1);
        }

        // std::cout << "------------------------------------------\n";
        // PrintBinary();

        for (u64 i = 0; i < RHSSIZE; ++i) {
            digits[i] += rhs.digits[i];
            digits[i+1] += digits[i] / U32MAX;
            digits[i] %= U32MAX;
        }
        for (u64 i = RHSSIZE; i < DIGITSIZE; ++i) {
            digits[i+1] += digits[i] / U32MAX;
            digits[i] %= U32MAX;
        }

        // PrintBinary();

        if (digits[digits.size() - 1] == 0) {
            digits.pop_back();
        }

        // PrintBinary();
        // std::cout << "------------------------------------------\n";

        return *this;
    }

    void SimpleUSubtract(u64 index, u64 comp_number) { // special function used only in USubtract
        if (digits[index] < comp_number) {
            digits[index] += U32MAX - comp_number;
            SimpleUSubtract(index + 1, 1);
        }
        else {
            digits[index] -= comp_number;
        }
    }

    BigInt& USubtract (const BigInt& rhs) { // assumed that |*this| >= |rhs|
        if (rhs.greater_modulus(*this)) throw(1); // abord if |rhs| > |*this|

        // std::cout << "------------------------------------------\n";
        // PrintBinary();

        for (u64 i = 0; i < rhs.digits.size(); ++i) {
            SimpleUSubtract(i, rhs.digits[i]);
        }
        while (digits[digits.size() - 1] == 0) {digits.pop_back();}
        // PrintBinary();
        // std::cout << "------------------------------------------\n";
        return *this;
    }

// Multiply for unsigned
    u64 RecFuncMultiply(const u64 mult_number, const u64 rest, const u64 i) {
        u64 new_rest = 0;
        digits[i] *= mult_number;
        new_rest += digits[i] / U32MAX;
        digits[i] %= U32MAX;
        digits[i] += rest;
        new_rest += digits[i] / U32MAX;
        digits[i] %= U32MAX;
        if (i == digits.size() - 1) {
            return new_rest;
        }
        return RecFuncMultiply(mult_number, new_rest, i + 1);
    }

    BigInt& SimpleUMultiply(const u64 mult_number, const u64 pow_index) {
        // умножить на mult_number
        // взять по модулю U32MAX и сделать перенос
        // перенести на pow_index строк в digits
        const u64 final_rest = RecFuncMultiply(mult_number, 0, 0);

        if (final_rest != 0) digits.push_back(final_rest);

        digits.insert(digits.cbegin(), pow_index, 0);
        return *this;
    }

    BigInt& UMultiply(const BigInt& rhs) {
        BigInt tmp = *this;
        BigInt result;
        for (u64 i = 0; i < rhs.digits.size(); ++i) {
            result += tmp.SimpleUMultiply(rhs.digits[i], i);
            result.PrintPow();
            tmp = *this;
        }
        *this = result;
        return *this;
    }


public:
    bool operator== (const BigInt& rhs) const {
        return (digits == rhs.digits) && (sign == rhs.sign);
    }

    bool greater_modulus (const BigInt& rhs) const { // return |*this| > |rhs|
        bool result;
        const u64 DIGITSIZE = digits.size();
        const u64 RHSSIZE = rhs.digits.size();

        // thinks like both are positive
        if (DIGITSIZE > RHSSIZE) result = 1;
        else if (DIGITSIZE < RHSSIZE) result = 0;
        else {
            u64 i = DIGITSIZE - 1;
            while (digits[i] == rhs.digits[i] && i != 0) {--i;}
            result = digits[i] > rhs.digits[i];
        }
        return result;
    }

    bool operator> (const BigInt& rhs) const {
        // fast cases
        if (sign == 0 && rhs.sign == 1) return 1;
        if (sign == 1 && rhs.sign == 0) return 0;

        bool result = (*this).greater_modulus(rhs);

        // remember that maybe some is negative
        if (sign == 1 && rhs.sign == 1) result = !result;
        return result;
    }

    bool operator>= (const BigInt& rhs) const {
        return (*this > rhs) || (*this == rhs);
    }

    bool operator<= (const BigInt& rhs) const {
        return !(*this > rhs);
    }

    bool operator< (const BigInt& rhs) const {
        return !(*this >= rhs);
    } 


    BigInt& operator+= (const BigInt& rhs) {
        *this = *this + rhs;
        return *this;
    }

    BigInt& operator-= (const BigInt& rhs) {
        *this = *this - rhs;
        return *this;
    }

    BigInt& operator*= (const BigInt& rhs) {
        sign = (sign == rhs.sign ? 0 : 1);
        return this->UMultiply(rhs);
    }

    friend BigInt operator+ (const BigInt& x, const BigInt& y);
    friend BigInt operator- (const BigInt& x, const BigInt& y);
};


BigInt operator+ (const BigInt& x, const BigInt& y) {
    BigInt result = (x.greater_modulus(y) ? x : y);
    const BigInt& LowerInModulus = (x.greater_modulus(y) ? y : x);
    if (x.sign == y.sign) {
        result.UAdd(LowerInModulus);
        return result;
    }
    result.USubtract(LowerInModulus);
    return result;
}

BigInt operator- (const BigInt& x, const BigInt& y) { // Here easier to copy and use operator+
    BigInt y_copy = y;
    y_copy.ChangeSign();
    return x + y_copy;
}

BigInt operator* (const BigInt& x, const BigInt& y) {
    BigInt result = x;
    return result *= y;
}


int main() {
    std::vector<u64> va = {6, 123};
    std::vector<u64> vb = {124, 1};
    BigInt a(va);
    const BigInt b(vb);
    std::cout << "a = "; a.PrintPow();
    std::cout << "b = "; b.PrintPow();

    a *= b; a.PrintPow();


}
