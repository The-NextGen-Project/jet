# ifndef NEXTGEN_TOKEN_H
# define NEXTGEN_TOKEN_H
# include <nextgen/support/str.h>
# include <nextgen/support/allocator.h>

namespace nextgen { namespace jet {

  // References the range in the actual source text.
  using SourceSpan = Range<const char *>;


  enum TokenClassification {
    Function     = 1 << 2,
    Assignment   = 1 << 3,
    Unsigned     = 1 << 4
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


  struct Token {
    using Allocator = nextgen::mem::ArenaSegment;

    struct SourceLocation {
      size_t line;
      size_t col;
    };

    struct TokenValue {
      TokenKind kind;
      union {
        decltype(UINTPTR_MAX) integer;
        str *string;
        bool boolean;
        double floating_point;
        char single;
      } value;
    };

    template<typename T>
    static auto New(const nextgen::str &span, TokenValue value, SourceLocation
          loc,
          T literal_value,
          TokenClassification flags = static_cast<TokenClassification>(0),
          bool assignment = false) -> Token {

        auto ret = Token {
          span,
          value,
          loc,
          flags,
          assignment
        };
        ret.setValue(literal_value);
        return ret;
      }


    NG_INLINE size_t Length() const {
      return id.size();
    }

    NG_INLINE const str Name() const {
      return id;
    }

    NG_INLINE SourceLocation getSourceLocation() const {
      return location;
    }

    template<typename T>
    NG_INLINE void setValue(T v) {
      value_set(v);
    }

    NG_INLINE void setFlag(TokenClassification flag) {
      this->flags |= flag;
    }

    template<typename T>
    NG_INLINE T getValue()  {
      T v;
      value_get(v);
      return v;
    }

    NG_INLINE TokenKind getKind() const {
      return this->value.kind;
    }

    NG_INLINE void setKind(TokenKind k) {
      this->value.kind = k;
    }

    nextgen::str id;
    TokenValue value;
    SourceLocation location;
    unsigned long flags;
    bool assignment;

  private:

    // Generic Type Inference on Inferred Value

    NG_AINLINE void value_get(double &v) const {
      v = this->value.value.floating_point;
    }

    NG_AINLINE void value_get(str *&v) const {
      v = this->value.value.string;
    }
    NG_AINLINE void value_get(decltype(UINTPTR_MAX) &v) const {
      v = this->value.value.integer;
    }

    NG_AINLINE void value_get(bool &v) const {
      v = this->value.value.boolean;
    }

    NG_AINLINE void value_get(char &v) const {
      v = this->value.value.single;
    }


    NG_AINLINE void value_set(double v) {
      this->value.value.floating_point = v;
    }

    NG_AINLINE void value_set(decltype(UINTPTR_MAX) v) {
      this->value.value.integer = v;
    }

    NG_AINLINE void value_set(const char *v) {

    }

    NG_AINLINE void value_set(bool v) {
      this->value.value.boolean = v;
    }

    NG_AINLINE void value_set(char v) {
      this->value.value.single = v;
    }

  };

}} // namespace nextgen::jet




# endif //NEXTGEN_TOKEN_H
