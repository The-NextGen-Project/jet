# ifndef NEXTGEN_PANIC_H
# define NEXTGEN_PANIC_H
# include "config.h"


namespace nextgen {

# if defined(NG_OS_LINUX) || defined(NG_OS_APPLE)

  // ANSI Terminal Colors for Linux and MacOS. The colors are strictly
  // pre-defined to be White, Red, Green, Yellow, Blue, and Cyan and nothing
  // more that is needed to highlight something in the terminal.
  struct Colors {
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

# endif

  struct Console {

# if defined(NG_OS_WINDOWS)

    // A little guessing trick that has the compiler infer which function gets
    // called. It chooses default template unless we use a color.
    static void Write(Colors color) {
      static HANDLE GenericConsole = GetStdHandle(STD_OUTPUT_HANDLE);
      SetConsoleTextAttribute(GenericConsole, color);
    }

    // Wrapper around std::cout to mask terminal color settings
    // in the other `Write` function.
    template <typename Arg>
    static void Write(Arg value) {
      std::cout << value;
    }

# endif

    template<typename ... Args>
    NG_INLINE static void Log(Args ... args) {
      using fold = int[];
# if defined(NG_OS_LINUX) || defined(NG_OS_APPLE)
      fold{
        (std::cout << args, 0)...
      };
# elif defined(NG_OS_WINDOWS)
      fold {
        ((Write(args)), 0)...
      };
# endif
    }
  };

# define PANIC(msg) nextgen::PanicAt<sizeof(msg), sizeof(__FILE__), sizeof \
("panic("#msg");"), __LINE__, false>(msg, __FILE__, "panic("#msg");")
# define ASSERT(cond, msg) if (!(cond)) nextgen::PanicAt<sizeof(msg), sizeof \
(__FILE__), sizeof("assert("#cond", "#msg");"), __LINE__, true>(msg, __FILE__, "assert("#cond", "#msg");")

  // Panic at a specific point in the code. This is simulated as an exception throw caught by
  // main and returns a failure as to avoid misuse of "std::exit". It prints out the line number,
  // and text of the panic message to have the user locate the failed point in the code.
  template<
    std::size_t N1, std::size_t N2, std::size_t N3,
    int LINE, bool Assert>
  void PanicAt(
    const char (&msg)[N1],  /* Panic Message */
    const char (&FILE)[N2], /* File Name of Error */
    const char (&dup)[N3]) { /* Line text of Panic */

    Console::Log(FILE, ":", LINE, " ", Colors::RED, "error: ",
                 Colors::WHITE,
                 msg);
    Console::Log('\n', Colors::RESET);

    auto whitespace = (std::to_string(LINE) + " | ").length();
    if (Assert) {
      Console::Log(Colors::RED, LINE, " | ", dup, Colors::WHITE,
                   " <-- ",
                   Colors::RED, "Assert Failed:", Colors::YELLOW,
                   " Thread panicked here", "\n", Colors::BLUE);
    } else {
      Console::Log(Colors::RED, LINE, " | ", dup, Colors::YELLOW,
                   " <-- Thread panicked here", "\n",
                   Colors::BLUE);
    }
    /* Leading Whitespace */
    for (auto i = 0; i < whitespace; ++i) Console::Log(" ");
    /* Highlighting region */
    for (auto i = 0; i < N3 - 1; ++i) Console::Log('~');
    Console::Log(Colors::RESET, '\n');
    throw std::exception();
  }
}


# endif //NEXTGEN_PANIC_H
