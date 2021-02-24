#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

std::string intToString(int x);

int pow(int x, int power) {
    int cur = 1;
    for (int i = 1; i <= power; ++i)
        cur *= x;
    return cur;
}


class BigInteger {
    friend class Rational;
    friend void sum_or_difference(BigInteger&, const BigInteger&, bool);
    std::vector<int> number;

    bool isNegative = 0;
public:
    static const int log10radix;
    static const int radix;
    void shrink();
    BigInteger(): number(0), isNegative(0) {}

    BigInteger(const BigInteger& x) {
        number = x.number;
        isNegative = x.isNegative;
        shrink();
    }

    const int& operator[](int index) const {
        return number[index];
    }

    int& operator[](int index) {
        return number[index];
    }

    BigInteger(int x) {
        number.clear();
        int x_copy = x;
        if (x_copy < 0) {
            isNegative = 1;
            x_copy *= -1;
        }
        while (x_copy) {
            number.push_back(x_copy % radix);
            x_copy /= radix;
        }
        shrink();
    }

    BigInteger abs() const;

    std::string toString() const;

    size_t size() const {
        return number.size();
    }

    bool empty() const {
        return !size();
    }

    void push_back(int a = 0) {
        number.push_back(a);
    }

    void pop_back() {
        number.pop_back();
    }

    void reverse() {
        std::reverse(number.begin(), number.end());
    }

    void resize(int sz = 0) {
        number.resize(sz);
    }

    void clear() {
        number.clear();
    }

    bool& sign() {
        return isNegative;
    }
    bool sign() const {
        return isNegative;
    }

    void changeSign(int sign = -1) {
        if (sign == -1) {
            isNegative ^= 1;
        } else {
            isNegative = sign;
        }
    }

    explicit operator bool() const {
        return number.size();
    }

    BigInteger operator-() const {
        BigInteger x = *this;
        x.isNegative ^= 1;
        return x;
    }

    BigInteger& operator-=(const BigInteger& a);

    BigInteger& operator+=(const BigInteger& a);


    BigInteger& operator++() {
        *this += 1;
        return *this;
    }

    BigInteger operator++(int) {
        BigInteger copy = *this;
        ++(*this);
        return copy;
    }

    BigInteger& operator--() {
        *this -= 1;
        return *this;
    }

    BigInteger operator--(int) {
        BigInteger copy = *this;
        --(*this);
        return copy;
    }

    BigInteger& operator*=(const BigInteger& a);

    BigInteger& operator/=(const BigInteger& a);

    BigInteger& operator%=(const BigInteger& a);
};

bool operator<(const BigInteger& b, const BigInteger& a) {
    if (b.sign() && !a.sign()) return true;
    if (!b.sign() && a.sign()) return false;

    if (a.size() != b.size()) {
        return b.sign() ^ (b.size() < a.size());
    }
    int answer = -1;
    for (int i = static_cast<int>(a.size()) - 1; i >= 0; --i) {
        int f = 0;
        if (static_cast<int>(b.size()) > i) f = b[i];
        int s = 0;
        if (static_cast<int>(a.size()) > i) s = a[i];
        if (f < s) {
            answer = 1;
            break;
        }
        if (f > s) {
            answer = 0;
            break;
        }
    }

    if (answer == -1) return false;
    if (!b.sign())
        return answer;
    return !answer;
}

