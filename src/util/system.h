#ifndef JET_SYSTEM_H
#define JET_SYSTEM_H

namespace sys {

  static void *calloc(size_t mem, size_t size) {
    if (mem == 0) return nullptr;
    auto p = std::calloc(mem, size);
    ASSERT(p, "Internal Error: Calloc Failed");
    return p;
  }

  static void *malloc(size_t size) {
    if (size == 0) return nullptr;
    auto p = std::malloc(size);
    ASSERT(p, "Internal Error: Malloc Failed");
    return p;
  }

  static void *realloc(void *buf, size_t size) {
    if (buf == nullptr && size == 0) return nullptr;
    auto p = std::realloc(buf, size);
    ASSERT(p, "Internal Error: Realloc Failed");
    return p;
  }

  static void *memmove(void *dest, const void *src, size_t size) {
    auto p = std::memmove(dest, src, size);
    ASSERT(p, "Internal Error: Memmove Failed");
    return p;
  }

  static void free(void *p) {
    std::free(p);
  }

}

#endif //JET_SYSTEM_H
