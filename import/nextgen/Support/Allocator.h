# ifndef NEXTGEN_ALLOCATOR_H
# define NEXTGEN_ALLOCATOR_H
# include "Core.h"


namespace nextgen { namespace mem { using namespace nextgen::core;

  // STD NAMING
  namespace os { // System allocators/libc


    void *calloc(size_t, size_t);
    void *malloc(size_t);
    void *realloc(void *buf, size_t size);
    void free(void *buf);

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

    // Default
    Box<T> Pointer  = Box<T>((T*) os::calloc(1, sizeof(T)));
    size_t Length   = 0;
    size_t Capacity = 1;

  public:
    Vec() = default;
    explicit Vec(size_t Cap) : Capacity(Cap) {
      Pointer = Box<T>((T*) os::calloc(Cap, sizeof(T)));
    }

    NG_INLINE T *IntoRaw() {
      return Pointer.get();
    }

    void Add(T elem) {
      if (Length + 1 > Capacity) {
        Capacity *= 2;
        auto NewPtr = (T *) os::realloc(IntoRaw(), sizeof(T) * Capacity);
        Pointer.release();
        Pointer.reset(NewPtr);
      }
      IntoRaw()[Length++] = elem;
    }

    void Pop() {
      ASSERT(Length >= 1, "Invalid List Pop.");
      return IntoRaw()[--Length];
    }

    NG_INLINE void Clear() {
      Length = 0;
      Pointer.reset();
    }

    NG_AINLINE auto Size() -> size_t const {
      return Length;
    }

    NG_AINLINE auto Cap() -> size_t const {
      return Capacity;
    }

    NG_AINLINE auto operator[](size_t Index) -> T* {
      ASSERT(Index < UINTPTR_MAX, "Outside Index Range");
      return IntoRaw() + Index;
    }

    NG_AINLINE auto At(size_t Index) -> T {
      ASSERT(Index < UINTPTR_MAX, "Outside Index Range");
      return IntoRaw()[Index];
    }
  };

    struct ArenaSegment  {

      ArenaSegment() = default;

      static auto New() -> ArenaSegment {
        return {};
      }

      static auto New(ArenaSegment *seg) -> ArenaSegment {
        return ArenaSegment(0, seg);
      }

      template<typename T = void>
      NG_INLINE auto Next(size_t allocation_size) -> T* {

        if (allocation_size == 0) {
          return (T*) nullptr;
        }

        if (allocation_size + Offset >= size) {
          if (NextSegment == nullptr) {
            NextSegment = (ArenaSegment*) os::malloc(sizeof(ArenaSegment));
            *NextSegment = ArenaSegment::New((ArenaSegment*) os::malloc(sizeof(ArenaSegment)));
          }
          // Switch to next allocator on fail
          return getNext()->Next<T>(allocation_size);
        }

        Offset += allocation_size;
        return (T*)(block + allocation_size);
      }

      NG_INLINE bool HasSpaceFor(size_t space) {
        return space + Offset <= size;
      }

      NG_INLINE auto GetAllocatorForAvailableSpace(size_t space) -> ArenaSegment*{
        if (!HasSpaceFor(space)) {
          if (NextSegment == nullptr) {
            NextSegment = (ArenaSegment*) os::malloc(sizeof(ArenaSegment));
            *NextSegment = ArenaSegment::New((ArenaSegment*) os::malloc(sizeof(ArenaSegment)));
          }
          return NextSegment->GetAllocatorForAvailableSpace(space);
        }
        return this;
      }

      NG_INLINE char *CopyValues(char *Buf, size_t Length) {
        auto Alloc = GetAllocatorForAvailableSpace(Length);
        auto Start = (char *) Alloc->BlockPoint();
        for (auto i = 0; i < Length; ++i) { // Copy into destination
          Start[i] = Buf[i];
        }
        Alloc->Offset += Length;
        return Start;
      }

      NG_INLINE auto getNext() -> ArenaSegment* {
        return this->NextSegment;
      }

      NG_INLINE void *BlockPoint() {
        return block + Offset;
      }

    private:

      ArenaSegment(size_t Offset, ArenaSegment *Next) : Offset(Offset),
      NextSegment(Next) {}

      size_t Offset  = 0;
      ArenaSegment *NextSegment = nullptr;
      static constexpr size_t size = 65536 << 2;
      char    block[size]{};
    };


    template <size_t N>
    struct Arena {
      Arena() {
        Begin = (ArenaSegment*) os::malloc(sizeof(ArenaSegment) * N);
        auto Node = Begin;
        for (auto i = 0; i < N; ++i) {
          *Node = ArenaSegment::New(Begin + i);
          Node = Node->getNext();
        }
      }


      ~Arena() {
        if (Begin)
          os::free(Begin);
      }
      ArenaSegment *Begin = nullptr;
    };

}} // namespace nextgen::mem




# endif //NEXTGEN_ALLOCATOR_H