bool operator==(const BigInteger& b, const BigInteger& a)  {
    if (b.size() != a.size() || a.sign() != b.sign()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (a[i] != b[i]) return false;
    return true;
}

bool operator!=(const BigInteger& b, const BigInteger& a)  {
    return !(b == a);
}

bool operator>=(const BigInteger& b, const BigInteger& a)  {
    return !(b < a);
}

bool operator>(const BigInteger& b, const BigInteger& a)  {
    return a < b;
}

bool operator<=(const BigInteger& b, const BigInteger& a)  {
    return !(b > a);
}

int get_power(int a, int b) {
    int first = -1;
    int second = -1;
    bool fl = 0;
    while (a != 0) {
        if (a % 2 != 0 && a != 1) fl = 1;
        a /= 2;

        ++first;
    }
    if (!fl) --first;
    fl = 0;
    while (b != 0) {
        if (b % 2 != 0 && b != 1) fl = 1;
        b /= 2;
        ++second;
    }
    if (!fl) second--;
    return pow(2, std::max(first, second) + 1);
}

void sum_or_difference(BigInteger& a, const BigInteger& b, bool is_difference = false) {
    for (size_t i = 0; i < a.size(); ++i) {
        long long temp = i < b.size() ? (is_difference ? -b[i] : b[i]) : 0;
        a.number[i] += temp;
        if (a.number[i] < 0) {
            a.number[i] += BigInteger::radix;
            a.number[i + 1]--;
        }
        if (a.number[i] >= BigInteger::radix) {
            if (i + 1 == a.size()) {
                a.push_back(0);
            }
            a.number[i] -= BigInteger::radix;
            a.number[i + 1]++;
        }
    }
}

BigInteger operator+(const BigInteger& a, const BigInteger& b);

BigInteger operator-(const BigInteger& a, const BigInteger& b);

BigInteger& BigInteger::operator-=(const BigInteger& a) {
    if (a.sign()) return *this = *this + (-a);
    if (sign()) return *this = -(-(*this) + a);
    if (*this < a) return *this = -(a - (*this));
    sum_or_difference(*this, a, true);
    shrink();
    return *this;
}

BigInteger& BigInteger::operator+=(const BigInteger& a) {
    if (isNegative) {
        if (a.isNegative) {
            return *this = -(-(*this) + (-a));
        } else {
            return *this = a - (-(*this));
        }
    } else if (a.isNegative) {
        return *this = *this - (-a);
    }
    if (*this < a) return *this = a + *this;
    sum_or_difference(*this, a);
    shrink();
    return *this;
}

BigInteger operator+(const BigInteger& a, const BigInteger& b) {
    BigInteger sum = a;
    sum += b;
    return sum;
}

BigInteger operator-(const BigInteger& a, const BigInteger& b) {
    BigInteger sum = a;
    sum -= b;
    return sum;
}

void BigInteger::shrink() {
    while (!number.empty() && number.back() == 0)
        number.pop_back();
    if (number.empty()) isNegative = 0;
    return;
}

std::string intToString(int x) {
    std::string s;
    int x_copy = x;
    while (x_copy != 0) {
        s = s + static_cast<char>(x_copy % 10 + '0');
        x_copy /= 10;
    }
    while (static_cast<int>(s.size()) != BigInteger::log10radix) s = s + "0";
    return s;
}

std::string BigInteger::toString() const {
    if (empty())
        return "0";

    std::string s = "";
    for (size_t i = 0; i < number.size(); ++i) {
        s += intToString(number[i]);
    }
    while (s.back() == '0')
        s.pop_back();
    if (isNegative)
        s.push_back('-');
    std::reverse(s.begin(), s.end());
    return s;
}

std::istream& operator>>(std::istream& in, BigInteger& i) {
    i.clear();
    i.sign() = 0;

    std::string s;
    in >> s;
    std::reverse(s.begin(), s.end());
    if (s.back() == '-') {
        i.sign() = 1;
        s.pop_back();
    }
    i.resize(s.size() / BigInteger::log10radix + s.size() % BigInteger::log10radix);
    for (size_t j = 0; j < s.size(); ++j) {
        i[(j) / BigInteger::log10radix] += static_cast<int>(s[j] - '0') * pow(10, (j) % BigInteger::log10radix);
    }
    i.shrink();
    if (i.size() == 0) i.sign() = 0;
    return in;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& i) {
    if (i.empty()) {
        out << 0;
        return out;
    }
    if (i.sign()) out << '-';
    out << i[i.size() - 1];
    for (int j = static_cast<int>(i.size()) - 2; j >= 0; --j) {
        int cpy = i[j];
        int cnt = 0;
        if (cpy == 0) cnt = 1;
        while (cpy) {
            ++cnt;
            cpy /= 10;
        }
        for (int ind = 0; ind < BigInteger::log10radix - cnt; ++ind)
            out << 0;
        out << i[j];
    }
    return out;
}

BigInteger mul(BigInteger a, BigInteger b) {
    a.shrink();
    b.shrink();
    if (a.size() <= 1 && b.size() <= 1) {
        if (a.size() == 0 || b.size() == 0) {
            return 0;
        }
        BigInteger answer;
        answer.resize(static_cast<size_t>(2));
        answer[0] = a[0] * b[0];
        answer[1] = answer[0] / BigInteger::radix;
        answer[0] %= BigInteger::radix;
        answer.shrink();
        return answer;
    }
    int n = get_power(a.size(), b.size());
    while (static_cast<int>(a.size()) != n) a.push_back();
    while (static_cast<int>(b.size()) != n) b.push_back();
    BigInteger a_left;
    a_left.resize(n / 2);
    BigInteger a_right;
    a_right.resize(n / 2);
    for (int i = 0; i < n; ++i) {
        if (i < n / 2) a_right[i] = a[i];
        else a_left[i - n / 2] = a[i];
    }
    BigInteger b_left;
    b_left.resize(n / 2);
    BigInteger b_right;
    b_right.resize(n / 2);
    for (int i = 0; i < n; ++i) {
        if (i < n / 2) b_right[i] = b[i];
        else b_left[i - n / 2] = b[i];
    }
//
    BigInteger first = mul(a_left, b_left);
    BigInteger second = mul(a_right, b_right);


    BigInteger third = mul(a_left + a_right, b_left + b_right);
    third -= first + second;

    for (int i = 0; i < n; ++i) first.push_back();
    for (int i = 0; i < n / 2; ++i) third.push_back();

    for (size_t i = first.size() - 1; i >= static_cast<size_t>(n); --i) {
        std::swap(first[i],first[i - n]);
    }
    for (size_t i = third.size() - 1; i >= static_cast<size_t>(n / 2); --i) {
        std::swap(third[i],third[i - n / 2]);
    }
    BigInteger answer = first + third + second;
    answer.shrink();
    return answer;
}

BigInteger operator*(const BigInteger& a, const BigInteger& b) {
    BigInteger result = a;
    result *= b;
    return result;
}

BigInteger& BigInteger::operator*=(const BigInteger& a) {
    if (*this == 0 || a == 0) return *this = 0;
    bool sig = sign();
    *this = mul(*this, a);
    if (a.sign() != sig)
        changeSign();
    if (a.size() == 0 || size() == 0)
        changeSign(0);
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& a) {
    bool sig = sign();
    changeSign(0);
    if (abs() < a.abs()) {
        *this = 0;
        return *this;
    }
    int n = size();
    BigInteger answer;
    std::vector<BigInteger> trying_number(radix);
    trying_number[0] = 0;
    for (int i = 1; i < radix; ++i) {
        trying_number[i] = trying_number[i - 1] + a.abs();
    }
    BigInteger coefficient;

    coefficient.resize(n);
    for (size_t i = 0; i < coefficient.size(); ++i) {
        coefficient[i] = 0;
    }

    coefficient.number.back() = 1;
    for (int i = n - 1; i >= 0; --i) {
        BigInteger current;
        for (int j = i; j < n; ++j) {
            current.push_back(number[j]);
        }
        current.shrink();

        int left =  0;
        int right = radix;
        while (right - left > 1) {
            int middle = (right + left) / 2;
            if (current >= trying_number[middle]) left = middle;
            else right = middle;
        }

        int digit = left;
        answer.push_back(digit);
        *this -= coefficient * digit * a.abs();
        if (coefficient.size() != 1) {
            coefficient.pop_back();
            coefficient[coefficient.size() - 1] = 1;
        }
    }
    answer.reverse();
    if (sig != a.sign()) answer.sign() = 1;
    answer.shrink();
    return *this = answer;
}

BigInteger operator/(const BigInteger& a, const BigInteger& b) {
    BigInteger x = a;
    x /= b;
    return x;
}

BigInteger& BigInteger::operator%=(const BigInteger& b) {
    BigInteger m = *this / b;
    *this -= b * m;
    return *this;
}

const BigInteger operator%(const BigInteger& a, const BigInteger& b) {
    BigInteger m = a;
    m %= b;
    return m;
}

BigInteger BigInteger::abs() const {
    BigInteger x = *this;
    x.isNegative = 0;
    return x;
}

const int BigInteger::radix = 10000;
const int BigInteger::log10radix = 4;

BigInteger find_gcd(const BigInteger& a, const BigInteger& b) {
    if (a == 0) return b;
    if (b == 0) return a;
    if (a < b) {
        return find_gcd(a, b % a);
    }
    return find_gcd(b, a % b);
}

class Rational {
    friend bool operator<(const Rational&, const Rational&);
private:
    BigInteger numerator;
    BigInteger denominator;
public:
    Rational() : numerator(0), denominator(1) {}

    Rational(const BigInteger& a): numerator(a), denominator(1) {}

    Rational(int a): numerator(a), denominator(1) {}

    void make_common() {
        BigInteger gcd = find_gcd(numerator.abs(), denominator.abs());
        numerator /= gcd;
        denominator /= gcd;
    }

    Rational& operator+=(const Rational& a) {
        numerator = numerator * a.denominator + a.numerator * denominator;
        denominator *= a.denominator;
        return *this;
    }

    Rational& operator-=(const Rational& a) {
        numerator = numerator * a.denominator - a.numerator * denominator;
        denominator *= a.denominator;
        return *this;
    }

    Rational& operator*=(const Rational& a) {
        numerator *= a.numerator;
        denominator *= a.denominator;
        return *this;
    }

    Rational& operator/=(const Rational& a) {
        numerator *= a.denominator;
        denominator *= a.numerator.abs();
        if (a.numerator.isNegative) numerator.isNegative ^= 1;
        return *this;
    }

    std::string toString() {
        make_common();
        std::string answer = numerator.toString();
        if (denominator != 1) answer += "/" + denominator.toString();
        return answer;
    }

    std::string asDecimal(size_t precision) const;

    explicit operator double() {
        std::string string_answer = asDecimal(18);
        bool fl = 0;
        if (string_answer[0] == '-') fl = 1;
        double answer = 0;
        double coef = 0.1;
        bool fl_2 = 0;
        for (size_t i = 0; i < string_answer.size(); ++i) {
            if (string_answer[i] == '.') {
                fl_2 = 1;
                continue;
            }
            if (string_answer[i] != '-') {
                if (!fl_2) {
                    answer *= 10;
                    answer += static_cast<int>(string_answer[i] - '0');
                } else {
                    answer += coef * static_cast<int>(string_answer[i] - '0');
                    coef /= 10;
                }
            }
        }
        if (fl) answer *= -1;
        return answer;
    }

    Rational operator-() const {
        Rational x = *this;
        x.numerator.isNegative ^= 1;
        return x;
    }
};
bool operator==(const Rational&, const Rational&);

bool operator!=(const Rational&, const Rational&);

bool operator<=(const Rational&, const Rational&);

bool operator>(const Rational&, const Rational&);

bool operator >=(const Rational&, const Rational&);

std::string Rational::asDecimal(size_t precision = 0) const {
    if (precision == 0) {
        return (numerator / denominator).toString();
    }
    if (*this == 0) {
        std::string answer = "0.";
        for (size_t i = 1; i <= precision; ++i)
            answer += "0";
        return answer;
    }

    Rational answer = *this;
    bool fl = 0;
    if (answer.numerator.isNegative) fl = 1;
    answer.numerator.isNegative = 0;
    BigInteger coefficient;
    coefficient.number.resize((precision) / BigInteger::log10radix + 2);
    int helping_value = precision % BigInteger::log10radix;
    coefficient.number[precision / BigInteger::log10radix] = pow(10, helping_value);

    coefficient.shrink();
    answer.numerator *= coefficient;
    answer.numerator /= answer.denominator;
    std::string string_result = answer.numerator.toString();
    std::string output;
    while (string_result.size() < precision)
        string_result = "0" + string_result;
    for (size_t i = 0; i < string_result.size(); ++i) {
        if (string_result.size() - i == precision) output += '.';
        output.push_back(string_result[i]);
    }
    if (output.front() == '.') output = "0" + output;
    if (fl) output = "-" + output;
    return output;
}

bool operator<(const Rational& b, const Rational& a) {
    BigInteger left = (b.numerator * a.denominator);
    BigInteger right = (a.numerator * b.denominator);
    return left <right;
}

bool operator==(const Rational& b, const Rational& a) {
    return !(b < a) && !(a < b);
}

bool operator!=(const Rational& b, const Rational& a) {
    return !(b == a);
}

bool operator>(const Rational& b, const Rational& a) {
    return a < b;
}

bool operator<=(const Rational& b, const Rational& a) {
    return !(b > a);
}

bool operator >=(const Rational& b, const Rational& a){
    return !(b < a);
}

Rational operator+(const Rational& a, const Rational& b) {
    Rational x = a;
    x += b;
    return x;
}

Rational operator-(const Rational& a, const Rational& b) {
    Rational x = a;
    x -= b;
    return x;
}

Rational operator*(const Rational& a, const Rational& b) {
    Rational x = a;
    x *= b;
    return x;
}

Rational operator/(const Rational& a, const Rational& b) {
    Rational x = a;
    x /= b;
    return x;
}

