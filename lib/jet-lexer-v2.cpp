#include <nextgen/jet/jet-lexer.h>

using namespace nextgen;
using namespace nextgen::jet;

#define TOKEN_SHORT(value, kind) \
new Token { \
  value, \
  {line, column}, \
  value, \
  kind \
}
#define TOKEN_ADD_SHORT(value, kind) \
if (OUTPUT_MODE) Console::Log(value), next((sizeof value) - 1); \
else (tokens.end = TOKEN_SHORT(value, kind), next((sizeof value) - 1)) \
\

static const constexpr TokenKind MatchTokenKind[256]{

		// Error code
		TokenKind::EOFToken, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error,
		TokenKind::Whitespace, // \a
		TokenKind::Whitespace, // \b
		TokenKind::Whitespace, // \t
		TokenKind::NewLine,    // \n
		TokenKind::Whitespace, // \v
		TokenKind::Error,

		TokenKind::NewLine, // \r

		TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error,


		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,

		TokenKind::Whitespace,    // <space>

		TokenKind::ExclamationPoint, // !
		TokenKind::String,        // '"'
		TokenKind::Error, TokenKind::Error,
		TokenKind::Percent,       // '%'
		TokenKind::AND,           // '&'
		TokenKind::Char,          // '\''
		TokenKind::LParenthesis,  // (
		TokenKind::RParenthesis,  // )
		TokenKind::Star,          // '*'
		TokenKind::Plus,          // '+'
		TokenKind::Comma,         // ','

		TokenKind::Minus, // '-'


		TokenKind::Dot, // '.'

		TokenKind::Slash, // '/'


		// Digits
		TokenKind::Zero, TokenKind::Integer, TokenKind::Integer,
		TokenKind::Integer, TokenKind::Integer,
		TokenKind::Integer, TokenKind::Integer, TokenKind::Integer,
		TokenKind::Integer, TokenKind::Integer,


		TokenKind::Colon, // ':'


		TokenKind::SemiColon, // ';'
		TokenKind::LessThan, // '<'
		TokenKind::Equals, // '='
		TokenKind::GreaterThan, // '>'
		TokenKind::QuestionMark, // '?'
		TokenKind::At, // @

		// Uppercase letters
		TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
		TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
		TokenKind::Identifier,
		TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
		TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
		TokenKind::Identifier,
		TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
		TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
		TokenKind::Identifier,
		TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
		TokenKind::Identifier, TokenKind::Identifier,


		TokenKind::LBracket,  // '['

		TokenKind::StringEscapeStart, // \\

		TokenKind::RBracket,  // ']'

		TokenKind::XOR,       // '^'

		TokenKind::Identifier,// '_'

		TokenKind::Error,

		// Lowercase letters
		TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
		TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
		TokenKind::Identifier,
		TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
		TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
		TokenKind::Identifier,
		TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
		TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
		TokenKind::Identifier,
		TokenKind::Identifier, TokenKind::Identifier, TokenKind::Identifier,
		TokenKind::Identifier, TokenKind::Identifier,


		TokenKind::LCurlyBrace, // '{'
		TokenKind::Pipe, // '|'
		TokenKind::RCurlyBrace, // '}'
		TokenKind::NOT, // '~'

		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error, TokenKind::Error,
		TokenKind::Error, TokenKind::Error,
};

constexpr static int IntegerBits[256] = {

		// Error code
		-3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
		-3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
		-3, -3,

		// Digits
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9,

		// Error Code
		-3, -3, -3, -3, -3, -3, -3,

		// Uppercase values
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,

		// Error code
		-3, -3, -3, -3, -3, -3,

		// Lowercase values
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,

		// Error code
		-3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
		-3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
		-3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
		-3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
		-3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
		-3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
};

struct ReservedIdent {
	const char *id;
	TokenKind type;
};

