#include <iostream>
#include <memory>
#include <list>

//namespace MyAllocator begin
namespace MyAllocator {


    const int const_size = 100;
    const int size_of_buffer = 28;

    template<size_t chunkSize = 1>
    class FixedAllocator {

    private:

        char* buffer_ = new char[1<<size_of_buffer];
        int pos_ = 0;

    public:

        FixedAllocator() = default;

        void* allocate(size_t n) {
            char* cur = buffer_ + pos_;
            pos_ += n * chunkSize;
            return reinterpret_cast<void*>(cur);
        }

        void deallocate(void*) {

        }

        ~FixedAllocator() {
            if (buffer_ == nullptr)
                return;
            delete[] buffer_;
        }
    };

    template <typename T>
    class FastAllocator {

    private:

        std::shared_ptr<FixedAllocator<>> pointer_;

    public:

        typedef T value_type;

        FastAllocator(): pointer_(std::make_shared<FixedAllocator<>>()) {}

        const std::shared_ptr<FixedAllocator<>>& get_pointer() const {
            return pointer_;
        }

        template<typename U>
        FastAllocator<T>(const FastAllocator<U>& another): pointer_(another.get_pointer()) {}

        ~FastAllocator() = default;

        T* allocate(size_t n) {
            if (n <= const_size) {
                return static_cast<T*>(pointer_->allocate(n * sizeof(T)));
            }
            return static_cast<T*>(::operator new(n * sizeof(T)));
        }

        void deallocate(T* pointer, size_t n) {
            if (n <= const_size) {
                pointer_->deallocate(pointer);
                return;
            }
            ::operator delete(pointer);
        }

        template <typename... Args>
        void construct(T* pointer, const Args&... args) {
            new (pointer) T(args...);
        }

        void destroy(T* pointer) {
            pointer->~T();
        }
    };

    template<typename F, typename S>
    bool operator==(const FastAllocator<F>&, const FastAllocator<S>&) {
        return false;
    }

    template<typename F>
    bool operator==(const FastAllocator<F>&, const FastAllocator<F>&) {
        return true;
    }

    template<typename F, typename S>
    bool operator!=(const FastAllocator<F>& a, const FastAllocator<S>& b) {
        return !(a == b);
    }


} // namespace MyAllocator end
/* */
using namespace MyAllocator;
/* */

//namespace MyList begin
namespace MyList {


    template <typename T, typename Allocator = MyAllocator::FastAllocator<T>>
    class List {

    private:

        struct node {
            node* next;
            node* prev;
            T data = 0;
            node() : next(nullptr), prev(nullptr), data() {}
            node(const T& x) : next(nullptr), prev(nullptr), data(x) {}
        };

        node* dummy_;
        int length_;

        using allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<node>;
        allocator_type allocator_;

        static const unsigned int LIST_MAX_SIZE = 1<<27;
    public:
//constructors:
        explicit List(const Allocator& alloc = Allocator()) : length_(0), allocator_(alloc) {
            dummy_ = std::allocator_traits<allocator_type>::allocate(allocator_, 1);
            dummy_->next = dummy_;
            dummy_->prev = dummy_;
        }

        List(size_t count, const T& value, const Allocator& alloc = Allocator()) : length_(count), allocator_(alloc) {
            dummy_ = std::allocator_traits<allocator_type>::allocate(allocator_, 1);
            dummy_->next = dummy_;
            dummy_->prev = dummy_;
            node* tmp = dummy_;
            for (size_t i = 0; i < count; ++i) {
                node* helper = std::allocator_traits<allocator_type>::allocate(allocator_, 1);
                std::allocator_traits<allocator_type>::construct(allocator_, helper, value);
                tmp->next = helper;
                helper->prev = tmp;
                tmp = helper;
            }
            dummy_->prev = tmp;
            tmp->next = dummy_;
        }
        List(size_t count, const Allocator& alloc = Allocator()) : length_(count), allocator_(alloc) {
            dummy_ = std::allocator_traits<allocator_type>::allocate(allocator_, 1);
            dummy_->next = dummy_;
            dummy_->prev = dummy_;
            node* tmp = dummy_;
            for (size_t i = 0; i < count; ++i) {
                node* helper = std::allocator_traits<allocator_type>::allocate(allocator_, 1);
                std::allocator_traits<allocator_type>::construct(allocator_, helper);
                tmp->next = helper;
                helper->prev = tmp;
                tmp = helper;
            }
            dummy_->prev = tmp;
            tmp->next = dummy_;
        }

