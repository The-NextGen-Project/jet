#include <nextgen/allocator.h>
#include <nextgen/cl.h>
#include <nextgen/io.h>

using namespace nextgen::mem;
using namespace nextgen;

typedef struct Jet {

  // Currently, following the way GCC does its optimization levels
  enum OptimizationLevel
    { NoOpt, O1, O2, O3 } opt;


  // CLI connection with the global Jet State
  CommandLine cl;


  // Whether terminal messages are to be enabled (default: true)
  bool colors_enabled = true;

} Jet;


auto add_args(Jet *J) {
  J->cl.add_arg("help", [&]() {
    J->cl.print_command_options();
  });

  J->cl.add_arg("O1", [&]() {
    J->opt = Jet::OptimizationLevel::O1;
  });

  J->cl.add_arg("O2", [&]() {
    J->opt = Jet::OptimizationLevel::O2;
  });

  J->cl.add_arg("O3", [&]() {
    J->opt = Jet::OptimizationLevel::O3;
  });
}

auto init_jet(int argc, char const **argv) {
  Jet *J = new struct Jet;
  J->cl  = CommandLine {
    argc,
    argv
  };
  add_args(J);
}


int main(int argc, char const **argv, char **envP) {
  try {
    auto expr = (((((((((((((((((1)))))))))))))))));
    auto writer = io::File_Writer("nice.txt");
    writer.write("Hi\n");

    writer.close();

  } catch(std::exception&) {
    mem::deinit();
  }
}