#ifndef NEXTGEN_NONE_H
#define NEXTGEN_NONE_H

namespace nextgen {
  enum class NoneValue {
    None = 1
  };
  static constexpr NoneValue None = NoneValue::None;
}

#endif //NEXTGEN_NONE_H