#ifndef NEXTGEN_CORE_H
#define NEXTGEN_CORE_H
#include "os-config.h"
#include "panic.h"


namespace nextgen {

  enum class NoneValue { None = 1 };
  static constexpr NoneValue None = NoneValue::None;

  template<typename T>
  struct Array {
    size_t len;
    const T arr[];
  };

  template<typename T, size_t N>
  constexpr size_t SizeOfArray(const T(&arr)[N]) {
    return N;
  }

  namespace core {

    /// Optional Type Value - Emulates Rust's style of an optional value in C++11.
    /// We must ensure that values are owned by the value when passed.
    template<typename T>
    class Option {
    public:
      // None value by default
      Option() : is(false) {};

      // Direct Copy
      Option(Option<T> const &other) : is(other.is), Some(other.Some) {}

      Option(T const &value) :
        Some(std::move(value)), is(true) {}

      /*implicit*/ Option(NoneValue) :
        is(false) {}

      NG_AINLINE bool isSome() const {
        return is;
      }

      NG_AINLINE bool isNone() const {
        return !is;
      }

      template<typename U>
      bool contains(U x) const {
        if (is) return x == Some;
        return false;
      }

      auto unwrap() const {
        ASSERT(is, "Unwrapped on None Value");
        return std::move(Some);
      }

      template<typename Lambda, LAMBDA(Lambda, T, void)>
      auto unwrap_or_else(Lambda f) {
        if (is) return std::move(Some);
        return f();
      }

    private:
      T Some;
      bool is;
    };

    template<typename T>
    static NG_AINLINE auto Some(T value) {
      return Option<T>(value); // NOTE: Make explicit
    }
  }
} // namespace nextgen::core


#endif //NEXTGEN_CORE_H