static const struct ReservedIdent Reserved[]{
		{"if",       KeywordIf},
		{"else",     KeywordElse},
		{"elif",     KeywordElif},
		{"while",    KeywordWhile},
		{"for",      KeywordFor},
		{"true",     KeywordTrue},
		{"false",    KeywordFalse},
		{"None",     KeywordNone},
		{"fn",       KeywordFunction},
		{"struct",   KeywordStruct},
		{"export",   KeywordExport},
		{"enum",     KeywordEnum},
		{"extern",   KeywordExtern},
		{"error",    KeywordError},
		{"var",      KeywordVar},
		{"return",   KeywordReturn},
		{"and",      KeywordAnd},
		{"or",       KeywordOr},
		{"break",    KeywordBreak},
		{"continue", KeywordContinue},
		{"defer",    KeywordDefer},
		{"union",    KeywordUnion},
		{"match",    KeywordMatch},
		{"in",       KeywordIn},
		{"str",      Typename_str},
		{"i8",       Typename_i8},
		{"i16",      Typename_i16},
		{"i32",      Typename_i32},
		{"i64",      Typename_i64},
		{"u8",       Typename_u8},
		{"u16",      Typename_u16},
		{"u32",      Typename_u32},
		{"u64",      Typename_u64},
		{"box",      Typename_box},
		{"f32",      Typename_f32},
		{"f64",      Typename_f64},
};

static const TokenKind MatchIdentToReserved(const char *ident, size_t len) {
	FOR(i, SizeOfArray(Reserved)) {
		if (::strncmp(Reserved[i].id, ident, len) == 0)
			return Reserved[i].type;
	}
	return Identifier;
}


template<LexMode Mode>
void Lexer<Mode>::lex_float(int skip, int start) {
	// Initialize start of token
	auto token_start = buffer - skip;
	auto token_start_col = column;

	next(skip - 1);

	double decimal = 0;
	double divisor = 1;
	double value = 0;

	do {
		auto digit = IntegerBits[curr()];
		if (digit < 0) break;
		divisor *= 10;
		value = value * 10 + digit;
	} while (next(1));

	decimal = (value / divisor);

	auto token_end = buffer;

	auto literal_token_representation
			= Range<const char *>(token_start, token_end);

	if (OUTPUT_MODE) {
		Console::Log(Colors::BLUE, str(literal_token_representation), Colors::RESET);
	}

	tokens.end = new Token{
			literal_token_representation,
			{line, column},
			(double) (start + decimal),
			TokenKind::Decimal,
			TokenClassification::Literal
	};
}

template<LexMode Mode>
void Lexer<Mode>::lex_int() {
	switch (curr()) {
		case '0':
			switch (peek(1)) {
				case 'b':
					lex_int < 2, 2 > ();
					return;
				case 'x':
					lex_int < 16, 2 > ();
					return;
				case '#':
					lex_int < 36, 2 > ();
					return;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					lex_int < 8, 1 > ();
					return;
				case '.':
					lex_float(2);
					return;
				default:
					lex_int < 10, 0 > ();
					return;
			}
		default:
			lex_int < 10, 0 > ();
	}
}

template<LexMode Mode>
template<int radix, int skip>
void Lexer<Mode>::lex_int() {


	// Initialize start of token
	const char *token_start = buffer;
	size_t token_start_col = column;

	// Skip any tokens, if any (for 0x, 0b, 00 cases)
	next(skip);

	// Note: Keep the type!
	decltype(UINTPTR_MAX) lexed_int = 0;
	do {
		auto digit = IntegerBits[curr()];
		if (digit < 0) break;

		// Invalid digit in number. For example, in base 10,
		// we accept 1234 as a valid number but not 123f as 'f' is out of
		// range for the accepted base.
		if (digit >= radix) {
			if (NORMAL_MODE) {
				diagnostics.build(LexError{
						LexErrorType::DigitOutOfRange,
						{line, column},
						Token{
								Range<const char *>(token_start, buffer),
								{line, token_start_col},
								TokenKind::Integer
						},
						{radix, digit + 1}
				});
			}
		}
		// Check whether value is about to overflow the maximum integer value.
		// Ensures that all integers don't exceed 64-bit max.
		if (lexed_int > (UINTPTR_MAX - digit) / radix) {
			if (NORMAL_MODE) {
				diagnostics.build(LexError{
						LexErrorType::IntegerOverflow,
						{line, column},
						Token{
								Range<const char *>(token_start, buffer),
								{line, token_start_col},
								TokenKind::Integer
						}
				});
			}
		}
		lexed_int = lexed_int * radix + digit;
	} while (next(1));


	if (curr() == '.') { // At this point, we've realized its f64, so we proceed
		lex_float(buffer - token_start, lexed_int);
		return;
	}

	const char *token_end = buffer;
	auto literal_token_representation
			= Range<const char *>(token_start, token_end);

	if (OUTPUT_MODE) {
		Console::Log(Colors::BLUE, str(literal_token_representation), Colors::RESET);
	}


	tokens.end = new Token{
			literal_token_representation,
			{line, column},
			(decltype(UINTPTR_MAX)) lexed_int,
			TokenKind::Integer,
			TokenClassification::Literal
	};
}