        List(const List& other) : List(std::allocator_traits<Allocator>::select_on_container_copy_construction(other.get_allocator())) {
            clear();
            for (const_iterator it = other.begin(); it != other.end(); ++it) {
                push_back(*it);
            }
        }

//destructor:
        ~List() {
            clear();
            std::allocator_traits<allocator_type>::deallocate(allocator_, dummy_, 1);
        }
//operators:
        List& operator=(const List& other) {
            if (this == &other)
                return *this;
            if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value
                && allocator_ != other.get_allocator()) {
                allocator_ = other.get_allocator();
            }
            List temp = other;
            std::swap(dummy_, temp.dummy_);
            std::swap(length_, temp.length_);
            return *this;
        }
//iterators:
        template<bool is_constant = false>
        class BasicIterator {

            friend List;

        public:
            //flags:
            using value_type = typename std::conditional<is_constant, const T, T>::type;
            using pointer = typename std::conditional<is_constant, const T*, T*>::type;
            using reference = typename std::conditional<is_constant, const T&, T&>::type;
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
        private:

            node* node_pointer_;
            //constructor from node:
            BasicIterator(node* node_pointer) : node_pointer_(node_pointer) {}

        public:
            //iterator constructors:
            BasicIterator(const BasicIterator& a) : node_pointer_(a.node_pointer_) {}

            operator BasicIterator<true>() const {
                return BasicIterator<true>(node_pointer_);
            }

            BasicIterator() = default;
            //operators:
            bool operator==(const BasicIterator& other_iterator) const {
                return node_pointer_ == other_iterator.node_pointer_;
            }

            bool operator!=(const BasicIterator& another_iterator) const {
                return !(*this == another_iterator);
            }

            BasicIterator& operator++() {
                node_pointer_ = node_pointer_->next;
                return *this;
            }

            BasicIterator& operator--() {
                node_pointer_ = node_pointer_->prev;
                return *this;
            }

            BasicIterator operator++(int) {
                BasicIterator copy = *this;
                node_pointer_ = node_pointer_->next;
                return copy;
            }

            BasicIterator operator--(int) {
                BasicIterator copy = *this;
                node_pointer_ = node_pointer_->prev;
                return copy;
            }

            reference operator*() const {
                return node_pointer_->data;
            }

            pointer operator->() const {
                return &node_pointer_->data;
            }
        };

        template<bool is_constant = true>
        class BasicReverseIterator {

            friend List;

        public:
            //flags:
            using value_type = typename std::conditional<is_constant, const T, T>::type;
            using pointer = typename std::conditional<is_constant, const T*, T*>::type;
            using reference = typename std::conditional<is_constant, const T&, T&>::type;
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
        private:

            node* node_pointer_;
            BasicReverseIterator(node* node_pointer) : node_pointer_(node_pointer) {}

        public:
            //reverse_iterator constructors:
            BasicReverseIterator() = default;

            BasicReverseIterator(const BasicReverseIterator& a) : node_pointer_(a.node_pointer_) {}

            BasicReverseIterator(const BasicIterator<is_constant>& a) : node_pointer_(a.node_pointer_) {}

            operator BasicReverseIterator<true>() const {
                BasicReverseIterator<true> const_it = node_pointer_;
                return const_it;
            }

            operator BasicIterator<true>() const {
                BasicIterator<true> it = node_pointer_;
                return it;
            }

