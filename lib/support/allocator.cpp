#include <nextgen/support/allocator.h>

void *nextgen::mem::os::calloc(size_t mem, size_t size) {
  if (mem == 0) return nullptr;
  auto *p = ::calloc(mem, size);
  if (p == nullptr)
    PANIC("malloc failed");
  return p;
}

void *nextgen::mem::os::malloc(size_t size) {
  if (size == 0) return nullptr;
  auto *p = ::malloc(size);
  if (p == nullptr)
    PANIC("malloc failed");
  return p;
}

void *nextgen::mem::os::realloc(void *buf, size_t size) {
  if (buf == nullptr && size == 0) return nullptr;
  auto *p = ::realloc(buf, size);
  if (p == nullptr)
    PANIC("realloc failed");
  return p;
}

void nextgen::mem::os::free(void **p) {

  if (*p != nullptr)
    std::free(*p);
  else
    *p = nullptr;
}
