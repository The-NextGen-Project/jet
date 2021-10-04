#ifndef JET_JET_SYNTAX_NODES_H
#define JET_JET_SYNTAX_NODES_H
# include "jet-token.h"

namespace nextgen { namespace jet {

    enum SyntaxKind {
      LiteralValue,
      Binary,
      Unary,
      List,
      If,
      Else,
      Elif,
      For,
      While,
      FunctionCall,
      Struct,
      ForList,
      ForRange,
      Enum,
      VariableAssignment,
      VariableValueAssignment,
      VariableReassignment,
      ArrayInitialization,
      Assert,
      FunctionDefault,
      FunctionStructProperty,
      Match,
    };

    enum SyntaxUnaryOp {
      BitwiseNot,
      Positive,
      Negative,
      ConditionalNot,
      MemoryReference,
      Inc,
      Dec
    };

    enum SyntaxBinaryOp {
      Addition,
      Subtraction,
      Division,
      Multiplication,
      Modulus,
      Greater,
      Less,
      GreaterEqualTo,
      LessEqualTo,
      BooleanOr,
      BooleanAnd,
      BinaryShLeft,
      BinaryShRight,
      BinaryXOR,
      BinaryAND,
      BinaryOR
    };

    enum SyntaxTypeAnnotation {
      Optional,   // ?int
      Reference,  // &int
      Pointer,    // *int
      Box,        // box int
      ArrayType    // [] int
    };

    enum SyntaxTypenameKind {
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
      StringValue,
      UserDefined
    };

    enum SyntaxAssignmentOp {
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

    struct SyntaxTypename {
      SyntaxTypenameKind kind;

      // When dealing with user-defined structs, when expanding the type
      // we need to ensure that the name is recorded somewhere so that
      // type-checking can proceed in the third pass of the compiler.
      Option<Range<const char *>> user_defined_typename;

      SyntaxTypename() = default;
      SyntaxTypename(SyntaxTypenameKind kind,
                     const Option<Range<const char *>> &optional)
                     : kind(kind), user_defined_typename(optional)
                     {}

      bool operator ==(const SyntaxTypename &other) const {
        if (
          kind == SyntaxTypenameKind::UserDefined
          &&
          other.kind == SyntaxTypenameKind::UserDefined
          )  {
          auto first = user_defined_typename.unwrap();
          auto second = other.user_defined_typename.unwrap();
          return strncmp(first.begin, second.begin, first.range());
        }
        return other.kind == kind;
      }
    };

    struct SyntaxNode {
      SyntaxKind kind;
      SyntaxNode() = default;
    };
    

    struct SyntaxLiteral : public SyntaxNode {

      explicit SyntaxLiteral(const Token *l) : literal(l) {}


      // contains the literal token value. Token's already contain the actual
      // data for the literal, and therefore nothing else is needed. The token
      // is required for any diagnostics that could be recorded as they contain
      // source location & pretty printing options.
      //
      // Literal examples can be seen in the [Lexer] class.
      const Token *literal;
    };


    struct SyntaxBinary : public SyntaxNode {

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
      const SyntaxBinaryOp operation; 
      const Token *op;

      // Left and right grouped expressions.
      const SyntaxNode *lhs;
      const SyntaxNode *rhs;

      /// Convert a `TokenKind` to `SyntaxBinaryOp` kind
      static NG_INLINE const SyntaxBinaryOp MatchOp(TokenKind kind) {
        switch(kind) {
          case TokenKind::Plus:
            return SyntaxBinaryOp::Addition;
          case TokenKind::Minus:
            return SyntaxBinaryOp::Subtraction;
          case TokenKind::Star:
            return SyntaxBinaryOp::Multiplication;
          case TokenKind::Slash:
            return SyntaxBinaryOp::Division;
          case TokenKind::Percent:
            return SyntaxBinaryOp::Modulus;
          case TokenKind::AND:
            return SyntaxBinaryOp::BinaryAND;
          case TokenKind::Pipe:
            return SyntaxBinaryOp::BinaryOR;
          case TokenKind::XOR:
            return SyntaxBinaryOp::BinaryXOR;
          case TokenKind::LeftShift:
            return SyntaxBinaryOp::BinaryShLeft;
          case TokenKind::RightShift:
            return SyntaxBinaryOp::BinaryShRight;
          case TokenKind::GreaterThan:
            return SyntaxBinaryOp::Greater;
          case TokenKind::GreaterThanEquals:
            return SyntaxBinaryOp::GreaterEqualTo;
          case TokenKind::LessThan:
            return SyntaxBinaryOp::Less;
          case TokenKind::LessThanEquals:
            return SyntaxBinaryOp::LessEqualTo;
          case TokenKind::KeywordOr:
            return SyntaxBinaryOp::BooleanOr;
          case TokenKind::KeywordAnd:
            return SyntaxBinaryOp::BooleanAnd;
          default:
            UNREACHABLE;
        }
      }

