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
  struct Vec {
    using Self = Vec;

    static auto Replace(const Vec<T> &vec) -> Self {
      auto x = Vec<T> {};
      x.len = vec.len;
      x.cap = vec.cap;
      x.ptr.swap((Box<T> &) std::move(vec.ptr));
      return x;
    }

    static auto Copy(const Vec<T> &vec) -> Self {
      T *raw = vec.ptr.get();
      size_t len = vec.Size();
      return Clone(raw, len);
    }

    static auto Clone(T *raw, size_t len) -> Vec<T> {
      auto vec = Vec<T>::WithCapacity(len);
      for (auto i = 0; i < len; ++i)
        vec.Add(raw[i]);
      return vec;
    }

    static auto New() -> Self {
      return Vec {
        Box<T>((T*)os::calloc(1, sizeof(T))),
        0,
        1
      };
    }

    static auto WithCapacity(size_t capacity) -> Self {
      auto vec = Vec {};
      vec.ptr = Box<T>((T*)os::calloc(capacity, sizeof(T)));
      vec.len = 0;
      vec.cap = capacity;
      return vec;
    }


    NG_INLINE T *IntoRaw() {
      return ptr.get();
    }

    void Add(T elem) {
      if (len + 1 > cap) {
        cap *= 2;
        auto new_ptr = (T *) os::realloc(IntoRaw(), sizeof(T) * cap);
        ptr.release();
        ptr.reset(new_ptr);
      }
      IntoRaw()[len++] = elem;
    }

    void Pop() {
      ASSERT(len >= 1, "Invalid List Pop.");
      return IntoRaw()[--len];
    }

    NG_INLINE void Clear() {
      len = 0;
      ptr.reset();
    }

    NG_AINLINE auto Size() -> size_t const {
      return len;
    }

    NG_AINLINE auto Capacity() -> size_t const {
      return cap;
    }

    NG_AINLINE auto operator[](size_t index) -> T {
      ASSERT(index < UINTPTR_MAX, "Outside Index Range");
      return IntoRaw()[index];
    }

    Box<T> ptr;

    size_t len;
    size_t cap;
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
        auto node = begin;
        for (auto i = 0; i < N; ++i) {
          node = (ArenaSegment*) os::malloc(sizeof(ArenaSegment));
          *node = ArenaSegment::New((ArenaSegment*) os::malloc(sizeof
                                                                      (ArenaSegment)));
          node = node->getNext();
        }
      }


      ~Arena() {
        /*auto start = begin;
        while (start->getNext()) {
          auto temp = start->getNext();
          os::free(start);
          start = temp;
        }*/
      }
      ArenaSegment *begin = nullptr;
    };

}} // namespace nextgen::mem




# endif //NEXTGEN_ALLOCATOR_H
