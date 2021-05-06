#include <iostream>
#include <memory>
#include <optional>


template <typename T, typename Allocator = std::allocator<T>>
struct MDeleter {

  MDeleter() = default;

  void operator()(T* a) const {
    Allocator alloc;
    std::allocator_traits<Allocator>::destroy(alloc, a);
    std::allocator_traits<Allocator>::deallocate(alloc, a, 1);
  }
};

struct ControlBlockBase {
  virtual size_t& getSharedCounter() = 0;
  virtual size_t& getWeakCounter() = 0;
  virtual void finish() = 0;
  virtual void deleter(void*) = 0;
  virtual void destroy_object() = 0;
};

template<typename U, typename Allocator = std::allocator<U>>
class ControlBlockDerived : public ControlBlockBase {
 public:
  U object;
  size_t sharedCounter;
  size_t weakCounter;
  Allocator uAlloc;
  MDeleter<U> uDeleter;

  using CBType = ControlBlockDerived<U, Allocator>;
  using CBAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<CBType>;
  using CBAllocTraits = std::allocator_traits<CBAlloc>;

  void deleter(void* ptr) override {
    uDeleter.operator()(static_cast<U*>(ptr));
  }

  void destroy_object() override {
    std::allocator_traits<Allocator>::destroy(uAlloc, &object);
  }
  template <typename... Args>
  ControlBlockDerived(const Allocator& allocator, Args&&... args) :
      object(std::forward<Args>(args)...), sharedCounter(1), weakCounter(0), uAlloc(allocator), uDeleter() {}

  size_t& getSharedCounter() override {
    return sharedCounter;
  }

  size_t& getWeakCounter() override {
    return weakCounter;
  }

  void finish() override {
    CBAlloc copy(uAlloc);
    CBAllocTraits::deallocate(copy, this, 1);
  }
};
template<typename U, typename Deleter = MDeleter<U, std::allocator<U>> ,typename Allocator = std::allocator<U> >
class ControlBlockDerived2 : public ControlBlockBase {
 public:
  size_t sharedCounter;
  size_t weakCounter;
  Allocator uAlloc;
  Deleter uDeleter;

  using CBType = ControlBlockDerived2<U, Deleter, Allocator>;
  using CBAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<CBType>;
  using CBAllocTraits = std::allocator_traits<CBAlloc>;

  void deleter(void* ptr) override {
    uDeleter.operator()(static_cast<U*>(ptr));
  }
  void destroy_object() override {}
  ControlBlockDerived2() :
      sharedCounter(1), weakCounter(0), uAlloc(), uDeleter() {}

  ControlBlockDerived2(const Deleter& deleter) :
      sharedCounter(1), weakCounter(0), uAlloc(), uDeleter(deleter) {}
  ControlBlockDerived2(const Deleter& deleter, const Allocator& allocator) :
      sharedCounter(1), weakCounter(0), uAlloc(allocator), uDeleter(deleter) {}

  size_t& getSharedCounter() override {
    return sharedCounter;
  }

  size_t& getWeakCounter() override {
    return weakCounter;
  }

  void finish() override {
    CBAlloc copy(uAlloc);
    CBAllocTraits::deallocate(copy, this, 1);
  }
};

template <typename U>
class WeakPtr;

template <typename T>
class SharedPtr {
  template<typename U, typename... Args>
  friend SharedPtr<U> makeShared(Args&&...);

  template <typename U, typename Allocator, typename... Args>
  friend SharedPtr<U> allocateShared(Allocator, Args&&...);

  template <typename U>
  friend class WeakPtr;

  template <typename U>
  friend class SharedPtr;

 private:
  T* ptr = nullptr;
  ControlBlockBase* cptr = nullptr;

  template <typename U>
  SharedPtr(const WeakPtr<U>& other) : ptr(other.ptr), cptr(other.cptr) {
    if (cptr)
      cptr->getSharedCounter() += 1;
  }


