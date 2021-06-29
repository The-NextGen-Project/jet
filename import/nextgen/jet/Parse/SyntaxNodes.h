#ifndef JET_SYNTAXNODES_H
#define JET_SYNTAXNODES_H
# include "../Lex/Token.h"

namespace nextgen { namespace jet {

    // Needed for reference to all other syntax expressions
    class SyntaxExpression;

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
      Token *Literal;
    };

    struct SyntaxBinary {
      SyntaxBinaryOp Operation;
      SyntaxExpression *LHS;
      SyntaxExpression *RHS;
      Token *Op;

      static NG_INLINE auto MatchOp(TokenKind Kind) -> SyntaxBinaryOp {
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
            Console::Log("I WAS HERE\n");
            break;
        }
      }

    };

    struct SyntaxType {
      SyntaxTypeAnnotation Modifier;
      SyntaxTypeValue Value;
      Option<int> Indirection;
    };

    // ST { Pointer,  }

    struct SyntaxList {
      Vec<SyntaxExpression*> Values;
    };

    struct SyntaxVariableAssignment {
      Token *Name;
      Option<SyntaxType> Type;
      SyntaxExpression *Expression;
    };

    struct SyntaxVariableReassignment {
      Token *Variable;
      SyntaxExpression *Expression;
    };

    struct SyntaxBlock {
      Vec<SyntaxExpression*> Statements;
    };

    struct SyntaxElse {
      SyntaxBlock Body;
    };

    struct SyntaxElif {
      SyntaxBlock Body;
    };

    struct SyntaxIf {
      SyntaxExpression *Condition;
      SyntaxBlock Body;
      SyntaxExpression *Else;
      SyntaxExpression *Elif;
    };

    struct SyntaxUnary {
      SyntaxUnaryOp Operation;
      Token *Op;
      SyntaxExpression *Expression;

      static NG_INLINE auto MatchOp(TokenKind Kind) -> SyntaxUnaryOp {
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
      Token *ParameterName;
      Option<SyntaxType> Type; // Generic Parameter (no repr types)
    };

    struct SyntaxFunction {
      Token *FunctionName;
      Option<SyntaxType> FunctionType;
      SyntaxBlock Body;
      Vec<SyntaxFunctionParameter> Parameters;
    };

    struct SyntaxFunctionCall {
      Token *FunctionName;
      Vec<SyntaxExpression*> Parameters;
    };

    struct SyntaxExpression {
      SyntaxKind Kind;
      union {
        SyntaxLiteral Literal;
        SyntaxBinary  Binary;
        SyntaxUnary   Unary;
        SyntaxList    List;
        SyntaxFunction Function;
        SyntaxIf   If;
        SyntaxElse Else;
        SyntaxElif Elif;
        SyntaxVariableAssignment VariableAssignment;
        SyntaxFunctionCall FunctionCall;
      };
    };
  }}
#endif //JET_SYNTAXNODES_H
