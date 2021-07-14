# include <nextgen/Support/String.h>

using nextgen::str;
using nextgen::StringCache;

// Inline in string intern on constant strings
str nextgen::operator""_intern(const char *s, size_t len) {
    str ss = str { s, len };
    ss.setHash();
    return StringCache::InsertOrGet(ss);
}

// We write using the length because `str` may contain shadowed data
// to Range<const char*>.
std::ostream &nextgen::operator<<(std::ostream &s, str &str) {
  s.write(str.begin(), str.size());
  return s;
}

