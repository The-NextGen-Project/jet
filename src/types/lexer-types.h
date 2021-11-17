#ifndef NEXTGEN_TOKEN_H
#define NEXTGEN_TOKEN_H
#include "util/str.h"
#include "allocator.h"

namespace nextgen { namespace jet {

  // TODO: This can be removed, but do it maybe whenever you feel like refactoring
  enum Token_Flags : unsigned short {
    Function     = 1 << 2,
    Assignment   = 1 << 3,
    Unsigned     = 1 << 4,
    Keyword_     = 1 << 5,
    Literal      = 1 << 6,
  };

  enum Token_Kind {

    // Primitives ---------------------------
    Integer = 1,
    Zero    = 1,
    String,
    Decimal,
    Boolean,
    //---------------------------------------

    // Valid Expression Tokens (DO NOT CHANGE ORDER) ---------------------------
    Identifier,   // [a-z] && [A-Z] && _
    Less_Than,     // <
    Greater_Than,  // >

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
    Plus_Equals,
    Minus_Equals,
    Div_Equals,
    Pow_Equals,
    Mul_Equals,
    AND_Equals,
    XOR_Equals,
    OR_Equals,
    Percent_Equals,
    Left_Shift_Equals,
    Right_Shift_Equals,
    Greater_Than_Equals,
    Less_Than_Equals,

    Equals_Equals, // ==
    Left_Shift,    // <<
    Right_Shift,   // >>
    Pow,          // **
    //---------------------------------------


    // Keyword_s  ---------------------------
    Keyword_If,
    Keyword_Else,
    Keyword_Elif,
    Keyword_While,
    Keyword_For,
    Keyword_And,
    Keyword_Or,
    Keyword_Break,
    Keyword_Continue,
    Keyword_Defer,
    Keyword_Struct,
    Keyword_Enum,
    Keyword_Export,
    Keyword_Extern,
    Keyword_Function,
    Keyword_Error,
    Keyword_Var,
    Keyword_True,
    Keyword_False,
    Keyword_None,
    Keyword_Return,
    Keyword_Union,
    Keyword_Match,
    Keyword_In,
    Keyword_Range,
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
    String_Escape_Start,
    //---------------------------------------

    LParenthesis, // (
    RParenthesis, // )
    LCurly_Brace,  // {
    RCurly_Brace,  // }
    LBracket,     // [
    RBracket,     // ]
    Colon,        // :
    Equals,       // =
    Question_Mark, // ?
    Then,         // ??
    At,     // @
    Semi_Colon, // ;
    Exclamation_Point, // !
    Comma, // ,

    Range_Span,    // ..
    Ellipsis,     // ...

    Plus_Plus,     // ++
    Minus_Minus,   // --
    Function_Arrow,// =>

    Colon_Equals,   // :=
    Arrow,         // ->
    Path, // ::

    Error,
    EOFToken,
  };

  struct Source_Location {
    size_t     line;
    size_t     column;
    Range<const char *> source_span;
  };


  class Token {
    Source_Location loc;
    Token_Kind      kind;
    std::string_view id;
    uint32_t        flags;
    union {
      decltype(UINTPTR_MAX) integer;
      bool    boolean;
      double  float64;
      char    character;
    };
  public: // Constructors
    Token() = default;

    template<typename T>
    Token(const std::string_view &id,
          Source_Location loc,
          T value,
          Token_Kind kind,
          unsigned flags = -1)
          : id(id), flags(flags), loc(loc), kind(kind) { this->set_value(value); }

    Token(const std::string_view &id,
          Source_Location loc,
          Token_Kind kind) : id(id), loc(loc), kind(kind) {}

