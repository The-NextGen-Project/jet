# ifndef NEXTGEN_CORE_H
# define NEXTGEN_CORE_H

# include "none.h"
# include "allocator.h"

namespace nextgen { namespace core {

  namespace detail {

    // PartialEq Equivalent in C++.
    template<typename T, typename K, typename = K>
    struct PartialEq : std::false_type {
    };

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

    // HasRange Concept Equivalent in C++11.
    template<typename T, typename K, typename = K>
    struct HasRange : std::false_type {};

    template<typename T, typename K>
    struct HasRange<T, K,
      decltype ((size_t) (std::declval<T>() - std::declval<T>()))>
      : std::true_type {};

  }
  template<typename T, typename K = bool>
  struct PartialEq : detail::PartialEq<T, K> {};

  template<typename T, typename K = bool>
  struct HasRange : detail::HasRange<T, K> {};


  template<typename T>
  class Option {
  public:
    template<typename X>
    static Option<X> from(X value) {
      return Option(value);
    }

    /*implicit*/ Option(T &value) :
      Some(std::move(value)), is(true) {}

    /*implicit*/ Option(T value) :
      Some(value), is(true) {}

    /*implicit*/ Option(NoneValue) :
      is(false) {}

    NG_AINLINE bool is_some() {
      return is;
    }

    NG_AINLINE bool is_none() {
      return !is;
    }

    template<typename U, typename = std::enable_if<PartialEq<T>::value>>
    bool contains(U x) {
      if (is) return x == Some;
      return false;
    }

    T unwrap() {
      ASSERT(is, "Unwrapped on None Value");
      return Some;
    }

    template<typename Lambda, typename
    = typename std::enable_if<std::is_convertible<Lambda, std::function<T(
      void)
    >>::value>::type>
    T unwrap_or_else(Lambda f) {
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

    template<typename X, typename Y>
    static Result<X, Y> make(X value) {
      return Result<X, Y>(value);
    }

    /*implicit*/ Result(detail::OkResult, T ok)
      : Ok(ok), is(true) {}


    /*implicit*/ Result(detail::ErrResult, E err)
      : Err(err), is(false) {}

    bool is_ok() {
      return is;
    }

    bool is_err() {
      return !is;
    }

    template<typename U, typename = std::enable_if<PartialEq<U>::value>>
    bool contains(U x) {
      if (is) return x == Ok;
      return false;
    }

    Option<T> ok() {
      if (is) return Option<T>(Ok);
      return None;
    }

    Option<E> err() {
      if (is) return None;
      return Option<E>(Err);
    }

    Result<T &, E &> as_ref() {
      if (is) return Result(&Ok);
      return Result(&Err);
    }

    template<typename Lambda,
      typename = typename std::enable_if<std::is_convertible<Lambda, std::function<T(Result<T, E>)
    >>::value>::type>
    Result<T, E> and_then(Lambda op) {
      if (is) return op(Ok);
      return Result(Err);
    }

    T unwrap()  {
      ASSERT(is, "Unwrapped Error Result!");
      return Ok;
    }

  private:
    T Ok;
    E Err;
    bool is;
  };



  // Bare-bones List structure for holding large amounts of elements. This is
  // meant to hold items in places where limits are tested, for example, a
  // program may have hundreds of thousands of tokens that need to be parsed
  // and so the size of the list increases quite a lot to reflect that need.
  template <typename T>
  class List {
  public:
    List() {
      list = (T*) mem::os::malloc(sizeof(T) * 2);
    }
    explicit List(size_t reserve) : cap(reserve)
    {
      list = (T*) mem::os::malloc(sizeof(T) * reserve);
    }

    void add(T element) {
      if (len + 1 > cap) {
        cap *= 3;
        list = (T*) mem::os::realloc(list, cap);
      }
      list[len++] = element;
    }

    T pop() {
      ASSERT(len >= 1, "Invalid List Pop.");
      return list[--len];
    }

    NG_AINLINE T operator[](size_t index) const {
      return list[index];
    }

  private:
    T *list;
    size_t len{0};
    size_t cap{2};
  };

  template<typename T, typename E>
  static Result<T, E> Ok(T value)  {
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