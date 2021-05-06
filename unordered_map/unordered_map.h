#include <iostream>
#include <memory>
#include <vector>

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
using namespace MyAllocator;

//namespace MyList begin
namespace MyList {


    template <typename T, typename Allocator = std::allocator<T>>
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

        List(const List& other) : List(std::allocator_traits<Allocator>::
                                       select_on_container_copy_construction(other.get_allocator())) {
            clear();
            for (const_iterator it = other.begin(); it != other.end(); ++it) {
                push_back(*it);
            }
        }

        List(List&& other) {
            dummy_ = other.dummy_;
            other.dummy_ = std::allocator_traits<allocator_type>::allocate(allocator_, 1);
            other.dummy_->prev = other.dummy_;
            other.dummy_->next = other.dummy_;
            length_ = other.length_;
            other.length_ = 0;
            if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
                allocator_ = other.allocator_;
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
            clear();
            if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value
                && allocator_ != other.get_allocator()) {
                allocator_ = other.get_allocator();
            }
            List temp = other;
            std::swap(dummy_, temp.dummy_);
            std::swap(length_, temp.length_);
            return *this;
        }

        void swap(List&& other) {
            dummy_ = other.dummy_;
            other.dummy_ = std::allocator_traits<allocator_type>::allocate(allocator_, 1);
            other.dummy_->prev = other.dummy_;
            other.dummy_->next = other.dummy_;
            length_ = other.length_;
            other.length_ = 0;
            if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
                allocator_ = other.allocator_;
            }
        }

        void swap(List& other) {
            dummy_ = other.dummy_;
            other.dummy_ = std::allocator_traits<allocator_type>::allocate(allocator_, 1);
            other.dummy_->prev = other.dummy_;
            other.dummy_->next = other.dummy_;
            length_ = other.length_;
            other.length_ = 0;
            if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
                allocator_ = other.allocator_;
            }
        }

        List& operator=(List&& other) {
            if (this == &other)
                return *this;
            swap(std::move(other));
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
                if (node_pointer_ == nullptr) {
                }
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

        iterator insert(const_iterator it, const T& a) {
            node* insert_node = std::allocator_traits<allocator_type>::allocate(allocator_, 1);
            std::allocator_traits<allocator_type>::construct(allocator_, insert_node, a);
            node* right = it.node_pointer_;
            node* left = it.node_pointer_->prev;
            insert_node->next = right;
            right->prev = insert_node;
            left->next = insert_node;
            insert_node->prev = left;
            ++length_;
            return iterator(insert_node);
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

using namespace MyList;

#include <list>


#include <assert.h>
#include <math.h>


template <
        typename Key,
        typename Value,
        typename Hash = std::hash<Key>,
        typename Equal = std::equal_to<Key>,
        typename Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
public:
    using NodeType = std::pair<const Key, Value>;

private:
    using listAllocType = typename std::allocator_traits<Alloc>::template rebind_alloc<NodeType*>;
    using listIterator = typename List<NodeType*, listAllocType>::const_iterator;
    using listIteratorAllocType = typename std::allocator_traits<Alloc>::template rebind_alloc<listIterator>;
    Hash hashFunc_;
    Equal equalFunction_;
    Alloc allocator;

    List<NodeType*, listAllocType> globalList_;
    std::vector<listIterator, listIteratorAllocType> hashTable_;

    int sz_ = 0;

    float maxLoadFactorValue = 0.6;
    int hashTableSize_ = 1;
public:
    Alloc get_allocator() const {
        return allocator;
    }

    //BasicIterator:____________________________________________________________________________________________________

    template <bool isConstant>
    class BasicIterator {
        friend class UnorderedMap;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = NodeType;
        using difference_type = int;
        using pointer = typename std::conditional<isConstant, const NodeType*, NodeType *>::type;
        using reference = typename std::conditional<isConstant, const NodeType&, NodeType&>::type;

    private:
        listIterator it;

    public:
        BasicIterator(listIterator a) : it(a) {}

        //iterator constructors:
        BasicIterator(const BasicIterator& a) : it(a.it) {}

        operator BasicIterator<true>() const {
            return BasicIterator<true>(it);
        }

        listIterator getListIterator() const {
            return it;
        }

        BasicIterator() = default;
        //operators:
        bool operator==(const BasicIterator& otherIterator) const {
            return it == otherIterator.it;
        }

        bool operator!=(const BasicIterator& otherIterator) const {
            return it != otherIterator.it;
        }

        BasicIterator& operator++() {
            ++it;
            return *this;
        }

        BasicIterator& operator--() {
            --it;
            return *this;
        }

        BasicIterator operator++(int) {
            BasicIterator copy = *this;
            ++(*this);
            return copy;
        }

        BasicIterator operator--(int) {
            BasicIterator copy = *this;
            --(*this);
            return copy;
        }

        reference operator*() const {
            return *(*it);
        }

        pointer operator->() const {
            return *it;
        }
    };


    using Iterator = BasicIterator<false>;
    using ConstIterator = BasicIterator<true>;

//constructor and destructor:___________________________________________________________________________________________

    UnorderedMap() : hashTable_(1), sz_(0) {
        for (int i = 0; i < 1; ++i) {
            hashTable_[i] = globalList_.end();
        }
    }

    //copy constructor
    UnorderedMap(const UnorderedMap& other) : hashFunc_(other.hashFunc_),
                                              equalFunction_(other.equalFunction_),
                                              allocator(std::allocator_traits<Alloc>::
                                                        select_on_container_copy_construction(other.allocator)),
                                              sz_(0), maxLoadFactorValue(other.maxLoadFactorValue) {
        hashTableSize_ = 10;
        hashTable_.resize(10);
        for(int i = 0; i < 10; ++i) {
            hashTable_[i] = globalList_.end();
        }
        for (auto it = other.begin(); it != other.end(); ++it) {
            insert(*it);
        }
    }

    //move constructor
    UnorderedMap(UnorderedMap&& other) : hashFunc_(std::move(other.hashFunc_)),
                                         equalFunction_(std::move(other.equalFunction_)),
                                         globalList_(std::move(other.globalList_)),
                                         hashTable_(std::move(other.hashTable_)),
                                         sz_(other.sz_), maxLoadFactorValue(other.maxLoadFactorValue) {
        if (std::allocator_traits<Alloc>::propagate_on_container_move_assignment::value) {
            allocator = other.allocator;
        }
        other.sz_ = 0;
    }

//operators:____________________________________________________________________________________________________________

    void swap(UnorderedMap&& other) {
        if (std::allocator_traits<Alloc>::propagate_on_container_copy_assignment::value) {
            allocator = other.allocator;
        }
        hashFunc_ = std::move(other.hashFunc_);
        hashTable_ = std::move(other.hashTable_);
        equalFunction_ = std::move(other.equalFunction_);
        for (auto it = globalList_.begin(); it != globalList_.end(); ++it) {
            std::allocator_traits<Alloc>::destroy(allocator, (*it));
            std::allocator_traits<Alloc>::deallocate(allocator, (*it), 1);
        }
        globalList_.clear();
        globalList_ = std::move(other.globalList_);
        sz_ = other.sz_;
        maxLoadFactorValue = other.maxLoadFactorValue;
    }

    UnorderedMap& operator=(const UnorderedMap& other) {
        if (this == &other)
            return *this;
        UnorderedMap copy = other;
        *this = std::move(copy);
        return *this;
    }

    //move operator=
    UnorderedMap& operator=(UnorderedMap&& other) {
        if (this == &other)
            return *this;
        swap(std::move(other));
        return *this;
    }

    //destructor
    ~UnorderedMap() {
        for (auto it = globalList_.begin(); it != globalList_.end(); ++it) {
            std::allocator_traits<Alloc>::destroy(allocator, (*it));
            std::allocator_traits<Alloc>::deallocate(allocator, (*it), 1);
        }
        globalList_.clear();
        hashTable_.clear();
        hashTable_.shrink_to_fit();
    }

    //operator []
    Value& operator[](const Key& key) {
        try {
            return at(key);
        } catch(std::out_of_range& exc) {
            std::pair<Iterator, bool> x = insert(std::make_pair(key, Value()));
            return x.first->second;
        }
    }

    //operator at
    Value& at(const Key& key) {
        Iterator it = find(key);
        if (it == end()) {
            throw std::out_of_range("out of range");
        }
        return it->second;
    }

    //operator size
    size_t size() const {
        return sz_;
    }

//Iterators methods:____________________________________________________________________________________________________

    Iterator begin() {
        return Iterator(globalList_.begin());
    }

    ConstIterator begin() const {
        return ConstIterator(globalList_.cbegin());
    }

    ConstIterator cbegin() const {
        return ConstIterator(globalList_.cbegin());
    }
    Iterator end() {
        return Iterator(globalList_.end());
    }

    ConstIterator end() const {
        return ConstIterator(globalList_.cend());
    }
    ConstIterator cend() const {
        return ConstIterator(globalList_.cend());
    }

    template <typename... Args>
    std::pair<Iterator, bool> emplace(Args&&... args) {
        check_load();
        NodeType* pointer = std::allocator_traits<Alloc>::allocate(allocator, 1);
        std::allocator_traits<Alloc>::construct(allocator, pointer, std::forward<Args>(args)...);
        if (pointer == nullptr)
            return {end(), false};
        auto find_it = find(pointer->first);
        if (find_it == end()) {
            auto h = hashFunc_(pointer->first) % hashTableSize_;
            hashTable_[h] = globalList_.insert(hashTable_[h], std::move(pointer));
            ++sz_;
            return std::make_pair(Iterator(hashTable_[h]), true);
        } else {
            return std::make_pair(find_it, false);
        }
    }

    std::pair<Iterator, bool> insert(const NodeType& x) {
        check_load();
        auto find_it = find(x.first);
        if (find_it != end()) {
            return std::make_pair(find_it, false);
        }
        NodeType *pointer = std::allocator_traits<Alloc>::allocate(allocator, 1);
        std::allocator_traits<Alloc>::construct(allocator, pointer, x);
        auto h = hashFunc_(pointer->first) % hashTableSize_;
        hashTable_[h] = globalList_.insert(hashTable_[h], pointer);
        ++sz_;
        return std::make_pair(Iterator(hashTable_[h]), true);
    }

    template <typename InputIterator>
    void insert(InputIterator first, InputIterator second) {
        for (auto it = first; it != second; ++it) {
            insert(*it);
        }
    }

    template<typename T>
    std::pair<Iterator, bool> insert(T&& x) {
        check_load();
        auto find_it = find(x.first);
        if (find_it != end()) {
            return std::make_pair(find_it, false);
        }
        NodeType *pointer = std::allocator_traits<Alloc>::allocate(allocator, 1);
        std::allocator_traits<Alloc>::construct(allocator, pointer, std::forward<T>(x));
        auto h = hashFunc_(pointer->first) % hashTableSize_;
        hashTable_[h] = globalList_.insert(hashTable_[h], pointer);
        ++sz_;
        return std::make_pair(Iterator(hashTable_[h]), true);
    }

    Iterator erase(ConstIterator iter) {
        if (iter == end())
            return end();
        listIterator it = iter.getListIterator();
        auto ret = ++it;
        --it;
        auto h = hashFunc_((*it)->first) % hashTableSize_;
        if (hashTable_[h] == it) {
            if (ret != globalList_.end()) {
                auto h_ret = hashFunc_((*ret)->first) % hashTableSize_;
                if (h_ret == h) {
                    hashTable_[h] = ret;
                } else {
                    hashTable_[h] = globalList_.end();
                }
            } else {
                hashTable_[h] = globalList_.end();
            }
        }
        globalList_.erase(it);
        --sz_;
        return Iterator(ret);
    }


    ConstIterator erase(ConstIterator left, ConstIterator right) {
        ConstIterator ret;
        ConstIterator last = right;
        --last;
        for (auto it = left; it != right; it = erase(it)) {
            if (it == last)
                return erase(it);
        }
        return end();
    }

    Iterator find(const Key& key) {
        if (hashTableSize_ != static_cast<int>(hashTable_.size())) {
            assert(false);
        }
        auto h = hashFunc_(key) % hashTableSize_;
        if (hashTable_[h] == globalList_.end())
            return end();
        auto it = hashTable_[h];
        while (it != globalList_.end() && hashFunc_((*it)->first) % hashTableSize_ == h) {
            if (equalFunction_((*it)->first, key)) {
                return it;
            }
            ++it;
        }
        return end();
    }

    void rehash(size_t count) {
        hashTable_.clear();
        List<NodeType*, listAllocType> copy = std::move(globalList_);

        hashTable_.resize(count);
        hashTableSize_ = count;
        for (int i = 0; i < hashTableSize_; ++i) {
            hashTable_[i] = globalList_.end();
        }


        for (listIterator it = copy.begin(); it != copy.end(); ++it) {
            if (hashTable_[hashFunc_((*it)->first) % hashTableSize_] == globalList_.end()) {
                hashTable_[hashFunc_((*it)->first) % hashTableSize_] =
                        globalList_.insert(globalList_.end(), *it);
            }
        }
    }

    void reserve(size_t count) {
        if (static_cast<int>(count) > hashTableSize_) {
            rehash(static_cast<int>(std::ceil(static_cast<float>(count) / max_load_factor())));
        }
    }

    float load_factor() const {
        return static_cast<float>(sz_) / hashTableSize_;
    }

    float max_load_factor() const {
        return maxLoadFactorValue;
    }

    void check_load() {
        if (load_factor() > max_load_factor())
            reserve(hashTableSize_ * 2);
    }
};


