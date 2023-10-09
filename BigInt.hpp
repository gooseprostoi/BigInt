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

using u32 = uint32_t;
using u64 = uint64_t;

// | Можно было использовать std::numeric_limits
// || std::numeric_limits можно вызывать только от unsigned long, не от uint64_t. А это разные вещи
const u64 U32MAX = static_cast<u64>(static_cast<u32>(-1)) + 1;


class BigUInt {
protected:
    std::vector<u64> digits; // digits[0] - 2^(0), digits[1] - 2^(32), digits[2] - 2^(64)

public:
    BigUInt(u32 x = 0) : digits(std::vector<u64>(1, x)) {}

    // | Чаще используют такой вариант:
    // |    BigUInt(std::vector<u64> v) : digits(std::move(v)) {}
    // | Но этот тоже хороший.
    BigUInt(const std::vector<u64>& v) : digits(v) {}

    // | См. также std::stringstream
    // || Можно так, только не совсем пончтно зачем
    virtual std::string to_pow() const { // print at x_0 + x_1 * 2^(32) + ... way
        std::string result;
        result += std::to_string(digits[0]);
        for (u64 i = 1; i < digits.size(); ++i) {
            result += " + " + std::to_string(digits[i]) + " * 2^(" + std::to_string(i * 32) + ')';
        }
        return result;
    }

    virtual std::string to_binary() const { // print BigUInt in binary
        std::string result;
        for (u64 i = 0; i < digits.size(); ++i) {
            result = std::bitset<32>(digits[i]).to_string() + result;
        }
        return result;
    }

    bool is_zero() const {return (digits[0] == 0 && digits.size() == 1);}

protected:
// comparison operations ///////////////////////////////////////////////////////
    bool operator== (const BigUInt& rhs) const { return (digits == rhs.digits); }

    bool operator> (const BigUInt& rhs) const { // return |*this| > |rhs|
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
    virtual BigUInt& operator+= (const BigUInt& rhs) {
        const u64 DIGITSIZE = digits.size();
        const u64 RHSSIZE = rhs.digits.size();
        if (DIGITSIZE < RHSSIZE) digits.resize(RHSSIZE + 1);
        else digits.resize(DIGITSIZE + 1);

        for (u64 i = 0; i < RHSSIZE; ++i) {
            digits[i] += rhs.digits[i];
            digits[i+1] += digits[i] / U32MAX;
            digits[i] %= U32MAX;
        }
        for (u64 i = RHSSIZE; i < DIGITSIZE; ++i) {
            digits[i+1] += digits[i] / U32MAX;
            digits[i] %= U32MAX;
        }

        if (digits[digits.size() - 1] == 0) digits.pop_back();
        return *this;
    }

    // | С while-ом вместо рекурсии, кажется, было бы проще
    void SimpleUSubtract(u64 index, u64 comp_number) { // rec func used only in operator-=
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
        while (digits[digits.size() - 1] == 0 && digits.size() != 1) { digits.pop_back(); }
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
        if (i == digits.size() - 1) return new_rest;
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
        if (is_zero() || rhs.is_zero()) return (*this = BigUInt());
        BigUInt tmp = *this, result;
        for (u64 i = 0; i < rhs.digits.size(); ++i) {
            result += tmp.SimpleUMultiply(rhs.digits[i], i);
            result.to_pow();
            tmp = *this;
        }
        *this = result;
        return *this;
    }
};

std::ostream& operator<< (std::ostream& os, const BigUInt& biguint) { return os << biguint.to_pow(); }


class BigInt : protected BigUInt {
protected:
    bool sign; // 0 - plus, 1 - minus

public:
    BigInt(int64_t x = 0) : BigUInt(std::abs(x)), sign(x / std::abs(x)) {}
    BigInt(const std::vector<u64>& v, bool sign = 0) : BigUInt(v), sign(sign) {}
    
    std::string to_pow() const override { // like to_pow in BigUInt but with sign
        std::string result;
        if (sign && !BigUInt::is_zero()) result += "-(";
        result += BigUInt::to_pow();
        if (sign && !BigUInt::is_zero()) result += ")";
        return result;
    }

    std::string to_binary() const override { // like to_binary in BigUInt but with sign in front, for example 110 = -2
        std::string result;
        for (u64 i = 0; i < digits.size(); ++i) {
            result = std::bitset<32>(digits[i]).to_string() + result;
        }
        result = (sign ? '1' : '0') + result;
        return result;
    }

// comparison operations ///////////////////////////////////////////////////////
    bool operator== (const BigInt& rhs) const {
        if (is_zero() && rhs.is_zero()) return true;
        return (digits == rhs.digits) && (sign == rhs.sign);
    }

    bool operator> (const BigInt& rhs) const {
        // fast case
        if (sign != rhs.sign) return rhs.sign;

        bool result = this->BigUInt::operator>( rhs );

        // remember that maybe some is negative
        if (sign == true && rhs.sign == true) result = !result;
        return result;
    }

    bool operator>= (const BigInt& rhs) const { return (*this > rhs) || (*this == rhs); }

    bool operator<= (const BigInt& rhs) const { return !(*this > rhs); }
    
    bool operator< (const BigInt& rhs) const { return !(*this >= rhs); } 

// += and -= (defined through operator+ and operator-)////////////////////////////
    BigInt& operator+= (const BigInt& rhs) {
        if (sign == rhs.sign) this->BigUInt::operator+=(rhs);
        else {
            bool is_larger = this->BigUInt::operator>(rhs);
            sign = is_larger ? sign : rhs.sign;
            if (is_larger) this->BigUInt::operator-=(rhs);
            else { // I don't know how to avoid copying here
                const BigInt tmp = *this;
                *this = rhs;
                this->BigUInt::operator-=(tmp);
            }
        }
        return *this;
    }

    BigInt& operator-= (const BigInt& rhs) {
        change_sign();
        (*this) += rhs;
        change_sign();
        return *this;
    }

// *= //////////////////////////////////////////////////////////////////////////////////////////////
    BigInt& operator*= (const BigInt& rhs) {
        if (BigUInt::is_zero() || rhs.BigUInt::is_zero()) return (*this = BigInt());
        sign = (sign == rhs.sign ? 0 : 1);
        this->BigUInt::operator*=(rhs);
        return *this;
    }

// change_sign and unary minus /////////////////////////////////////////////////////////////////////
    BigInt& change_sign() {
        sign = !sign;
        return *this;
    }

    // | Унарный минус должен создавать копию, иначе это очень контринтуитивно
    BigInt operator- () {
        this->change_sign();
        return *this;
    }

// friends ///////////////////////////////////////////////////////////////////////////////////////
    friend BigInt operator+ (const BigInt& x, const BigInt& y);
    friend BigInt operator- (const BigInt& x, const BigInt& y);
};

BigInt operator+ (const BigInt& x, const BigInt& y) {
    BigInt result = x; 
    result += y;
    return result;
}

BigInt operator- (const BigInt& x, const BigInt& y) {
    BigInt result = x;
    result -= y;
    return result;
};

BigInt operator* (const BigInt& x, const BigInt& y) { // Here easier to copy and use operator+
    BigInt result = x;
    result *= y;
    return result;
};

std::ostream& operator<< (std::ostream& os, const BigInt& biguint) { return os << biguint.to_pow(); }
