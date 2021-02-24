#include <iostream>
#include <array>
#include <cmath>
#include <assert.h>
#include <vector>

unsigned euler_function(unsigned N) {
    if (N == 0) return 0;
    std::vector<std::pair<unsigned,unsigned>> divisors;
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
    for (std::pair<unsigned, unsigned> t : divisors) {
        N_copy *= (t.first - 1);
    }
    return N_copy;
}

template<bool condition>
struct compilation_error {
    static int d[condition ? 1 : -1];
    ~compilation_error() = default;
};




template<unsigned N, unsigned S>
struct is_prime_helping {
    static const bool value =
            (N % S == 0 ? false : is_prime_helping<N, S - 1>::value);
    static const int minimal_divisor =
            (value ? N : is_prime_helping<N, S - 1>::value
                            ? S
                            : is_prime_helping<N, S - 1>::minimal_divisor);
};

template<unsigned N>
struct is_prime_helping <N, 1> {
    static const bool value = true;
    static const int minimal_divisor = N;
};

template<unsigned N>
struct is_prime {
    static const bool value =
            is_prime_helping<N, static_cast<unsigned>(sqrt(N))>::value;
};

template<>
struct is_prime<1> {
    static const bool value = false;
};

template<unsigned N>
const bool is_prime_v = is_prime<N>::value;

template<unsigned N, unsigned P>
struct is_degree_of_prime {
    static const bool value =
            (N % P == 0 ? is_degree_of_prime<N / P, P>::value : false);

};

template<unsigned N>
struct is_degree_of_prime<N, 1> {
    static const bool value = false;
};

template<unsigned N>
struct is_degree_of_prime<N, 0> {
    static const bool value = false;
};

template<unsigned P>
struct is_degree_of_prime<1, P> {
    static const bool value = true;
};

template<unsigned P>
struct is_degree_of_prime<0, P> {
    static const bool value = false;
};


template<unsigned N>
struct is_degree_of_prime_general {
    static const bool value = is_degree_of_prime<
            N,
            is_prime_helping<
                    N,
                    static_cast<int>(sqrt(N))>::minimal_divisor>::value;
};

template<unsigned N>
struct has_primitive_root {
    static const bool value =
            N % 4 == 0
            ? false
            : is_degree_of_prime_general<N % 2 == 0 ? N / 2 : N>::value;
};

template<>
struct has_primitive_root<1> {
    static const bool value = false;
};

template<>
struct has_primitive_root<2> {
    static const bool value = true;
};

template<>
struct has_primitive_root<4> {
    static const bool value = true;
};


template<unsigned N>
const bool has_primitive_root_v = has_primitive_root<N>::value;

template<typename T, typename U>
struct is_same {
    static const bool value = false;
};

template<typename T>
struct is_same<T, T> {
    static const bool value = true;
};

template<typename T, typename U>
const bool is_same_v = is_same<T, U>::value;

int gcd(int x, int y) {
    if (x == 0) return y;
    return gcd(y % x, x);
}

template<unsigned N>
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
//
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
const unsigned Residue<N>::EF = euler_function(N);
