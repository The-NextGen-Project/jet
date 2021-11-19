#include <allocator.h>
#include <util/cl.h>

using namespace nextgen::mem;
using namespace nextgen;

typedef struct Jet {

  // Currently, following the way GCC does its optimization levels
  enum OptimizationLevel
    { NoOpt, O1, O2, O3 } opt;


  // CLI connection with the global Jet State
  Command_Line cl;


  // Whether terminal messages are to be enabled (default: true)
  bool colors_enabled = true;

} Jet;


auto add_args(Jet *J) {
  J->cl.add_arg("help", "Gives this help menu", [&]() {
    J->cl.print_command_options();
  });

  J->cl.add_arg("O1", "Basic optimization level", [&]() {
    J->opt = Jet::OptimizationLevel::O1;
  });

  J->cl.add_arg("O2", "Medium optimization level",[&]() {
    J->opt = Jet::OptimizationLevel::O2;
  });

  J->cl.add_arg("O3", "Aggressive optimization level",[&]() {
    J->opt = Jet::OptimizationLevel::O3;
  });
}

auto init_jet(int argc, char const **argv) {
  Jet *J = new struct Jet;
  J->cl = Command_Line { argc, argv };
  add_args(J);
  J->cl.parse_args();
  return J;
}

int main(int argc, char const **argv, char **envP) {
  try {
    auto value = nextgen::string_buf("Hi ");
    value.format_thing("{}", 213.2);
    value.appendf("{}", 213.2);
    fmt::formatted_size("{}", 213.2);
    std::cout << value << '\n';

  } catch(std::exception&) {
    mem::deinit();
  }
}