#ifndef JET_CL_H
#define JET_CL_H

#include "panic.h"
#include "str.h"

namespace nextgen {

  struct Command_Line_Arg {
    std::string_view name;
    std::string_view description = "";
    std::function<void()> action;
  };


  class Command_Line {
    int argc;
    char const **argv;
    std::vector<Command_Line_Arg> args;
  public: // Constructor
    Command_Line() = default;
    explicit Command_Line(int argc, char const **argv) : argc(argc), argv(argv) {}
  public: // Impl
    template<typename Lambda, LAMBDA(Lambda, void, void)>
    auto add_arg(const std::string_view &name, Lambda action)  {
      this->args.emplace_back(Command_Line_Arg {name, "", action });
    }

    template<typename Lambda, LAMBDA(Lambda, void, void)>
    auto add_arg(const std::string_view &name, const std::string_view &desc, Lambda action)  {
      this->args.emplace_back(Command_Line_Arg {name, desc, action });
    }

    auto print_command_options(void) {
      Console::Log(Colors::YELLOW, "Jet Command Options\n", Colors::RESET);
      Console::Log("Available options are:\n");
      for (auto arg : this->args) {
        Console::Log("  -", arg.name, "\t", arg.description, '\n');
      }
    }

    auto parse_args(void) {
      // Right now, we ignore input that does not follow the format "-" and ignore all instructions
      // that are not register. We do not report invalid commands given.
      FOR(i, this->argc) {
        auto arg = std::string_view(this->argv[i]);
        if (arg.starts_with("-")) {
          auto parsed_arg = arg.substr(1);
          auto value = std::find_if(this->args.begin(), this->args.end(), [parsed_arg](auto arg) {
            return arg.name == parsed_arg;
          });
          if (value not_eq this->args.end()) {
            value->action();
          }
        }
      }
    }
  };

}



#endif //JET_CL_H
