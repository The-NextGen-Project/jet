# ifndef NEXTGEN_NONE_H
# define NEXTGEN_NONE_H
# include "Panic.h"

namespace nextgen { // Putting this in the global namespace
  enum class NoneValue { None = 1 };
  static constexpr NoneValue None = NoneValue::None;

  template<typename T, size_t N>
  struct Array {
    T array[N];
  };

  template<typename T, size_t N>
  constexpr size_t SizeOfArray(const T(&arr)[N]) {
    return N;
  }

# define match(...) switch (pair(__VA_ARGS__))
# define group(...) case pair(__VA_ARGS__)

#   ifdef BIT64
#       define SHIFT_PAIR 16
#   else
#       define SHIFT_PAIR 8
#   endif

  template <typename Enum>
  constexpr int pair(Enum k1) { // STD NAMING
    return k1;
  }

  template<typename Enum1, typename Enum2>
  constexpr int pair(Enum1 k1, Enum2 k2) {
    return ((k1 + k2) << SHIFT_PAIR) | (size_t)k2;
  }

  template<typename Enum1, typename Enum2, typename Enum3>
  constexpr int pair(Enum1 k1, Enum2 k2, Enum3 k3) {
    return ((k1 + k2 + k3) << SHIFT_PAIR) | (k2 * k3);
  }

  template<typename Enum1, typename Enum2, typename Enum3, typename Enum4>
  constexpr int pair(Enum1 k1, Enum2 k2, Enum3 k3, Enum4 k4) {
    return ((k1 + k2 + k3 + k4) << SHIFT_PAIR) | (k2 * k3 * k4);
  }
}

# endif //NEXTGEN_NONE_H
