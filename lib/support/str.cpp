# include <nextgen/support/str.h>

// Declare interned strings
std::unordered_set<nextgen::str, nextgen::str::intern_hash,
nextgen::str::intern_eq>
  nextgen::StringInterner::Strings;

// We write using the length because `str` may contain shadowed data
// to Range<const char*>.
std::ostream &operator<<(std::ostream &s, nextgen::str &str) {
  s.write(str.begin(), str.size());
  return s;
}

// Inline in string intern on constant strings
nextgen::str nextgen::operator""_intern(const char *s, size_t len) {
  using nextgen::Range;
  using nextgen::StringInterner;

  str ss = str { s, len };
  ss.setHash();
  return StringInterner::Intern(ss);
}

