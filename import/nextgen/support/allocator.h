# ifndef NEXTGEN_ALLOCATOR_H
# define NEXTGEN_ALLOCATOR_H
# include "core.h"


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
      auto x = Vec<T> {
        .len = vec.len,
        .cap = vec.cap,
      };
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
        .ptr = Box<T>((T*)os::calloc(1, sizeof(T))),
        .len = 0,
        .cap = 1
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

      static auto New() -> ArenaSegment {
        return ArenaSegment {
          .offset = 0,
          .next_segment = nullptr,
          .block = {}
        };
      }

      static auto New(ArenaSegment *seg) -> ArenaSegment {
        return ArenaSegment {
          .offset = 0,
          .next_segment = seg,
          .block = {}
        };
      }

      template<typename T = void>
      NG_INLINE auto Next(size_t allocation_size) -> T* { // STD NAMING

        if (allocation_size == 0) {
          return (T*) nullptr;
        }

        if (allocation_size + offset >= size) {
          if (next_segment == nullptr) {
            next_segment = (ArenaSegment*) os::malloc(sizeof(ArenaSegment));
            *next_segment = ArenaSegment::New((ArenaSegment*) os::malloc(sizeof(ArenaSegment)));
          }
          // Switch to next allocator on fail
          return getNext()->Next<T>(allocation_size);
        }

        offset += allocation_size;
        return (T*)(block + allocation_size);
      }

      NG_INLINE bool HasSpaceFor(size_t space) {
        return space + offset <= size;
      }

      NG_INLINE auto GetAllocatorForAvailableSpace(size_t space) -> ArenaSegment*{
        if (!HasSpaceFor(space)) {
          if (next_segment == nullptr) {
            next_segment = (ArenaSegment*) os::malloc(sizeof(ArenaSegment));
            *next_segment = ArenaSegment::New((ArenaSegment*) os::malloc(sizeof(ArenaSegment)));
          }
          return next_segment->GetAllocatorForAvailableSpace(space);
        }
        return this;
      }


      NG_INLINE auto getNext() -> ArenaSegment* {
        return this->next_segment;
      }

      NG_INLINE void *BlockPoint() {
        return block + offset;
      }
      size_t offset;
      ArenaSegment *next_segment;
      static constexpr size_t size = 65536 << 2;
      int    block[size];
    };


    template <size_t N = 0>
    struct Arena {
      Arena() {
        begin = (ArenaSegment*) os::malloc(sizeof(ArenaSegment));
        for (auto i = 0; i < N; ++i) {
          *begin = ArenaSegment::New((ArenaSegment*) os::malloc(sizeof(ArenaSegment)));
          begin = begin->getNext();
        }
      }

      ~Arena() {
        auto start = begin;
        while (start->getNext()) {
          auto temp = start->getNext();
          os::free(start);
          start = temp;
        }
      }
      ArenaSegment *begin;
    };

}} // namespace nextgen::mem




# endif //NEXTGEN_ALLOCATOR_H
