#include "allocator.h"

using namespace nextgen::mem;



Allocator nextgen::mem::GLOBAL_OBJECT_ALLOC = Allocator(
  arena::OVER_COMMIT_MEMORY_SIZE - arena::DATA_ALLOCATION_MEMORY_SIZE
);
Allocator nextgen::mem::GLOBAL_DATA_ALLOC = Allocator(
  arena::DATA_ALLOCATION_MEMORY_SIZE
);


// Very important section. In general, you probably wouldn't want to overload
// the new and delete operator, but for this code base, it just makes the code
// much cleaner, and since the arena pool is global, it should be okay.
void *operator new(std::size_t size) {
  return ::GLOBAL_OBJECT_ALLOC.allocate(size);
}
void operator delete([[maybe_unused]] void *p) noexcept {}
