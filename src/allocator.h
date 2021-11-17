# ifndef NEXTGEN_ALLOCATOR_H
# define NEXTGEN_ALLOCATOR_H
# include "util/misc.h"

namespace nextgen { namespace mem { using namespace nextgen::core;

  namespace arena {

    static constexpr auto PAGE_ALLOCATION_SIZE = 65'536;

    /** @brief 100 GiB */
    static constexpr decltype(UINTPTR_MAX) OVER_COMMIT_MEMORY_SIZE = 107374182400ULL;

    /** @brief 10 GiB */
    static constexpr decltype(UINTPTR_MAX) DATA_ALLOCATION_MEMORY_SIZE = 10737418240ULL;

    static uint8_t *PROGRAM_MEMORY =
#ifdef NG_OS_WINDOWS
  (uint8_t *) VirtualAlloc(nullptr,OVER_COMMIT_MEMORY_SIZE,MEM_RESERVE, PAGE_READWRITE);
#endif
#ifdef NG_OS_LINUX
      (uint8_t*) mmap(nullptr, OVER_COMMIT_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE |
      MAP_ANON | MAP_NORESERVE, -1, 0);
#endif
#ifdef NG_OS_APPLE
      (uint8_t*) mmap(nullptr, OVER_COMMIT_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE |
      MAP_ANON, -1, 0);
#endif


    NG_INLINE auto &length() {
      static size_t bytes_written = 0;
      return bytes_written;
    }

    /** @summary Give portion of memory from the arena  */
    NG_INLINE auto give(decltype(UINTPTR_MAX) memory) {
      uint8_t * bytes = nullptr;
      switch (memory) {
        case 0: return bytes;
        default:
          auto written_bytes = arena::length();
          bytes = PROGRAM_MEMORY + written_bytes;
          arena::length() += memory;
          return bytes;
      }
    }


    /** @summary Get the memory from the arena and ensure the OS maps it  */
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

    NG_INLINE auto commit(uint8_t *arena, decltype(UINTPTR_MAX) memory) {
#ifdef NG_OS_WINDOWS
      VirtualAlloc(arena, memory, MEM_COMMIT, PAGE_READWRITE);
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
      return this->bytes + this->bytes_written;
    }

    auto length() const {
      return this->bytes_written;
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
      VirtualFree(arena::PROGRAM_MEMORY, arena::length(), MEM_DECOMMIT);
#else
      munmap(arena::PROGRAM_MEMORY, GLOBAL_DATA_ALLOC.length()+GLOBAL_OBJECT_ALLOC.length());
#endif
    }

}} // namespace nextgen::mem

# endif //NEXTGEN_ALLOCATOR_H
