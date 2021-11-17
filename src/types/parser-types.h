#ifndef JET_PARSER_TYPES_H
#define JET_PARSER_TYPES_H

#include "lexer-types.h"

namespace nextgen { namespace jet {

  enum Syntax_Kind {
    Literal_Value,
    Binary,
    Unary,
    List,
    If,
    Else,
    Elif,
    For,
    While,
    Function_Call,
    Struct,
    ForList,
    ForRange,
    Enum,
    Variable_Assignment,
    Variable_Value_Assignment,
    Variable_Reassignment,
    Array_Initialization,
    Assert,
    Function_Default,
    Function_Struct_Property,
    Match,
    Path_Node,
    Path_Struct,
    Struct_Init,
    Return_Value
  };

  enum Syntax_Unary_Op {
    Bitwise_Not,
    Positive,
    Negative,
    Conditional_Not,
    Memory_Reference,
    Inc,
    Dec
  };

  enum Syntax_Binary_Op {
    Addition,
    Subtraction,
    Division,
    Multiplication,
    Modulus,
    Greater,
    Less,
    Greater_Equal_To,
    Less_Equal_To,
    Boolean_Or,
    Boolean_And,
    Binary_ShLeft,
    Binary_ShRight,
    Binary_XOR,
    Binary_AND,
    Binary_OR
  };

  enum Syntax_Type_Annotation {
    Optional,   // ?int
    Reference,  // &int
    Pointer,    // *int
    Box,        // box int
    ArrayType    // [] int
  };

  enum Syntax_Typename_Tag {
    Integer8,
    Integer16,
    Integer32,
    Integer64,
    UInteger8,
    UInteger16,
    UInteger32,
    UInteger64,
    Boxed,
    Float32,
    Float64,
    String_Value,
    User_Defined
  };

  enum Syntax_Assignment_Op {
    AddAssign,
    SubAssign,
    MulAssign,
    DivAssign,
    ModAssign,
    PowAssign,
    LShiftAssign,
    RShiftAssign,
    ORAssign,
    XORAssign,
    ANDAssign,
  };


  struct Syntax_Typename {
    Syntax_Typename_Tag kind;

    // When dealing with user-defined structs, when expanding the type
    // we need to ensure that the name is recorded somewhere so that
    // type-checking can proceed in the third pass of the compiler.
    Option<std::string_view> user_defined_typename;
  public: // Constructors
    Syntax_Typename() = default;
    Syntax_Typename(Syntax_Typename_Tag kind, const Option<std::string_view> &optional)
                   : kind(kind), user_defined_typename(optional)
                   {}

  public: // Impl
    bool operator ==(const Syntax_Typename &other) const {
      if (
        this->kind == Syntax_Typename_Tag::User_Defined
        &&
        other.kind == Syntax_Typename_Tag::User_Defined
        )
      {
        auto first = this->user_defined_typename.unwrap();
        auto second = other.user_defined_typename.unwrap();
        return strncmp(first.data(), second.data(), first.size());
      }
      return other.kind == this->kind;
    }
  };

  struct Syntax_Node {
    Syntax_Kind kind;
    Syntax_Node() = default;
  };

  struct Return : public Syntax_Node {
    const Syntax_Node *value;
  public: // Constructors
    Return(const Syntax_Node *value) : value(value) {
      this->kind = Syntax_Kind::Return_Value;
    }
  };


  struct Syntax_Path : public Syntax_Node {
    const Token *path_origin;
    const Token *path_value;
  public: // Constructors
    Syntax_Path(const Token *t, const Token *l) : path_origin(t), path_value(l) {
      this->kind = Syntax_Kind::Path_Node;
    }
  };

