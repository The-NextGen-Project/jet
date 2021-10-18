# ifndef NEXTGEN_LEXER_H
# define NEXTGEN_LEXER_H

# include "jet-token.h"
# include "jet-diagnostics.h"

namespace nextgen {
	namespace jet {
		using namespace nextgen::core;

		enum LexErrorType {
			// Other
			UnexpectedEOF,
			OutOfRange,
			Unreachable,

			// Strings
			MalformedUTF8,
			InvalidChar,
			InvalidStringEscape,
			InvalidUnicodeEscapeClose,
			HexEscapeOutOfRange,
			MissingClosingDelim,

			// Numbers
			DigitOutOfRange,
			InvalidDot,
			InvalidNumberValue,
			IntegerOverflow,
			FloatingPointOverflow,
		};

		struct LexError {
			// The type of error
			LexErrorType error;

			// Error location
			TokenTraits::SourceLocation location;

			// Token value lexed until error
			Token failed_token;

			// Error metadata
			int metadata[5];
		};

		// Lexical analysis has two different modes for two purposes. Depending on
		// the case, the lexer will either register tokens or it will simply, print
		// tokens to the terminal.
		//
		// TokenMode: Specifies the mode in which tokens are lexed and output is
		// returned.
		template<LexMode Mode = TokenMode>
		class Lexer {
			using File = char;

			const File *buffer;
			const size_t buf_size;

			size_t line = 1;
			size_t column = 1;
			size_t buf_pos = 0;

			Diagnostic diagnostics;
			ArenaVec<Token> tokens;

			bool fatal = false;

			// Ensure that branches are taken away during compile-time with guaranteed
			// compile-time value if statements. This is used so that both printing mode
			// and normal mode do not have to be separate functions.
			static constexpr bool OUTPUT_MODE = Mode == LexMode::PrintingMode;
			static constexpr bool NORMAL_MODE = Mode == LexMode::TokenMode;

		public:
			friend class Parser;

			Lexer(const File *buffer, const char *file_name,
			      const size_t buffer_size)
					: buffer(buffer), buf_size(buffer_size) {
				diagnostics = Diagnostic(buffer, buffer_size, file_name);
			}

			void lex_int();

			template<int radix, int skip>
			void lex_int();

			void lex_float(int skip, int start = 0);

			void lex_str();

			void lex_ident();

			ArenaVec<Token> lex();


			/// peek `NChars` characters in the file buffer.
			char peek(size_t NChars) {
				return *(buffer + NChars);
			}

			/// Get the latest character in the file buffer
			char curr() {
				return *buffer;
			}

			/// Move the lexer forward by `n` and fill in `Error` for
			/// unexpected EOF or Out of Range Error that may be encountered.
			char next(size_t n) {

				if (buf_pos >= buf_size) {
					if (Mode == TokenMode) {
						diagnostics.build(LexError{
								LexErrorType::OutOfRange,
								{line, column},
								Token("\0", {line, column}, TokenKind::EOFToken)
						});
						UNREACHABLE;
					} else fatal = true;
				}

				column += n;
				buf_pos += n;
				buffer += n;
				return *buffer;
			}

		private:

			/// Matches a new line encountered in the buffer and handles
			/// them appropriately depending on \n or \r.
			void skip_new_line() {
				line++;
				column = 1;

				next(1);

				char current = *buffer;
				char next_ch = peek(1);

				if (next_ch != current && (next_ch == '\n' || next_ch == '\r')) {
					if (Mode == LexMode::PrintingMode) Console::Log(next_ch);
					++line, column = 1, next(1);
				}
			}

		};


	}
} // namespace nextgen::jet



# endif //NEXTGEN_LEXER_H
