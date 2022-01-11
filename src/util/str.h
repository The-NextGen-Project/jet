#ifndef NEXTGEN_STR_H
#define NEXTGEN_STR_H

#include "misc.h"
#include "system.h"

namespace nextgen { using namespace core;

  template<typename T>
  struct Range {
    T begin;
    T end;
  public: // Constructors
    Range() = default;
    Range(T begin, T end) : begin(begin), end(end) {}
  public: // Impl
    NG_AINLINE auto range() const {
      return (size_t) (this->end - this->begin);
    }
  };

  class string_buf {
    char    *buffer = const_cast<char*>("");
    size_t  length  = 0;
    size_t  capacity= 1;
  public: // Constructors
    string_buf() = default;
    explicit string_buf(size_t initial_size) : length(1), capacity(initial_size) {
      this->buffer = new char[initial_size];
    }
    template<size_t N>
    /*implicit*/ string_buf(const char (&s)[N]) : length(N-1), capacity(N), buffer((char*)s) {}
    /*implicit*/ string_buf(const string_buf &new_buf)
      : buffer(new_buf.buffer), length(new_buf.length), capacity(new_buf.capacity) {}


  public: // Impl

    template<size_t N, typename ... Args>
    void appendf(const char (&fmt)[N], Args ... args) {

      auto counting_buf = fmt::detail::counting_buffer<>();

      fmt::detail::vformat_to(counting_buf, fmt::string_view(fmt), fmt::make_format_args(args...),
                              {});
      auto [fmt_size, fmt_buf] = tuple(counting_buf.count(), counting_buf.data());
      auto new_size = fmt_size + this->length;
      auto added_size = fmt_size-1;

      char * apply_buf = this->buffer + this->length;

      if (new_size >= this->capacity) {
        auto old_buf = this->buffer;
        this->capacity = (new_size) * 2;
        this->buffer = new char[this->capacity];
        sys::memmove(this->buffer, old_buf, this->length);
        apply_buf = this->buffer+(this->length);
      }

      sys::memmove(apply_buf, fmt_buf, fmt_size);
      this->length = new_size;
    }

    [[nodiscard]] NG_AINLINE auto size() const {
      return this->length;
    }

  public: // Global init
    static auto empty() {
        return nextgen::string_buf();
    }
  public: // Operator Overloads
    NG_AINLINE operator std::string() const {
      return std::string(this->buffer, this->length);
    }
    NG_AINLINE operator std::string_view() const {
      return std::string_view(this->buffer, this->length-1);
    }
    NG_AINLINE constexpr operator const char *() const {
      return static_cast<const char *>(this->buffer);
    }
  };


  struct string_view_hash {
    struct hash {
      auto operator()(std::string_view const &s) const {
        auto data = s.data();
        auto hash = FNV_OFF;
        for (auto i = 0; i < s.length(); ++i) {
          hash ^= (data[i]);
          hash *= FNV_PRIME;
        }
        return hash;
      }
    };
    struct eq {
      auto operator()(std::string_view const &lhs, std::string_view const &rhs) const {
        return lhs == rhs;
      }
    };
  };

  std::ostream &operator<<(std::ostream &os, const string_buf &buf);
}


template <> struct fmt::formatter<nextgen::string_buf>: formatter<std::string_view> {
  template <typename FormatContext>
  auto format(nextgen::string_buf sbuf, FormatContext& ctx) const {
    return formatter<std::string_view>::format(sbuf, ctx);
  }
};

#endif //NEXTGEN_STR_H