  struct Syntax_Path_Struct : public Syntax_Node {
    const Token *path_origin;
    const Token *path_value;
    std::vector<Syntax_Node*> struct_values_in_order;
  public: // Constructors
    Syntax_Path_Struct(const Token *origin,
                       const Token *value,
                       const std::vector<Syntax_Node*> struct_values)
    : path_origin(origin), path_value(value), struct_values_in_order(struct_values)
    { this->kind = Syntax_Kind::Path_Struct; }
  };

  struct Syntax_Literal : public Syntax_Node {
    // contains the literal token value. Token's already contain the actual
    // data for the literal, and therefore nothing else is needed. The token
    // is required for any diagnostics that could be recorded as they contain
    // source location & pretty printing options.
    //
    // Literal examples can be seen in the [Lexer] class.
    const Token *literal;

  public: // Constructors
    explicit Syntax_Literal(const Token *l) : literal(l) {
      this->kind = Syntax_Kind::Literal_Value;
    }
  };


  struct Syntax_Binary : public Syntax_Node {
    // Binary expression syntax operation.
    // Ex:
    // +  : Addition
    // -  : Subtraction
    // *  : Multiplication
    // /  : Division
    // ** : Exponential
    // << : Left Shift
    // >> : Right Shift
    // ^  : XOR
    // |  : OR
    // &  : AND
    const Syntax_Binary_Op operation;
    const Token *op;

    // Left and right grouped expressions.
    const Syntax_Node *lhs;
    const Syntax_Node *rhs;

  public: // Constructors
    Syntax_Binary(const Syntax_Binary_Op op,
                  const Token *oo,
                  const Syntax_Node *lhs,
                  const Syntax_Node*rhs)
      : operation(op), op(oo), lhs(lhs), rhs(rhs) { this->kind = Syntax_Kind::Binary; }

  public: // Static Functions

    static const Syntax_Binary_Op MatchOp(Token_Kind kind) {
      switch(kind) {
        case Token_Kind::Plus: return Syntax_Binary_Op::Addition;
        case Token_Kind::Minus: return Syntax_Binary_Op::Subtraction;
        case Token_Kind::Star: return Syntax_Binary_Op::Multiplication;
        case Token_Kind::Slash: return Syntax_Binary_Op::Division;
        case Token_Kind::Percent: return Syntax_Binary_Op::Modulus;
        case Token_Kind::AND: return Syntax_Binary_Op::Binary_AND;
        case Token_Kind::Pipe: return Syntax_Binary_Op::Binary_OR;
        case Token_Kind::XOR: return Syntax_Binary_Op::Binary_XOR;
        case Token_Kind::Left_Shift: return Syntax_Binary_Op::Binary_ShLeft;
        case Token_Kind::Right_Shift: return Syntax_Binary_Op::Binary_ShRight;
        case Token_Kind::Greater_Than: return Syntax_Binary_Op::Greater;
        case Token_Kind::Greater_Than_Equals: return Syntax_Binary_Op::Greater_Equal_To;
        case Token_Kind::Less_Than: return Syntax_Binary_Op::Less;
        case Token_Kind::Less_Than_Equals: return Syntax_Binary_Op::Less_Equal_To;
        case Token_Kind::Keyword_Or: return Syntax_Binary_Op::Boolean_Or;
        case Token_Kind::Keyword_And: return Syntax_Binary_Op::Boolean_And;
        default: UNREACHABLE;
      }
    }
  };

  struct Syntax_Type : public Syntax_Node {
    const Option<Syntax_Type_Annotation> modifier = None;
    const Option<Syntax_Typename>       ty_name  = None;
    const Syntax_Type *type = nullptr;

  public: // Constructors
    Syntax_Type() = default;
    Syntax_Type(const Option<Syntax_Type_Annotation> &modifier,
                const Option<Syntax_Typename> &ty,
                const Syntax_Type *type)
               : modifier(modifier), ty_name(ty), type(type) {}

