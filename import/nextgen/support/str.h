# ifndef NEXTGEN_STR_H
# define NEXTGEN_STR_H

# include "core.h"

namespace nextgen {
using namespace core;


  template <typename T>
  struct Range {

    const T begin;
    const T end;

    Range(T begin, T end) : begin(begin), end(end) {}
    Range() = default;

    [[nodiscard]]
    NG_INLINE unsigned long range() const {
      return (unsigned long) (end - begin);
    }
  };



  class str  {
  public:
    str() = default;

    template <unsigned long N>
    constexpr str(const char(&data)[N]) : len(N), _(data)
    {}

    str(const char *data) : len(strlen(data)), _(data) {}
    str(std::string &data) {
      len = data.length();
      _   = data.c_str();
    }

    // It is important to understand the the string type created here does
    // not own the pointer. The caller owns this pointer and nextgen::str is
    // not responsible for the cleanup of the passed pointer.
    str(Range<const char *> range) : len(range.range()) {
      _ = (const char *) range.begin;
    }

    [[nodiscard]]
    unsigned long size() const { return len; }

    [[nodiscard]]
    unsigned long hash() const { /* FNV Hash Algorithm */
      unsigned long val = FNV_OFF;
      for (auto i = 0; i < len; ++i) {
        val ^= *_ + i;
        val *= FNV_PRIME;
      }
      return val;
    }


    // Finds the specified pattern (not regex) but a consistent sequence of
    // values. For example, calling find for "aa" will send the index to that
    // first 'a' point and if not found returns a NoneValue.
    /*Option<size_t> find(const char *pattern) {
      size_t index = strcspn(_, pattern);
      if (index < len)
        return Some(index);
      return None;
    }*/

    char operator[](size_t index) {
      return (static_cast<const char *>(_))[index];
    }

    operator std::string() {
      return std::string(static_cast<const char *>(_));
    }

    operator const char *() const {
      return static_cast<const char *>(_);
    }

    operator unsigned long() const {
      return (unsigned long)(const char *)_;
    }

    unsigned long operator -(const str RHS) const {
      return (unsigned long)(*this) - (unsigned long)(RHS);
    }

    NG_INLINE str operator +(int offset) const {
      return {
        _ + offset
      };
    }

    // Mutation.

    NG_INLINE str operator ++() {
      _++;
      return *this;
    }

    NG_INLINE str operator--() {
      _--;
      return *this;
    }

    NG_INLINE str operator+=(int offset) {
      _ += (size_t)(offset);
      return *this;
    }

    NG_INLINE str operator -=(int offset) {
      _ -= (size_t)(offset);
      return *this;
    }


    friend std::ostream &operator<<(std::ostream &s, const str &str);

  private:
    const char *_{};     // char* data
    unsigned long len{}; // String length
  };

}

#endif //NEXTGEN_STR_H