      SyntaxBinary(const SyntaxBinaryOp op,
                   const Token *oo,
                   const SyntaxNode *lhs,
                   const SyntaxNode*rhs)
      : operation(op), op(oo), lhs(lhs), rhs(rhs) {}
    };

    struct SyntaxType : public SyntaxNode {
      SyntaxType() = default;
      SyntaxType(const Option<SyntaxTypeAnnotation> &modifier,
                 const Option<SyntaxTypename> &ty,
                 const SyntaxType *type)
                 : modifier(modifier), ty_name(ty), type(type) {}


      const Option<SyntaxTypeAnnotation> modifier = None;
      const Option<SyntaxTypename>       ty_name  = None;
      const SyntaxType *type = nullptr;

      static NG_INLINE SyntaxTypename MatchTypename(const Token *kind) {
        switch (kind->getKind()) {
          case Typename_str:
            return SyntaxTypename(SyntaxTypenameKind::StringValue, None);
          case Typename_i8:
            return SyntaxTypename(SyntaxTypenameKind::Integer8, None);
          case Typename_i16:
            return SyntaxTypename(SyntaxTypenameKind::Integer16, None);
          case Typename_i32:
            return SyntaxTypename(SyntaxTypenameKind::Integer32, None);
          case Typename_i64:
            return SyntaxTypename(SyntaxTypenameKind::Integer64, None);
          case Typename_u8:
            return SyntaxTypename(SyntaxTypenameKind::UInteger8, None);
          case Typename_u16:
            return SyntaxTypename(SyntaxTypenameKind::UInteger16, None);
          case Typename_u32:
            return SyntaxTypename(SyntaxTypenameKind::UInteger32, None);
          case Typename_u64:
            return SyntaxTypename(SyntaxTypenameKind::UInteger64, None);
          case Typename_box:
            return SyntaxTypename(SyntaxTypenameKind::Boxed, None);
          case Identifier:
            return SyntaxTypename(SyntaxTypenameKind::UserDefined,
                                  (Range<const char *>)kind->name());
          default:
            UNREACHABLE;
        }
      }


      bool has_typename() const {
        return ty_name.isSome();
      }

      bool has_modifier() const {
        return modifier.isSome();
      }

      bool has_type() const {
        return type;
      }

      bool operator ==(const SyntaxType &other) const {
        auto ret = (other.has_typename() && has_typename())
                   && (other.has_modifier() && has_modifier())
                   && (other.has_type());
        if (ret) {
          return (*type) == (*other.type)
             &&  ty_name.contains(other.ty_name.unwrap())
             &&  modifier.contains(other.modifier.unwrap());
        }
        return ret;
      }

      const SyntaxTypename get_typename() const {
        if (!has_typename()) {
          type->get_typename();
        }
        return ty_name.unwrap();
      }


      // *int  = SyntaxType { modifier: Pointer, ty_name: None,
      // type: SyntaxType {
      //    modifier: None,
      //    ty_name: Integer32,
      //    type: None
      // }

    };

    struct SyntaxList : public SyntaxNode {
      ObjectVector<const SyntaxNode*> values;
    };

    struct SyntaxVariableValueAssignment : public SyntaxNode {
      const Token *name = nullptr;
      const SyntaxNode *expression = nullptr;
      const SyntaxAssignmentOp op;

      static NG_INLINE SyntaxAssignmentOp MatchOp(TokenKind kind) {
        switch(kind) {
          case PlusEquals:
            return SyntaxAssignmentOp::AddAssign;
          case MinusEquals:
            return SyntaxAssignmentOp::SubAssign;
          case DivEquals:
            return SyntaxAssignmentOp::DivAssign;
          case PowEquals:
            return SyntaxAssignmentOp::PowAssign;
          case MulEquals:
            return SyntaxAssignmentOp::MulAssign;
          case LeftShiftEquals:
            return SyntaxAssignmentOp::LShiftAssign;
          case RightShiftEquals:
            return SyntaxAssignmentOp::RShiftAssign;
          case ANDEquals:
            return SyntaxAssignmentOp::ANDAssign;
          case XOREquals:
            return SyntaxAssignmentOp::XORAssign;
          case OREquals:
            return SyntaxAssignmentOp::ORAssign;
          case PercentEquals:
            return SyntaxAssignmentOp::ModAssign;
          default:
            break;
        }
      }

