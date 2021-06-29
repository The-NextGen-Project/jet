# ifndef NEXTGEN_CORE_H
# define NEXTGEN_CORE_H

# include "None.h"

namespace nextgen { namespace core {

  namespace detail {

    // PartialEq Equivalent in C++.
    template<typename T, typename K, typename = K>
    struct PartialEq : std::false_type {};

    template<typename T, typename K>
    struct PartialEq<T, K,
      decltype(
      // In Rust, a PartialEq value must have the '==' operator and the '!='
      // operator overloaded.
      std::declval<T>() == std::declval<T>() &&
      std::declval<T>() != std::declval<T>()
      )>
      : std::true_type {
    };

  }
  template<typename T, typename K = bool>
  struct PartialEq : detail::PartialEq<T, K> {};

  /// Optional Type Value - Emulates Rust's style of an optional value in C++11.
  /// We must ensure that values are owned by the value when passed.
  template<typename T>
  class Option {
  public:
    // None value by default
    Option() : is(false) {};

    // Direct Copy
    Option(const Option<T> &Other) : is(Other.is), Some(Other.Some) {}

    explicit Option(T &value) :
    Some(std::move(value)), is(true) {}

    /*implicit*/ Option(NoneValue) :
    is(false) {}

    NG_AINLINE bool isSome() {
      return is;
    }

    NG_AINLINE bool isNone() {
      return !is;
    }

    template<typename U, typename = std::enable_if<PartialEq<T>::value>>
    bool Contains(U x) {
      if (is) return x == Some;
      return false;
    }

    T Unwrap() {
      ASSERT(is, "Unwrapped on None Value");
      return std::move(Some);
    }

    template<typename Lambda, LAMBDA(Lambda, T, void)>
    T UnwrapOrElse(Lambda f) {
      if (is) return std::move(Some);
      return f();
    }

  private:
    T Some;
    bool is;
  };

  namespace detail {
    enum OkResult {
      Ok
    };
    enum ErrResult {
      Err
    };
  }

  template<typename T, typename E>
  class Result {
  public:

    /*implicit*/ Result(detail::OkResult, T &ok)
      : Ok(ok), is(true) {}


    /*implicit*/ Result(detail::ErrResult, E err)
      : Err(err), is(false) {}

    NG_AINLINE bool IsOk() {
      return is;
    }

    NG_AINLINE bool IsErr() {
      return !is;
    }

    template<typename U, typename = std::enable_if<PartialEq<U>::value>>
    bool Contains(U x) {
      if (is) return x == Ok;
      return false;
    }

    auto Error() -> Option<E> {
      if (!is) return Option<E>(Err);
      return None;
    }

    auto AsRef() -> Result<T &, E &>{
      if (is) return Result(&Ok);
      return Result(&Err);
    }

    template<typename Lambda, LAMBDA(Lambda, T, Result<T, E>)>
    auto AndThen(Lambda op) -> Result<T, E>  {
      if (is) return op(Ok);
      return Result(Err);
    }

    NG_INLINE T Unwrap() {
      ASSERT(is, "Unwrapped Error Result!");
      return Ok;
    }

  private:
    T Ok;
    E Err;
    bool is;
  };


  template<typename T, typename E>
  static auto Ok(T value) -> Result<T, E> {
    return Result<T, E>(detail::OkResult::Ok, value);
  }

  template<typename T, typename E>
  static auto Err(E value) -> Result<T, E> {
    return Result<T, E>(detail::ErrResult::Err, value);
  }

  template<typename T>
  static NG_AINLINE auto Some(T value) -> Option<T> {
    return Option<T>(value); // Make explicit
  }


}} // namespace nextgen::core


#endif //NEXTGEN_CORE_H