            BasicIterator<is_constant> base() const {
                BasicIterator a = node_pointer_->next;
                return a;
            }

            //operators:
            bool operator==(const BasicReverseIterator& other_iterator) const {
                return node_pointer_ == other_iterator.node_pointer_;
            }

            bool operator!=(const BasicReverseIterator& other_iterator) const {
                return !(*this == other_iterator);
            }

            BasicReverseIterator& operator++() {
                node_pointer_ = node_pointer_->prev;
                return *this;
            }

            BasicReverseIterator& operator--() {
                node_pointer_ = node_pointer_->next;
                return *this;
            }

            BasicReverseIterator operator++(int) {
                BasicReverseIterator copy = *this;
                node_pointer_ = node_pointer_->prev;
                return copy;
            }

            BasicReverseIterator operator--(int) {
                BasicReverseIterator copy = *this;
                node_pointer_ = node_pointer_->next;
                return copy;
            }

            reference operator*() const {
                return node_pointer_->data;
            }

            pointer operator->() const {
                return &node_pointer_->data;
            }
        };
    public: //named requirements : List (as a container)
        using value_type = T;
        using reference = T&;
        using const_reference = const T&;
        //types of iterators in List:
        using iterator = BasicIterator<false>;
        using const_iterator = BasicIterator<true>;
        using reverse_iterator = BasicReverseIterator<false>;
        using const_reverse_iterator = BasicReverseIterator<true>;
        unsigned int difference_type = typename std::iterator_traits<iterator>::difference_type();
        unsigned int size_type = LIST_MAX_SIZE;
        //methods dependent on iterators:
        iterator begin() {
            return iterator(dummy_->next);
        }

        iterator end() {
            return iterator(dummy_);
        }

        const_iterator begin() const {
            return const_iterator(dummy_->next);
        }

        const_iterator end() const {
            return const_iterator(dummy_);
        }

        const_iterator cbegin() const {
            return const_iterator(dummy_->next);
        }

        const_iterator cend() const {
            return const_iterator(dummy_);
        }

        reverse_iterator rbegin() {
            return reverse_iterator(dummy_->prev);
        }

        const_reverse_iterator rbegin() const {
            return const_reverse_iterator(dummy_->prev);
        }

        const_reverse_iterator rend() const {
            return const_reverse_iterator(dummy_);
        }

        const_reverse_iterator crbegin() const {
            return const_reverse_iterator(dummy_->prev);
        }

        const_reverse_iterator crend() const {
            return const_reverse_iterator(dummy_);
        }

        void insert(const_iterator it, const T& a) {
            node* insert_node = std::allocator_traits<allocator_type>::allocate(allocator_, 1);
            std::allocator_traits<allocator_type>::construct(allocator_, insert_node, a);
            node* right = it.node_pointer_;
            node* left = it.node_pointer_->prev;
            insert_node->next = right;
            right->prev = insert_node;
            left->next = insert_node;
            insert_node->prev = left;
            ++length_;
        }

        void erase(const_iterator it) {
            node* right = it.node_pointer_->next;
            node* left = it.node_pointer_->prev;
            right->prev = left;
            left->next = right;
            std::allocator_traits<allocator_type>::destroy(allocator_, it.node_pointer_);
            std::allocator_traits<allocator_type>::deallocate(allocator_, it.node_pointer_, 1);
            --length_;
        }

        void clear() {
            while (size()) {
                erase(begin());
            }
        }

        Allocator get_allocator() const {
            return allocator_;
        }

        void push_back(const T& a) {
            insert(end(), a);
        }
        void push_front(const T& a) {
            insert(begin(), a);
        }

        void pop_back() {
            erase(rbegin());
        }

        void pop_front() {
            erase(begin());
        }

        size_t size() const {
            return length_;
        }

        bool empty() const {
            return !static_cast<bool>(size());
        }
    };


}// namespace MyList end

/* */
using namespace MyList;
/* */

