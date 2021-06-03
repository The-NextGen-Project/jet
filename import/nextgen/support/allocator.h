# ifndef NEXTGEN_ALLOCATOR_H
# define NEXTGEN_ALLOCATOR_H
# include "core.h"


namespace nextgen { namespace mem { using namespace nextgen::core;



  // STD NAMING
  namespace os { // System allocators/libc


    void *calloc(size_t, size_t);
    void *malloc(size_t);
    void *realloc(void *buf, size_t size);
    void free(void **buf);

  } // namespace nextgen::mem::os

  namespace detail {
    struct c_deleter {
      void operator()(void *ptr) const {
        os::free(&ptr);
      }
    };
  }

  template<typename T>
  using Box = std::unique_ptr<T, detail::c_deleter>;

  template<typename T>
  using Rc = std::shared_ptr<T>;


  class ArenaSegment final  {
  public:
    explicit ArenaSegment(ArenaSegment *next)
      : next_segment (next) {}

    template<typename T = void>
    NG_INLINE Option<T*> next(size_t allocation_size)  { // STD NAMING

      if (allocation_size == 0) {
        return Some((T*) nullptr);
      }

      if (allocation_size + offset >= size) {
        if (next_segment == nullptr) {
          next_segment = (ArenaSegment*) os::malloc(sizeof(ArenaSegment));
          *next_segment = ArenaSegment { (ArenaSegment*) os::malloc(sizeof
                                                                      (ArenaSegment))};
        }
        // Switch to next allocator on fail
        return None;
      }

      offset += allocation_size;
      return Some((T*)(block + allocation_size));
    }

    NG_INLINE ArenaSegment *getNext() {
      return this->next_segment;
    }

  private:
    static constexpr size_t size = 65536 << 2;
    int    block[size]{};
    size_t offset = 0;
    ArenaSegment *next_segment;
  };


  template <size_t N = 0>
  struct Arena {
    Arena() {
      begin = (ArenaSegment*) os::malloc(sizeof(ArenaSegment));
      for (auto i = 0; i < N; ++i) {
        *begin = ArenaSegment { (ArenaSegment*) os::malloc(sizeof
                                                             (ArenaSegment))};
        begin = begin->getNext();
      }
    }

    ~Arena() {
      auto start = begin;
      while (start->getNext()) {
        auto temp = start->getNext();
        os::free(reinterpret_cast<void **>(&start));
        start = temp;
      }
    }
    ArenaSegment *begin;
  };


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
        auto new_ptr = (T *) ::reallocarray(IntoRaw(), cap, sizeof(T));
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


}} // namespace nextgen::mem




# endif //NEXTGEN_ALLOCATOR_H
