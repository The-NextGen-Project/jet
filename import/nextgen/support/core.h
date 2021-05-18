# ifndef NEXTGEN_CORE_H
# define NEXTGEN_CORE_H

# include "none.h"
# include "panic.h"

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


  template<typename T>
  class Option {
  public:

    /*implicit*/ Option(T &value) :
      Some(std::move(value)), is(true) {}

    /*implicit*/ Option(T value) :
      Some(value), is(true) {}

    /*implicit*/ Option(NoneValue) :
      is(false) {}

    NG_AINLINE bool IsSome() {
      return is;
    }

    NG_AINLINE bool IsNone() {
      return !is;
    }

    template<typename U, typename = std::enable_if<PartialEq<T>::value>>
    bool Contains(U x) {
      if (is) return x == Some;
      return false;
    }

    T Unwrap() {
      ASSERT(is, "Unwrapped on None Value");
      return Some;
    }

    template<typename Lambda, typename
    = typename std::enable_if<std::is_convertible<Lambda, std::function<T(
      void)
    >>::value>::type>
    T UnwrapOrElse(Lambda f) {
      if (is) return Some;
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

    /*implicit*/ Result(detail::OkResult, T ok)
      : Ok(ok), is(true) {}


    /*implicit*/ Result(detail::ErrResult, E err)
      : Err(err), is(false) {}

    bool IsOk() {
      return is;
    }

    bool IsErr() {
      return !is;
    }

    template<typename U, typename = std::enable_if<PartialEq<U>::value>>
    bool Contains(U x) {
      if (is) return x == Ok;
      return false;
    }

    Result<T &, E &> AsRef() {
      if (is) return Result(&Ok);
      return Result(&Err);
    }

    template<typename Lambda,
      typename = typename std::enable_if<std::is_convertible<Lambda, std::function<T(
        Result<T, E>)
      >>::value>::type>
    Result<T, E> AndThen(Lambda op) {
      if (is) return op(Ok);
      return Result(Err);
    }

    T Unwrap() {
      ASSERT(is, "Unwrapped Error Result!");
      return Ok;
    }

  private:
    T Ok;
    E Err;
    bool is;
  };


  template<typename T, typename E>
  static Result<T, E> Ok(T value) {
    return Result<T, E>(detail::OkResult::Ok, value);
  }

  template<typename T, typename E>
  static Result<T, E> Err(E value) {
    return Result<T, E>(detail::ErrResult::Err, value);
  }

  template<typename T>
  static NG_AINLINE Option<T> Some(T value) {
    return value;
  }

}} // namespace nextgen::core


#endif //NEXTGEN_CORE_H