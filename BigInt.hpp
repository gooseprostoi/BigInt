/*
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@          *@@@@@&   &@@@@@@@@@@@@@@@@@@%       #@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@   @@@@@#    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@@@@@@@@@   @@@@@@
@@@   @@@@@@.   @@@@   @@@@@.       #  *@@@@@   @@@@@*  @        @@@&         @@
@@@          ,@@@@@@   @@@@   *@@@@@   *@@@@@   @@@@@*   &@@@@%   @@@@#   @@@@@@
@@@   @@@@@&    @@@@   @@@%   @@@@@@*  *@@@@@   @@@@@*   @@@@@@   @@@@#   @@@@@@
@@@   @@@@@@@   @@@@   @@@&   @@@@@@,  *@@@@@   @@@@@*   @@@@@@   @@@@#   @@@@@@
@@@   ****.    .@@@@   @@@@*   /@@%    *@@@@#   %@@@@*   @@@@@@   @@@@@   *@@%@@
@@@       ./&@@@@@@@   @@@@@@&    #@.  *@@%       #@@*   @@@@@@   @@@@@@#    *@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   &@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@(         ,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*/


#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <bitset>


using u32 = u_int32_t;
using u64 = u_int64_t;
const u64 U32MAX = static_cast<u64>(static_cast<u32>(-1)) + 1;


class BigUInt {
protected:
    std::vector<u64> digits; // digits[0] - 2^(0), digits[1] - 2^(32), digits[2] - 2^(64)

public:
    BigUInt() : digits(std::vector<u64>(0)) {}
    BigUInt(const std::vector<u64>& v) : digits(v) {}

    virtual std::string PrintPow() const { // print at x_0 + x_1 * 2^(32) + ... way
        std::string result;
        result += std::to_string(digits[0]);
        for (u64 i = 1; i < digits.size(); ++i) {
            result += " + " + std::to_string(digits[i]) + " * 2^(" + std::to_string(i * 32) + ')';
        }
        return result;
    }

    virtual std::string PrintBinary() const { // print BigUInt in binary
        std::string result;
        for (u64 i = 0; i < digits.size(); ++i) {
            result = std::bitset<32>(digits[i]).to_string() + result;
        }
        return result;
    }

protected:
// comparison operations ///////////////////////////////////////////////////////
    virtual bool operator== (const BigUInt& rhs) const {
        return (digits == rhs.digits);
    }

    virtual bool operator> (const BigUInt& rhs) const { // return |*this| > |rhs|
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


// += and -= ///////////////////////////////////////////////////////////////////
    virtual BigUInt& operator+= (const BigUInt& rhs) { // Add BigUInt
        const u64 DIGITSIZE = digits.size();
        const u64 RHSSIZE = rhs.digits.size();
        if (DIGITSIZE < RHSSIZE) {
            digits.resize(RHSSIZE + 1);
        }
        else {
            digits.resize(DIGITSIZE + 1);
        }

        for (u64 i = 0; i < RHSSIZE; ++i) {
            digits[i] += rhs.digits[i];
            digits[i+1] += digits[i] / U32MAX;
            digits[i] %= U32MAX;
        }
        for (u64 i = RHSSIZE; i < DIGITSIZE; ++i) {
            digits[i+1] += digits[i] / U32MAX;
            digits[i] %= U32MAX;
        }

        if (digits[digits.size() - 1] == 0) {
            digits.pop_back();
        }
        return *this;
    }

    void SimpleUSubtract(u64 index, u64 comp_number) { // rec func used only in USubtract
        if (digits[index] < comp_number) {
            digits[index] += U32MAX - comp_number;
            SimpleUSubtract(index + 1, 1);
        }
        else {
            digits[index] -= comp_number;
        }
    }

    virtual BigUInt& operator-= (const BigUInt& rhs) { // assumed that |*this| >= |rhs|
        if (rhs > (*this)) throw(1); // abord if |rhs| > |*this|

        for (u64 i = 0; i < rhs.digits.size(); ++i) {
            SimpleUSubtract(i, rhs.digits[i]);
        }
        while (digits[digits.size() - 1] == 0) {digits.pop_back();}
        return *this;
    }


// *= //////////////////////////////////////////////////////////////////////////

    // rec func used only in SimpleUMultiply
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

    // multiplying on number like mult_number * 2^(32 * pow_index)
    BigUInt& SimpleUMultiply(const u64 mult_number, const u64 pow_index) {
        const u64 final_rest = RecFuncMultiply(mult_number, 0, 0);

        if (final_rest != 0) digits.push_back(final_rest);

        digits.insert(digits.cbegin(), pow_index, 0);
        return *this;
    }

    virtual BigUInt& operator*= (const BigUInt& rhs) {
        BigUInt tmp = *this;
        BigUInt result;
        for (u64 i = 0; i < rhs.digits.size(); ++i) {
            result += tmp.SimpleUMultiply(rhs.digits[i], i);
            result.PrintPow();
            tmp = *this;
        }
        *this = result;
        return *this;
    }

};


class BigInt : protected BigUInt {
protected:
    bool sign = 0; // 0 - plus, 1 - minus

