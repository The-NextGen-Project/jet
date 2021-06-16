# ifndef NEXTGEN_TOKEN_H
# define NEXTGEN_TOKEN_H
# include <nextgen/Support/String.h>
# include <nextgen/Support/Allocator.h>

namespace nextgen { namespace jet {

  // References the range in the actual source text.
  using SourceSpan = Range<const char *>;


  enum TokenClassification {
    Function     = 1 << 2,
    Assignment   = 1 << 3,
    Unsigned     = 1 << 4,
    Keyword      = 1 << 5,
  };

  enum TokenKind {

    // Primitives ---------------------------
    Integer = 1,  // + sizeof(decltype(UINTPTR_MAX))
    String,   // + sizeof(const char *)
    Decimal,  // + sizeof(double)
    Boolean,  // + sizeof(bool)
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


    EqualsEquals, // ==
    LeftShift,    // <<
    RightShift,   // >>
    Pow,          // **
    RangeSpan,        // ..
    Ellipsis,     // ...

    PlusPlus,     // ++
    MinusMinus,   // --

    Error
  };

  struct TokenTraits {

    /// Definitive Source Location of a Token that is lexed, may also
    /// apply to failed tokens as well.
    struct SourceLocation {
      size_t Line;
      size_t Column;
      Range<const char *> SourceRange;
    };

    /// Since std::variant<Args...> does not exist in C++11, we must use a
    /// controlled union Value type in order to hold values that can be
    /// represented in C++.
    struct Value {
      TokenKind Kind;
      union {
        decltype(UINTPTR_MAX) Integer;
        bool Boolean;
        double Float64;
        char SingleByte;
      };
    };


    struct ErrorInfo {
      int metadata[5];
    };

  };

  class Token {
  private:
    using TokenValue     = TokenTraits::Value;
    using SourceLocation = TokenTraits::SourceLocation;

    nextgen::str ID;
    SourceLocation Location;
    TokenValue InternalValue{};
    unsigned Flags;


  public:

    /// Default constructor is used to avoid any single initialization conflicts
    Token() = default;

    /// Set literal token value and generate correct Token Instance
    template<typename T>
    static auto New(const nextgen::str &ID, SourceLocation Location, T Value,
         TokenKind Kind,
         TokenClassification Flags = static_cast<TokenClassification>(0))
      -> Token {

      Token Instance = Token(ID, Location, Flags);
      Instance.setValue(Value);
      Instance.setKind(Kind);
      return Instance;
    }

    /// Trivial Token creation for Failed Tokens Built during lexing
    static auto New(nextgen::str ID, SourceLocation Loc, TokenKind Kind) ->
    Token {
      Token Instance;
      Instance.setKind(Kind);
      Instance.ID = ID;
      Instance.Location = Loc;
      return Instance;
    }

    /// Get the token's length
    NG_AINLINE size_t Length() const {
      return ID.size();
    }

    /// Return String instance that represents lexed Token
    NG_AINLINE str Name() const {
      return ID;
    }

    /// Set the token's value depending on valid construct.
    template<typename T>
    NG_INLINE void setValue(T v) {
      ValueSet(v);
    }


    /// Set the token type (used in cases where type is determined later)
    NG_AINLINE void setKind(TokenKind Kind) {
      InternalValue.Kind = Kind;
    }

    /// Get the token type
    NG_AINLINE auto getKind() -> TokenKind {
      return InternalValue.Kind;
    }

    /// Retrieve Source Location of the Token (Source Range)
    NG_AINLINE auto getSourceLocation() -> SourceLocation  {
      return Location;
    }

    /// Set a flag for extraneous metadata for the token
    NG_AINLINE void setFlag(TokenClassification flag) {
      Flags |= flag;
    }


    /// @unsafe Get the value of the token. Does not check for correct
    /// value retrieval, may result in UB if used incorrectly!
    template<typename T>
    NG_INLINE T getValue()  {
      T v;
      ValueSetAndGet(v);
      return v;
    }

    /// Determine whether the token is a given Keyword or Typename
    NG_AINLINE bool isKeyword() {
      return Flags & TokenClassification::Keyword;
    }

    /// Pretty Print Token to the Screen. This usually works nicely for
    /// Diagnostics, as tokens on the line need to be printed normally.
    void PrettyPrint() {
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
        case Error:
          break;
        default:
          break;
      }

      Console::Log(Name());
      Console::Log(Colors::RESET);
    }

    bool assignment = false; // (ie: +=, -=, *=)
  private:

    /// Private initialization constructor
    Token(const nextgen::str &ID, SourceLocation Location,
          TokenClassification Flags)
      : ID(ID), Flags(Flags), Location(Location) {}

    // Generic Type Inference on Inferred Value

    NG_AINLINE void ValueSetAndGet(double &V) const {
      V = InternalValue.Float64;
    }

    NG_AINLINE void ValueSetAndGet(decltype(UINTPTR_MAX) &V) const {
      V = InternalValue.Integer;
    }

    NG_AINLINE void ValueSetAndGet(char &V) const {
      V = InternalValue.SingleByte;
    }

    NG_AINLINE void ValueSetAndGet(bool &V) const {
      V = InternalValue.Boolean;
    }

    NG_AINLINE void ValueSet(double V)  {
      InternalValue.Float64 = V;
    }

    NG_AINLINE void ValueSet(bool V)  {
      InternalValue.Boolean = V;
    }

    NG_AINLINE void ValueSet(char V)  {
      InternalValue.SingleByte = V;
    }

    NG_AINLINE void ValueSet(decltype(UINTPTR_MAX) V)  {
      InternalValue.Integer = V;
    }

    NG_AINLINE void ValueSet(const char *V)  {
      InternalValue.Integer = (unsigned long long) V;
    }

  };

}} // namespace nextgen::jet

# endif //NEXTGEN_TOKEN_H
