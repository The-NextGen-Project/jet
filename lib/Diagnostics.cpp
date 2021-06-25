#include <nextgen/jet/Diagnostics.h>
#include <nextgen/jet/Lex/Lexer.h>
#include <nextgen/jet/Parse/Parser.h>


using namespace nextgen::jet;

void Diagnostic::Build(LexError Error) {
  SourceLine = GetNthLineOfBuffer(Error.Location.Line);
  Console::Log(Colors::RED, "Error "
                                "---------------------------------------------------------- ", FileName, ":",
                    Error.Location.Line, ":", Error.Location
                      .Column, "\n",
                    Colors::RESET);
  switch (Error.Error) {
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

void Diagnostic::Build(ParseError Error) {
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

nextgen::str Diagnostic::GetNthLineOfBuffer(size_t Nth) {
  bool Found = false;
  int Count = 1;
  const char *Copy = FileBuffer;
  const char *FoundPoint;

  if (Nth == 1) {
    return str {FileBuffer, BufferLength};
  }

  for (auto I = 0; I < BufferLength; ++I) {
    if (*Copy == '\n') {
      if (Found) {
        return str(Range<const char *>(FoundPoint, Copy));
      }
      Count++;
    }
    if (Count == Nth) {
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


void Diagnostic::ErrorMissingClosingDelim(nextgen::jet::LexError &Error) {
  Console::Log(Colors::RESET, "Missing Closing Delim\n\n");
  auto Line = std::to_string(Error.Location.Line);

  ErrorLexSetup(Line, "Reached EOF Before Closing Delim", Error);
  AddHint(Line, Colors::CYAN, "= ", Colors::GREEN, "try: ",
          Colors::RESET, "Add `", (char) Error.Metadata[0], "` to the end of "
                                                            "the statement");
}

void Diagnostic::ErrorIntegerOverflow(LexError &Error) {
  Console::Log(Colors::RESET, "Integer Value Too Large\n\n");
  auto Line = std::to_string(Error.Location.Line);

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


void Diagnostic::ErrorDigitOutOfRange(LexError &Error) {
  Console::Log(Colors::RESET, "Digit Value Out Of Range\n\n");

  auto Line = std::to_string(Error.Location.Line);
  ErrorLexSetup(Line, "Digit does not match Integer Radix", Error);

  AddHint(Line, Colors::BLUE, "= ", Colors::GREEN, "hint: ", Colors::RESET,
          "The integer value you wrote was ",
          Colors::YELLOW, "Base ", Error.Metadata[0], Colors::RESET,
          " which accepts digits ");

  switch (Error.Metadata[0]) {
    case 10:
      Console::Log("[", Colors::YELLOW, "0-9", Colors::RESET, "]");
      break;
    case 16:
      Console::Log(Colors::YELLOW, "0-9, a-f, and A-F", Colors::RESET,
                   " not ", Colors::RED, "'", SourceLine[Error.Location.Column],
                   "'",Colors::RESET, '\n');
      break;
    default:
      UNREACHABLE;
  }
  AddHint(Line, Colors::BLUE, "= ", Colors::CYAN, "try: ",
          Colors::RESET, "Insert ");
  Error.FailedToken.PrettyPrint();

  switch (Error.Metadata[0]) {
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

void Diagnostic::ErrorHexEscapeOutOfRange(LexError &Error) {
  Console::Log(Colors::RESET, "Escape Digit Value Out Of Range\n\n");

  auto Line = std::to_string(Error.Location.Line);
  ErrorLexSetup(Line, "Digit is not Base 16", Error);

  AddHint(Line, Colors::BLUE, "= ", Colors::GREEN, "hint: ",
          Colors::RESET, "Value started as Base 16"
                         " but value encountered was Base ",
          Error.Metadata[0], '\n');

  AddHint(Line, Colors::BLUE, "= ", Colors::CYAN, "try: ",
          Colors::RESET, "Insert ");
  Error.FailedToken.PrettyPrint();
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

void Diagnostic::ErrorInvalidStringEscape(LexError &Error) {
  Console::Log(Colors::RESET, "Not a Valid String Escape\n\n");

  auto Line = std::to_string(Error.Location.Line);
  ErrorLexSetup(Line, "Not a String Escape", Error);

  AddHint(Line, Colors::BLUE, "= ", Colors::CYAN, "try: ",
          Colors::GREEN,
          R"(\a, \b, \v, \n, \r, \', \", \x[0-9a-fA-F]{2}, \u{[0-9a-fA-F]}{1,6})",
          "\n\n");

  Console::Log(Colors::BLUE, "String Escapes are used to escape "
               "characters that ""are otherwise hard to display. \n",
               Colors::RED, "'\\",
               SourceLine[Error.Location.Column - 1], "'", Colors::BLUE,
               " is not a valid string escape. "
               "View the complete list in the language documentation: \n",
               "https://github.com/The-NextGen-Project/jet/blob/main/LANG.md",
               Colors::RESET);

}

void Diagnostic::ErrorLexSetup(std::string& Line, const char *Message,
                               LexError &Error) {

  auto LexPrint = Lexer(Memory, (const char *) SourceLine, FileName,
                             SourceLine.size(),
                             LexMode::PrintingMode);

  // Log Error Line
  Console::Log(Colors::RESET, Line, " |\t ");
  LexPrint.LexPrint(); // Print Failed Token

  Console::Log("\n");
  // Align space
  for (int i = 0; i < Line.length() + 1; ++i) {
    Console::Log(" ");
  }
  Console::Log("\t", Colors::RED);
  for (auto i = 0; i < SourceLine.size(); ++i) {
    if (i == Error.FailedToken.getSourceLocation().Column) {
      for (int j = 0; j < Error.FailedToken.Name().size(); ++j) {
        Console::Log("^");
      }
      break;
    }
    Console::Log(" ");
  }
  Console::Log("_ <-- ", Message);
  Console::Log("\n\n");
}

template<typename... Args>
NG_INLINE void Diagnostic::AddHint(std::string &Line, Args&& ... Hint) {

  // Align space
  for (int i = 0; i < Line.length() + 1; ++i) {
    Console::Log(" ");
  }

  Console::Log(Hint..., Colors::RESET);
}


