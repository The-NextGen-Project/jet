#ifndef JET_SYNTAXNODES_H
#define JET_SYNTAXNODES_H
# include "../Lex/Token.h"

namespace nextgen { namespace jet {

    // Needed for reference to all other syntax expressions
    class SyntaxExpression;

    enum SyntaxKind {
      LiteralValue,
      Binary,
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
        UNREACHABLE;
      }

    };

    struct SyntaxType {
      Token *OrginatingType; // if nullptr infer type
      Option<Token*> TypeModifiers; // Ex: int&, ~int, int?, int*
    };

    struct SyntaxVariableAssignment {
      Token *Name;
      SyntaxType Type;
      SyntaxExpression *Expression;
    };

    struct SyntaxVariableReassignment {
      Token *Variable;
      SyntaxExpression *Expression;
    };

    struct SyntaxBlock {
      SyntaxExpression **Statements;
    };

    struct SyntaxElse {
      SyntaxBlock *Body;
    };

    struct SyntaxElif {
      SyntaxBlock *Body;
    };

    struct SyntaxIf {
      SyntaxExpression *Condition;
      SyntaxBlock *Body;
      Option<SyntaxElse> Else;
      Option<SyntaxElif> Elif;
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
      Option<SyntaxType*> Type; // Generic Parameter (no repr types)
    };

    struct SyntaxFunction {
      Token *FunctionName;
      Option<SyntaxType*> FunctionType;
      SyntaxBlock *Body;
      SyntaxFunctionParameter *Parameters;
    };

    struct SyntaxExpression {
      SyntaxKind Kind;
      union {
        SyntaxLiteral Literal;
        SyntaxBinary  Binary;
        SyntaxUnary   Unary;
        SyntaxFunction Function;
        SyntaxIf   If;
        SyntaxElse Else;
        SyntaxElif Elif;
        SyntaxVariableAssignment VariableAssignment;
      };
    };
  }}
#endif //JET_SYNTAXNODES_H
