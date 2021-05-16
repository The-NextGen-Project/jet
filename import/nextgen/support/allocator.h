# ifndef NEXTGEN_ALLOCATOR_H
# define NEXTGEN_ALLOCATOR_H


# include "panic.h"


namespace nextgen { namespace mem {


  namespace os { // System allocators/libc


    NG_INLINE void *malloc(size_t size) {
      if (size == 0) return nullptr;
      auto *p = ::malloc(size);
      if (p == nullptr)
        PANIC("malloc failed");
      return p;
    }

    NG_INLINE void *realloc(void *buf, size_t size) {
      if (buf == nullptr && size == 0) return nullptr;
      buf = ::realloc(buf, size);
      if (buf == nullptr)
        PANIC("realloc failed");
      return buf;
    }

    NG_INLINE void free(void *p) {
      ::free(p);
    }


  } // namespace nextgen::mem::os



  class BlockAllocator  {
  public:

    // Free the allocated block.
    virtual void free() = 0;

    // Get the pointer to the next block. It may be None depending on the
    // size of the segment that is left.
    virtual void *next(size_t) = 0;
  };


  // A pool allocator is a block-based allocator. This means that each
  // Arena contains multiple allocators, each of which is passed on to
  // the object that does allocation, and is all freed at the end. This
  // allows for minimal program allocation. Good thing about this is that
  // if multiple similar parts of a program are in the same memory region
  // its easier to manage and access.
  //
  // Example:
  //
  template <size_t SHIFT>
  class PoolAllocator final : public BlockAllocator {
  public:
    static constexpr auto SIZE = 65536 << SHIFT;

    PoolAllocator() {
      pool = (char*) os::malloc(SIZE);
    }


    void *next(size_t allocation_size) override {
      if (allocation_size == 0)
        return nullptr;

      if (allocation_size + offset >= size) {
        os::realloc(pool, size * 2);
      }

      offset += allocation_size;
      pool += allocation_size;
      return pool;
    }


    NG_AINLINE void free() override {
      os::free(pool);
    }

  private:
    char *pool;
    size_t offset = 0;
    size_t size = SIZE;
  };


  class ArenaSegment final : public BlockAllocator {
  public:
    explicit ArenaSegment(BlockAllocator &backing)
      : backing_allocator (backing) {}

    void *next(size_t allocation_size) override {

      if (allocation_size == 0)
        return nullptr;

      if (allocation_size + offset >= size) {
        return backing_allocator.next(allocation_size);
      }

      offset += allocation_size;
      return block + allocation_size;
    }

    NG_AINLINE void free() override {
      // do nothing ...
    }

  private:
    static constexpr size_t size = 65536 << 2;
    int    block[size]{};
    size_t offset = 0;
    BlockAllocator &backing_allocator;
  };


  template <size_t N = 0>
  class Arena {
  public:
    Arena() {

      for (auto i = 0; i < N; ++i) {
        auto backing = PoolAllocator<2> {};
        segments[i]  = ArenaSegment { backing };
      }
    }

    ~Arena() {
      auto i = 0;
      for (; i < this->size; ++i)
        segments[i].free();
    }


  private:
    std::array<ArenaSegment, N> segments{};
  };

}} // namespace nextgen::mem




# endif //NEXTGEN_ALLOCATOR_H