 public:
  template <typename Y, typename Allocator = std::allocator<Y>>
  explicit SharedPtr(ControlBlockDerived<Y, Allocator>* cptr) : cptr(cptr) {}
  T* get() const {
    if (ptr)
      return ptr;
    return reinterpret_cast<T*>(cptr);
  }
  void swap(SharedPtr& other) {
    std::swap(ptr, other.ptr);
    std::swap(cptr, other.cptr);
  }

  SharedPtr() : ptr(nullptr), cptr(nullptr) {}

  template<class Y>
  explicit SharedPtr(Y* ptr_) : ptr(ptr_) {
    using CBType = ControlBlockDerived2<Y, MDeleter<Y>, std::allocator<Y> >;
    using CBAllocTraits = std::allocator_traits<std::allocator<CBType>>;
    std::allocator<CBType> alloc;
    auto mem = CBAllocTraits::allocate(alloc, 1);
    cptr = mem;
    new(cptr) CBType();
  }

  template<class Y, class Deleter>
  SharedPtr(Y* ptr_, Deleter d) : ptr(ptr_) {
    using CBType = ControlBlockDerived2<Y, Deleter, std::allocator<Y> >;
    using CBAllocTraits = std::allocator_traits<std::allocator<CBType>>;
    std::allocator<CBType> alloc;
    auto mem = CBAllocTraits::allocate(alloc, 1);
    cptr = mem;
    new(cptr) CBType(d);
  }

  template<class Y, class Deleter, class Allocator>
  SharedPtr(Y* ptr_, Deleter d, Allocator allocator) : ptr(ptr_) {
    using CBType = ControlBlockDerived2<Y, Deleter, Allocator>;
    using CBAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<CBType>;
    using CBAllocTraits = std::allocator_traits<CBAlloc>;
    CBAlloc alloc(allocator);
    auto mem = CBAllocTraits::allocate(alloc, 1);
    cptr = mem;
    new(cptr) CBType(d, alloc);
  }

  template<typename U>
  SharedPtr(const SharedPtr<U>& other) : ptr(other.ptr), cptr(other.cptr) {
    if (cptr)
      cptr->getSharedCounter() += 1;
  }

  SharedPtr(const SharedPtr<T>& other) : ptr(other.ptr), cptr(other.cptr) {
    if (cptr)
      cptr->getSharedCounter() += 1;
  }

  template<typename U>
  SharedPtr& operator=(const SharedPtr<U>& other) {
    SharedPtr copy = other;
    this->swap(copy);
    return *this;
  }

  SharedPtr& operator=(const SharedPtr<T>& other) {
    SharedPtr copy = other;
    this->swap(copy);
    return *this;
  }

  template <typename U>
  SharedPtr(SharedPtr<U>&& other) : ptr(other.ptr), cptr(other.cptr) {
    other.ptr = nullptr;
    other.cptr = nullptr;
  }

  SharedPtr(SharedPtr<T>&& other) : ptr(other.ptr), cptr(other.cptr) {
    other.ptr = nullptr;
    other.cptr = nullptr;
  }

  template <typename U>
  SharedPtr& operator=(SharedPtr<U>&& other) {
    SharedPtr move_copy = std::move(other);
    this->swap(move_copy);
    return *this;
  }

  SharedPtr& operator=(SharedPtr<T>&& other) {
    SharedPtr move_copy = std::move(other);
    this->swap(move_copy);
    return *this;
  }

  T& operator*() const {
    if (ptr)
      return *ptr;
    return static_cast<ControlBlockDerived<T>*>(cptr)->object;
  }

  T* operator->() const {
    if (ptr)
      return ptr;
    return &(static_cast<ControlBlockDerived<T>*>(cptr)->object);
  }

