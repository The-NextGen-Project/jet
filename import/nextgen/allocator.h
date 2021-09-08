# ifndef NEXTGEN_ALLOCATOR_H
# define NEXTGEN_ALLOCATOR_H
# include "core-util.h"


namespace nextgen { namespace mem { using namespace nextgen::core;

  namespace os { // System allocators/libc

    void *calloc(size_t, size_t);
    void *malloc(size_t);
    void *realloc(void *, size_t);
    void free(void *);
    void memmove(void *dst, const void *src, size_t size);

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

  namespace vec {
    /**
   * @summary Gets the beginning of the actual allocated vector pointer. The
   * vector that is passed around is the real pointer incremented from a
   * size_t value which contains the `size` and `cap` of the vector.
   *
   *
   * @param vec Non-Null vector value
   */
    template<typename VT>
    static NG_AINLINE size_t *raw_vec_ptr(VT *&vec) {
      return ((size_t*)(void*)(vec) - 2);
    }

    template<typename VT>
    static NG_AINLINE size_t size(VT *&vec) {
      return raw_vec_ptr(vec)[0];
    }

    template<typename VT>
    static NG_AINLINE size_t cap(VT *&vec) {
      return raw_vec_ptr(vec)[1];
    }

    template<typename VT>
    static NG_AINLINE bool should_grow(VT *&vec) {
      return (!vec || size(vec) == cap(vec));
    }

    template<typename VT>
    static void *realloc(VT *&vec, size_t vec_sz) {
      size_t size, cap;
      if (!vec) {
        size = 0; cap = 1;
      } else {
        size = vec::size(vec);
        cap  = vec::cap(vec);
        cap *= 2;
      }

      // We know that this is safe because realloc behaves correctly with nullptr
      // values, so we do not need to worry about undefined behavior.
      size_t *new_vec = (size_t *) os::realloc(vec ? raw_vec_ptr(vec) : 0,
                                               vec_sz * cap + sizeof(size_t) * 2);
      new_vec[0] = size;
      new_vec[1] = cap;
      return new_vec + 2;
    }

    template<typename VT>
    static NG_AINLINE void grow(VT *&arr) {
      (should_grow(arr) ? arr = (VT*) vec::realloc(arr, sizeof(VT)) : 0);
    }

    template<typename VT>
    static NG_AINLINE void init(VT *&arr, size_t n_elems) {
      vec::realloc(arr, sizeof(VT) * n_elems);
    }

    template<typename VT>
    static NG_AINLINE void push(VT *&vec, VT elem) {
      ((grow(vec), (vec)[(*raw_vec_ptr(vec))++] = elem));
    }

    template<typename VT>
    static NG_INLINE void rem(VT *&vec, size_t index) {
      memmove(&*(vec + index),
              &*((vec + index + 1)),
              vec_size(vec) * sizeof(VT));
      size(vec)--;
    }

    template<typename VT>
    static NG_INLINE void erase(VT *&vec, size_t start, size_t end) {
      memmove(&*(vec + start),
              &*((vec + end * start + 1)),
              vec_size(vec) * sizeof(VT));
      size(vec) -= end - start;
    }

    template<typename VT>
    static NG_AINLINE void clear(VT *&vec) {
      os::free(raw_vec_ptr(vec));
    }
  }


  /*template<typename T>
  class Vec {
    T     *pointer  = ((T*) os::malloc(sizeof(T)));
    size_t length   = 0;
    size_t capacity = 1;
  public:
    Vec() {
      length = 0;
      capacity = 1;
    }
    Vec(const Vec &other) : pointer(other.pointer), length(other.length), capacity(other.capacity) {}
    explicit Vec(size_t cap) : capacity(cap) {
      pointer = ((T*) os::malloc(cap * sizeof(T)));
      length  = 0;
    }

    NG_AINLINE T *raw_ptr() {
      return pointer;
    }

    void push(T elem) {
      if (length + 1 > capacity) {

        // @Problem <ashishbailkeri123@gmail.com>
        // Something clearly went wrong here. We have to figure out way the
        // vector does not initialize the capacity at all.
        if (capacity == 0) {
          capacity = 1;
          pointer = (T*) os::calloc(1, sizeof(T));
        }

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

  };*/

  namespace arena {

    static constexpr auto PAGE_ALLOCATION_SIZE = 65'536;
    static constexpr decltype(UINTPTR_MAX)OVER_COMMIT_MEMORY_SIZE = 107374182400ULL;


    static uint8_t *ArenaMemory =
#ifdef NG_OS_WINDOWS
  (uint8_t *) VirtualAlloc(nullptr,OVER_COMMIT_MEMORY_SIZE,MEM_RESERVE, PAGE_READWRITE);
#endif

    /** @summary Number of bytes written by the allocator not `ArenaMemory` */
    // @Cleanup Should this be static inside the allocator function?
    static decltype(UINTPTR_MAX) BytesWritten = 0;

    /** @summary Give portion of memory from the arena  */
    NG_INLINE uint8_t *give(decltype(UINTPTR_MAX) memory) {
      uint8_t * bytes;
      switch (memory) {
        case 0: return nullptr;
        default:
          bytes = ArenaMemory + BytesWritten;
          // BytesWritten += memory;
          return bytes;
      }
    }

