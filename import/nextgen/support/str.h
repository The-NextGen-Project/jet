# ifndef NEXTGEN_STR_H
# define NEXTGEN_STR_H
# include "core.h"

namespace nextgen { using namespace core;

  template<typename T>
  struct Range {

    const T begin;
    const T end;

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

    template<unsigned long N>
    /*implicit*/ constexpr
    str(const char(&data)[N]) : len(N), _(data) {}

    /*implicit*/ str(const char *data) : len(strlen(data)),
                                         _(data) {}

    /*implicit*/ str(std::string &data) {
      len = data.length();
      _ = data.c_str();
    }

    // It is important to understand the the string type created here does
    // not own the pointer. The caller owns this pointer and nextgen::str is
    // not responsible for the cleanup of the passed pointer.
    explicit str(Range<const char *> range) : len(range.range()) {
      _ = (const char *) range.begin;
    }

    [[nodiscard]]
    unsigned long size() const { return len; }

    // Standard FNV Hash algorithm on the contents of the internal buffer.
    // NOTE: decltype is done on the offset only to have a type that matches
    // the system that we are on.
    [[nodiscard]]
    decltype(FNV_OFF) hash() const {
      decltype(FNV_OFF) val = FNV_OFF;
      for (auto i = 0; i < len; ++i) {
        val ^= *_ + i;
        val *= FNV_PRIME;
      }
      return val;
    }

    char operator[](size_t index) const {
      return (static_cast<const char *>(_))[index];
    }

    explicit operator std::string() const {
      return std::string((const char *) *this);
    }

    /*implicit*/ operator const char *() const {
      return static_cast<const char *>(_);
    }

    size_t operator-(const str RHS) const {
      return (size_t) (_ - RHS._);
    }

    NG_INLINE str operator+(int offset) const {
      return {
        _ + offset
      };
    }

    // Mutation.

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


    friend std::ostream &
    operator<<(std::ostream &s, const str &str);

  private:
    const char *_{};     // char* data
    unsigned long len{}; // String length
  };

}

#endif //NEXTGEN_STR_H
