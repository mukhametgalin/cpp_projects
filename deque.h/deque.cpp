#include <iostream>
#include <vector>
#include <assert.h>

template <typename T>
class Deque {

friend class iterator;
friend class const_iterator;
private:
    std::vector<T> buffer;
    int begin_index;
    int end_index;
    int shift = 0;

    void expand_buffer() {
        int old_size = std::max(1, (end_index - begin_index));
        std::vector<T> new_buffer(old_size * 3);
        for (int i = begin_index; i < end_index; ++i) {
            new_buffer[old_size + i - begin_index] = buffer[i];
        }
        int help = end_index - begin_index;
        begin_index = old_size;
        end_index = begin_index + help;
        buffer.swap(new_buffer);
    }

public:

    Deque(): buffer(3), begin_index(1), end_index(1) , shift(0) {}

    Deque(const Deque& d1): buffer(d1.buffer), begin_index(d1.begin_index), end_index(d1.end_index), shift(0) {}

    Deque(int size) {
        size = std::max(size, 1);
        buffer = std::vector<T>(size * 3);
        begin_index = size;
        end_index = size * 2;
        shift = 0;
    }

    Deque(int size, T t) {
        size = std::max(size, 1);
        buffer = std::vector<T>(size * 3, t);
        begin_index = size;
        end_index = size * 2;
        shift = 0;
    }

    Deque& operator=(const Deque& d) {
        buffer = d.buffer;
        begin_index = d.begin_index;
        end_index = d.end_index;
        shift = d.shift;
        return *this;
    }

    size_t size() const {
        return end_index - begin_index;
    }

    T& operator[](int index) {
        return buffer[begin_index + index];
    }

    const T& operator[](int index) const {
        return buffer[begin_index + index];
    }

    T& at(int index) {
        if (index < 0 || index >= static_cast<int>(size())) {
            throw std::out_of_range("A");
        }
        return buffer[begin_index + index];
    }

    const T& at(int index) const {
        if (index < 0 || index >= static_cast<int>(size())) {
            throw std::out_of_range("A");
        }
        return buffer[begin_index + index];
    }

    void push_back(const T& x) {
        if (end_index >= static_cast<int>(buffer.size())) {
            expand_buffer();
        }
        buffer[end_index] = x;
        ++end_index;
    }

    void push_front(const T& x) {
        if (begin_index <= 0) {
            expand_buffer();
        }
        --begin_index;
        ++shift;
        buffer[begin_index] = x;
    }

    void pop_back() {
        --end_index;
    }

    void pop_front() {
        ++begin_index;
        --shift;
    }

    class iterator {
        friend class Deque;
    private:
        Deque* owner = nullptr;
        int index = 0;
    public:
        iterator(): owner(nullptr), index() {}

        iterator(const iterator& a): owner(a.owner), index(a.index) {}

        iterator(const Deque<T>* a, int index): owner(a), index(index) {}

        iterator(Deque<T>* a, int index): owner(a), index(index) {}

        iterator& operator++() {
            ++index;
            return *this;
        }

        iterator operator++(int) {
            iterator copy = *this;
            ++index;
            return copy;
        }

        iterator& operator--() {
            --index;
            return *this;
        }

        iterator operator--(int) {
            iterator copy = *this;
            --index;
            return copy;
        }

        bool operator<(const iterator& a) const {
            return index < a.index;
        }

        bool operator>(const iterator& a) const {
            return index > a.index;
        }

        bool operator<=(const iterator& a) const {
            return index <= a.index;
        }

        bool operator>=(const iterator& a) const {
            return index >= a.index;
        }

        bool operator==(const iterator& a) const {
            return owner == a.owner && index == a.index;
        }

        bool operator!=(const iterator& a) const {
            return owner != a.owner || index != a.index;
        }

        T& operator*() const {
            return owner->buffer.at(owner->begin_index + owner->shift + index);
        }

        T* operator->() const {
            return &(owner->buffer.at(owner->begin_index + owner->shift + index));
        }

        iterator& operator+=(int x) {
            index += x;
            return *this;
        }

        iterator& operator-=(int x) {
            index -= x;
            return *this;
        }

        iterator operator+(int x) const {
            iterator a = *this;
            a += x;
            return a;
        }

        iterator operator-(int x) const {
            iterator a = *this;
            a -= x;
            return a;
        }

        int operator-(iterator& a) const {
            return index - a.index;
        }
    };

    class const_iterator {
    private:
        const Deque* owner = nullptr;
        int index = 0;
    public:
        const_iterator(): owner(nullptr), index() {}

        const_iterator(iterator& a): owner(a.owner), index(a.index) {}

        const_iterator(const const_iterator& a): owner(a.owner), index(a.index) {}

        const_iterator(const Deque<T>* a, int index): owner(a), index(index) {}

        const_iterator(Deque<T>* a, int index): owner(a), index(index) {}

        const_iterator& operator++() {
            ++index;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator copy = *this;
            ++index;
            return copy;
        }

        const_iterator& operator--() {
            --index;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator copy = *this;
            --index;
            return copy;
        }
        const T* operator->() const {
            return &(owner->buffer[owner->begin_index + owner->shift + index]);
        }

        bool operator<(const const_iterator& a) const {
            return index < a.index;
        }

        bool operator>(const const_iterator& a) const {
            return index > a.index;
        }

        bool operator<=(const const_iterator& a) const {
            return index <= a.index;
        }

        bool operator>=(const const_iterator& a) const {
            return index >= a.index;
        }

        bool operator==(const const_iterator& a) const {
            return this.index == a.index;
        }

        bool operator!=(const const_iterator& a) const {
            return index != a.index;
        }

        const T& operator*() const {
            return owner->buffer[owner->begin_index + owner->shift + index];
        }



        const_iterator& operator+=(int x) {
            index += x;
            return *this;
        }

        const_iterator& operator-=(int x) {
            index -= x;
            return *this;
        }

        const_iterator operator+(int x) const {
            const_iterator a = *this;
            a += x;
            return a;
        }

        const_iterator operator-(int x) const {
            const_iterator a = *this;
            a -= x;
            return a;
        }

        int operator-(const_iterator& a) const {
            return index - a.index;
        }
    };

    iterator begin() {
        iterator a(this, -shift);
        return a;
    }

    const_iterator begin() const {
        const_iterator a(this, -shift);
        return a;
    }

    const_iterator cbegin() const {
        const_iterator a(this, -shift);
        return a;
    }

    iterator end() {
        iterator a(this, end_index - begin_index - shift);
        return a;
    }

    const_iterator end() const {
        const_iterator a(this, end_index - begin_index - shift);
        return a;
    }

    const_iterator cend() const {
        const_iterator a(this, end_index - begin_index - shift);
        return a;
    }

    void insert(iterator pos, T a) {
        push_back(a);
        for (int i = end_index - 2; i >= begin_index + shift + pos.index; --i) {
            buffer[i + 1] = buffer[i];
        }
        *pos = a;
    }

    void erase(iterator pos) {
        assert(pos != end());
        for (int i = begin_index + shift + pos.index; i < end_index - 1; ++i) {
            buffer[i] = buffer[i + 1];
        }
        pop_back();
    }
};