    /** @summary Get the memory from the arena and ensure the OS maps it  */
    // TODO: Setup Linux and macOS  memory allocation
    NG_INLINE uint8_t *give_and_commit(decltype(UINTPTR_MAX) memory) {
      uint8_t *mem = arena::give(memory);
#ifdef NG_OS_WINDOWS

      // Windows is a weird specimen. We cannot over-commit, so we need to
      // reserve an impossible amount of memory and then request it from the
      // same buffer.
      return (uint8_t *) VirtualAlloc(mem, memory, MEM_COMMIT, PAGE_READWRITE);
#else
      return mem;
#endif
    }

    // TODO: Setup Linux and macOS  memory allocation
    NG_INLINE void commit(uint8_t *arena, decltype(UINTPTR_MAX) memory) {
#ifdef NG_OS_WINDOWS
      VirtualAlloc(arena, memory, MEM_COMMIT,
                   PAGE_READWRITE);
#endif
    }

    NG_INLINE uint8_t *&allocator() {
      static uint8_t *Allocator = arena::give_and_commit(PAGE_ALLOCATION_SIZE);
      return Allocator;
    }

    /** @summary Allocate memory from the allocator that holds at least 2^16
     * bytes in memory */
    NG_INLINE uint8_t *page_allocate(size_t allocation_size) {
      static decltype(UINTPTR_MAX) capacity   = PAGE_ALLOCATION_SIZE;

      auto allocator = arena::allocator();

      if (BytesWritten + allocation_size > capacity) {
        arena::commit(allocator + BytesWritten, PAGE_ALLOCATION_SIZE);
        capacity += PAGE_ALLOCATION_SIZE;
      }
      auto ret = allocator + BytesWritten;
      BytesWritten += allocation_size;
      return ret;
    }

    /** @summary The current point at which the allocator can write bytes from
     * memory */
    NG_INLINE uint8_t *current() {
      return arena::allocator()+BytesWritten+1;
    }

    /** @summary Push elements of any size directly into the buffer. */
    template<typename T>
    NG_INLINE void push(T elem) {
      static constexpr auto ELEM_SIZE = sizeof(T);

      auto begin = BytesWritten + 1;
      arena::page_allocate(ELEM_SIZE);
      arena::allocator()[begin] = elem;
    }

    /** @summary Allocate type `T` value into the arena buffer. */
    template<typename T>
    NG_INLINE T *allocate(size_t allocation_size = 1) {
      return (T*) arena::page_allocate(sizeof(T) * allocation_size + 1);
    }

    NG_INLINE void deinit() {
#ifdef NG_OS_WINDOWS
      VirtualFree(ArenaMemory, BytesWritten, MEM_DECOMMIT);
#endif
    }
  }

//  class ArenaSegment  {
//  private:
//    static constexpr size_t size = 65536 << 2;
//    char current[size]{};
//
//    size_t        offset = 0;
//    ArenaSegment *next   = nullptr;
//  public:
//    ArenaSegment() = default;
//    ArenaSegment(size_t off, ArenaSegment *next)
//      : offset(off), next(next) {}
//    ArenaSegment(ArenaSegment *next)
//      : offset(0), next(next) {}
//
//    template<typename T>
//    T *allocate(size_t allocation_size = 1) {
//      auto change = sizeof(T) * allocation_size + 1;
//
//      if (allocation_size == 0) {
//        return (T*) nullptr;
//      }
//
//      if (!has_space_for(change)) {
//        if (!next) {
//          next = (ArenaSegment*) os::malloc(sizeof(ArenaSegment));
//          *next = ArenaSegment((ArenaSegment*) os::malloc(sizeof(ArenaSegment)));
//        }
//        // Switch to next allocator on fail
//        // TODO: Figure out a way to switch the current allocator
//        return getNext()->allocate<T>(allocation_size);
//      }
//      auto ret = (T*)(current + offset);
//      offset += change;
//      return ret;
//    }
//
//    NG_INLINE bool has_space_for(size_t space) {
//      return space + offset <= size;
//    }
//
//    ArenaSegment *allocator_for_space(size_t space) {
//      if (!has_space_for(space)) {
//        if (next == nullptr) {
//          next = (ArenaSegment*) os::malloc(sizeof(ArenaSegment));
//          *next = ArenaSegment((ArenaSegment*) os::malloc(sizeof(ArenaSegment)));
//        }
//        return next->allocator_for_space(space);
//      }
//      return this;
//    }
//
//    void consume(size_t value) {
//      offset += value;
//    }
//
//    void *current_point() {
//      return current + offset;
//    }
//
//    ArenaSegment *getNext() {
//      return this->next;
//    }
//  };
//
//
//  template <size_t N>
//  struct Arena {
//    ArenaSegment *begin = nullptr;
//    Arena() {
//      begin = (ArenaSegment*) os::malloc(sizeof(ArenaSegment) * N);
//      auto node = begin;
//      FOR(i, N) {
//        *node = ArenaSegment(begin + 1);
//        node  = node->getNext();
//      }
//    }
//    ~Arena() {
//      if (begin) os::free(begin);
//    }
//  };


}} // namespace nextgen::mem




# endif //NEXTGEN_ALLOCATOR_H
