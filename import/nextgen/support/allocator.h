# ifndef NEXTGEN_ALLOCATOR_H
# define NEXTGEN_ALLOCATOR_H
# include "core.h"


namespace nextgen { namespace mem { using namespace nextgen::core;




  // STD NAMING
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
      auto *p = ::realloc(buf, size);
      if (p == nullptr)
        PANIC("realloc failed");
      return p;
    }

    NG_INLINE void free(void *p) {
      ::free(p);
    }


  } // namespace nextgen::mem::os



  class ArenaSegment final  {
  public:
    explicit ArenaSegment(ArenaSegment *next)
      : next_segment (next) {}

    template<typename T>
    Option<T*> next(size_t allocation_size)  { // STD NAMING

      if (allocation_size == 0)
        return nullptr;

      if (allocation_size + offset >= size) {
        if (next_segment == nullptr) {
          next_segment = (ArenaSegment*) os::malloc(sizeof(ArenaSegment));
          *next_segment = ArenaSegment { (ArenaSegment*) os::malloc(sizeof
                                                                      (ArenaSegment))};
        }
        return None;
      }

      offset += allocation_size;
      return block + allocation_size;
    }

    ArenaSegment *getNext() {
      return this->next_segment;
    }
  private:
    static constexpr size_t size = 65536 << 2;
    int    block[size]{};
    size_t offset = 0;
    ArenaSegment *next_segment;
  };


  template <size_t N = 0>
  class Arena {
  public:
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
        os::free(start);
        start = temp;
      }
    }
  private:
    ArenaSegment *begin;
  };

  // Bare-bones List structure for holding large amounts of elements. This is
  // meant to hold items in places where limits are tested, for example, a
  // program may have hundreds of thousands of tokens that need to be parsed
  // and so the size of the list increases quite a lot to reflect that need.
  template<typename T>
  class list { // STD NAMING
  public:
    list() {
      arr = (T *) mem::os::malloc(sizeof(T) * 2);
    }

    explicit list(size_t reserve) : cap(reserve) {
      arr = (T *) mem::os::malloc(sizeof(T) * reserve);
    }

    void add(T element) {
      if (len + 1 > cap) {
        cap *= 3;
        arr = (T *) mem::os::realloc(arr, sizeof(T) * cap);
      }
      arr[len++] = element;
    }

    T pop() {
      ASSERT(len >= 1, "Invalid List Pop.");
      return arr[--len];
    }

    NG_AINLINE T operator[](size_t index) const {
      return arr[index];
    }

  private:
    T *arr;
    size_t len{0};
    size_t cap{2};
  };

}} // namespace nextgen::mem




# endif //NEXTGEN_ALLOCATOR_H