  public: // Static Functions
    static auto MatchTypename(const Token *kind) {
      switch (kind->type()) {
        case Typename_str: return Syntax_Typename(Syntax_Typename_Tag::String_Value, None);
        case Typename_i8: return Syntax_Typename(Syntax_Typename_Tag::Integer8, None);
        case Typename_i16: return Syntax_Typename(Syntax_Typename_Tag::Integer16, None);
        case Typename_i32: return Syntax_Typename(Syntax_Typename_Tag::Integer32, None);
        case Typename_i64: return Syntax_Typename(Syntax_Typename_Tag::Integer64, None);
        case Typename_u8: return Syntax_Typename(Syntax_Typename_Tag::UInteger8, None);
        case Typename_u16: return Syntax_Typename(Syntax_Typename_Tag::UInteger16, None);
        case Typename_u32: return Syntax_Typename(Syntax_Typename_Tag::UInteger32, None);
        case Typename_u64: return Syntax_Typename(Syntax_Typename_Tag::UInteger64, None);
        case Typename_box: return Syntax_Typename(Syntax_Typename_Tag::Boxed, None);
        case Identifier: return Syntax_Typename(Syntax_Typename_Tag::User_Defined, kind->name());
        default: UNREACHABLE;
      }
    }

  public: // Impl
    NG_AINLINE bool has_typename() const {
      return this->ty_name.is_some();
    }

    NG_AINLINE bool has_modifier() const {
      return this->modifier.is_some();
    }

    NG_AINLINE bool has_type() const {
      return this->type;
    }

    bool operator ==(const Syntax_Type &other) const {
      auto ret = (other.has_typename() && has_typename())
                 && (other.has_modifier() && has_modifier())
                 && (other.has_type());
      if (ret) {
        return (*this->type) == (*other.type)
           &&  this->ty_name.contains(other.ty_name.unwrap())
           &&  this->modifier.contains(other.modifier.unwrap());
      }
      return ret;
    }

    const Syntax_Typename get_typename() const {
      if (!has_typename()) {
        type->get_typename();
      }
      return ty_name.unwrap();
    }


    // *int  = Syntax_Type { modifier: Pointer, ty_name: None,
    // type: Syntax_Type {
    //    modifier: None,
    //    ty_name: Integer32,
    //    type: None
    // }

  };

  struct Syntax_List : public Syntax_Node {
    std::vector<const Syntax_Node*> values;
  };

  struct Syntax_Variable_Value_Assignment : public Syntax_Node {
    const Token *name = nullptr;
    const Syntax_Node *expression = nullptr;
    const Syntax_Assignment_Op op;
  public: // Constructors
    Syntax_Variable_Value_Assignment(const Token *name,
                                     const Syntax_Node *expr,
                                     const Syntax_Assignment_Op op) :
      name(name), expression (expr), op(op)
    { this->kind = Syntax_Kind::Variable_Value_Assignment; }

  public: // Static Functions
    static auto MatchOp(Token_Kind kind) {
      switch(kind) {
        case Plus_Equals: return Syntax_Assignment_Op::AddAssign;
        case Minus_Equals: return Syntax_Assignment_Op::SubAssign;
        case Div_Equals: return Syntax_Assignment_Op::DivAssign;
        case Pow_Equals: return Syntax_Assignment_Op::PowAssign;
        case Mul_Equals: return Syntax_Assignment_Op::MulAssign;
        case Left_Shift_Equals: return Syntax_Assignment_Op::LShiftAssign;
        case Right_Shift_Equals: return Syntax_Assignment_Op::RShiftAssign;
        case AND_Equals: return Syntax_Assignment_Op::ANDAssign;
        case XOR_Equals: return Syntax_Assignment_Op::XORAssign;
        case OR_Equals: return Syntax_Assignment_Op::ORAssign;
        case Percent_Equals: return Syntax_Assignment_Op::ModAssign;
        default: break;
      }
    }
  };

  struct Syntax_Variable_Assignment : public Syntax_Node {
    // The actual variable name; ex: var my_variable_name = ...
    //                                   ^^^^^^^^^^^^^^^^
    const Token *name = nullptr;

