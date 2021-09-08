#include <nextgen/jet/jet-diagnostics.h>
#include <nextgen/jet/jet-parser.h>

using nextgen::Console;
using nextgen::jet::Diagnostic;
using nextgen::str;
using nextgen::Range;

// Color namespacing
#ifdef NG_OS_WINDOWS
  using nextgen::Colors;
#else
  using namespace nextgen::Colors;
#endif

static str GetNthLineOfBuffer(size_t nth,
                              const char *file_buf,
                              size_t buf_len) {
  bool found = false;
  int count = 1;
  const char *copy = file_buf;
  const char *found_point;

  if (nth == 1) {
    return str {file_buf, buf_len};
  }

  for (auto I = 0; I < buf_len; ++I) {
    if (*copy == '\n') {
      if (found) {
        return str(Range<const char *>(found_point, copy));
      }
      count++;
    }
    if (count == nth) {
      if (!found) {
        found_point = copy + 1;
        found = true;
      }
    }
    copy++;
  }

  return str {found_point, buf_len};
}

template<typename... Args>
static void AddHint(std::string &line, Args&& ... hint) {


  // Align space
  FOR(i, line.length() + 1) {
    Console::Log(" ");
  }

  Console::Log(hint..., nextgen::Colors::RESET);
}

void Diagnostic::build(LexError error) {
  source_line = GetNthLineOfBuffer(error.location.line, file_buf, buf_len);
  Console::Log(Colors::RED,
         "Error ---------------------------------------------------------- ",
         file_name, ":", error.location.line, ":", error.location.column, "\n",
         Colors::RESET);

  switch (error.error) {
    case MalformedUTF8:
      break;
    case InvalidChar:
      Console::Log("Invalid Char");
      break;
    case UnexpectedEOF:
      Console::Log("EOF");
      break;
    case OutOfRange:
      Console::Log("Out of Buffer Range");
      break;
    case Unreachable:
      Console::Log("Unreachable");
      break;
    case IntegerOverflow:
      ErrorIntegerOverflow(error);
      break;
    case FloatingPointOverflow:
      break;
    case DigitOutOfRange:
      ErrorDigitOutOfRange(error);
      break;
    case MissingClosingDelim:
      ErrorMissingClosingDelim(error);
      break;
    case InvalidDot:
      Console::Log("Invalid Dot");
      break;
    case InvalidNumberValue:
      Console::Log("Invalid Number");
      break;
    case InvalidStringEscape:
      ErrorInvalidStringEscape(error);
      break;
    case InvalidUnicodeEscapeClose:
      break;
    case HexEscapeOutOfRange:
      ErrorHexEscapeOutOfRange(error);
      break;
    default:
      UNREACHABLE;
  }
  Console::Log(Colors::RESET, "\n");
}


void Diagnostic::build(ParseError error) {
  switch (error.error) {
    case ReservedIdentifierAsVariableName:
      break;
    case UnexpectedExpression:
      break;
    case InvalidToken:
      break;
    case ExpectedToken:
      break;
    case MissingFunctionName:
      break;
    case MissingVariableName:
      break;
  }
}


/// ------------- Lexing Errors -------------


void Diagnostic::ErrorMissingClosingDelim(nextgen::jet::LexError &error) {
  Console::Log(Colors::RESET, "Missing Closing Delim\n\n");
  auto Line = std::to_string(error.location.line);

  ErrorLexSetup(Line, "Reached EOF Before Closing Delim", error);
  AddHint(Line, Colors::CYAN, "= ", Colors::GREEN, "try: ",
          Colors::RESET, "Add `", (char) error.metadata[0], "` to the end of "
                                                            "the statement");
}

void Diagnostic::ErrorIntegerOverflow(LexError &error) {
  Console::Log(Colors::RESET, "Integer Value Too Large\n\n");
  auto Line = std::to_string(error.location.line);

  ErrorLexSetup(Line, "Overflow Occurs Here", error);

  AddHint(Line, Colors::BLUE, "= ", Colors::GREEN, "hint: ",
               Colors::RESET, "Integer values must be less than ", UINTPTR_MAX,
                              ".\n",
               Colors::RESET, '\n');

  Console::Log("An ", Colors::RED,
               "Integer Overflow", Colors::RESET,
               " occurs when a numeric literal's value ", Colors::BLUE,
               "exceeds ", Colors::RESET,
               "the limit bound by their computer. \n", Colors::YELLOW);
# ifdef BIT32
    Console::Log("Your computer is 32-Bit", Colors::RESET,
                 " which has a limit of ",
                 Colors::GREEN, UINTPTR_MAX, Colors::RESET, " but ",
                 "your value exceeds that.");
# else
  Console::Log("Your computer is 64-Bit", Colors::RESET,
                 " which has a limit of ",
                 Colors::GREEN, UINTPTR_MAX, Colors::RESET, " but ",
                 "your value exceeds that.");
# endif
}

