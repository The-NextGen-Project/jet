#ifndef JET_SYNTAXNODES_H
#define JET_SYNTAXNODES_H
# include "../Lex/Token.h"

namespace nextgen { namespace jet {

    // Needed for reference to all other syntax expressions
    struct SyntaxNode;

    enum SyntaxKind {
      LiteralValue,
      Binary,
      Unary,
      List,
      If,
      Else,
      Elif,
      For,
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
      Optional,
      Reference,
      Pointer,
      Box,
      ListType
    };

    enum SyntaxTypeValue {
      Integer32,
      Integer64,
      Float32,
      Float64,
      StringValue,
      UserDefined
    };

    struct SyntaxLiteral {
      Token *literal;
    };


    struct SyntaxBinary {
      SyntaxBinaryOp operation;
      SyntaxNode *lhs;
      SyntaxNode *rhs;
      Token *op;

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
    };

    struct SyntaxType {
      SyntaxTypeAnnotation modifier;
      SyntaxTypeValue value;
      Option<int> indirection;
    };

    struct SyntaxList {
      Vec<SyntaxNode*> values;
    };

    struct SyntaxVariableAssignment {
      Token *name;
      Option<SyntaxType> type;
      SyntaxNode *expression;
    };


    struct SyntaxVariableReassignment {
      Token *variable;
      SyntaxNode *expression;
    };

    struct SyntaxBlock {
      Vec<SyntaxNode*> statements;
    };


    struct SyntaxElse {
      SyntaxBlock body;
    };

    struct SyntaxElif {
      SyntaxBlock body;
    };

    struct SyntaxIf {
      SyntaxNode *condition;
      SyntaxBlock body;
      SyntaxNode *else_;
      SyntaxNode *elif;
    };

    struct SyntaxUnary {
      SyntaxUnaryOp operation;
      Token *op;
      SyntaxNode *expression;

      static NG_INLINE SyntaxUnaryOp MatchOp(TokenKind Kind) {
        switch(Kind) {
          case TokenKind::Plus:
            return SyntaxUnaryOp::Positive;
          case TokenKind::NOT:
            return SyntaxUnaryOp::BitwiseNot;
          case TokenKind::Minus:
            return SyntaxUnaryOp::Negative;
          case TokenKind::ExclamationPoint:
            return SyntaxUnaryOp::ConditionalNot;
          default:
            UNREACHABLE;
        }
        UNREACHABLE;
      }
    };

    struct SyntaxFunctionParameter {
      Token *param_name;
      Option<SyntaxType> type; // Generic Parameter (no repr types)
    };

    struct SyntaxFunction {
      Token *function_name;
      Option<SyntaxType> function_type;
      SyntaxBlock body;
      Vec<SyntaxFunctionParameter> parameters;
    };

    struct SyntaxFunctionCall {
      Token *function_name;
      Vec<SyntaxNode*> parameters;
    };

    struct SyntaxForList {
      Token *list_name;
      Token *list_var;
      SyntaxBlock body;
    };

    struct SyntaxForRange {
      Token *list_var;
      SyntaxBlock body;
    };

    struct SyntaxNode {
      SyntaxKind kind;
      union {
        SyntaxLiteral Literal;
        SyntaxBinary  Binary;
        SyntaxUnary   Unary;
        SyntaxList    List;
        SyntaxFunction Function;
        SyntaxIf   If;
        SyntaxElse Else;
        SyntaxElif Elif;
        SyntaxForList ForList;
        SyntaxForRange ForRange;
        SyntaxVariableAssignment VariableAssignment;
        SyntaxFunctionCall FunctionCall;
      };
    };
}}
#endif //JET_SYNTAXNODES_H
