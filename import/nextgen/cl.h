#ifndef JET_CL_H
#define JET_CL_H

#include "panic.h"
#include "str.h"

namespace nextgen {

  struct CommandLineArg {
    str name;
    str associated_value = "";
    std::function<void()> action;
  };


  class CommandLine {
    ObjectVector<CommandLineArg, 100> args;
    ArrayView<const char *> input;
  public:
    CommandLine() = default;
    explicit CommandLine(int argc, char const **argv) {
      input = ArrayView<const char*> { size_t(argc), argv };
    }

    template<typename Lambda, LAMBDA(Lambda, void, void)>
    auto add_arg(const str &name, Lambda action)  {
      args << CommandLineArg { name, "", action };
    }

    template<typename Lambda, LAMBDA(Lambda, void, void)>
    auto add_arg(const str &name, const str &v, Lambda action)  {
      args << CommandLineArg { name, v, action };
    }


    auto print_command_options() {

    }
  };

}



#endif //JET_CL_H
