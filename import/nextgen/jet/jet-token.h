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

    // Valid Expression Tokens (DO NOT CHANGE ORDER) ---------------------------
    Identifier,   // [a-z] && [A-Z] && _
    LessThan,     // <
    GreaterThan,  // >

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

    // TODO: Fix this ... Should not be apart of binary expression. It should
    //  be apart of the assignment modifier.
    PlusEquals,
    MinusEquals,
    DivEquals,
    PowEquals,
    MulEquals,
    ANDEquals,
    XOREquals,
    OREquals,
    PercentEquals,
    LeftShiftEquals,
    RightShiftEquals,
    GreaterThanEquals,
    LessThanEquals,

    EqualsEquals, // ==
    LeftShift,    // <<
    RightShift,   // >>
    Pow,          // **
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
    KeywordRange,
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

    // File Control ---------------------------
    NewLine,
    Whitespace,
    StringEscapeStart,
    //---------------------------------------

    LParenthesis, // (
    RParenthesis, // )
    LCurlyBrace,  // {
    RCurlyBrace,  // }
    LBracket,     // [
    RBracket,     // ]
    Colon,        // :
    Equals,       // =
    QuestionMark, // ?
    Then,         // ??
    At,     // @
    SemiColon, // ;
    ExclamationPoint, // !
    Comma, // ,

    RangeSpan,    // ..
    Ellipsis,     // ...

    PlusPlus,     // ++
    MinusMinus,   // --
    FunctionArrow,// =>

    ColonEquals,   // :=
    Arrow,         // ->
    Path, // ::

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

  };

  class Token {
  private:
    using SourceLocation = TokenTraits::SourceLocation;

    /** @brief The string value representing token */
    str id;

    // TODO: Do I really need this?

    /** @brief Way to categorize the token */
    unsigned       flags = TokenClassification::Literal;


    /** @brief Actual location of the token */
    SourceLocation loc   = {};


    /** @brief The type of token (Used in Parsing) */
    TokenKind kind;
  public:
    Token() = default;

    union {
      decltype(UINTPTR_MAX) integer;
      bool    boolean;
      double  float64;
      char    character;
    };

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

    static NG_INLINE char const *GetTokenKindName(TokenKind const kind) {
      switch(kind){
        case LessThan:
          return "<";
        case GreaterThan:
          return ">";
        case Plus:
          return "+";
        case Minus:
          return "-";
        case Slash:
          return "/";
        case Star:
          return "*";
        case XOR:
          return "^";
        case AND:
          return "&";
        case NOT:
          return "~";
        case Pipe:
          return "|";
        case Percent:
          return "%";
        case Char:
          return "'";
        case Dot:
          return ".";
        case PlusEquals:
          return "+=";
        case MinusEquals:
          return "-=";
        case DivEquals:
          return "/=";
        case PowEquals:
          return "**=";
        case MulEquals:
          return "*=";
        case GreaterThanEquals:
          return ">=";
        case LessThanEquals:
          return "<=";
        case LeftShiftEquals:
          return "<<=";
        case RightShiftEquals:
          return ">>=";
        case ANDEquals:
          return "&=";
        case XOREquals:
          return "^=";
        case OREquals:
          return "|=";
        case PercentEquals:
          return "%=";
        case EqualsEquals:
          return "==";
        case LeftShift:
          return "<<";
        case RightShift:
          return ">>";
        case Pow:
          return "**";
        case KeywordIf:
          return "if";
        case KeywordElse:
          return "else";
        case KeywordElif:
          return "elif";
        case KeywordWhile:
          return "while";
        case KeywordFor:
          return "for";
        case KeywordAnd:
          return "and";
        case KeywordOr:
          return "or";
        case KeywordBreak:
          return "break";
        case KeywordContinue:
          return "continue";
        case KeywordDefer:
          return "defer";
        case KeywordStruct:
          return "struct";
        case KeywordEnum:
          return "enum";
        case KeywordExport:
          return "export";
        case KeywordExtern:
          return "extern";
        case KeywordFunction:
          return "fn";
        case KeywordError:
          return "error";
        case KeywordTrue:
          return "true";
        case KeywordFalse:
          return "false";
        case KeywordNone:
          return "None";
        case KeywordReturn:
          return "return";
        case KeywordUnion:
          return "union";
        case KeywordMatch:
          return "match";
        case KeywordIn:
          return "in";
        case KeywordRange:
          return "range";
        case Typename_str:
          return "str";
        case Typename_i8:
          return "i8";
        case Typename_i16:
          return "i16";
        case Typename_i32:
          return "i32";
        case Typename_i64:
          return "i64";
        case Typename_u8:
          return "u8";
        case Typename_u16:
          return "u16";
        case Typename_u32:
          return "u32";
        case Typename_u64:
          return "u64";
        case Typename_box:
          return "box";
        case Typename_f32:
          return "f32";
        case Typename_f64:
          return "f64";
        case LParenthesis:
          return "(";
        case RParenthesis:
          return ")";
        case LCurlyBrace:
          return "{";
        case RCurlyBrace:
          return "}";
        case LBracket:
          return "[";
        case RBracket:
          return "]";
        case Colon:
          return ":";
        case Equals:
          return "=";
        case QuestionMark:
          return "?";
        case Then:
          return "??";
        case At:
          return "@";
        case SemiColon:
          return ";";
        case ExclamationPoint:
          return "!";
        case Comma:
          return ",";
        case RangeSpan:
          return "..";
        case Ellipsis:
          return "...";
        case PlusPlus:
          return "++";
        case MinusMinus:
          return "--";
        case FunctionArrow:
          return "=>";
        case ColonEquals:
          return ":=";
        case Arrow:
          return "->";
        case Path:
          return "::";
        case EOFToken:
          return "EOF";
        default:
          UNREACHABLE;
      }
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

    bool isClosingDelim() const {
      return kind == TokenKind::RBracket || kind == TokenKind::RCurlyBrace
      || kind == TokenKind::RParenthesis;
    }

    bool isValidExpressionType() const {
      return unsigned(kind) >= Integer && unsigned(kind) <= Pow;
    }

    bool isValueAssignmentOp() const {
      return unsigned(kind) >= PlusEquals && unsigned(kind) <= RightShiftEquals;
    }

    bool isValidTypename() const {
      return unsigned (kind) >= Typename_str && unsigned(kind) <=
      Typename_f64 || kind == Identifier;
    }

    bool isValidTypenameStart() const {
      return kind == Star || kind == LBracket || kind == LParenthesis;
    }

    SourceLocation getSourceLocation() const {
      return loc;
    }

    void setFlag(TokenClassification flag) {
      flags |= flag;
    }

    template<typename T>
    T getValue() const {
      static_assert(std::is_same<T, char>::value                  ||
                    std::is_same<T, decltype(UINTPTR_MAX)>::value ||
                    std::is_same<T, bool>::value                  ||
                    std::is_same<T, double>::value,
                    "Attempting to get invalid value from Token.");

      T v; setFromInternalRepr(v);
      return v;
    }

    /// Pretty Print individual token to the screen.
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
      V = float64;
    }

    void setFromInternalRepr(decltype(UINTPTR_MAX) &V) const {
      V = integer;
    }

    void setFromInternalRepr(char &V) const {
      V = character;
    }

    void setFromInternalRepr(bool &V) const {
      V = boolean;
    }

    void setInternalRepr(double V)  {
      float64 = V;
    }

    void setInternalRepr(bool V)  {
      boolean = V;
    }

    void setInternalRepr(char V)  {
      character = V;
    }

    void setInternalRepr(decltype(UINTPTR_MAX) V)  {
      integer = V;
    }

    void setInternalRepr(const char *V)  {
      integer = (unsigned long long) V;
    }

  };

}} // namespace nextgen::jet

# endif //NEXTGEN_TOKEN_H