void Diagnostic::ErrorInvalidChar(LexError &error) {

}

void Diagnostic::ErrorMalformedUTF8(LexError &error) {

}


void Diagnostic::ErrorDigitOutOfRange(LexError &error) {
  Console::Log(Colors::RESET, "Digit Value Out Of Range\n\n");

  auto Line = std::to_string(error.location.line);
  ErrorLexSetup(Line, "Digit does not match Integer Radix", error);

  AddHint(Line, Colors::BLUE, "= ", Colors::GREEN, "hint: ", Colors::RESET,
          "The integer value you wrote was ",
          Colors::YELLOW, "Base ", error.metadata[0], Colors::RESET,
          " which accepts digits ");

  switch (error.metadata[0]) {
    case 10:
      Console::Log("[", Colors::YELLOW, "0-9", Colors::RESET, "]");
      break;
    case 16:
      Console::Log(Colors::YELLOW, "0-9, a-f, and A-F", Colors::RESET,
                   " not ", Colors::RED, "'", source_line[error.location.column],
                   "'", Colors::RESET, '\n');
      break;
    default:
      UNREACHABLE;
  }
  AddHint(Line, Colors::BLUE, "= ", Colors::CYAN, "try: ",
          Colors::RESET, "Insert ");
  error.failed_token.PrettyPrint();

  switch (error.metadata[0]) {
    case 10:
      Console::Log(Colors::RED, "_ <-- ", Colors::RESET, "[",
                   Colors::YELLOW, "0-9", Colors::RESET, "]\n");
      break;
    case 16:
      Console::Log(Colors::RED, "_ <-- ", Colors::RESET, "[",
                   Colors::YELLOW, "0-9a-fA-F", Colors::RESET, "]\n");
      break;
    default:
      UNREACHABLE;
  }

}

void Diagnostic::ErrorHexEscapeOutOfRange(LexError &error) {
  Console::Log(Colors::RESET, "Escape Digit Value Out Of Range\n\n");

  auto Line = std::to_string(error.location.line);
  ErrorLexSetup(Line, "Digit is not Base 16", error);

  AddHint(Line, Colors::BLUE, "= ", Colors::GREEN, "hint: ",
          Colors::RESET, "Value started as Base 16"
                         " but value encountered was Base ",
          error.metadata[0], '\n');

  AddHint(Line, Colors::BLUE, "= ", Colors::CYAN, "try: ",
          Colors::RESET, "Insert ");
  error.failed_token.PrettyPrint();
  Console::Log(Colors::RED, "_ <-- ", Colors::RESET, "[",
               Colors::YELLOW, "0-9a-fA-F", Colors::RESET, "]\n\n");

  Console::Log("Hex escapes turn", Colors::BLUE," 2 ", Colors::RESET,
               "hexadecimal chars in to a single char; for "
               "example:\n", Colors::GREEN, "\\x32 ", Colors::RESET,
               "becomes the", Colors::GREEN, " <space>", Colors::RESET,
               " character. Hex escapes accept digits", Colors::YELLOW,
               " 0-9, a-f, and A-F ", Colors::RESET, "\nbut your value was "
                                                     "none of "
                                                    "these.");
}

void Diagnostic::ErrorInvalidStringEscape(LexError &error) {
  Console::Log(Colors::RESET, "Not a Valid String Escape\n\n");

  auto Line = std::to_string(error.location.line);
  ErrorLexSetup(Line, "Not a String Escape", error);

  AddHint(Line, Colors::BLUE, "= ", Colors::CYAN, "try: ",
          Colors::GREEN,
          R"(\a, \b, \v, \n, \r, \', \", \x[0-9a-fA-F]{2}, \u{[0-9a-fA-F]}{1,6})",
          "\n\n");

  Console::Log(Colors::BLUE, "String Escapes are used to escape "
               "characters that ""are otherwise hard to display. \n",
               Colors::RED, "'\\",
               source_line[error.location.column - 1], "'", Colors::BLUE,
               " is not a valid string escape. "
               "View the complete list in the language documentation: \n",
               "https://github.com/The-NextGen-Project/jet/blob/main/LANG.md",
               Colors::RESET);

}

void Diagnostic::ErrorLexSetup(std::string& line, const char *message,
                               LexError &error) {

  auto printer = Lexer<PrintingMode>((const char *) source_line,
                                file_name,
                       source_line.size());
  // Log error line
  Console::Log(Colors::RESET, line, " |\t ");
  printer.lex(); // Print Failed Token

  Console::Log("\n");

  // Align space
  FOR(i, line.length() + 1)  Console::Log(" ");

  Console::Log("\t", Colors::RED);
  FOR(column, source_line.size()) {
    if (column == error.failed_token.getSourceLocation().column) {
      FOR(ch, error.failed_token.name().size()) {
        Console::Log("^");
      }
      break;
    }
    Console::Log(" ");
  }

  Console::Log("_ <-- ", message);
  Console::Log("\n\n");
}