  size_t use_count() const {
    if (cptr)
      return cptr->getSharedCounter();
    return 0;
  }
  ~SharedPtr() {
    //std::cout << "1" << std::endl;
    if (!cptr)
      return;
    --(cptr->getSharedCounter());
    if (use_count() > 0)
      return;

    //std::cout << "2" << std::endl;
    if (ptr) {
      cptr->deleter(ptr);
    } else {

      cptr->destroy_object();
    }

    // std::cout << "3" << std::endl;
    if (cptr->getWeakCounter() == 0) {
      cptr->finish();
      cptr = nullptr;
    }
    //std::cout << "4" << std::endl;
  }


  void reset() {
    SharedPtr<T>().swap(*this);
  }

  template <typename Y>
  void reset(Y* ptr) {
    SharedPtr<T>(ptr).swap(*this);
  }

  template <typename Y, typename Deleter>
  void reset(Y* ptr, Deleter deleter) {
    SharedPtr<T>(ptr, deleter).swap(*this);
  }

  template <typename Y, typename Deleter, typename Allocator>
  void reset(Y* ptr, Deleter deleter, Allocator allocator) {
    SharedPtr<T>(ptr, deleter, allocator).swap(*this);
  }


};




template <typename T, typename Allocator = std::allocator<T>, typename... Args>
SharedPtr<T> allocateShared(Allocator alloc, Args&&... args) {
  using CBType = ControlBlockDerived<T, Allocator>;
  using CBAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<CBType>;
  using CBAllocTraits = std::allocator_traits<CBAlloc>;

  CBAlloc localAlloc;

  ControlBlockDerived<T, Allocator>* storage = CBAllocTraits::allocate(localAlloc, 1);

  CBAllocTraits::construct(localAlloc, storage, alloc, std::forward<Args>(args)...);

  return SharedPtr<T>(storage);
}
template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
  return allocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}
template <typename T>
class WeakPtr {
  template <typename U>
  friend class WeakPtr;

  template <typename U>
  friend class SharedPtr;

 private:

  T* ptr = nullptr;
  ControlBlockBase* cptr = nullptr;

 public:

  WeakPtr() : ptr(nullptr), cptr(nullptr) {}
  template <typename U>
  WeakPtr(const SharedPtr<U>& a) : ptr(a.ptr), cptr(a.cptr) {
    if (cptr)
      cptr->getWeakCounter() += 1;
  }

  WeakPtr(const SharedPtr<T>& a) : ptr(a.ptr), cptr(a.cptr) {
    if (cptr) {
      cptr->getWeakCounter() += 1;
    }
  }
  template <typename U>
  WeakPtr(const WeakPtr<U>& a) : ptr(a.ptr), cptr(a.cptr) {
    if (cptr)
      cptr->getWeakCounter() += 1;
  }
  WeakPtr(const WeakPtr<T>& a) : ptr(a.ptr), cptr(a.cptr) {
    if (cptr)
      cptr->getWeakCounter() += 1;
  }

  void swap(WeakPtr& a) {
    std::swap(ptr, a.ptr);
    std::swap(cptr, a.cptr);
  }
  template<typename U>
  WeakPtr& operator=(const SharedPtr<U>& other) {
    WeakPtr copy = other;
    swap(copy);
    return *this;
  }

  WeakPtr& operator=(const SharedPtr<T>& other) {
    WeakPtr copy = other;
    swap(copy);
    return *this;
  }

  size_t use_count() const {
    if (cptr) {
      return cptr->getSharedCounter();
    }
    return 0;
  }

  SharedPtr<T> lock() const {
    if (expired())
      return SharedPtr<T>();

    return SharedPtr<T>(*this);
  }

  bool expired() const {
    return use_count() == 0;
  }

  ~WeakPtr() {
    if (!cptr)
      return;

    --(cptr->getWeakCounter());
    if (cptr->getWeakCounter() > 0)
      return;

    if (cptr->getSharedCounter() == 0) {
      cptr->finish();
    }


  }

};