template<LexMode Mode>
void Lexer<Mode>::lex_ident() {
	const char *token_start = buffer;
	auto token_start_col = column;
	auto val = FNV_OFF;
	do {
		val ^= (curr());
		val *= FNV_PRIME;
	} while (MatchTokenKind[next(1)] == TokenKind::Identifier);
	const char *token_end = buffer;

	auto literal_token_representation = str(Range<const char *>(token_start,
	                                                            token_end));
	literal_token_representation.setHash(val);
	TokenKind reserved =
			MatchIdentToReserved(token_start, token_end - token_start);

	if (OUTPUT_MODE) {
		if (reserved != TokenKind::Identifier) {
			Console::Log(Colors::RED, literal_token_representation,
			             Colors::RESET);
		} else {
			Console::Log(Colors::YELLOW, literal_token_representation, Colors::RESET);
		}
	}

	tokens.end = new Token{
			literal_token_representation,
			{line, token_start_col},
			"",
			reserved,
			static_cast<unsigned int>(reserved == Identifier ? Keyword : -1)
	};
}

template<LexMode Mode>
void Lexer<Mode>::lex_str() {
	auto token_start = buffer;
	auto token_start_col = column;
	auto offset = 1;

	if (OUTPUT_MODE) Console::Log(Colors::GREEN);

	// @NOTE: Remember that this is DATA not OBJECT for allocation
	auto lexed_string = (char *) (GLOBAL_DATA_ALLOC.current() + 1);
	size_t len = 0;

	char current_char = next(1);
	if (OUTPUT_MODE) Console::Log("\"");
	do {
		current_char = curr();
		if (OUTPUT_MODE) {
			Console::Log(current_char);
			if (current_char == '\0' || peek(1) == '\0') {
				offset = 0;
				break;
			}
		} else {
			if (current_char == '\0') {
				diagnostics.build(LexError{
						LexErrorType::MissingClosingDelim,
						{line, column},
						Token{
								Range<const char *>(token_start, buffer),
								{line, token_start_col},
								TokenKind::String
						},
						{(int) ('"'
						)}
				});
			}

			if (current_char == '\\') {
				switch (next(1)) {
					case 'a':
						current_char = '\a';
						break;
					case 'b':
						current_char = '\b';
						break;
					case 'r':
						current_char = '\r';
						break;
					case 'n':
						current_char = '\n';
						break;
					case 'v':
						current_char = '\v';
						break;
					case 't':
						current_char = '\t';
						break;
					case 'x': {
						auto value = IntegerBits[next(1)];
						if (value >= 16) {
							diagnostics.build(LexError{
									LexErrorType::HexEscapeOutOfRange,
									{line, column},
									Token{
											Range<const char *>(token_start, buffer),
											{line, token_start_col},
											TokenKind::String
									},
									{value + 1} // Pass in the invalid digit for the escape
							});
						}
						auto value2 = IntegerBits[next(1)];
						if (value2 >= 16) {
							diagnostics.build(LexError{
									LexErrorType::HexEscapeOutOfRange,
									{line, column},
									Token{
											Range<const char *>(token_start, buffer),
											{line, token_start_col},
											TokenKind::String
									},
									{value2 + 1} // Pass in the invalid digit for the escape
							});
						}
						int val = value;
						val = val * 16 + value2; // @Opt: (val << 4) | value2 ?

						current_char = (char) val;
						break;
					}
					default:
						diagnostics.build(LexError{
								LexErrorType::InvalidStringEscape,
								{line, column},
								Token{
										Range<const char *>(token_start, buffer),
										{line, token_start_col},
										TokenKind::String
								}
						});
				}
			}
			lexed_string[len++] = current_char;
		}
	} while (next(1) != '"');

	next(1); // Skip '"'

	// For missing closing expect_delim this needs to be here
	if (OUTPUT_MODE && offset != 0) Console::Log("\"");
	if (NORMAL_MODE) {
		lexed_string[len] = '\0';

		// We wrote to the data buffer without telling the size of the string
		// because we did not know what it was, so we tell it the size of the
		// string after lexing it. `reserve(size_t)` in this case is used to
		// increment the number of bytes written.
		GLOBAL_DATA_ALLOC.reserve(len);
		tokens.end = new Token{
				str{lexed_string, len},
				{line, column},
				"",
				TokenKind::String,
				TokenClassification::Literal
		};
	}
}

