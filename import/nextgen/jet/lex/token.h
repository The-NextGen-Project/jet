# ifndef NEXTGEN_TOKEN_H
# define NEXTGEN_TOKEN_H
# include <nextgen/support/core.h>
# include <nextgen/support/str.h>
# include <nextgen/support/allocator.h>

namespace nextgen { namespace jet {

  // References the range in the actual source text.
  using SourceSpan = Range<const char *>;


  enum TokenClassification : unsigned {
    Reserved = 1 << 2,
    Function = 1 << 3,
    Variable = 1 << 4,
  };

  enum TokenKind {
    // Primitives ---------------------------
    Integer,  // + sizeof(decltype(UINTPTR_MAX))
    String,   // + sizeof(const char *)
    Decimal,  // + sizeof(double)
    Boolean,  // + sizeof(bool)
    //---------------------------------------
    Identifier,

    LParenthesis,
    RParenthesis,
    LCurlyBrace,
    RCurlyBrace,
    LBracket,
    RBracket,
    Colon,
    LessThan,
    GreaterThan,
    Equals,
    QuestionMark,

    Plus,
    Minus,
    Slash,  // /
    Star,   // *
    XOR,    // ^
    AND,    // &
    NOT,    // ~
    Pipe,   // |
    Percent,
    Char, // \'
    Dot, // .


    EqualsEquals, // ==
    LeftShift,
    RightShift,
    Pow, // **

    Error
  };


    class Token {
    public:
      using Allocator = nextgen::mem::ArenaSegment;

      // A basic wrapper around location of a SourceText of a file.
      struct SourceLocation {
        size_t line;
        size_t col;
      };

      Token() = default;
      Token(nextgen::str span, SourceLocation loc, const TokenKind *kind,
            TokenClassification flags)
        : id(span), kind(kind), flags(flags), location(loc) {}


      // Builds a TokenKind* with the correct type included with the pointer.
      // This allows for the type be retrieved with the token kind, and so the
      // whole token is not required.
      //
      // Example:
      // Arena<7> arena = ...
      // auto kind = Token::BuildTokenKind<int>(arena[...], TokenKind::Integer);
      template<typename T>
      static TokenKind *BuildTokenKind(Allocator *allocator, TokenKind kind) {
        constexpr auto size = sizeof(TokenKind) + sizeof(T) + 1;
        auto ptr = allocator->next<TokenKind>(size).UnwrapOrElse([&]() {
          allocator->getNext()->next<TokenKind>(size).Unwrap();
        });
        *ptr = kind;
        return ptr;
      }

      NG_INLINE TokenKind getKind() const {
        return this->kind[0];
      }

    private:
      nextgen::str        id;      // Token String representation
      TokenClassification flags{}; // Token Flags (Parsing Info)
      SourceLocation      location{0, 0};// Location in SourceText
      const TokenKind *kind{}; // Type and Value (if primitive) of Token
    };


}} // namespace nextgen::jet




# endif //NEXTGEN_TOKEN_H