    // Optional variable type. It may need to be inferred so it can be set
    // to 'None'. ex: (var ident: int) = 23 OR (var ident = 23)
    //                            ^^^                    ^  No type here
    Option<Syntax_Type*> type = None;

    // Node representing the actual value that the variable contains
    const Syntax_Node *expression;

  public: // Constructors
    Syntax_Variable_Assignment(const Token *name,
                               const Option<Syntax_Type*> &ty,
                               const Syntax_Node *expr)
                             : name(name), type(ty), expression(expr) {
      this->kind = Syntax_Kind::Variable_Assignment;
    }
  };


  struct Syntax_Variable_Reassignment : public Syntax_Node {

    // Identifier representing the variable that is being reassigned.
    // Ex: var some_var = 23
    //         ^^^^^^^^ First declared here
    //     some_var = 242
    //     ^^^^^^^^ Reassigned here. Note: You cannot reassign variables with
    // values that are not the same as its first initialization. For example,
    // the variable `some_var` is declared as an 'int' and cannot be
    // reassigned with a value of 'str'.
    const Token *variable = nullptr;

    // Node representing the actual value that the variable contains
    const Syntax_Node *expression = nullptr;

  public: // Constructors

    Syntax_Variable_Reassignment(const Token *v, const Syntax_Node *expr)
    : variable(v), expression(expr) {}
  };

  struct Syntax_Block : public Syntax_Node {

    // A sequential list of statements categorized as a current node.
    // Ex:
    // {
    //   x := 23
    //   x = x ** x
    //   a_function_call(x)
    // }
    // There are 3 statements here, and the current will contain 3 statements.
    // Note: There is an exception to parsing of this node, in statements such
    // as 'while', 'if', 'elif', 'else', 'for' may contain a single statement
    // after them without a set of corresponding '{}'.
    std::vector<const Syntax_Node*> statements;

    std::vector<Return> returns;

    // Special statement for function / lambda return check. Allows for void
    // function type inference.
    bool contains_return = false;
  };

  struct Syntax_Else : public Syntax_Node {

    explicit Syntax_Else(Syntax_Block block) : body(block) {}


    // Else statement current in an if-elif-else statement chain
    // Ex:
    // if cond { ... }
    // else { ... }
    // ^^^^ Declared here. Note that the curly braces may be omitted for one
    // statement bodies.
    Syntax_Block body;
  };

  struct Syntax_Elif : public Syntax_Node {

    explicit Syntax_Elif(Syntax_Block block) : body(block) {}


    // Elif statement current in an if-elif-else statement chain
    // Ex:
    // if cond { ... }
    // elif cond { ... }
    // ^^^^ Declared here. Note that the curly braces may be omitted for one
    // statement bodies.
    Syntax_Block body;
  };

  struct Syntax_While : public Syntax_Node {
    // Boolean expression representing the while condition.
    // Ex: while cond
    //        ^^^^ Declared here
    const Syntax_Node *condition;
    // Statement list of commands executed upon the condition above being true
    // Ex: while cond {
    //    variable := "Hello"
    // }
    Syntax_Block body;

    Syntax_While(const Syntax_Node *cond, const Syntax_Block &body) : condition(cond), body(body) {
      this->kind = Syntax_Kind::While;
    }
  };


  // TODO: Most difficult part of parsing is the match statement, the most important construct in
  //  the language though, so will be completed later, when analysis is performed on other syntax
  //  nodes.
  struct Syntax_Match : public Syntax_Node {

    struct MatchPair {
      const Syntax_Node *value;
      Syntax_Block action;
    };

    const Syntax_Node *value;
    std::vector<MatchPair> pairs;
  };

  struct Syntax_If : public Syntax_Node {

    // Boolean expression representing the if condition.
    // Ex: if cond
    //        ^^^^ Declared here
    const Syntax_Node *condition;