      SyntaxVariableValueAssignment(const Token *name,
                                    const SyntaxNode *expr,
                                    const SyntaxAssignmentOp op)
                                    : name(name), expression(expr), op(op)
                                { this->kind = SyntaxKind::VariableAssignment; }

    };

    struct SyntaxVariableAssignment : public SyntaxNode {

      SyntaxVariableAssignment(const Token *name,
                               const Option<SyntaxType*> &ty,
                               const SyntaxNode*expr)
                               : name(name), type(ty), expression(expr) {
        this->kind = SyntaxKind::VariableAssignment;
      }

      // The actual variable name; ex: var my_variable_name = ...
      //                                   ^^^^^^^^^^^^^^^^
      const Token *name = nullptr;

      // Optional variable type. It may need to be inferred so it can be set
      // to 'None'. ex: (var ident: int) = 23 OR (var ident = 23)
      //                            ^^^                    ^  No type here
      Option<SyntaxType*> type = None;

      // Node representing the actual value that the variable contains
      const SyntaxNode *expression;
    };


    struct SyntaxVariableReassignment : public SyntaxNode {

      SyntaxVariableReassignment(const Token *v, const SyntaxNode *expr)
      : variable(v), expression(expr) {}

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
      const SyntaxNode *expression = nullptr;
    };

    struct SyntaxBlock : public SyntaxNode {

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
      ObjectVector<const SyntaxNode*> statements;


      // Special statement for function / lambda return check. Allows for void
      // function type inference.
      bool contains_return = false;
    };

    struct SyntaxElse : public SyntaxNode {

      explicit SyntaxElse(SyntaxBlock block) : body(block) {}


      // Else statement current in an if-elif-else statement chain
      // Ex:
      // if cond { ... }
      // else { ... }
      // ^^^^ Declared here. Note that the curly braces may be omitted for one
      // statement bodies.
      SyntaxBlock body;
    };

    struct SyntaxElif : public SyntaxNode {

      explicit SyntaxElif(SyntaxBlock block) : body(block) {}


      // Elif statement current in an if-elif-else statement chain
      // Ex:
      // if cond { ... }
      // elif cond { ... }
      // ^^^^ Declared here. Note that the curly braces may be omitted for one
      // statement bodies.
      SyntaxBlock body;
    };

    struct SyntaxWhile : public SyntaxNode {
      // Boolean expression representing the while condition.
      // Ex: while cond
      //        ^^^^ Declared here
      const SyntaxNode *condition;
      // Statement list of commands executed upon the condition above being true
      // Ex: while cond {
      //    variable := "Hello"
      // }
      SyntaxBlock body;

      SyntaxWhile(const SyntaxNode *cond, const SyntaxBlock &body)
      : condition(cond), body(body) {
        this->kind = SyntaxKind::While;
      }
    };

    struct SyntaxMatch : public SyntaxNode {

      struct MatchPair {
        const SyntaxNode *value;
        SyntaxBlock action;
      };

      const SyntaxNode *value;
      ArenaVec<MatchPair> pairs;
    };

    struct SyntaxIf : public SyntaxNode {

      // Boolean expression representing the if condition.
      // Ex: if cond
      //        ^^^^ Declared here
      const SyntaxNode *condition;

      // Statement list of commands executed upon the condition above being true
      // Ex: if cond {
      //    flag = true
      // }
      SyntaxBlock body;

      // See `SyntaxElse`
      const SyntaxElse *else_ = nullptr;

      // See `SyntaxElif`
      const SyntaxNode *elif  = nullptr;

      SyntaxIf(const SyntaxNode *cond,
               const SyntaxBlock &body,
               const SyntaxElse *else_,
               const SyntaxNode *elif)
               : condition(cond), body(body), else_(else_), elif(elif) {
        this->kind = SyntaxKind::If;
      }
    };

    struct SyntaxUnary : public SyntaxNode {

