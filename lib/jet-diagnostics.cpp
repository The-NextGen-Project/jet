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

static Range<const char *>
  FileLines(size_t nth, const char *file_buf, size_t buf_len) {
  int max_num_after_found_point = 3;

  bool found = false;
  int  count = 1;

  auto copy = file_buf;
  const char *found_point;
  const char *last_new_line;

  if (nth == 1) {
    return Range<const char *>(file_buf, file_buf + buf_len);
  }


  FOR(ch, buf_len) {
    if (*copy == '\n' || *copy == '\r') {
      if (found) {
        return Range<const char *>(file_buf, copy);
      }
      count++;
    }
    if (count == nth) {
      if (!found) {
        found = true;
      }
    }
    copy++;
  }
}

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

  FOR(i, buf_len) {
    if (*copy == '\n' || *copy == '\r' || *copy == '\0') {
      if (found) {
        return str(Range<const char *>(found_point, copy));
      }
      count++;
    }
    if (count == nth) {
      if (!found) {
        found_point = copy+1;
        found = true;
      }
    }
    copy++;
  }

  return str {found_point, static_cast<size_t>(copy - found_point)};
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

  // Lexer errors are always fatal to a program's continuation.
  send_exception();
}


void Diagnostic::build(ParseError error) {
  source_line = GetNthLineOfBuffer(error.location.line, file_buf, buf_len);
  Console::Log(Colors::RED,
               "Error ---------------------------------------------------------- ",
               file_name, ":", error.location.line, ":", error.location.column, "\n",
               Colors::RESET);

  switch (error.error) {
    case ReservedIdentifierAsVariableName:
      break;
    case UnexpectedExpression:
      Console::Log("Unexpected Expression");
      break;
    case InvalidToken:
      Console::Log("Invalid Token");
      break;
    case ExpectedToken:
      ErrorParseExpectedToken(error);
      break;
    case MissingForLoopVariable:
      break;
    case ExpectedIdentifierForFunctionParameter:
      break;
    case ExpectedIdentifierForStructProperty:
      break;
    case MissingClosingPair:
      ErrorParseMissingClosingDelim(error);
      break;
    case UnexpectedEndOfFile:
      Console::Log("EOF");
      break;
    case MissingFunctionName:
      break;
    case MissingVariableName:
      break;
    case InvalidTokenAfterIdentInGlobalScope:
      ErrorParseInvalidTokenAfterIdentInGlobalScope(error);
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

  auto line = std::to_string(error.location.line);
  ErrorLexSetup(line, "Not a String Escape", error);

  AddHint(line, Colors::BLUE, "= ", Colors::CYAN, "try: ",
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

void Diagnostic::ErrorParseInvalidTokenAfterIdentInGlobalScope(const ParseError &error) {
  Console::Log(Colors::RESET, "Invalid Token\n\n");

  auto err = error.metadata;
  auto invalid_token = err.begin()->misc_tok;

  auto loc  = invalid_token->getSourceLocation();
  auto line = std::to_string(loc.line);
  ErrorParseSetup<true>(loc.line, "Unexpected Token After Identifier",
                  invalid_token, error.location);
  AddHint(line, Colors::BLUE, "= ",
          Colors::CYAN, "try: ",
          Colors::RESET, "Adding ", Colors::YELLOW, "=>", Colors::BLUE, " or "
                                                                        "",
                                                                        Colors::YELLOW,
          ":="
          "\n", Colors::RESET);

  AddHint(line, Colors::BLUE, "= ", Colors::GREEN, "help: ",
          Colors::RESET, "Expected one of ", Colors::BLUE, "':=', ",
          "'=>', ", Colors::RESET,
          "in global scope declaration.");
  Console::Log("\n");
  FOR(i, line.length() + 9)  Console::Log(" ");
  Console::Log("Identifiers in the global scope only represent two values: "
               "\n", Colors::GREEN);
  FOR(i, line.length() + 9)  Console::Log(" ");
  Console::Log("Global Variable "
                             "Declaration ",
                             Colors::RESET,
                             "and ",
                             Colors::BLUE,
                             "Function "
                             "Declaration",
                             Colors::RESET,
                             " but your token was "
                             "", Colors::RED,
                             "none of these.\n");
}

void Diagnostic::ErrorParseMissingClosingDelim(ParseError const &error) {
  Console::Log(Colors::RESET, "Missing Closing Delim\n\n");

  auto err = error.metadata;
  auto expected_kind = Token::GetTokenKindName(err.begin()->misc_kind);
  auto token_got_instead = (err.begin()+1)->misc_tok;
  auto token_loc         = (err.begin()+2)->location;

  ErrorParseSetup(error.location.line, (std::string("Missing Closing Delim '") +
    std::string(expected_kind) + std::string("'")).c_str(), token_got_instead,
    error.location);

  std::string line = std::to_string(error.location.line);
  AddHint(line, Colors::BLUE, "= ",
                         Colors::CYAN, "try: ",
          Colors::RESET, "Adding ", Colors::YELLOW, "'", expected_kind,
          "'\n", Colors::RESET);
  AddHint(line, Colors::BLUE, "= ", Colors::GREEN, "help: ",
          Colors::RESET, "Delimiters like ", Colors::BLUE, "'(', ",
          "'[', ", "'{', ", Colors::RESET,
          "must be met with their closing pairs.\n");
}

void Diagnostic::ErrorParseExpectedToken(ParseError const &error) {
  Console::Log(Colors::RESET, "Expected Token\n\n");

  auto err = error.metadata.begin()->expected_error;
  auto line = std::to_string(error.location.line);
  ErrorParseSetup(error.location.line, err.message, err.got, error.location);
  AddHint(line, Colors::BLUE, "= ", Colors::CYAN, "try: ",
          Colors::GREEN, "Adding ", Colors::YELLOW, "'",
          Token::GetTokenKindName(err.expected), "'\n");
  AddHint(line, Colors::BLUE, "= ", Colors::GREEN, "help: ",
          Colors::RESET, "View docs here: "
                         "https://github"
                         ".com/The-NextGen-Project/jet/blob/main/LANG.md for "
                         "syntax.\n");
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

template<bool POINT>
void Diagnostic::ErrorParseSetup(size_t const ln,
                                 const char *message,
                                 Token const *reported_token,
                                 TokenTraits::SourceLocation loc) {
  std::string line = std::to_string(ln);
  if (ln > 1) {
    Console::Log(Colors::RESET, ln - 1, " |\t ");

    str new_line;

    if (ln - 1 == 1) {
      new_line = str(file_buf, source_line.begin() - file_buf - 1);
    }
    else {
      new_line = GetNthLineOfBuffer(ln - 1, file_buf, buf_len);
    }

    Lexer<PrintingMode>(new_line.begin(),
                        file_name,
                        new_line.size()).lex();
    Console::Log("\n");
  }

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

  auto size = reported_token->getKind() == EOFToken ? source_line.size()+2 :
    source_line.size();

  auto nth_column = POINT ? loc.column-1 : loc.column;
  FOR(column, source_line.size()) {
    if (column == nth_column) {
      FOR(ch, reported_token->name().size()) {
          Console::Log("^");
      }
      break;
    }
    Console::Log(" ");
  }

  if (!POINT)
    Console::Log("_ <-- ", message);
  else Console::Log(" <-- ", message);
  Console::Log("\n\n");
}


