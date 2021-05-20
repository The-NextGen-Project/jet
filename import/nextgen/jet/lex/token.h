# ifndef NEXTGEN_TOKEN_H
# define NEXTGEN_TOKEN_H
# include <nextgen/support/core.h>
# include <nextgen/support/str.h>
# include <nextgen/support/allocator.h>

namespace nextgen { namespace jet {

  // References the range in the actual source text.
  using SourceSpan = Range<const char *>;


  enum TokenClassification : unsigned {
    Reserved  = 1 << 2,
    Function     = 1 << 3,
    Assignment = 1 << 4,
  };

  enum TokenKind {

    // Primitives ---------------------------
    Integer,  // + sizeof(decltype(UINTPTR_MAX))
    String,   // + sizeof(const char *)
    Decimal,  // + sizeof(double)
    Boolean,  // + sizeof(bool)
    //---------------------------------------

    // File Control ---------------------------
    NewLine,
    Whitespace,
    StringEscapeStart,
    //---------------------------------------

    Identifier,   // [a-z] && [A-Z] && _
    LParenthesis, // (
    RParenthesis, // )
    LCurlyBrace,  // {
    RCurlyBrace,  // }
    LBracket,     // [
    RBracket,     // ]
    Colon,        // :
    LessThan,     // <
    GreaterThan,  // >
    Equals,       // =
    QuestionMark, // ?

    Plus,   // +
    Minus,  // -
    Slash,  // /
    Star,   // *
    XOR,    // ^
    AND,    // &
    NOT,    // ~
    Pipe,   // |
    Percent,// %
    Char,   // \'
    Dot,    // .


    EqualsEquals, // ==
    LeftShift,    // <<
    RightShift,   // >>
    Pow,          // **
    Ellipsis,     // ...

    Error
  };


  class Token {
  public:
    using Allocator = nextgen::mem::ArenaSegment;
    struct SourceLocation {
      size_t line;
      size_t col;
    };

    Token() = default;
    Token(nextgen::str span, const TokenKind *kind, SourceLocation loc,
          TokenClassification flags, bool assignment = false)
      : id(span), kind(kind), flags(flags), assignment(assignment),
      location(loc) {}


    // Builds a TokenKind* with the correct type included with the pointer.
    // This allows for the type be retrieved with the token kind, and so the
    // whole token is not required.
    template<typename T>
    static TokenKind *BuildTokenKind(Allocator *allocator, TokenKind kind) {
      constexpr auto size = sizeof(TokenKind) + sizeof(T) + 1;
      auto ptr = allocator->next<TokenKind>(size).UnwrapOrElse([&]() {
        return allocator->getNext()->next<TokenKind>(size).Unwrap();
      });
      *ptr = kind;
      return ptr;
    }

    NG_INLINE TokenKind getKind() const {
      return this->kind[0];
    }
    bool assignment = false;
  private:
    nextgen::str        id;      // Token String representation
    TokenClassification flags{}; // Token Flags (Parsing Info)
    SourceLocation      location{0, 0};// Location in SourceText
    const TokenKind *kind{}; // Type and Value (if primitive) of Token
  };

}} // namespace nextgen::jet




# endif //NEXTGEN_TOKEN_H
