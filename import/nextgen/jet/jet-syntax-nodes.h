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
      ListType    // [] int
    };

    enum SyntaxTypenameKind {
      Integer32,
      Integer64,
      Float32,
      Float64,
      StringValue,
      UserDefined
    };

    struct SyntaxTypename {
      SyntaxTypenameKind kind;

      // When dealing with user-defined structs, when expanding the type
      // we need to ensure that the name is recorded somewhere so that
      // type-checking can proceed in the third pass of the compiler.
      Option<Range<const char *>> user_defined_typename;

      bool operator ==(const SyntaxTypename &other) {
        if (kind == SyntaxTypenameKind::UserDefined && other.kind ==
        SyntaxTypenameKind::UserDefined) {
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

      explicit SyntaxLiteral(Token *l) : literal(l) {}


      // contains the literal token value. Token's already contain the actual
      // data for the literal, and therefore nothing else is needed. The token
      // is required for any diagnostics that could be recorded as they contain
      // source location & pretty printing options.
      //
      // Literal examples can be seen in the [Lexer] class.
      Token *literal;
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
      SyntaxBinaryOp operation; Token *op;

      // Left and right grouped expressions.
      SyntaxNode *lhs;
      SyntaxNode *rhs;

      /// Convert a `TokenKind` to `SyntaxBinaryOp` kind
      static NG_INLINE SyntaxBinaryOp MatchOp(TokenKind Kind) {
        switch(Kind) {
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
          default:
            UNREACHABLE;
        }
      }

      SyntaxBinary(SyntaxBinaryOp op, Token *oo, SyntaxNode *lhs, SyntaxNode*rhs)
      : operation(op), op(oo), lhs(lhs), rhs(rhs) {}
    };

    struct SyntaxType : public SyntaxNode {
      SyntaxType() = default;
      SyntaxType(const Option<SyntaxTypeAnnotation> &modifier,
                 const Option<SyntaxTypename> &ty,
                 const Option<SyntaxType*> type)
                 : modifier(modifier), ty_name(ty), type(type) {}


      Option<SyntaxTypeAnnotation> modifier = None;
      Option<SyntaxTypename>       ty_name  = None;
      Option<SyntaxType*>          type     = None;

      bool has_typename() const {
        return ty_name.isSome();
      }

      bool has_modifier() const {
        return modifier.isSome();
      }

      bool has_type() const {
        return type.isSome();
      }

      bool operator ==(const SyntaxType &other) {
        auto ret = (other.has_typename() && has_typename())
                   && (other.has_modifier() && has_modifier())
                   && (other.has_type());
        if (ret) {
          return type.contains(other.type.unwrap())
             &&  ty_name.contains(other.ty_name.unwrap())
             &&  modifier.contains(other.modifier.unwrap());
        }
        return ret;
      }

      SyntaxTypename get_typename() {
        if (!has_typename()) {
          type.unwrap()->get_typename();
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
      ObjectVector<SyntaxNode*> values;
    };

    struct SyntaxVariableAssignment : public SyntaxNode {

      SyntaxVariableAssignment(Token *name, const Option<SyntaxType> &ty,
                               SyntaxNode*expr)
                               : name(name), type(ty), expression(expr) {}

      // The actual variable name; ex: var my_variable_name = ...
      //                                   ^^^^^^^^^^^^^^^^
      Token *name = nullptr;

      // Optional variable type. It may need to be inferred so it can be set
      // to 'None'. ex: (var ident: int) = 23 OR (var ident = 23)
      //                            ^^^                    ^  No type here
      Option<SyntaxType> type = None;

      // Node representing the actual value that the variable contains
      SyntaxNode *expression;
    };


    struct SyntaxVariableReassignment : public SyntaxNode {

      SyntaxVariableReassignment(Token *v, SyntaxNode *expr)
      : variable(v), expression(expr) {}

      // Identifier representing the variable that is being reassigned.
      // Ex: var some_var = 23
      //         ^^^^^^^^ First declared here
      //     some_var = 242
      //     ^^^^^^^^ Reassigned here. Note: You cannot reassign variables with
      // values that are not the same as its first initialization. For example,
      // the variable `some_var` is declared as an 'int' and cannot be
      // reassigned with a value of 'str'.
      Token *variable = nullptr;

      // Node representing the actual value that the variable contains
      SyntaxNode *expression = nullptr;
    };

    struct SyntaxBlock : public SyntaxNode {

      // A sequential list of statements categorized as a current node.
      // Ex:
      // {
      //   var x = 23
      //   x = x ** x
      //   a_function_call(x)
      // }
      // There are 3 statements here, and the current will contain 3 statements.
      // Note: There is an exception to parsing of this node, in statements such
      // as 'while', 'if', 'elif', 'else', 'for' may contain a single statement
      // after them without a set of corresponding '{}'.
      ObjectVector<SyntaxNode*> statements;


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
      SyntaxNode *condition;
      // Statement list of commands executed upon the condition above being true
      // Ex: while cond {
      //    variable := "Hello"
      // }
      SyntaxBlock body;
    };

    struct SyntaxMatch : public SyntaxNode {

      struct MatchPair {
        SyntaxNode *value;
        SyntaxBlock action;
      };

      SyntaxNode *value;
      ArenaVec<MatchPair> pairs;
    };

    struct SyntaxIf : public SyntaxNode {

      // Boolean expression representing the if condition.
      // Ex: if cond
      //        ^^^^ Declared here
      SyntaxNode *condition;

      // Statement list of commands executed upon the condition above being true
      // Ex: if cond {
      //    flag = true
      // }
      SyntaxBlock body;

      // See `SyntaxElse`
      SyntaxElse *else_ = nullptr;

      // See `SyntaxElif`
      SyntaxNode *elif  = nullptr;
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
      SyntaxUnaryOp operation; Token *op;

      // Expression to apply unary operation
      // Ex: ++some_var, &memory_address, *pointer_value, -some_value
      SyntaxNode *expression = nullptr;

      /// Convert `TokenKind` to `SyntaxUnaryOp` operation
      static NG_INLINE SyntaxUnaryOp MatchOp(TokenKind kind) {
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

      SyntaxUnary(SyntaxUnaryOp op, Token *oo, SyntaxNode *expr)
      : operation(op), op(oo), expression(expr) {}
    };

    struct SyntaxFunctionParameter : public SyntaxNode {
      
      SyntaxFunctionParameter(Token *pn, SyntaxType ty)
      : param_name(pn), type(ty) {}

      // Function parameter name
      Token *param_name = nullptr;

      // Function Parameter type. If no type is given, then it is inferred to
      // be a generic parameter.
      // Ex:
      // fn some_func(param_1: int, param_2: str, $param_3)
      //                                          ^^^^^^^^ Generic Parameter
      SyntaxType type;
    };

    struct SyntaxFunction : public SyntaxNode {

      // Function name
      Token *function_name = nullptr;

      // Function return type. If 'None' then infer return type.
      Option<SyntaxType> function_type = None;

      // Function body
      SyntaxBlock body;

      // Function parameters
      ArenaVec<SyntaxFunctionParameter> parameters{nullptr, nullptr};
    };

    struct SyntaxFunctionCall : public SyntaxNode {

      // NOTE: Function Declarations should be lowered
      // before function call declarations.
      //
      // Ex:
      // some_function(1, 2, 2, 3, 5)
      Token      *function_name = nullptr;
      ObjectVector<SyntaxNode*> parameters;
    };

    struct SyntaxForList : public SyntaxNode {
      SyntaxForList(Token *ln, Token *lv, SyntaxBlock body)
      : list_name(ln), list_var(lv), body(body) {}
      
      // For loop iteration over a variable of type list.
      // Ex:
      // for num    in    numbers { ... }
      //     ^^^ Loop var ^^^^^^^ List variable
      Token *list_name = nullptr;
      Token *list_var  = nullptr;
      SyntaxBlock body;
    };

    struct SyntaxForRange : public SyntaxNode {
      SyntaxForRange(Token *list_var, SyntaxBlock body) :
      list_var(list_var), body(body) {}

      // Range based for loop. A range consists of a few factors:
      // start, end, and step.
      // Start - the value that initializes the loop variable.
      // End   - the last value the variable should loop to.
      // Step  - is how much a loop variable should change. A step of 2, for
      // example, will increment the loop variable by 2 each time.

      // Ex:
      // for i in range(1, 5) { ... } OR
      // for i in range(1, 232, 3) { ... }
      Token *list_var = nullptr;
      SyntaxBlock body;
    };


}}
#endif //JET_JET_SYNTAX_NODES_H