    // Statement list of commands executed upon the condition above being true
    // Ex: if cond {
    //    flag = true
    // }
    Syntax_Block body;

    // See `Syntax_Else`
    const Syntax_Else *else_ = nullptr;

    // See `Syntax_Elif`
    const Syntax_Node *elif  = nullptr;

    Syntax_If(const Syntax_Node *cond,
             const Syntax_Block &body,
             const Syntax_Else *else_,
             const Syntax_Node *elif)
             : condition(cond), body(body), else_(else_), elif(elif) {
      this->kind = Syntax_Kind::If;
    }
  };

  struct Syntax_Unary : public Syntax_Node {

    // Unary expression. We denote a few unary operators:
    // ++ : Increment and return latest value
    // -- : Decrement and return latest value
    // &  : Take the memory address of the expression on the right
    // *  : Dereference the expression on the right
    // +  : Assert RHS expression is positive
    // -  : Negate RHS expression
    // ~  : Bitwise NOT
    const Syntax_Unary_Op operation;
    const Token *op;

    // Expression to apply unary operation
    // Ex: ++some_var, &memory_address, *pointer_value, -some_value
    const Syntax_Node *expression = nullptr;

  public: // Constructors
    Syntax_Unary(const Syntax_Unary_Op op, const Token *oo, const Syntax_Node *expr)
      : operation(op), op(oo), expression(expr) {
      this->kind = Syntax_Kind::Unary;
    }

  public: // Static Functions

    static const Syntax_Unary_Op MatchOp(Token_Kind kind) {
      switch(kind) {
        case Token_Kind::Plus: return Syntax_Unary_Op::Positive;
        case Token_Kind::NOT: return Syntax_Unary_Op::Bitwise_Not;
        case Token_Kind::Minus: return Syntax_Unary_Op::Negative;
        case Token_Kind::Plus_Plus: return Syntax_Unary_Op::Inc;
        case Token_Kind::Minus_Minus: return Syntax_Unary_Op::Dec;
        case Token_Kind::Exclamation_Point: return Syntax_Unary_Op::Conditional_Not;
        default: UNREACHABLE;
      }
      UNREACHABLE;
    }
  };

  struct Syntax_Function_Parameter : public Syntax_Node {

    Syntax_Function_Parameter(const Token *pn, Syntax_Type *ty) : param_name(pn), type(ty) {}

    // Function parameter name
    const Token *param_name = nullptr;

    // Function Parameter type. If no type is given, then it is inferred to
    // be a generic parameter.
    // Ex:
    // fn some_func(param_1: int, param_2: str, $param_3)
    //                                          ^^^^^^^^ Generic Parameter
    const Syntax_Type *type;
  };

  struct Syntax_Function : public Syntax_Node {
    const Token *function_name = nullptr;
    Option<Syntax_Type*> function_type;  // If 'None' infer the type
    Syntax_Block body;
    std::vector<Syntax_Function_Parameter> parameters;
  public: // Constructors
    Syntax_Function(const Token *name,
                   const Option<Syntax_Type*> &ty,
                   const Syntax_Block &body,
                   const std::vector<Syntax_Function_Parameter> &params)
                   : function_name(name), body(body), function_type(ty),
                    parameters(params) {}
  };

  struct Syntax_Function_Call : public Syntax_Node {
    const Token *function_name = nullptr;
    std::vector<const Syntax_Node*> parameters;
  public: // Constructors
    Syntax_Function_Call(const Token *name) : function_name(name)
                      { this->kind = Syntax_Kind::Function_Call; }
  };

  struct Syntax_ForList : public Syntax_Node {

    // For loop iteration over a variable of type list.
    // Ex:
    // for num    in    numbers { ... }
    //     ^^^ List var ^^^^^^^ List name
    const Token *list_name = nullptr;
    const Token *list_var  = nullptr;
    Syntax_Block body;

