# ifndef NEXTGEN_NONE_H
# define NEXTGEN_NONE_H
# include "panic.h"

namespace nextgen { // Putting this in the global namespace
  enum class NoneValue { None = 1 };
  static constexpr NoneValue None = NoneValue::None;


  template<typename T, size_t N>
  constexpr size_t SizeOfArray(const T(&arr)[N]) {
    return N;
  }

# define match(...) switch (pair(__VA_ARGS__))
# define set(...) case pair(__VA_ARGS__)

  template <typename Enum, typename = std::enable_if<std::is_enum<Enum>::value>>
  constexpr int pair(Enum k1) { // STD NAMING
    return k1;
  }

  template<typename Enum1, typename Enum2>
  constexpr int pair(Enum1 k1, Enum2 k2) {
# ifdef BIT32
    return ((k1 + k2) << 8) | k2;
# endif
# ifdef BIT64
    return ((k1 + k2) << 16) | k2;
# endif
  }

  template<typename Enum1, typename Enum2, typename Enum3>
  constexpr int pair(Enum1 k1, Enum2 k2, Enum3 k3) {
# ifdef BIT32
    return ((k1 + k2 + k3) << 8) | (k2 * k3);
# endif
# ifdef BIT64
    return ((k1 + k2 + k3) << 16) | (k2 * k3);
# endif
  }

  template<typename Enum1, typename Enum2, typename Enum3, typename Enum4>
  constexpr int pair(Enum1 k1, Enum2 k2, Enum3 k3, Enum4 k4) {
# ifdef BIT32
    return ((k1 + k2 + k3 + k4) << 8) | (k2 * k3 * k4);
# endif
# ifdef BIT64
    return ((k1 + k2 + k3 + k4) << 16) | (k2 * k3 * k4);
# endif
  }


}

# endif //NEXTGEN_NONE_H
