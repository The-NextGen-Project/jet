# ifndef NEXTGEN_ALLOCATOR_H
# define NEXTGEN_ALLOCATOR_H
# include "Core.h"


namespace nextgen { namespace mem { using namespace nextgen::core;

  namespace os { // System allocators/libc

    void *calloc(size_t, size_t);
    void *malloc(size_t);
    void *realloc(void *, size_t);
    void free(void *);

  } // namespace nextgen::mem::os

  namespace detail {
    struct c_deleter {
      void operator()(void *ptr) const {
        os::free(ptr);
      }
    };
  }

  template<typename T>
  using Box = std::unique_ptr<T, detail::c_deleter>;
  template<typename T>
  using Rc = std::shared_ptr<T>;


  template<typename T>
  class Vec {
    T     *pointer  = ((T*) os::calloc(1, sizeof(T)));
    size_t length   = 0;
    size_t capacity = 1;
  public:
    Vec() = default;
    explicit Vec(size_t cap) : capacity(cap) {
      pointer = ((T*) os::calloc(cap, sizeof(T)));
    }
    ~Vec() { clear(); }

    NG_AINLINE T *raw_ptr() {
      return pointer;
    }

    void push(T elem) {
      if (length + 1 > capacity) {
        capacity *= 2;
        pointer = (T*) os::realloc(pointer, sizeof(T) * capacity);
      }
      pointer[length++] = elem;
    }

    template<typename ... Args>
    void push(Args ... args) {
      if (length + 1 > capacity) {
        capacity *= 2;
        pointer = (T*) os::realloc(pointer, sizeof(T) * capacity);
      }
      pointer[length++] = T(std::forward<Args>(args)...);
    }

    void pop() {
      ASSERT(length >= 1, "Invalid List Pop.");
      return pointer[--length];
    }

    bool empty() {
      return length == 0;
    }

    void clear() {
      length = 0;
      os::free(pointer);
    }

    size_t size()  {
      return length;
    }

    size_t cap()  {
      return capacity;
    }

    T operator[](size_t index)  {
      ASSERT(index < UINTPTR_MAX, "Outside Index Range");
      return pointer[index];
    }

    T *get_pointer_at(size_t index) {
      ASSERT(index < UINTPTR_MAX, "Outside Index Range");
      return pointer + index;
    }

  };

  class ArenaSegment  {
  private:
    static constexpr size_t size = 65536 << 2;
    char block[size]{};

    size_t        offset = 0;
    ArenaSegment *next   = nullptr;
  public:
    ArenaSegment() = default;
    ArenaSegment(size_t off, ArenaSegment *next)
      : offset(off), next(next) {}
    ArenaSegment(ArenaSegment *next)
      : offset(0), next(next) {}

    template<typename T = void>
    T *allocate(size_t allocation_size = 1) {
      auto change = sizeof(T) * allocation_size;

      if (allocation_size == 0) {
        return (T*) nullptr;
      }

      if (!has_space_for(change)) {
        if (!next) {
          next = (ArenaSegment*) os::malloc(sizeof(ArenaSegment));
          *next = ArenaSegment((ArenaSegment*) os::malloc(sizeof(ArenaSegment)));
        }
        // Switch to next allocator on fail
        // TODO: Figure out a way to switch the current allocator
        return getNext()->allocate<T>(allocation_size);
      }
      offset += change;
      return (T*)(block + offset);
    }

    NG_INLINE bool has_space_for(size_t space) {
      return space + offset <= size;
    }

    ArenaSegment *allocator_for_space(size_t space) {
      if (!has_space_for(space)) {
        if (next == nullptr) {
          next = (ArenaSegment*) os::malloc(sizeof(ArenaSegment));
          *next = ArenaSegment((ArenaSegment*) os::malloc(sizeof(ArenaSegment)));
        }
        return next->allocator_for_space(space);
      }
      return this;
    }

    void *current_point() {
      return block + offset;
    }

    ArenaSegment *getNext() {
      return this->next;
    }
  };


  // Linked-List of Large memory chunks passed among classes in the compiler
  // in order to allocate static allocations (ones that do not require
  // realloc). This is very useful because most objects that are allocated do
  // not require their places in memory to change, what handles that is the
  // simple [Vec(T)] implementation above.
  template <size_t N>
  struct Arena {
    ArenaSegment *begin = nullptr;
    Arena() {
      begin = (ArenaSegment*) os::malloc(sizeof(ArenaSegment) * N);
      auto node = begin;
      FOR(i, N) {
        *node = ArenaSegment(begin + 1);
        node  = node->getNext();
      }
    }
    ~Arena() {
      if (begin) os::free(begin);
    }
  };

}} // namespace nextgen::mem




# endif //NEXTGEN_ALLOCATOR_H