  public: // Constructors
    Syntax_ForList(const Token *ln, const Token *lv, Syntax_Block body)
      : list_name(ln), list_var(lv), body(body) {}
  };

  struct Range_Function {
    size_t set[3] = {0, 0, 0};
  };

  struct Syntax_ForRange : public Syntax_Node {
    // Range based for loop. A range consists of a few factors:
    // start, end, and step.
    // Start - the value that initializes the loop variable.
    // End   - the last value the variable should loop to.
    // Step  - is how much a loop variable should change. A step of 2, for
    // example, will increment the loop variable by 2 each time.

    // Ex:
    // for i in range(1, 5) { ... } OR
    // for i in range(1, 232, 3) { ... }
    const Token *list_var = nullptr;
    const Range_Function range;
    Syntax_Block body;

  public: // Constructors
    Syntax_ForRange(const Token *list_var, const Range_Function range, Syntax_Block body)
      : list_var(list_var), range(range), body(body) {}
  };

  struct Syntax_Struct_Member {
    // Ex: a_member: i32
    const Token *name = nullptr;
    Syntax_Type *type;
  public: // Constructors
    Syntax_Struct_Member(const Token *name, Syntax_Type *ty) : name(name), type(ty) {}
  };

  struct Syntax_Struct_Member_Initialization : public Syntax_Node {
    const Token *name = nullptr;
    const Syntax_Node *value = nullptr;
  public: // Constructors
    Syntax_Struct_Member_Initialization(const Token *name, const Syntax_Node *value)
    : name(name), value(value) {}
  };

  struct Syntax_Struct_Instantiation : public Syntax_Node {
    const Token *name = nullptr;
    std::vector<Syntax_Struct_Member_Initialization> values;
  public: // Constructors
    Syntax_Struct_Instantiation(const Token *name,
                                const std::vector<Syntax_Struct_Member_Initialization>& values)
                              : name(name), values(values)
                              { this->kind = Syntax_Kind::Struct_Init; }
  };

  struct Syntax_Struct : public Syntax_Node {
    const Token *name = nullptr;
    std::vector<Syntax_Struct_Member> members;
  public: // Constructor
    Syntax_Struct(const Token *name, const std::vector<Syntax_Struct_Member> &members)
      : name(name), members(members) {}
  };

  struct Syntax_Enum {
    struct Value;

    const Token *name = nullptr;
    std::vector<Value> enumeration_values;

  public: // Structs
    struct Value {
      enum {
        Static_Value,
        Tagged_Value
      } tag;

      union  {
        const Token *static_enumeration;
        const Syntax_Struct *tagged_enumeration;
      };

      explicit Value(const Token *se) : tag(Static_Value), static_enumeration(se) {}
      explicit Value(const Syntax_Struct *ss) : tag(Tagged_Value), tagged_enumeration(ss) {}
    };

  };


  enum Parse_Error_Kind {
    Reserved_Identifier_As_Variable_Name,
    Unexpected_Expression,
    Invalid_Token,
    Expected_Token,

    Missing_Function_Name,
    Missing_Variable_Name,
    Missing_For_Loop_Variable,

    Expected_Identifier_For_Function_Parameter,
    Expected_Identifier_For_Struct_Property,
    Invalid_Token_After_Ident_In_Global_Scope,

    Missing_Closing_Pair,
    Unexpected_End_Of_File
  };

  enum Parse_Context {
    Anonymous_Block,
    Statement_Block,
    Loop_Block,
    Lambda_Block,
    General_Scope
  };

  struct Parse_Result {
    std::vector<const Syntax_Function*>     functions;
    std::vector<const Syntax_Struct*>       structures;
    std::vector<const Syntax_Enum*>         enumerations;
    std::vector<const Syntax_Variable_Assignment*> global_variables;
  };

}}
#endif //JET_PARSER_TYPES_H