      // Unary expression. We denote a few unary operators:
      // ++ : Increment and return latest value
      // -- : Decrement and return latest value
      // &  : Take the memory address of the expression on the right
      // *  : Dereference the expression on the right
      // +  : Assert RHS expression is positive
      // -  : Negate RHS expression
      // ~  : Bitwise NOT
      const SyntaxUnaryOp operation;
      const Token *op;

      // Expression to apply unary operation
      // Ex: ++some_var, &memory_address, *pointer_value, -some_value
      const SyntaxNode *expression = nullptr;

      /// Convert `TokenKind` to `SyntaxUnaryOp` operation
      static NG_INLINE const SyntaxUnaryOp MatchOp(TokenKind kind) {
        switch(kind) {
          case TokenKind::Plus:
            return SyntaxUnaryOp::Positive;
          case TokenKind::NOT:
            return SyntaxUnaryOp::BitwiseNot;
          case TokenKind::Minus:
            return SyntaxUnaryOp::Negative;
          case TokenKind::PlusPlus:
            break;
          case TokenKind::MinusMinus:
            break;
          case TokenKind::ExclamationPoint:
            return SyntaxUnaryOp::ConditionalNot;
          default:
            UNREACHABLE;
        }
        UNREACHABLE;
      }

      SyntaxUnary(const SyntaxUnaryOp op,
                  const Token *oo,
                  const SyntaxNode *expr)
      : operation(op), op(oo), expression(expr) {
        this->kind = SyntaxKind::Unary;
      }
    };

    struct SyntaxFunctionParameter : public SyntaxNode {
      
      SyntaxFunctionParameter(const Token *pn, SyntaxType *ty)
      : param_name(pn), type(ty) {}

      // Function parameter name
      const Token *param_name = nullptr;

      // Function Parameter type. If no type is given, then it is inferred to
      // be a generic parameter.
      // Ex:
      // fn some_func(param_1: int, param_2: str, $param_3)
      //                                          ^^^^^^^^ Generic Parameter
      const SyntaxType *type;
    };

    struct SyntaxFunction : public SyntaxNode {

      // Function name
      const Token *function_name = nullptr;

      // Function return type. If 'None' then infer return type.
      Option<SyntaxType*> function_type;

      // Function body
      SyntaxBlock body;

      // Function parameters
      ArenaVec<SyntaxFunctionParameter> parameters{nullptr, nullptr};

      SyntaxFunction(const Token *name,
                     const Option<SyntaxType*> &ty,
                     const SyntaxBlock &body,
                     const ArenaVec<SyntaxFunctionParameter> &params)
                     : function_name(name), body(body), function_type(ty),
                      parameters(params){}
    };

    struct SyntaxFunctionCall : public SyntaxNode {

      // NOTE: Function Declarations should be lowered
      // before function call declarations.
      //
      // Ex:
      // some_function(1, 2, 2, 3, 5)
      const Token      *function_name = nullptr;
      ObjectVector<const SyntaxNode*> parameters;

      SyntaxFunctionCall(const Token *name)
                         : function_name(name)
                        { this->kind = SyntaxKind::FunctionCall; }
    };

    struct SyntaxForList : public SyntaxNode {
      SyntaxForList(const Token *ln, const Token *lv, SyntaxBlock body)
      : list_name(ln), list_var(lv), body(body) {}
      
      // For loop iteration over a variable of type list.
      // Ex:
      // for num    in    numbers { ... }
      //     ^^^ Loop var ^^^^^^^ List variable
      const Token *list_name = nullptr;
      const Token *list_var  = nullptr;
      SyntaxBlock body;
    };

    struct RangeFunction {
      size_t set[3] = {0, 0, 0};
    };

    struct SyntaxForRange : public SyntaxNode {
      SyntaxForRange(const Token *list_var, const RangeFunction
      range, SyntaxBlock body) :
      list_var(list_var), range(range), body(body) {}

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
      const RangeFunction range;
      SyntaxBlock body;
    };

    struct SyntaxStructMember {
      const Token *name = nullptr;
      SyntaxType *type;
      SyntaxStructMember(const Token *name, SyntaxType *ty)
      : name(name), type(ty) {}
    };

    struct SyntaxStruct : public SyntaxNode {
      const Token *name = nullptr;
      ArenaVec<SyntaxStructMember> members;
      SyntaxStruct(const Token *name, const ArenaVec<SyntaxStructMember>
        &members) : name(name), members(members) {}
    };

}}
#endif //JET_JET_SYNTAX_NODES_H
