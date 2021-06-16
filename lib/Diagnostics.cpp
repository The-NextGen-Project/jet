#include <nextgen/jet/Diagnostics.h>
#include <nextgen/jet/Lex/Lexer.h>


using namespace nextgen::jet;

nextgen::str Diagnostic::GetNthLineOfBuffer(size_t Nth) {
  bool Found = false;
  int Count = 1;
  const char *Copy = FileBuffer;
  const char *FoundPoint;


  for (;;) {
    if (*Copy == '\n') {
      if (Found) {
        return str(Range<const char *>(FoundPoint, FileBuffer));
      }
      Count++;
    }

    if (Count == Nth) {
      if (Found) // Prevent Infinite Loop where buffer input is only 1 line long
        return str {FileBuffer, BufferLength};
      Found = true;
      FoundPoint = Copy;
    }

    Copy++;
  }
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
               Colors::RESET);
}

void Diagnostic::ErrorInvalidChar(LexError &Error) {

}

void Diagnostic::ErrorMalformedUTF8(LexError &Error) {

}


void Diagnostic::ErrorDigitOutOfRange(LexError &Error) {
  Console::Log(Colors::RESET, "Digit Value Out Of Range\n\n");

  auto Line = std::to_string(Error.Location.Line);
  ErrorLexSetup(Line, "Digit does not match Integer Radix", Error);

  constexpr char ValidDigits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

  AddHint(Line, Colors::BLUE, "= ", Colors::GREEN, "hint: ",
          Colors::RESET, "Value started as Base ", Error.Metadata[0],
          " but value encountered was Base ",
          Error.Metadata[1], Colors::RESET, '\n');
  AddHint(Line, Colors::BLUE, "= ", Colors::CYAN, "try: ",
          Colors::RESET);
  Error.FailedToken.PrettyPrint();
  Console::Log(Colors::GREEN, ValidDigits[0], Colors::RESET, ", ");
  for (auto i = 1; i < Error.Metadata[0]; ++i) {
    if (i % 4 == 0) {
      Console::Log("\n");
      // Align space
      for (auto j = 0; j < Line.length() + 1; ++j) {
        Console::Log(" ");
      }
      Console::Log("       ");
    }
    Error.FailedToken.PrettyPrint();
    Console::Log(Colors::GREEN, ValidDigits[i], Colors::RESET);

    if (i != Error.Metadata[0] - 1)
      Console::Log(", ");

  }
  Console::Log('\n');
  AddHint(Line, Colors::BLUE, "= ", Colors::YELLOW, "note: ",
          Colors::RESET, "When using letters in numbers, casing does not "
                         "matter");

/*  Console::Log(Colors::RED, "_ <-- ", Colors::RESET, "[", Colors::GREEN, "'a'.."
                                                                     ".'f' "
                                                                  "or "
                                                              "'A'.."
                                                        ".'F'",
               Colors::RESET, "]");*/
}

void Diagnostic::ErrorHexEscapeOutOfRange(LexError &Error) {
  Console::Log(Colors::RESET, "Escape Digit Value Out Of Range\n\n");

  auto Line = std::to_string(Error.Location.Line);
  ErrorLexSetup(Line, "Digit is not Base 16", Error);

  constexpr char ValidDigits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

  AddHint(Line, Colors::BLUE, "= ", Colors::GREEN, "hint: ",
          Colors::RESET, "Value started as Base 16"
                         " but value encountered was Base ",
          Error.Metadata[0], '\n');

  AddHint(Line, Colors::BLUE, "= ", Colors::CYAN, "try: ",
          Colors::RESET);
  Error.FailedToken.PrettyPrint();
  Console::Log(Colors::BLUE, ValidDigits[0], Colors::GREEN, '"',
               Colors::RESET, ", ");

  for (auto i = 1; i < 16; ++i) {
    if (i % 4 == 0) {
      Console::Log("\n");
      // Align space
      for (auto j = 0; j < Line.length() + 1; ++j) {
        Console::Log(" ");
      }
      Console::Log("       ");
    }
    Error.FailedToken.PrettyPrint();
    Console::Log(Colors::BLUE, ValidDigits[i], Colors::GREEN, '"',
                 Colors::RESET);

    if (i != Error.Metadata[0] - 1)
      Console::Log(", ");
  }

  Console::Log('\n');
  AddHint(Line, Colors::BLUE, "= ", Colors::YELLOW, "note: ",
          Colors::RESET, "When using letters in numbers, casing does not "
                         "matter");
}

void Diagnostic::ErrorInvalidStringEscape(LexError &Error) {
  Console::Log(Colors::RESET, "Not a Valid String Escape\n\n");

  auto Line = std::to_string(Error.Location.Line);
  ErrorLexSetup(Line, "Not a String Escape", Error);

  AddHint(Line, Colors::BLUE, "= ", Colors::CYAN, "try: ",
          Colors::GREEN, R"(\a, \b, \v, \n, \r, \x[0-9a-fA-F]{2}, \u{[0-9a-fA-F]})");
}

void Diagnostic::ErrorLexSetup(std::string& Line, const char *Message,
                               LexError &Error) {

  auto SourceLine = GetNthLineOfBuffer(Error.Location.Line);
  auto LexPrint = Lexer::New(Memory, (const char *) SourceLine, FileName,
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

