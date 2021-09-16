#include <nextgen/allocator.h>

using namespace nextgen::mem;

void *os::calloc(size_t mem, size_t size) {
  if (mem == 0) return nullptr;
  auto p = std::calloc(mem, size);
  ASSERT(p, "Internal Error: Calloc Failed");
  return p;
}

void *os::malloc(size_t size) {
  if (size == 0) return nullptr;
  auto p = std::malloc(size);
  ASSERT(p, "Internal Error: Malloc Failed");
  return p;
}

void *os::realloc(void *buf, size_t size) {
  if (buf == nullptr && size == 0) return nullptr;
  auto p = std::realloc(buf, size);
  ASSERT(p, "Internal Error: Realloc Failed");
  return p;
}

void *os::memmove(void *dest, const void *src, size_t size) {
  auto p = std::memmove(dest, src, size);
  ASSERT(p, "Internal Error: Memmove Failed");
  return p;
}

void os::free(void *p) {
  std::free(p);
}

Allocator nextgen::mem::GLOBAL_OBJECT_ALLOC = Allocator(
  arena::OVER_COMMIT_MEMORY_SIZE - arena::DATA_ALLOCATION_MEMORY_SIZE
);
Allocator nextgen::mem::GLOBAL_DATA_ALLOC = Allocator(
  arena::DATA_ALLOCATION_MEMORY_SIZE
);

// Very important section. In general, you probably wouldn't want to overload
// the new and delete operator, but for this code base, it just makes the code
// much cleaner, and since the arena pool is global, it should be okay.
void *operator new(size_t size) {
  return ::GLOBAL_OBJECT_ALLOC.allocate(size);
}
void operator delete([[maybe_unused]] void *p) noexcept {}
