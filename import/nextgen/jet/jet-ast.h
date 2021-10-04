# ifndef JET_AST_H
# define JET_AST_H

namespace nextgen { namespace jet {

  enum ASTKind {
    BinaryExprNode,
    LiteralExprNode,
    IfNode,
    ElseNode,
    ElifNode,
    WhileNode,
    ForListNode,
    ForRangeNode,
    Structure,
    FunctionNode,
    MatchNode
  };

  struct AST {
    ASTKind kind;
  };

  enum TypeTag {
    Void,
    I8,
    I16,
    I32,
    I65,
    U8,
    U16,
    U32,
    U64,
    Str,
    PointerTy,
    ArrayView,
    SmartPointer
  };

  struct Type {
    TypeTag tag;
    Type *ref;
  };

  struct Pointer : public Type {
    Type pointer_of;
    /*implicit*/ Pointer(const Type &p) : pointer_of(p) {}
  };

  struct ArrayView : public Type {
    Type array_of;
    size_t size   = 0;
    bool has_size = false;
    /*implicit*/ ArrayView(const Type &a) : array_of(a) {}
  };

  struct Variable {
    const Token *initial_decl;
    const Type type;
    bool is_mutable = false;

    struct intern_hash {
      size_t operator()(Variable const &s) const {
        return s.initial_decl->name().getHashCache();
      }
    };

    struct intern_eq {
      bool operator()(Variable const &LHS, Variable const &RHS) const {
        auto lhs = LHS.initial_decl->name();
        auto rhs = RHS.initial_decl->name();
        return strncmp(lhs.begin(), rhs.begin(), lhs.size()) == 0;
      }
    };
  };

  struct Scope {
    Scope *parent;
    std::unordered_set<Variable, Variable::intern_hash,
    Variable::intern_eq> variables;
  };

  struct NodeLiteral {
    Type type;
    const Token *literal_token;
  };

  struct NodeBinary {
    enum Op {
      MemberGet, // struct_instance.member_that_is_struct.its_member
      Add,
      Sub,
      Div,
      Mul,
      Power,
      Gtr,
      GtrEq,
      LessThanValue,
      LessEq,
      LShft,
      LShftEq,
      RShft,
      RShftEq,
      l_AND, // and
      l_OR,  // or
      _XOR, // ^
      _AND, // &
      _OR   // |
    };

    const AST *lhs = nullptr;
    const AST *rhs = nullptr;
    Op op;
  };

  struct NodeWhile {
    const AST *cond;
    ArenaVec<AST> statements;
  };

  struct NodeElse {
    ArenaVec<AST> statements;
  };

  struct NodeElif {
    ArenaVec<AST> statements;
  };

  struct NodeIf {
    const AST *cond;
    NodeElif *else_;
    Option<ArenaVec<NodeElif>> elif_branches;
  };

  struct FunctionParameter {
    const Token *param_name;
    const Type type;
  };

  struct NodeFunction {
    const Token *name = nullptr;
    ArenaVec<AST> statements;
    ArenaVec<FunctionParameter> parameters;

    struct intern_hash {
      size_t operator()(NodeFunction const &s) const {
        return s.name->name().getHashCache();
      }
    };

    struct intern_eq {
      bool operator()(NodeFunction const &LHS, NodeFunction const &RHS) const {
        auto lhs = LHS.name->name();
        auto rhs = RHS.name->name();
        return strncmp(lhs.begin(), rhs.begin(), lhs.size()) == 0;
      }
    };
  };

  using VariableMap = std::unordered_set<Variable, Variable::intern_hash,
  Variable::intern_eq> ;
  using FunctionMap = std::unordered_set<NodeFunction, NodeFunction::intern_hash,
  NodeFunction::intern_eq> ;

}}



# endif //JET_AST_H