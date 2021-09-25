# ifndef NEXTGEN_ALLOCATOR_H
# define NEXTGEN_ALLOCATOR_H
# include "core-util.h"



namespace nextgen { namespace mem { using namespace nextgen::core;

  namespace os { // System allocators/libc

    void *calloc(size_t, size_t);
    void *malloc(size_t);
    void *realloc(void *, size_t);
    void free(void *);
    void *memmove(void *dst, const void *src, size_t size);

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

  namespace arena {

    static constexpr auto PAGE_ALLOCATION_SIZE = 65'536;

    /** @brief 100 GiB */
    static constexpr decltype(UINTPTR_MAX)
                            OVER_COMMIT_MEMORY_SIZE = 107374182400ULL;

    /** @brief 10 GiB */
    static constexpr decltype(UINTPTR_MAX)
                            DATA_ALLOCATION_MEMORY_SIZE = 10737418240ULL;

    static uint8_t *ArenaMemory =
#ifdef NG_OS_WINDOWS
  (uint8_t *) VirtualAlloc(nullptr,OVER_COMMIT_MEMORY_SIZE,MEM_RESERVE, PAGE_READWRITE);
#endif
#ifdef NG_OS_LINUX
      (uint8_t*) mmap(nullptr, OVER_COMMIT_MEMORY_SIZE, PROT_READ |
      PROT_WRITE, MAP_PRIVATE | MAP_ANON | MAP_NORESERVE, -1, 0);
#endif


    NG_INLINE size_t &length() {
      static size_t BytesWritten = 0;
      return BytesWritten;
    }

    /** @summary Give portion of memory from the arena  */
    NG_INLINE uint8_t *give(decltype(UINTPTR_MAX) memory) {
      uint8_t * bytes;
      switch (memory) {
        case 0: return nullptr;
        default:
          auto written_bytes = arena::length();

          bytes = ArenaMemory + written_bytes;
          arena::length() += memory;
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
  }

  class Allocator {
    uint8_t *bytes = nullptr;
    size_t  bytes_written = 0;
    size_t  capacity      = 0;
  public:
    explicit Allocator(size_t reserved_memory)
    : bytes(arena::give_and_commit(arena::PAGE_ALLOCATION_SIZE)),
      bytes_written(0),
      capacity(reserved_memory) {}

    auto current() const {
      return bytes + bytes_written;
    }

    auto length() const {
      return bytes_written;
    }

    auto reserve(size_t size) {
      auto curr = current();
      bytes_written += size;
      return curr;
    }

    auto allocate(size_t allocation_size) {
      auto ret_value = bytes_written + bytes;
      if (bytes_written + allocation_size > capacity) {
        arena::commit(ret_value, arena::PAGE_ALLOCATION_SIZE);
      }
      bytes_written += allocation_size;
      return ret_value;
    }

    template<typename T>
    auto allocate(size_t n) {
      return (T*) (allocate(sizeof(T) * n));
    }
  };

  extern Allocator GLOBAL_OBJECT_ALLOC;
  extern Allocator GLOBAL_DATA_ALLOC;

    NG_INLINE void deinit() {
#ifdef NG_OS_WINDOWS
      VirtualFree(arena::ArenaMemory, arena::length(), MEM_DECOMMIT);
#else
      munmap(arena::ArenaMemory, GLOBAL_DATA_ALLOC.length()
      +GLOBAL_OBJECT_ALLOC.length());
#endif
    }


  template<typename T, size_t N = 1>
  class ObjectVector {
    T *objects = GLOBAL_OBJECT_ALLOC.template allocate<T>(N);
    size_t len = 0;
    size_t cap = N;
  public:

    T operator[](size_t index) {
      return objects[index];
    }

    void operator<<(T const &elem) {
      if (len >= cap) {
        cap *= 2;
        auto new_ptr = GLOBAL_OBJECT_ALLOC.template allocate<T>(cap);
        FOR(i, len) {
          new_ptr[i] = objects[i];
        }
        objects = new_ptr;
      }
      objects[len++] = elem;
    }
  };

  template<typename T>
  struct ArenaVec {
    /** @brief Start of vector */
    T *begin;
    /** @brief End of vector */
    T *end;

    /** @brief Indexes the value of the vector. This is slightly more
     * expensive than normal array access */
    NG_INLINE T *operator[](size_t index) {
      return (T*) (((uint8_t *)begin) + (sizeof(T) * index));
    }

    /** @brief Iterates over the vector with any other potential factors */
    template<typename Lambda, LAMBDA(Lambda, void, void)>
    NG_AINLINE void iter(Lambda lambda) {
      FOR(i, len()) lambda();
    }

    /** @brief Span of the vector in memory */
    auto NG_AINLINE len() {
      return end-begin;
    }
  };

}} // namespace nextgen::mem

# endif //NEXTGEN_ALLOCATOR_H
