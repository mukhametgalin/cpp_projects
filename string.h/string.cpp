#include <iostream>
#include <cstring>

class String {

private:

    char* str = nullptr;

    static const size_t MIN_STR_SIZE;
    static const double EXP_COEF;
    size_t strSize;
    size_t realSize;
    void setNewSize(size_t newSize) {
        newSize = std::max(MIN_STR_SIZE, newSize);
        if (newSize == strSize) return;
        char* newStr = new char[newSize];
        std::memcpy(newStr, str, realSize);
        delete[] str;
        str = newStr;
        strSize = newSize;
    }

public:

    String(): str(nullptr), strSize(0), realSize(0) {}

    ~String() {
        delete[] str;
    }

    String(size_t n, char c): str(new char[n]),
            strSize(std::max(MIN_STR_SIZE, static_cast<size_t>(n * EXP_COEF))),
            realSize(n) {
        memset(str, c, realSize);
    }
    String(const String& s): str(new char[s.strSize]),
            strSize(s.strSize), realSize(s.realSize) {
        memcpy(str, s.str, s.realSize);
    }

    String (const char* s): String(strlen(s), 0) {
        memcpy(str, s, strlen(s));
    }

    String (char c): String(1, c) {}
    void push_back(char c);

    void pop_back();

    char& front();
    const char& front() const;

    char& back();
    const char& back() const;

    bool empty() const;

    void clear();

    void swap(String& s);

    int length() const;

    char* begin() const;

    char* end() const;

    size_t find(const String& substring) const;

    size_t rfind(const String& substring) const;

    String substr(int start, int count) const;


    String& operator+=(char c) {
        push_back(c);
        return *this;
    }

    String& operator+=(const String& s) {
        if (strSize <= (realSize + s.realSize))
            setNewSize(static_cast<size_t>((realSize + s.realSize) * EXP_COEF));
        memcpy(str + realSize, s.str, s.realSize);
        realSize += s.realSize;
        return *this;
    }

    String& operator=(const String& s) {
        if (this == &s) return *this;
        String newString = s;
        swap(newString);
        return *this;
    }


    const char& operator[](size_t index) const {
        return str[index];
    }
    char& operator[](size_t index) {
        return *(str + index);
    }
    bool operator==(const char* c) const {
        if (realSize != strlen(c)) return false;
        for (size_t i = 0; i < realSize; ++i) {
            if ((*this)[i] != c[i]) return false;
        }
        return true;
    }
};

void String::push_back(char c) {
    if (strSize == realSize)
        setNewSize(static_cast<size_t>(strSize * EXP_COEF));
    str[realSize++] = c;
}

void String::pop_back() {
    --realSize;
    if (realSize * EXP_COEF * EXP_COEF <= strSize) {
        setNewSize(static_cast<size_t>(strSize / EXP_COEF));
    }
}

char& String::front() {
    return *str;
}
const char& String::front() const {
    return *str;
}

char& String::back() {
    return *(str + realSize - 1);
}
const char& String::back() const {
    return *(str + realSize - 1);
}



bool String::empty() const {
    return realSize == 0;
}

void String::clear() {
    realSize = 0;
    setNewSize(MIN_STR_SIZE);
    return;
}

void String::swap(String& s) {
    std::swap(str, s.str);
    std::swap(strSize, s.strSize);
    std::swap(realSize, s.realSize);
    return;
}

int String::length() const {
    return realSize;
}

char* String::begin() const {
    return str;
}

char* String::end() const {
    return (str + realSize);
}

size_t String::find(const String& substring) const {
    for (size_t i = 0; i < realSize; ++i) {
        if (!memcmp(str + i, substring.str, substring.length())) return i;
    }
    return realSize;
}
size_t String::rfind(const String& substring) const {
    for (int i = static_cast<int>(realSize) - 1; i >= 0; --i) {
        if (!memcmp(str + i, substring.str, substring.length())) return i;
    }
    return realSize;
}
String String::substr(int start, int count) const {
    String result = String(count, 0);
    memcpy(result.str, str + start, count);
    return result;
}
String operator+(const String& left, const String& right) {
    String result = left;
    result += right;
    return result;
}

std::ostream& operator<<(std::ostream& out, const String& s) {
    for (int i = 0; i < s.length(); ++i) {
        out << s[i];
    }
    return out;
}

std::istream& operator>>(std::istream& in, String& s) {
    char current_symbol;

    in >> std::noskipws;
    s.clear();

    while (in >> current_symbol && current_symbol != ' '
           && current_symbol != '\n') {
        s.push_back(current_symbol);
    }
    return in;
}

//sorry for being inattentive

const size_t String::MIN_STR_SIZE = 4;
const double String::EXP_COEF = 2.0;

