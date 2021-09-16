# ifndef NEXTGEN_TOKEN_H
# define NEXTGEN_TOKEN_H
# include <nextgen/str.h>
# include <nextgen/allocator.h>

namespace nextgen { namespace jet {

  enum TokenClassification : unsigned short {
    Function     = 1 << 2,
    Assignment   = 1 << 3,
    Unsigned     = 1 << 4,
    Keyword      = 1 << 5,
    Literal      = 1 << 6,
  };

  enum TokenKind {

    // Primitives ---------------------------
    Integer = 1,
    Zero    = 1,
    String,
    Decimal,
    Boolean,
    //---------------------------------------

    // File Control ---------------------------
    NewLine,
    Whitespace,
    StringEscapeStart,
    //---------------------------------------

    // Keywords  ---------------------------
    KeywordIf,
    KeywordElse,
    KeywordElif,
    KeywordWhile,
    KeywordFor,
    KeywordAnd,
    KeywordOr,
    KeywordBreak,
    KeywordContinue,
    KeywordDefer,
    KeywordStruct,
    KeywordEnum,
    KeywordExport,
    KeywordExtern,
    KeywordFunction,
    KeywordError,
    KeywordVar,
    KeywordTrue,
    KeywordFalse,
    KeywordNone,
    KeywordReturn,
    KeywordUnion,
    KeywordMatch,
    KeywordIn,
    //---------------------------------------

    // Typename  ---------------------------
    Typename_str,
    Typename_i8,
    Typename_i16,
    Typename_i32,
    Typename_i64,
    Typename_u8,
    Typename_u16,
    Typename_u32,
    Typename_u64,
    Typename_box,
    Typename_f32,
    Typename_f64,
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
    Then,         // ??

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
    At,     // @
    SemiColon, // ;
    ExclamationPoint, // !
    Comma, // ,

    PlusEquals,
    MinusEquals,
    DivEquals,
    PowEquals,
    MulEquals,
    GreaterThanEquals,
    LessThanEquals,
    LeftShiftEquals,
    RightShiftEquals,
    ANDEquals,
    XOREquals,
    OREquals,
    PercentEquals,


    EqualsEquals, // ==
    LeftShift,    // <<
    RightShift,   // >>
    Pow,          // **
    RangeSpan,    // ..
    Ellipsis,     // ...

    PlusPlus,     // ++
    MinusMinus,   // --
    FunctionArrow,// =>

    ColonEquals,   // :=
    Arrow,         // ->

    Error,
    EOFToken,
  };

  struct TokenTraits {

    using SourceSpan = Range<const char *>;

    struct SourceLocation {
      size_t     line;
      size_t     column;
      SourceSpan source;
    };

    // We have to very careful here. We do not have access to std::variant<...>
    // in C++11, so we must use a controlled union to get a Token's value.
    struct Value {
      union {
        decltype(UINTPTR_MAX) integer;
        bool    boolean;
        double  float64;
        char    character;
      };
    };

  };

  class Token {
  private:
    using TokenValue     = TokenTraits::Value;
    using SourceLocation = TokenTraits::SourceLocation;

    str id;

    unsigned       flags = TokenClassification::Literal;
    TokenValue     repr  = {};
    SourceLocation loc   = {};

    TokenKind kind;
  public:
    Token() = default;

    template<typename T>
    Token(const str &id, SourceLocation loc, T value,
          TokenKind kind, unsigned flags = -1)
          : id(id), flags(flags), loc(loc), kind(kind) {
        setValue(value);
    }

    // Note: This is only for lex errors. Failed tokens are important
    // for the diagnostic class generate valid errors.
    Token(str id, SourceLocation loc, TokenKind kind)
    : id(id), loc(loc) {
      setKind(kind);
    }

    size_t len() const {
      return id.size();
    }

    str name() const {
      return id;
    }

    template<typename T>
    void setValue(T v) {
      setInternalRepr(v);
    }

    void setKind(TokenKind k) {
      this->kind = k;
    }

    TokenKind getKind() const {
      return this->kind;
    }

    bool isKeyword() const {
      return flags & TokenClassification::Keyword;
    }

    bool isLiteral() const {
      return flags & TokenClassification::Literal;
    }

    SourceLocation getSourceLocation() {
      return loc;
    }

    void setFlag(TokenClassification flag) {
      flags |= flag;
    }

    template<typename T>
    T getValue()  {
      static_assert(std::is_same<T, char>::value                  ||
                    std::is_same<T, decltype(UINTPTR_MAX)>::value ||
                    std::is_same<T, bool>::value                  ||
                    std::is_same<T, double>::value,
                    "Attempting to get invalid value from Token.");

      T v; setFromInternalRepr(v);
      return v;
    }

    /// Pretty Print individual token to the screen.
    /// TODO: May not need this anymore ...
    void PrettyPrint() const {
      if (isKeyword()) {
        Console::Log(Colors::RED);
      }
      auto Kind = getKind();
      switch (Kind) {
        case String:
          Console::Log(Colors::GREEN);
          break;
        case Integer:
        case Decimal:
          Console::Log(Colors::BLUE);
          break;
        case Char:
          Console::Log(Colors::GREEN, "\'");
          break;
        case Identifier:
          Console::Log(Colors::YELLOW);
          break;
        default:
          break;
      }
      Console::Log(name());
      Console::Log(Colors::RESET);
    }

    bool assignment = false; // (ie: +=, -=, *=)
  private:

    // This is the restricted usage of union values.
    // We are guaranteed to know the actual type of the value from the
    // [Token] constructor, therefore setting and retrieving values is not UB.

    void setFromInternalRepr(double &V) const {
      V = repr.float64;
    }

    void setFromInternalRepr(decltype(UINTPTR_MAX) &V) const {
      V = repr.integer;
    }

    void setFromInternalRepr(char &V) const {
      V = repr.character;
    }

    void setFromInternalRepr(bool &V) const {
      V = repr.boolean;
    }

    void setInternalRepr(double V)  {
      repr.float64 = V;
    }

    void setInternalRepr(bool V)  {
      repr.boolean = V;
    }

    void setInternalRepr(char V)  {
      repr.character = V;
    }

    void setInternalRepr(decltype(UINTPTR_MAX) V)  {
      repr.integer = V;
    }

    void setInternalRepr(const char *V)  {
      repr.integer = (unsigned long long) V;
    }

  };

}} // namespace nextgen::jet

# endif //NEXTGEN_TOKEN_H
