#include <nextgen/jet/Diagnostics.h>

using namespace nextgen::jet;

void Diagnostic::ErrorMissingClosingDelim(
  nextgen::jet::LexError &Error) {

}

void Diagnostic::ErrorIntegerOverflow(LexError &Error) {
  Console::Log(Colors::RED, "Error "
                            "---------------------------------------------------------- ", FileName, "\n");
  Console::Log(Colors::RESET, "Integer Value Too Large\n\n");
  auto SourceLine = GetNthLineOfBuffer(Error.Location.Line);
  auto LexPrint = Lexer::New(Memory, (const char *) SourceLine, SourceLine
  .size(),
                             LexMode::PrintingMode);

  // We get the string version of the line to align hints and notes to error
  // messages.
  auto Line = std::to_string(Error.Location.Line);

  // Log Error Line
  Console::Log(Colors::WHITE, Line, " |\t ");
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
  Console::Log("_ <-- Overflow Occurs Here");
  Console::Log("\n\n");

  // Align space
  for (int i = 0; i < Line.length() + 1; ++i) {
    Console::Log(" ");
  }
  Console::Log(Colors::BLUE, "= ", Colors::GREEN, "hint: ",
               Colors::RESET, "Integer values must be less than ", UINTPTR_MAX,
                              ".\n",
               Colors::RESET);
}

void Diagnostic::ErrorInvalidChar(LexError &Error) {

}

void Diagnostic::ErrorMalformedUTF8(LexError &Error) {

}

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
        return str(FileBuffer);
      Found = true;
      FoundPoint = Copy;
    }

    Copy++;
  }
}


