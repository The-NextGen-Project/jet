#ifndef NEXTGEN_CORE_H
#define NEXTGEN_CORE_H
#include "config.h"
#include "panic.h"


namespace nextgen {

  enum class NoneValue { None = 1 };
  static constexpr NoneValue None = NoneValue::None;


  template<typename T, size_t N>
  constexpr size_t SizeOfArray(const T(&arr)[N]) {
    return N;
  }


  template<typename ... Args>
  constexpr auto tuple(Args&& ... args) {
    return std::forward_as_tuple(args...);
  }

  namespace core {

    template<typename T>
    class Option {
      T Some;
      bool is_value;
    public:
      // None value by default
      Option() : is_value(false) {};

      // Direct Copy
      Option(Option<T> const &other) : is_value(other.is_value), Some(other.Some) {}

      Option(T const &value) : Some(std::move(value)), is_value(true) {}
      /*implicit*/ Option(NoneValue) : is_value(false) {}

      [[nodiscard]] NG_AINLINE bool is_some() const {
        return is_value;
      }

      [[nodiscard]] NG_AINLINE bool is_none() const {
        return !is_value;
      }

      template<typename U>
      NG_AINLINE bool contains(U x) const {
        return is_value && x == Some;
      }

      auto unwrap() const {
        ASSERT(is_value, "Unwrapped on None Value");
        return std::move(Some);
      }

      template<typename Lambda, LAMBDA(Lambda, T, void)>
      auto unwrap_or_else(Lambda f) const {
        return is_value ? std::move(Some) : f();
      }
    };

    template<typename T>
    static NG_AINLINE auto Some(T value) {
      return Option<T>(value); // NOTE: Make explicit
    }
  }
} // namespace nextgen::core


#endif //NEXTGEN_CORE_H