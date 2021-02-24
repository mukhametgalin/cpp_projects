#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <complex>
using complex = std::complex < double >;

template <typename N>
using polynom = std::vector<N>;


std::string intToString(const int x);

int pow(int x, int power) {
    int cur = 1;
    for (int i = 1; i <= power; ++i)
        cur *= x;
    return cur;
}


class BigInteger {
    friend class Rational;
    friend void sum_or_difference(BigInteger&, const BigInteger&, bool);
    std::vector < long long > number;

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

    ~BigInteger() {
        number.clear();
    }

    const long long& operator[](const int index) const {
        return number[index];
    }
    long long& operator[](int index) {
        return number[index];
    }
    BigInteger(const long long x) {
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
        this->shrink();
    }
    BigInteger abs() const;
    std::string toString() const;

    size_t size() const {
        return number.size();
    }
    bool empty() const {
        return !size();
    }
    void push_back(const int a = 0) {
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
    void normalize();
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

bool operator<=(const BigInteger& b, const BigInteger& a)  {
    return (b < a) || (b == a);
}

bool operator>(const BigInteger& b, const BigInteger& a)  {
    return a < b;
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

std::string intToString(const int x) {
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

BigInteger multiply(const BigInteger &first, const BigInteger &second) {
    BigInteger result;
    result.resize(first.size() + second.size() + 5);
    for (size_t i = 0; i < first.size(); ++i) {
        for (size_t j = 0; j < second.size(); ++j) {
            if (result[i + j] >= BigInteger::radix) {
                if (i + j == result.size()) {
                    result.push_back(0);
                }
                result[i + j + 1] += result[i + j] / BigInteger::radix;
                result[i + j] %= BigInteger::radix;
            }
            result[i + j] += first[i] * second[j];
            if (result[i + j] >= BigInteger::radix) {
                if (i + j == result.size()) {
                    result.push_back(0);
                }
                result[i + j + 1] += result[i + j] / BigInteger::radix;
                result[i + j] %= BigInteger::radix;
            }
        }
    }
    return result;
}

BigInteger operator*(const BigInteger& a, const BigInteger& b) {
    BigInteger result = a;
    result *= b;
    return result;
}
void BigInteger::normalize() {
    for (size_t i = 0; i < size(); ++i) {
        if (number[i] >= radix) {
            if (i + 1 == size())
                push_back(0);
            number[i + 1] += number[i] / radix;
            number[i] %= radix;
        }
    }
}
BigInteger& BigInteger::operator*=(const BigInteger& a) {
    if (*this == 0 || a == 0) return *this = 0;
    bool sig = sign();
    *this = multiply(*this, a);
    normalize();
    shrink();
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
            if (current >= a * middle) left = middle;
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
const int BigInteger::radix = 1000000000;
const int BigInteger::log10radix = 9;

bool two_divides(BigInteger& a) {
    if (a == 0) return true;
    return a[0] % 2 == 0;
}

void div2(BigInteger& a) {
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] % 2 != 0) {
            a[i - 1] += BigInteger::radix / 2;
        }
        a[i] /= 2;
    }
    a.shrink();
    if (a == 0) a.sign() = 0;
}

void mul2(BigInteger& a) {
    for (size_t i = 0; i < a.size(); ++i) {
        a[i] *= 2;
        if (a[i] >= BigInteger::radix) {
            if (i + 1 == a.size()) {
                a.push_back(0);
            }
            a[i + 1] += a[i] / BigInteger::radix;
            a[i] %= BigInteger::radix;
        }
    }
}

BigInteger find_gcd(const BigInteger& a1, const BigInteger& b1) {
    if (a1 == 0) {
        return b1.abs();
    }
    BigInteger a = a1.abs();
    BigInteger b = b1.abs();
    BigInteger coef = 1;
    while (true) {
        if (a == 0) return b * coef;
        if (b == 0) return a * coef;
        while (two_divides(a) && two_divides(b)) {
            div2(a);
            div2(b);
            mul2(coef);
        }
        while (two_divides(a)) div2(a);
        while (two_divides(b)) div2(b);
        if (a > b) a -= b;
        else b -= a;
    }
}

class Rational {
    friend bool operator<(const Rational&, const Rational&);
    friend std::ostream& operator<<(std::ostream&,  Rational&);
    template <typename T> friend bool compare_to_zero(T&);
private:


public:
    BigInteger numerator;
    BigInteger denominator;
    Rational() : numerator(0), denominator(1) {}
    Rational(const BigInteger& a): numerator(a), denominator(1) {}
    Rational(const int a): numerator(a), denominator(1) {}

    void make_common() {
        BigInteger gcd = find_gcd(numerator, denominator);
        if (gcd == 2) {
            div2(numerator);
            div2(denominator);
            return;
        }
        numerator /= gcd;
        denominator /= gcd;
    }

    Rational& operator+=(const Rational& a) {
        make_common();
        numerator = numerator * a.denominator + a.numerator * denominator;
        denominator *= a.denominator;
        return *this;
    }
    Rational& operator-=(const Rational& a) {
        make_common();
        numerator = numerator * a.denominator - a.numerator * denominator;
        denominator *= a.denominator;
        return *this;
    }
    Rational& operator*=( Rational& a) {
        make_common();
        a.make_common();
        numerator *= a.numerator;
        denominator *= a.denominator;
        return *this;
    }
    Rational& operator/=( Rational& a) {
        make_common();
        a.make_common();
        numerator *= a.denominator;
        denominator *= a.numerator.abs();
        if (a.numerator.isNegative && numerator != 0) numerator.isNegative ^= 1;
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
std::ostream& operator<<(std::ostream& out,  Rational& i) {
    out << i.asDecimal(10);
    return out;
}
bool operator==(const Rational&, const Rational&);
bool operator!=(const Rational&, const Rational&);
bool operator<=(const Rational&, const Rational&);
bool operator>(const Rational&, const Rational&);
bool operator >=(const Rational&, const Rational&);
std::istream& operator>>(std::istream& in, Rational& i) {
    BigInteger a;
    in >> a;
    i = a;
    return in;
}
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
    return !(a < b) && !(b < a);
}
bool operator!=(const Rational& b, const Rational& a) {

    return !(b == a);
}
bool operator<=(const Rational& b, const Rational& a) {
    return (b < a) || (b == a);
}
bool operator>(const Rational& b, const Rational& a) {
    return a < b;
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
Rational operator*( Rational& a,  Rational& b) {
    Rational x = a;
    x *= b;
    return x;
}
Rational operator/( Rational& a,  Rational& b) {
    Rational x = a;
    x /= b;
    return x;
}


unsigned euler_function(unsigned N) {
    if (N == 0) return 0;
    std::vector < std::pair < unsigned, unsigned > > divisors;
    unsigned N_copy = N;
    for (unsigned i = 2; i * i <= N; ++i) {
        if (N % i == 0) {
            unsigned cnt = 0;
            while (N % i == 0) {
                N /= i;
                ++cnt;
            }
            N_copy /= i;
            divisors.push_back({i, cnt});
        }
    }
    if (N != 1) {
        N_copy /= N;
        divisors.push_back({N, 1});
    }
    for (std::pair < unsigned, unsigned > t : divisors)
        N_copy *= (t.first - 1);
    return N_copy;
}

template<bool condition>
struct compilation_error {
    static int d[condition ? 1 : -1];
    ~compilation_error() = default;
};




template <unsigned N, unsigned S>
struct is_prime_helping {
    static const bool value = (N % S == 0 ? false : is_prime_helping<N, S - 1>::value);
    static const int minimal_divisor = (value ? N : is_prime_helping<N, S - 1>::value ? S : is_prime_helping<N, S - 1>::minimal_divisor);
};

template <unsigned N>
struct is_prime_helping <N, 1> {
    static const bool value = true;
    static const int minimal_divisor = N;
};
template <unsigned N>
struct is_prime {
    static const bool value = is_prime_helping<N, static_cast<unsigned>(sqrt(N))>::value;
};

template <>
struct is_prime<1> {
    static const bool value = false;
};

template <unsigned N>
    const bool is_prime_v = is_prime<N>::value;

template <unsigned N, unsigned P>
struct is_degree_of_prime {
    static const bool value = N % P == 0 ? is_degree_of_prime<N / P, P>::value : false;
};

template <unsigned N>
struct is_degree_of_prime<N, 1> {
    static const bool value = false;
};
template <unsigned N>
struct is_degree_of_prime<N, 0> {
    static const bool value = false;
};

template <unsigned P>
struct is_degree_of_prime<1, P> {
    static const bool value = true;
};
template <unsigned P>
struct is_degree_of_prime<0, P> {
    static const bool value = false;
};


template <unsigned N>
struct is_degree_of_prime_general {
    static const bool value = is_degree_of_prime<N, is_prime_helping<N, static_cast<int>(sqrt(N))>::minimal_divisor>::value;
};
template <unsigned N>
struct has_primitive_root {
    static const bool value = N % 4 == 0 ? false : is_degree_of_prime_general<N % 2 == 0 ? N / 2 : N>::value;
};
template <>
struct has_primitive_root<1> {
    static const bool value = false;
};

template <>
struct has_primitive_root<2> {
    static const bool value = true;
};

template <>
struct has_primitive_root<4> {
    static const bool value = true;
};


template <unsigned N>
const bool has_primitive_root_v = has_primitive_root<N>::value;

template <typename T, typename U>
struct is_same {
    static const bool value = false;
};

template<typename T>
struct is_same<T, T> {
    static const bool value = true;
};

template <typename T, typename U>
const bool is_same_v = is_same<T, U>::value;

int gcd(int x, int y) {
    if (x == 0) return y;
    return gcd(y % x, x);
}

template <unsigned N>
class Residue {
private:
    unsigned int number = 0;
    static const unsigned EF;
public:

    explicit Residue(int x) {
        number = (1LL * x + 1LL * (-x / N  + 2) * N) % N;
    }
    Residue(): number(0) {}
    explicit operator int() const {
        int x = number;
        return x;
    }
    Residue<N>& operator=(int x) {
        number = x;
        return *this;
    }
    Residue<N>& operator=(const Residue<N>& x) {
        number = x.number;
        return *this;
    }
    Residue<N>& operator+=(const Residue<N>& x) {
        number = (x.number + number) % N;
        return *this;
    }
    Residue<N> operator+(const Residue<N>& x) const {
        Residue<N> a = *this;
        a += x;
        return a;
    }

    Residue<N>& operator-=(const Residue<N>& x) {
        number = (number - x.number + N) % N;
        return *this;
    }
    Residue<N> operator-(const Residue<N>& x) const {
        Residue<N> a = *this;
        a -= x;
        return a;
    }
    Residue<N>& operator*=(const Residue<N>& x) {
        number = (1LL * number * x.number) % N;
        return *this;
    }
    Residue<N> operator*(const Residue<N>& x) const {
        Residue<N> a = *this;
        a *= x;
        return a;
    }
    bool operator==(const Residue<N>& x) const {
        return number == x.number;
    }

    bool operator!=(const Residue<N>& x) const {
        return number != x.number;
    }

    Residue<N> pow(unsigned k) const {
        if (k == 0)
            return Residue<N>(1);
        if (k == 1)
            return *this;
        Residue<N> help = pow(k / 2);
        help *= help;
        if (k % 2 != 0) help *= *this;
        return help;
    }
    Residue<N> getInverse() const {
        compilation_error<is_prime_v<N> || number == 0> a;
        a = a;
        return pow(N - 2);
    }
    Residue<N>& operator/=(const Residue<N>& x) {
        Residue<N> inv_x = x.getInverse();
        return *this *= inv_x;
    }
    Residue<N> operator/(const Residue<N>& x) const {
        Residue<N> a = *this;
        a /= x;
        return a;
    }
    unsigned int order() const {
        if (number == 0 || EF == 0)
            return 0;
        if (gcd(number, N) != 1)
            return 0;
        if (number == 1)
            return 1;

        unsigned int ans = EF;
        for (unsigned i = 1; i * i <= EF; ++i) {
            if (static_cast<int>(pow(i)) == 1)
                ans = std::min(ans, i);
            if (static_cast<int>(pow(EF / i)) == 1)
                ans = std::min(ans, EF / i);
        }
        assert(static_cast<int>(pow(ans)) == 1);
        return ans;
    }

    static Residue<N> getPrimitiveRoot() {
        compilation_error<has_primitive_root_v<N>> a;
        a = a;

        for (unsigned i = 1; i < N; ++i) {
            if (Residue<N>(i).order() == EF)
                return Residue<N>(i);
        }
        return Residue<N>(0);
    }

};
template <unsigned N>
bool operator==(const Residue<N>& a, int b) {
    return a == Residue<N>(b);
}

template <unsigned N>
bool operator!=(const Residue<N>& a, int b) {
    return a != Residue<N>(b);
}

template <unsigned N>
const unsigned Residue<N>::EF = euler_function(N);

template <unsigned N>
std::istream& operator>>(std::istream& in, Residue<N>& i) {
    int a;
    in >> a;
    i = a;
    return in;
}

#include <initializer_list>



template <unsigned M, unsigned N, typename Field = Rational>
class Matrix {
template <unsigned M1, unsigned N1, unsigned K, typename Field1>
friend Matrix<M1, N1, Field1> strassen(const Matrix<M1, K, Field1>& a, const Matrix<K, N1, Field1>& b);
private:
    std::vector < std::vector < Field > > core;//(M, std::vector<Field>(N));
public:
    Matrix() {
        core.resize(M);
        for (unsigned i = 0; i < M; ++i) {
            core[i].resize(N);
        }
    }

    Matrix(const std::initializer_list<std::vector<int>> &I){
        core.resize(M);
        int it = 0;
        for (auto &i : I) {
            core[it].resize(N);
            for (unsigned j = 0; j < i.size(); ++j) {
                core[it][j] = Field(i[j]);
            }
            ++it;
        }
    }
    Matrix(const std::initializer_list<std::initializer_list<int>> &I){
        core.resize(M);
        int it = 0;
        for (auto &i : I) {
            core[it].resize(N);
            int it2 = 0;
            for (auto &j : i) {
                core[it][it2] = Field(j);
                ++it2;
            }
            ++it;
        }
    }
    Matrix(const std::vector < std::vector < int > > &I) {
        core.resize(M);
        int it = 0;
        for (auto &i : I) {
            core[it].resize(N);
            for (size_t j = 0; j < i.size(); ++j) {
                core[it][j] = Field(i[j]);
            }
            ++it;
        }
    }
    Matrix(const std::vector < std::vector < Field > > &I) {
        core.resize(M);
        int it = 0;
        for (auto &i : I) {
            core[it].resize(N);
            for (size_t j = 0; j < i.size(); ++j) {
                core[it][j] = i[j];
            }
            ++it;
        }
    }
    Matrix(const Matrix& a) {
        core.resize(M);
        for (unsigned i = 0; i < M; ++i) {
            core[i].resize(N);
            for (unsigned j = 0; j < N; ++j) {
                core[i][j] = a[i][j];
            }
        }
    }
    Matrix<M, N, Field>& operator=(const Matrix<M, N, Field>&);//
    Matrix<M, N, Field>& operator+=(const Matrix<M, N, Field>&);//
    Matrix<M, N, Field> operator+(const Matrix<M, N, Field>&) const;//
    Matrix<M, N, Field>& operator-=(const Matrix<M, N, Field>&);//
    Matrix<M, N, Field> operator-(const Matrix<M, N, Field>&) const;//
    Matrix<M, N, Field>& operator*=(const Field&);//
    Matrix<M, N, Field> operator*(const Field&) const;//

    Matrix<N, M, Field> transposed() const;//
    Matrix<M, N, Field> getGauss() const;//
    unsigned rank() const;//

    std::vector<Field> getRow(unsigned) const;//
    std::vector<Field> getColumn(unsigned) const;//

    std::vector<Field>& operator[](unsigned);//
    std::vector<Field> operator[](unsigned) const;//

//squared:
    Matrix<M, N, Field>& operator*=(Matrix<M, N, Field>& a);
    Field det() const;//
    Matrix<M, N, Field> inverted() const;
    void invert();
    Field trace() const;//
};

template <unsigned M, unsigned N, unsigned K, typename Field>
Matrix<M, N, Field> operator*(const Matrix<M, K, Field>&, const Matrix<K, N, Field>&);//

template <unsigned M, unsigned N, unsigned K, unsigned L, typename Field>
bool operator==(const Matrix<M, N, Field>&, const Matrix<K, L, Field>&);//

template <unsigned M, unsigned N, unsigned K, unsigned L, typename Field>
bool operator!=(const Matrix<M, N, Field>&, const Matrix<K, L, Field>&);//

template <unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> operator*(const Field&, const Matrix<M, N, Field>&);//

template <unsigned N, typename Field = Rational>
using SquareMatrix = Matrix<N, N, Field>;



//---------------------------------------------------------------------------------------------------------------------------------------


template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator=(const Matrix<M, N, Field>& a) {
    Matrix<M, N, Field>::Matrix(a);
    return *this;
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator+=(const Matrix<M, N, Field>& a) {
    for (unsigned i = 0; i < M; ++i) {
        for (unsigned j = 0; j < N; ++j) {
            core[i][j] += a[i][j];
        }
    }
    return *this;
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::operator+(const Matrix<M, N, Field>& a) const {
    Matrix<M, N, Field> res = *this;
    return res += a;
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator-=(const Matrix<M, N, Field>& a) {
    for (unsigned i = 0; i < M; ++i) {
        for (unsigned j = 0; j < N; ++j) {
            core[i][j] -= a[i][j];
        }
    }
    return *this;
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::operator-(const Matrix<M, N, Field>& a) const {
    Matrix<M, N, Field> res = *this;
    return res -= a;
}

template <unsigned M, unsigned N, unsigned K, unsigned L, typename Field>
bool operator==(const Matrix<M, N, Field>& a, const Matrix<K, L, Field>& b) {
    if (M != K || N != L)
        return false;

    for (unsigned i = 0; i < M; ++i) {
        for (unsigned j = 0; j < N; ++j) {
            if (a[i][j] != b[i][j])
                return false;
        }
    }
    return true;
}

template <unsigned M, unsigned N, unsigned K, unsigned L, typename Field>
bool operator!=(const Matrix<M, N, Field>& a, const Matrix<K, L, Field>& b) {
    return !(a == b);
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator*=(const Field& a) {
    for (unsigned i = 0; i < M; ++i) {
        for (unsigned j = 0; j < N; ++j) {
            core[i][j] *= a;
        }
    }
    return *this;
}

template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::operator*(const Field& a) const {
    Matrix<M, N, Field> res = *this;
    return res *= a;
}
template<unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> operator*(const Field& a, const Matrix<M, N, Field>& b) {
    return b * a;
}


template<unsigned M, unsigned N, typename Field>
Matrix<N, M, Field> Matrix<M, N, Field>::transposed() const {
    Matrix<N, M, Field> res;
    for (unsigned i = 0; i < M; ++i) {
        for (unsigned j = 0; j < N; ++j) {
            res[j][i] = core[i][j];
        }
    }
    return res;
}
template<unsigned M, unsigned N, typename Field>
std::vector < Field > Matrix<M, N, Field>::getRow(unsigned i) const {
    return (*this)[i];
}

template<unsigned M, unsigned N, typename Field>
std::vector < Field > Matrix<M, N, Field>::getColumn(unsigned i) const {
    std::vector < Field > res(M);
    for (unsigned j = 0; j < M; ++j) {
        res[j] = core[j][i];
    }
    return res;
}
template <unsigned M, unsigned N, typename Field>
std::vector<Field>& Matrix<M, N, Field>::operator[](unsigned i) {
    return core[i];
}
template <unsigned M, unsigned N, typename Field>
std::vector<Field> Matrix<M, N, Field>::operator[](unsigned i) const {
    return core[i];
}
template <unsigned M, unsigned N, typename Field>
Field Matrix<M, N, Field>::trace() const {
    if (M != N) {
        compilation_error<M == N> a;
        a = a;
    }
    Field res;
    res = 0;
    for (unsigned i = 0; i < M; ++i) {
        res += core[i][i];
    }
    return res;
}

template<typename Field>
std::vector<std::vector<Field>> sum(std::vector<std::vector<Field>> a, std::vector<std::vector<Field>> b, bool is_dif = false) {
    std::vector<std::vector<Field>> result(a.size(), std::vector<Field>(a.size() / 2));
    int n = a.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result[i][j] = a[i][j] + (is_dif ? -b[i][j] : b[i][j]);
        }
    }
    return result;
}

template<typename Field>
std::vector<std::vector<Field>> solve_strassen(std::vector<std::vector<Field>> a, std::vector<std::vector<Field>> b) {
    int n = a.size();
    if (n == 1) {
        return a[0][0] * b[0][0];
    }
    std::vector<std::vector<Field>> a11(n / 2, std::vector<Field>(n / 2));
    std::vector<std::vector<Field>> a12(n / 2, std::vector<Field>(n / 2));
    std::vector<std::vector<Field>> a21(n / 2, std::vector<Field>(n / 2));
    std::vector<std::vector<Field>> a22(n / 2, std::vector<Field>(n / 2));
    std::vector<std::vector<Field>> b11(n / 2, std::vector<Field>(n / 2));
    std::vector<std::vector<Field>> b12(n / 2, std::vector<Field>(n / 2));
    std::vector<std::vector<Field>> b21(n / 2, std::vector<Field>(n / 2));
    std::vector<std::vector<Field>> b22(n / 2, std::vector<Field>(n / 2));
    for (int i = 0; i < n / 2; ++i) {
        for (int j = 0; j < n / 2; ++j) {
            a11[i][j] = a[i][j];
            a12[i][j] = a[i][j + n / 2];
            a21[i][j] = a[i + n / 2][j];
            a22[i][j] = a[i + n / 2][j + n / 2];
            b11[i][j] = b[i][j];
            b12[i][j] = b[i][j + n / 2];
            b21[i][j] = b[i + n / 2][j];
            b22[i][j] = b[i + n / 2][j + n / 2];
        }
    }
    std::vector<std::vector<Field>> P1 = solve_strassen(sum(a11, a22), sum(b11, b22));
    std::vector<std::vector<Field>> P2 = solve_strassen(sum(a21, a22), b11);
    std::vector<std::vector<Field>> P3 = solve_strassen(a11, sum(b12, b22, true));
    std::vector<std::vector<Field>> P4 = solve_strassen(a22, sum(b12, b11, true));
    std::vector<std::vector<Field>> P5 = solve_strassen(sum(a11, a12), b22);
    std::vector<std::vector<Field>> P6 = solve_strassen(sum(a21, a11, true), sum(b11, b12));
    std::vector<std::vector<Field>> P7 = solve_strassen(sum(a12, a22, true), sum(b21, b22));
    std::vector<std::vector<Field>> c(n, std::vector<std::vector<Field>>(n));
    for (int i = 0; i < n / 2; ++i) {
        for (int j = 0; j < n / 2; ++j) {
            c[i][j] = P1[i][j] + P4[i][j] - P5[i][j] + P7[i][j];
            c[i][j + n / 2] = P3[i][j] + P5[i][j];
            c[i + n / 2][j] = P2[i][j] + P4[i][j];
            c[i + n / 2][j + n / 2] = P1[i][j] - P2[i][j] + P3[i][j] + P6[i][j];
        }
    }
    return c;
}

int get_size(int n) {
    int ans = 1;
    while (ans < n) {
        ans *= 2;
    }
    ans *= 2;
    return ans;
}
template <unsigned M, unsigned N, unsigned K, typename Field>
Matrix<M, N, Field> strassen(const Matrix<M, K, Field>& a, const Matrix<K, N, Field>& b) {
    std::vector<std::vector<Field>> new_a = a.core;
    std::vector<std::vector<Field>> new_b = b.core;
    int new_size = get_size(std::max(M, std::max(N, K)));
    new_a.resize(new_size);
    new_b.resize(new_size);
    for (int i = 0; i < new_size; ++i) {
        new_a[i].resize(new_size);
        new_b[i].resize(new_size);
    }
    Matrix<M, N, Field> result(solve_strassen(new_a, new_b));
    return result;
}
template <unsigned M, unsigned N, unsigned K, typename Field>
Matrix<M, N, Field> operator*(const Matrix<M, K, Field>& a, const Matrix<K, N, Field>& b) {
    if (M > 64 || N > 64)
        return strassen(a, b);
    Matrix <M, N, Field> result;

    for (unsigned i = 0; i < M; ++i) {
        for (unsigned j = 0; j < N; ++j) {
            for (unsigned k = 0; k < K; ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    return result;
}
template <unsigned M, unsigned N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator*=(Matrix<M, N, Field>& a) {
    if (M != N) {
        compilation_error<M == N> a;
        a == a;
    }
    if (M > 64) {
        return *this = strassen(*this, a);
    }
    for (unsigned i = 0; i < M; ++i) {
        std::vector < Field > copy = core[i];
        for (unsigned j = 0; j < M; ++j) {
            core[i][j] = 0;
            for (unsigned k = 0; k < M; ++k) {
                core[i][j] += copy[k] * a[k][j];
            }
        }
    }
    return *this;
}
#include <assert.h>

template <typename T>
bool compare_to_zero(T& a) {
    return a == 0;
}

template <>
bool compare_to_zero<Rational>(Rational& a) {
    return a.numerator == 0;
}
const double eps = 1e-7;
template<>
bool compare_to_zero<double>(double& a) {
    return abs(a) < eps;
}
const int kek = 17;
template <unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::getGauss() const {
    Matrix<M, N, Field> h(core);
    unsigned place = 0;
    int ans = 0;
    for (unsigned J = 0; J < M; ++J) {
        int pos = -1;
        for (unsigned i = 0; i < M; ++i) {
            if (!compare_to_zero(h[i][J])) {
                bool fl = 0;
                for (unsigned j = 0; j < J; ++j) {
                    if (h[i][j] != 0) {
                        fl = 1;
                        break;
                    }
                }
                if (fl) continue;
                pos = i;
                break;
            }
        }
        if (pos != -1) {
            std::swap(h.core[pos], h.core[place]);
            ++ans;
            pos = place;
            ++place;
            for (unsigned i = 0; i < M; ++i) {
                if (!compare_to_zero(h[i][J]) && i != static_cast<unsigned>(pos)) {
                    Field con = h[i][J] / h[pos][J];
                    for (unsigned j = J; j < N; ++j) {
                        h[i][j] -= h[pos][j] * con;

                        assert(compare_to_zero(h[i][J]));
                    }
                }
            }
        }
    }

    return h;
}
template <unsigned M, unsigned N, typename Field>
unsigned Matrix<M, N, Field>::rank() const {
    unsigned ans = 0;
    Matrix<M, N, Field> gauss = getGauss();
    for (unsigned i = 0; i < M; ++i) {
        for (unsigned j = 0; j < N; ++j) {
            if (gauss[i][j] != 0) {
                ++ans;
                break;
            }
        }
    }
    assert(ans <= M && ans <= N);
    return ans;
}

template <unsigned M, unsigned N, typename Field>
Field Matrix<M, N, Field>::det() const {
    if (M != N) {
        compilation_error<M == N> a;
        a = a;
    }

    Matrix<M, N, Field> gaussed = getGauss();
    Field res;
    res = 1;
    for (unsigned i = 0; i < M; ++i) {
        res *= gaussed[i][i];
    }
    if (res != 0) {
        bool fl = 1;
        for (unsigned i = 0; i < M; ++i) {
            for (unsigned j = 0; j < N; ++j) {
                if (i != j && gaussed[i][j] != 0) {
                    fl = 0;
                    break;
                }
            }
        }
        assert(fl);
    }
    return res;
}

template <unsigned M, unsigned N, typename Field>
void Matrix<M, N, Field>::invert() {

    if (M != N) {
        compilation_error<M == N> a;
        a = a;
    }
    Matrix<M, M + N, Field> copy;
    for (unsigned i = 0; i < M; ++i) {
        copy[i] = core[i];
        for (unsigned j = 0; j < N; ++j) {
            copy[i].push_back(Field(0));
        }
        copy[i][N + i] = 1;
    }
    Matrix<M, M + N, Field> gaussed = copy.getGauss();
    for (unsigned i = 0; i < M; ++i) {
        Field con = gaussed[i][i];
        for (unsigned j = N; j < 2 * N; ++j) {
            gaussed[i][j] /= con;
            core[i][j - N] = gaussed[i][j];
        }
    }

}

template <unsigned M, unsigned N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::inverted() const {
    Matrix<M, N, Field> result = *this;
    result.invert();
    return result;
}