  public: // Static Functions
    static auto GetTokenKindName(const Token_Kind kind) {
      switch(kind){
        case Less_Than: return "<";
        case Greater_Than: return ">";
        case Plus: return "+";
        case Minus: return "-";
        case Slash: return "/";
        case Star: return "*";
        case XOR: return "^";
        case AND: return "&";
        case NOT: return "~";
        case Pipe: return "|";
        case Percent: return "%";
        case Char: return "'";
        case Dot: return ".";
        case Plus_Equals: return "+=";
        case Minus_Equals: return "-=";
        case Div_Equals: return "/=";
        case Pow_Equals: return "**=";
        case Mul_Equals: return "*=";
        case Greater_Than_Equals: return ">=";
        case Less_Than_Equals: return "<=";
        case Left_Shift_Equals: return "<<=";
        case Right_Shift_Equals: return ">>=";
        case AND_Equals: return "&=";
        case XOR_Equals: return "^=";
        case OR_Equals: return "|=";
        case Percent_Equals: return "%=";
        case Equals_Equals: return "==";
        case Left_Shift: return "<<";
        case Right_Shift: return ">>";
        case Pow: return "**";
        case Keyword_If: return "if";
        case Keyword_Else: return "else";
        case Keyword_Elif: return "elif";
        case Keyword_While: return "while";
        case Keyword_For: return "for";
        case Keyword_And: return "and";
        case Keyword_Or: return "or";
        case Keyword_Break: return "break";
        case Keyword_Continue: return "continue";
        case Keyword_Defer: return "defer";
        case Keyword_Struct: return "struct";
        case Keyword_Enum: return "enum";
        case Keyword_Export: return "export";
        case Keyword_Extern: return "extern";
        case Keyword_Function: return "fn";
        case Keyword_Error: return "error";
        case Keyword_True: return "true";
        case Keyword_False: return "false";
        case Keyword_None: return "None";
        case Keyword_Return: return "return";
        case Keyword_Union: return "union";
        case Keyword_Match: return "match";
        case Keyword_In: return "in";
        case Keyword_Range: return "range";
        case Typename_str: return "str";
        case Typename_i8: return "i8";
        case Typename_i16: return "i16";
        case Typename_i32: return "i32";
        case Typename_i64: return "i64";
        case Typename_u8: return "u8";
        case Typename_u16: return "u16";
        case Typename_u32: return "u32";
        case Typename_u64: return "u64";
        case Typename_box: return "box";
        case Typename_f32: return "f32";
        case Typename_f64: return "f64";
        case LParenthesis: return "(";
        case RParenthesis: return ")";
        case LCurly_Brace: return "{";
        case RCurly_Brace: return "}";
        case LBracket: return "[";
        case RBracket: return "]";
        case Colon: return ":";
        case Equals: return "=";
        case Question_Mark: return "?";
        case Then: return "??";
        case At: return "@";
        case Semi_Colon: return ";";
        case Exclamation_Point: return "!";
        case Comma: return ",";
        case Range_Span: return "..";
        case Ellipsis: return "...";
        case Plus_Plus: return "++";
        case Minus_Minus: return "--";
        case Function_Arrow: return "=>";
        case Colon_Equals: return ":=";
        case Arrow: return "->";
        case Path: return "::";
        case EOFToken: return "EOF";
        default: UNREACHABLE;
      }
    }

  public: // Impl

    template<typename T>
    auto get_value() const {
      static_assert(std::is_same<T, char>::value                  ||
                    std::is_same<T, decltype(UINTPTR_MAX)>::value ||
                    std::is_same<T, bool>::value                  ||
                    std::is_same<T, double>::value,
                    "Attempting to get invalid value from Token.");

      T v;
      this->set_from_internal_repr(v);
      return v;
    }

    template<typename T>
    auto set_value(T v) {
      this->set_internal_repr(v);
    }

    auto len() const {
      return this->id.size();
    }

    auto name() const {
      return this->id;
    }

    auto type() const {
      return this->kind;
    }

    bool is_keyword() const {
      return unsigned(this->kind) >= Keyword_If && unsigned(this->kind) <= Keyword_Range;
    }

    bool is_literal() const {
      return this->kind == Token_Kind::Integer
          || this->kind == Token_Kind::String
          || this->kind == Token_Kind::Char
          || this->kind == Token_Kind::Decimal
          || this->kind == Token_Kind::Keyword_True
          || this->kind == Token_Kind::Keyword_False;
    }

    bool is_closing_delim() const {
      return    this->kind == Token_Kind::RBracket
             || this->kind == Token_Kind::RCurly_Brace
             || this->kind == Token_Kind::RParenthesis;
    }

    bool is_valid_expression_type() const {
      return unsigned(kind) >= Integer && unsigned(kind) <= Pow;
    }

    bool is_value_assignment_op() const {
      return unsigned(kind) >= Plus_Equals && unsigned(kind) <=
      Right_Shift_Equals;
    }

    bool is_valid_typename() const {
      return unsigned (kind) >= Typename_str && unsigned(kind) <=
      Typename_f64 || kind == Identifier;
    }

    bool is_valid_typename_start() const {
      return kind == Star || kind == LBracket || kind == LParenthesis;
    }

    auto location() const {
      return this->loc;
    }

    auto pretty_print() const {

      if (is_keyword()) {
        Console::Log(Colors::RED);
      }

      switch (this->kind) {
        case String: Console::Log(Colors::GREEN); break;
        case Integer: case Decimal: Console::Log(Colors::BLUE); break;
        case Char: Console::Log(Colors::GREEN, "\'"); break;
        case Identifier: Console::Log(Colors::YELLOW); break;
        default: break;
      }

      Console::Log(this->name());
      Console::Log(Colors::RESET);
    }
  private:
    // This is the restricted usage of union values.
    // We are guaranteed to know the actual type of the value from the
    // [Token] constructor, therefore setting and retrieving values is not UB.
    void set_from_internal_repr(double &V) const {
      V = this->float64;
    }

    void set_from_internal_repr(decltype(UINTPTR_MAX) &V) const {
      V = this->integer;
    }

    void set_from_internal_repr(char &V) const {
      V = this->character;
    }

    void set_from_internal_repr(bool &V) const {
      V = this->boolean;
    }

    void set_internal_repr(double V)  {
      this->float64 = V;
    }

    void set_internal_repr(char V)  {
      this->character = V;
    }

    void set_internal_repr(decltype(UINTPTR_MAX) V)  {
      this->integer = V;
    }

    void set_internal_repr(const char *V)  {
      this->integer = (unsigned long long) V;
    }

  };

}} // namespace nextgen::jet

#endif //NEXTGEN_TOKEN_H
