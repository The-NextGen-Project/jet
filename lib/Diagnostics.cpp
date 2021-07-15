#include <nextgen/jet/Diagnostics.h>
#include <nextgen/jet/Lex/Lexer.h>
#include <nextgen/jet/Parse/Parser.h>



using nextgen::Console;
using nextgen::Colors;
using nextgen::jet::Diagnostic;


template<typename... Args>
static void AddHint(std::string &line, Args&& ... hint) {

  // Align space
  FOR(i, line.length() + 1) {
    Console::Log(" ");
  }

  Console::Log(hint..., Colors::RESET);
}

void Diagnostic::build(LexError Error) {
  SourceLine = GetNthLineOfBuffer(Error.location.line);
  Console::Log(Colors::RED, "Error "
                                "---------------------------------------------------------- ", FileName, ":",
               Error.location.line, ":", Error.location
                      .column, "\n",
               Colors::RESET);
  switch (Error.error) {
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
      ErrorIntegerOverflow(Error);
      break;
    case FloatingPointOverflow:
      break;
    case DigitOutOfRange:
      ErrorDigitOutOfRange(Error);
      break;
    case MissingClosingDelim:
      ErrorMissingClosingDelim(Error);
      break;
    case InvalidDot:
      Console::Log("Invalid Dot");
      break;
    case InvalidNumberValue:
      Console::Log("Invalid Number");
      break;
    case InvalidStringEscape:
      ErrorInvalidStringEscape(Error);
      break;
    case InvalidUnicodeEscapeClose:
      break;
    case HexEscapeOutOfRange:
      ErrorHexEscapeOutOfRange(Error);
      break;
    default:
      UNREACHABLE;
  }
  Console::Log(Colors::RESET, "\n");
}


void Diagnostic::build(ParseError Error) {
  switch (Error.Error) {
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

nextgen::str Diagnostic::GetNthLineOfBuffer(size_t nth) {
  bool Found = false;
  int Count = 1;
  const char *Copy = FileBuffer;
  const char *FoundPoint;

  if (nth == 1) {
    return str {FileBuffer, BufferLength};
  }

  for (auto I = 0; I < BufferLength; ++I) {
    if (*Copy == '\n') {
      if (Found) {
        return str(Range<const char *>(FoundPoint, Copy));
      }
      Count++;
    }
    if (Count == nth) {
      if (!Found) {
        FoundPoint = Copy + 1;
        Found = true;
      }
    }
    Copy++;
  }

  return str {FoundPoint, BufferLength};


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

void Diagnostic::ErrorIntegerOverflow(LexError &Error) {
  Console::Log(Colors::RESET, "Integer Value Too Large\n\n");
  auto Line = std::to_string(Error.location.line);

  ErrorLexSetup(Line, "Overflow Occurs Here", Error);

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

void Diagnostic::ErrorInvalidChar(LexError &Error) {

}

void Diagnostic::ErrorMalformedUTF8(LexError &Error) {

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
                   " not ", Colors::RED, "'", SourceLine[error.location.column],
                   "'",Colors::RESET, '\n');
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
               SourceLine[error.location.column - 1], "'", Colors::BLUE,
               " is not a valid string escape. "
               "View the complete list in the language documentation: \n",
               "https://github.com/The-NextGen-Project/jet/blob/main/LANG.md",
               Colors::RESET);

}

void Diagnostic::ErrorLexSetup(std::string& line, const char *message,
                               LexError &error) {

  auto printer = Lexer(Memory, (const char *) SourceLine, FileName,
                       SourceLine.size(),
                       LexMode::PrintingMode);

  // Log error line
  Console::Log(Colors::RESET, line, " |\t ");
  printer.LexPrint(); // Print Failed Token

  Console::Log("\n");

  // Align space
  FOR(i, line.length() + 1)  Console::Log(" ");

  Console::Log("\t", Colors::RED);
  FOR(column, SourceLine.size()) {
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