    void ChangeSign () {sign = !sign;}

public:
    BigInt() : BigUInt() {}
    BigInt(const std::vector<u64>& v) : BigUInt(v) {}
    
    std::string PrintPow() const override { // like PrintPow in BigUInt but with sign
        std::string result;
        if (sign) result += "-(";
        result += BigUInt::PrintPow();
        if (sign) result += ")";
        return result;
    }

    std::string PrintBinary() const override { // like PrintBinary in BigUInt but with sign in front, for example 110 = -2
        std::string result;
        for (u64 i = 0; i < digits.size(); ++i) {
            result = std::bitset<32>(digits[i]).to_string() + result;
        }
        if (sign) result = '1' + result;
        else result = '0' + result;
        return result;
    }

// comparison operations ///////////////////////////////////////////////////////
    bool operator== (const BigInt& rhs) const {
        return (digits == rhs.digits) && (sign == rhs.sign);
    }

    bool operator> (const BigInt& rhs) const {
        // fast cases
        if (sign == 0 && rhs.sign == 1) return 1;
        if (sign == 1 && rhs.sign == 0) return 0;

        bool result = this->BigUInt::operator>( rhs );

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

// += and -= (defined through operator+ and operator-)////////////////////////////
    BigInt& operator+= (const BigInt& rhs) {
        *this = *this + rhs;
        return *this;
    }

    BigInt& operator-= (const BigInt& rhs) {
        *this = *this - rhs;
        return *this;
    }

// *= //////////////////////////////////////////////////////////////////////////////////////////////

    BigInt& operator*= (const BigInt& rhs) {
        sign = (sign == rhs.sign ? 0 : 1);
        this->BigUInt::operator*=(rhs);
        return *this;
    }

// unary minus ////////////////////////////////////////////////////////////////////////////////////

    BigInt& operator- () {
        ChangeSign();
        return *this;
    }


// friends ///////////////////////////////////////////////////////////////////////////////////////
    friend BigInt operator+ (const BigInt& x, const BigInt& y);
    friend BigInt operator- (const BigInt& x, const BigInt& y);
};

BigInt operator+ (const BigInt& x, const BigInt& y) {
    BigInt result = (x.BigUInt::operator>(y) ? x : y);
    const BigInt& LowerInModulus = (x.BigUInt::operator>(y) ? y : x);
    if (x.sign == y.sign) {
        result.BigUInt::operator+=(LowerInModulus);
        return result;
    }
    result.BigUInt::operator-=(LowerInModulus);
    return result;
}

BigInt operator- (const BigInt& x, const BigInt& y) { // Here easier to copy and use operator+
    BigInt y_copy = y;
    y_copy.ChangeSign();
    return x + y_copy;
};