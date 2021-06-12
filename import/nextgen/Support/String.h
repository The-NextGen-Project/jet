# ifndef NEXTGEN_STR_H
# define NEXTGEN_STR_H
# include "Core.h"
# include "Allocator.h"

namespace nextgen { using namespace core; using namespace nextgen::mem;

  template<typename T>
  struct Range {

    T begin;
    T end;

    Range(T begin, T end) : begin(begin), end(end) {}

    Range() = default;

    [[nodiscard]]
    NG_INLINE size_t range() const { // STD NAMING
      return (size_t) (end - begin);
    }
  };


  class str { // STD NAMING
  public:
    str() = default;

    struct intern_hash { // std::unordered_map<str, str::intern_hash>
      size_t operator()(str const &s) const {
        return s.getHashCache();
      }
    };

    struct intern_eq {
      NG_INLINE bool operator()(str const &LHS, str const &RHS) const {
        return strcmp(LHS._, RHS._) == 0;
      }
    };


    template<unsigned long N>
    /*implicit*/ constexpr
    str(const char(&data)[N]) : len(N), _(data) {}

    /*implicit*/ str(const char *data)
    : len(strlen(data)), _(data) {}

    /*implicit*/ str(std::string &data) {
      len = data.length();
      _ = data.c_str();
    }

    /*implicit*/ str(char c) {
      len = 1;
      _ = &c;
    }

    explicit str(const char *data, size_t len)
    : len(len), _(data) {}


    explicit str(Range<const char *> range, bool ALLOC = false)
    : len(range.range())  {
      if (ALLOC) {
        // TODO: Allocate Range
      } else {
        // Set empty
        _ = (const char *) range.begin;
      }
    }



    [[nodiscard]]
    NG_INLINE size_t size() const { return len; }


    // String FNV-1a Hashing Algorithm (Internal)
    [[nodiscard]]
    size_t hash() const {
      auto val = FNV_OFF;
      for (auto i = 0; i < len; ++i) {
        val ^= (_[i]);
        val *= FNV_PRIME;
      }
      return val;
    }

    NG_INLINE bool operator==(str const &other) const {
      return _ == other._;
    }

    NG_INLINE char operator[](size_t index) const {
      return (static_cast<const char *>(_))[index];
    }

    NG_INLINE /*implicit*/ operator std::string() const {
      return std::string((const char *) *this);
    }

    /*implicit*/ NG_INLINE operator const char *() const {
      return static_cast<const char *>(_);
    }


    NG_INLINE str operator-(const size_t size) const {
      return {
        _ - size
      };
    }

    NG_INLINE str operator+(int offset) const {
      return {
        _ + offset
      };
    }

    NG_AINLINE char operator*() const {
      return *_;
    }

    NG_AINLINE const char *begin() const {
      return _;
    }

    NG_AINLINE const char *end() const {
      return _ + len;
    }

    // Only used in intern_hash. DO NOT call this
    // function if you have not set your hash value, otherwise your hash
    // results may be invalid.
    NG_INLINE size_t getHashCache() const {
      return hash_cache;
    }

    // Mutation.

    NG_INLINE void setHash() {
      hash_cache = hash();
    }

    NG_INLINE void setHash(size_t hash) {
      hash_cache = hash;
    }

    NG_INLINE str operator++() {
      _++;
      return *this;
    }

    NG_INLINE str operator--() {
      _--;
      return *this;
    }

    NG_INLINE str operator+=(int offset) {
      _ += (size_t) (offset);
      return *this;
    }

    NG_INLINE str operator-=(int offset) {
      _ -= (size_t) (offset);
      return *this;
    }
    const char *_{};     // char* data
    bool is_heap_allocated = false;
  private:
    unsigned long len{}; // String length
    size_t hash_cache = 0;
  };


  // Very thin wrapper around std::unordered_set for String interning.
  // Mostly O(1) Retrieval (FNV-1a is fast and good String hash).
  // Designed around the symbols retrieved for the token and not the token
  // values.
  class StringInterner {
  public:
    using Allocator = mem::ArenaSegment;

    static std::unordered_set<str, str::intern_hash, str::intern_eq> Strings;
    static NG_INLINE str Intern(const str &s) {
      return (*Strings.insert(str(s)).first);
    }

    static NG_INLINE str InsertOrGet(Allocator *Mem,
                                     str &Str) {
      auto Find = Strings.find(Str);
      if (Find != Strings.end()) {
        return *Find;
      } else {

        // Ident, no longer than 256 characters
        auto Size = Str.size();
        auto Insert = (str)
          Mem->CopyValues(const_cast<char *>(Str.begin()), Size);
        Insert.setHash(Str.getHashCache());
        return (*Strings.insert(Insert).first);
      }
    }

  };

  str operator""_intern(const char *s, size_t len);
}

#endif //NEXTGEN_STR_H
