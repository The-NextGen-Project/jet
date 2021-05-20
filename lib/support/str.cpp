# include <nextgen/support/str.h>
# include <nextgen/support/allocator.h>

// We use std::ostream::write because we want to write only our specified
// length. We take in Range<const char *> as a constructor sometimes and
// cannot be sure whether we are dealing with a pointer to that region or
// not. Since nextgen::str does not always own its pointer value, we must
// use the write function.
std::ostream &operator<<(std::ostream &s, const nextgen::str &str) {
  s.write(str, str.size());
  return s;
}

decltype(FNV_OFF) operator "" _hash(const char *s, size_t len) {
  return nextgen::str(s, len).hash();
}
  

