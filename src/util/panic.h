#ifndef NEXTGEN_PANIC_H
#define NEXTGEN_PANIC_H
#include "config.h"

#if defined(NG_OS_LINUX) || defined(NG_OS_APPLE)

// ANSI Terminal Colors for Linux and MacOS. The colors are strictly
  // pre-defined to be White, Red, Green, Yellow, Blue, and Cyan and nothing
  // more that is needed to highlight something in the terminal.

  // NOTE: This is namespace because of weird debug issue with GCC. For some
  // reason in debug mode, the symbols are not resolved by the linker but works
  // in release mode. The effect is the same either way with a namespace so we
  // leave it as such.
  namespace Colors {
    static constexpr auto WHITE   = COLOR_WHITE;
    static constexpr auto RED     = COLOR_RED;
    static constexpr auto GREEN   = COLOR_GREEN;
    static constexpr auto YELLOW  = COLOR_YELLOW;
    static constexpr auto BLUE    = COLOR_BLUE;
    static constexpr auto CYAN    = COLOR_CYAN;
    static constexpr auto RESET   = COLOR_RESET;
  };
# elif defined(NG_OS_WINDOWS)

// NOTE: These colors are number values designated as console "modes" for
// the Windows Terminal. Windows uses these modes to render text as a
// specific color.
enum Colors {
  WHITE  = COLOR_WHITE,
  RED    = COLOR_RED,
  GREEN  = COLOR_GREEN,
  YELLOW = COLOR_YELLOW,
  BLUE   = COLOR_BLUE,
  CYAN   = COLOR_CYAN,
  RESET  = COLOR_RESET
};

# else
namespace Colors {
    static constexpr auto WHITE   = "";
    static constexpr auto RED     = "";
    static constexpr auto GREEN   = "";
    static constexpr auto YELLOW  = "";
    static constexpr auto BLUE    = "";
    static constexpr auto CYAN    = "";
    static constexpr auto RESET   = "";
  };
# endif


namespace nextgen {

  template<size_t N, typename ... Args>
  void print(const char (&fmt)[N], Args&& ... args) {
    fmt::print(fmt, args...);
  }

  struct Console {

# if defined(NG_OS_WINDOWS)

    // A little guessing trick that has the compiler infer which function gets
    // called. It chooses default template unless we use a color.
    static void Write(Colors color) {

      // TODO: Global Program State will check whether colors have been enabled before writing to
      //  the terminal
      static HANDLE GenericConsole = GetStdHandle(STD_OUTPUT_HANDLE);
      (void) SetConsoleTextAttribute(GenericConsole, color);
    }

    // Wrapper around std::cout to mask terminal color settings
    // in the other `Write` function.
    template <typename Arg>
    [[maybe_unused]] static void Write(Arg value) {
      std::cout << value;
    }
# endif
    template<typename ... Args>
    static void Log(Args ... args) {
# if defined(NG_OS_LINUX) || defined(NG_OS_APPLE)
      ((std::cout << args), ...);
# elif defined(NG_OS_WINDOWS)
      ((Write(args)), ...);
# endif
    }
  };


// TODO: Port this to std::source_location but like, that will be annoying
# define PANIC(msg) nextgen::PanicAt<sizeof(msg), sizeof(__FILE__), sizeof \
("PANIC("#msg");"), __LINE__, false>(msg, __FILE__, "PANIC("#msg");")
# define ASSERT(cond, msg) if (!(cond)) nextgen::PanicAt<sizeof(msg), sizeof \
(__FILE__), sizeof("ASSERT("#cond", "#msg");"), __LINE__, true>(msg, __FILE__, \
"ASSERT("#cond", "#msg");")
# define ASSERT_EQ(cond, value) if (!((cond) == (value))) nextgen::PanicAt<sizeof \
("Expected equality"), sizeof (__FILE__), sizeof("ASSERT("#cond", "#value");"), __LINE__, \
true>("Expected equality", __FILE__, \
"ASSERT("#cond", "#value");")
# define ASSERT_TRUE(cond) if (!(cond)) \
nextgen::PanicAt<sizeof \
("Expected value to be true"), sizeof (__FILE__), sizeof("ASSERT_TRUE("#cond");"), __LINE__, \
true>("Expected value to be true", __FILE__, \
"ASSERT_TRUE("#cond");")


  template<std::size_t N1, std::size_t N2, std::size_t N3, int LINE, bool Assert>
  static void PanicAt(const char (&msg)[N1], const char (&FILE)[N2], const char (&dup)[N3]) {
    Console::Log(FILE, ":", LINE, " ", Colors::RED, "error: ", Colors::WHITE, msg);
    Console::Log('\n', Colors::RESET);

    auto whitespace = fmt::formatted_size("{} | ", LINE);
    if (Assert) {
      Console::Log(Colors::RED, LINE, " | ", dup, Colors::WHITE, " <-- ", Colors::RED,
                   "Assert "
                   "Failed:", Colors::YELLOW, " Thread panicked here", "\n", Colors::BLUE);
    } else {
      Console::Log(Colors::RED, LINE, " | ", dup, Colors::YELLOW, " <-- Thread panicked here\n",
                   Colors::BLUE);
    }
    /* Leading Whitespace */
    FOR(i, whitespace) Console::Log(" ");
    /* Highlighting region */
    FOR(i, N3 - 1) Console::Log('~');
    Console::Log(Colors::RESET, '\n');
    throw std::exception();
  }
}



#endif //NEXTGEN_PANIC_H
