# ifndef NEXTGEN_TOKEN_H
# define NEXTGEN_TOKEN_H
# include <nextgen/support/core.h>
# include <nextgen/support/str.h>
# include <nextgen/support/allocator.h>

namespace nextgen { namespace jet {

  // References the range in the actual source text.
  using SourceSpan = Range<const char *>;

  // Token Classification.
  //
  // This is to determine different types of token in conjunction with normal
  // parsing comparison to result in faster branches. Costs are as simple id
  // checks and lookaheads. For example, token: `myVariable = 23`, id
  // `myVariable` will lookahead for an `=` to determine whether it is a
  // variable and will be added the flag of 'kVariable'.
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

  };


  // This Token class is not made to be space efficient. It is designed to hold
  // as much information about a Token as possible construct an accurate and
  // easy to make AST (Abstract Syntax Tree) of the program to be lowered. It
  // contains constructs useful for error reporting like `SourceLocation`.
  // The pointer to its value `const TokenKind *` is allocated by the Lexer's
  // allocator which handles its cleanup.
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
    template <typename T>
    static TokenKind *BuildTokenKind(Allocator *allocator, TokenKind kind) {
      constexpr auto size = sizeof(TokenKind) + sizeof(T) + 1;
      auto ptr = static_cast<TokenKind *>(allocator->next(size));
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