template<LexMode Mode>
ArenaVec<Token> Lexer<Mode>::lex() {
	tokens.begin = (Token *) GLOBAL_OBJECT_ALLOC.current();

	TokenKind kind;
	while (true) {
		if (fatal) break; // Special case: Diagnostics may crash

		auto current_char = curr();
		kind = MatchTokenKind[current_char];
		if (OUTPUT_MODE) Console::Log(Colors::RESET);

		switch (kind) {
			case EOFToken: {
				tokens.end = new Token("a", {line, column}, TokenKind::EOFToken);
				return tokens;
			}
			case Integer:
				lex_int();
				break;
			case String:
				lex_str();
				break;
			case Identifier:
				lex_ident();
				break;
			case NewLine:
				skip_new_line();
				break;
			case Whitespace: {
				if (OUTPUT_MODE)
					Console::Log(current_char);
				next(1);
				break;
			}
			case Char: {
				auto token_start = buffer;
				auto token_start_col = column;
				if (OUTPUT_MODE) {
					Console::Log(Colors::GREEN, "'");
					Console::Log(next(1), Colors::RESET);
				} else {
					TOKEN_SHORT(next(1), TokenKind::Char);
				}
				if (curr() != '\'') {
					if (NORMAL_MODE) {
						diagnostics.build(LexError{
								LexErrorType::MissingClosingDelim,
								{line, column}, // 'x[?] <-- Missing Closing!
								Token{
										Range<const char *>(token_start, buffer),
										{line, token_start_col},
										TokenKind::String
								},
								{(char) '\''}
						});
					}
					next(1); // Skip "'"
					break;
				} else if (OUTPUT_MODE) Console::Log("'", Colors::RESET);
				next(1); // Skip the "'"
				break;
			}
			case LessThan: { // <, <=, <<, <<=
				auto n = peek(1);
				if (n == '<') {
					if (peek(2) == '=') {
						TOKEN_ADD_SHORT("<<=", TokenKind::LeftShiftEquals);
						break;
					}
					TOKEN_ADD_SHORT("<<", TokenKind::LeftShift);
					break;
				} else if (n == '=') {
					TOKEN_ADD_SHORT("<=", TokenKind::LessThanEquals);
					break;
				}
				TOKEN_ADD_SHORT("<", TokenKind::LessThan);
				break;
			}
			case GreaterThan: {
				auto n = peek(1);
				if (n == '>') {
					if (peek(2) == '=') {
						TOKEN_ADD_SHORT(">>=", TokenKind::RightShiftEquals);
						break;
					}
					TOKEN_ADD_SHORT(">>", TokenKind::RightShift);
					break;
				} else if (n == '=') {
					TOKEN_ADD_SHORT(">=", TokenKind::GreaterThanEquals);
					break;
				}
				TOKEN_ADD_SHORT(">", TokenKind::GreaterThan);
				break;
			}
			case Star: {
				auto n = peek(1);
				if (n == '*') {
					if (peek(2) == '=') {
						TOKEN_ADD_SHORT("**=", TokenKind::PowEquals);
						break;
					}
					TOKEN_ADD_SHORT("**", TokenKind::Pow);
					break;
				} else if (n == '=') {
					TOKEN_ADD_SHORT("*=", TokenKind::MulEquals);
					break;
				}
				TOKEN_ADD_SHORT("*", TokenKind::Star);
				break;
			}
			case Dot: {
				auto n = peek(1);
				if (n == '.') {
					if (peek(2) == '.') {
						TOKEN_ADD_SHORT("...", TokenKind::Ellipsis);
						break;
					}
					TOKEN_ADD_SHORT("..", TokenKind::RangeSpan);
					break;
				}
				TOKEN_ADD_SHORT(".", TokenKind::Dot);
				break;
			}
			case Plus: {
				auto n = peek(1);
				if (n == '+') { TOKEN_ADD_SHORT("++", TokenKind::PlusPlus); }
				else if (n == '=') { TOKEN_ADD_SHORT("+=", TokenKind::PlusEquals); }
				else {
					TOKEN_ADD_SHORT("+", TokenKind::Plus);
				}
				break;
			}
			case Minus: {
				auto n = peek(1);
				if (n == '-') {
					TOKEN_ADD_SHORT("--", TokenKind::MinusMinus);
				} else if (n == '=') {
					TOKEN_ADD_SHORT("-=", TokenKind::MinusMinus);
				} else if (n == '>') {
					TOKEN_ADD_SHORT("->", TokenKind::Arrow);
				} else {
					TOKEN_ADD_SHORT("+", TokenKind::Minus);
				}
				break;
			}
			case Slash: {
				auto n = peek(1);
				if (n == '=') {
					TOKEN_ADD_SHORT("/=", TokenKind::DivEquals);
				} else if (n == '/') {
					if (OUTPUT_MODE) Console::Log("/");
					while (MatchTokenKind[next(1)] != NewLine) {
						if (OUTPUT_MODE) {
							Console::Log(curr());
						}
					}
					skip_new_line();
				} else {
					TOKEN_ADD_SHORT("/", TokenKind::Slash);
				}
				break;
			}
			case Equals: {
				auto n = peek(1);
				if (n == '=') {
					TOKEN_ADD_SHORT("==", TokenKind::EqualsEquals);
				} else if (n == '>') {
					TOKEN_ADD_SHORT("=>", TokenKind::FunctionArrow);
				} else {
					TOKEN_ADD_SHORT("=", TokenKind::Equals);
				}
				break;
			}
			case Percent: {
				auto n = peek(1);
				if (n == '=') {
					TOKEN_ADD_SHORT("%=", TokenKind::PercentEquals);
				} else {
					TOKEN_ADD_SHORT("%", TokenKind::Percent);
				}
				break;
			}
			case AND: {
				auto n = peek(1);
				if (n == '=') {
					TOKEN_ADD_SHORT("&=", TokenKind::AND);
				} else {
					TOKEN_ADD_SHORT("&", TokenKind::ANDEquals);
				}
				break;
			}
			case Pipe: {
				auto n = peek(1);
				if (n == '=') {
					TOKEN_ADD_SHORT("|=", TokenKind::OREquals);
				} else {
					TOKEN_ADD_SHORT("|", TokenKind::Pipe);
				}
				break;
			}
			case XOR: {
				auto n = peek(1);
				if (n == '=') {
					TOKEN_ADD_SHORT("^=", TokenKind::XOREquals);
				} else {
					TOKEN_ADD_SHORT("^", TokenKind::XOR);
				}
				break;
			}
			case LParenthesis:
				TOKEN_ADD_SHORT("(", TokenKind::LParenthesis);
				break;
			case RParenthesis:
				TOKEN_ADD_SHORT(")", TokenKind::RParenthesis);
				break;
			case LCurlyBrace:
				TOKEN_ADD_SHORT("{", TokenKind::LCurlyBrace);
				break;
			case RCurlyBrace:
				TOKEN_ADD_SHORT("}", TokenKind::RCurlyBrace);
				break;
			case LBracket:
				TOKEN_ADD_SHORT("[", TokenKind::LBracket);
				break;
			case RBracket:
				TOKEN_ADD_SHORT("]", TokenKind::RBracket);
				break;
			case SemiColon:
				TOKEN_ADD_SHORT(";", TokenKind::SemiColon);
				break;
			case At:
				TOKEN_ADD_SHORT("@", TokenKind::At);
				break;
			case NOT:
				TOKEN_ADD_SHORT("~", TokenKind::NOT);
				break;
			case ExclamationPoint:
				TOKEN_ADD_SHORT("!", TokenKind::ExclamationPoint);
				break;
			case QuestionMark:
				TOKEN_ADD_SHORT("?", TokenKind::QuestionMark);
				break;
			case Colon:
				if (peek(1) == '=')
					TOKEN_ADD_SHORT(":=", TokenKind::ColonEquals);
				else TOKEN_ADD_SHORT(":", TokenKind::Colon);
				break;
			case Comma:
				TOKEN_ADD_SHORT(",", TokenKind::Comma);
				break;
			default:
				break;
		}
	}
	return tokens;
}

// Forces compilation of the two lexer classes
// so that we can have definitions in a cpp file without
// crowding the header file.
namespace nextgen {
	namespace jet {
		template
		class Lexer<TokenMode>;

		template
		class Lexer<PrintingMode>;
	}
}