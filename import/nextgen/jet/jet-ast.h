# ifndef JET_AST_H
# define JET_AST_H

namespace nextgen { namespace jet {

  template<typename T>
  using Map = std::unordered_map<str, T, str::intern_hash, str::intern_eq>;

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
    I64,
    U8,
    U16,
    U32,
    U64,
    F32,
    F64,
    Str,
    Bool,
    PointerTy,
    ArrayView,
    SmartPointer,
    UserDefinedType
  };

  struct Type {
    TypeTag tag;
    Type *ref;

    Type() = default;
    Type(TypeTag tag, Type *ref)
    : tag(tag), ref(ref) {}
    Type(const Type& other)
    : tag(other.tag), ref(other.ref) {}
  };

  struct PointerType : public Type {
    Type pointer_of;
    explicit PointerType(const Type &p) : pointer_of(p) {
      this->tag = TypeTag::PointerTy;
    }
  };

  struct ArrayViewType : public Type {
    Type array_of;
    size_t size   = 0;
    bool has_size = false;
    explicit ArrayViewType(const Type &a) : array_of(a) {
      this->tag = TypeTag::ArrayView;
    }
  };

  struct StructType : public Type {
    const str name = nullptr;
    explicit StructType(const str &name) : name(name) {
      this->tag = TypeTag::UserDefinedType;
    }
  };

  struct Variable {
    const Token *initial_decl;
    Type type = Type();
    bool is_mutable = false;

    Variable() = default;
    Variable(const Token *decl,
             Type type)
             : initial_decl(decl), type(type) {}
  };

  struct Scope {
    Scope *parent;
    Map<Variable> variables;
    Scope() = default;
    Scope(Scope *parent) : parent(parent) {}
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

  struct StructMember {
    const Token *name = nullptr;
    const Type type;
  };

  struct NodeStruct {
    const Token *name = nullptr;
    ArenaVec<StructMember> members;

    struct intern_hash {
      size_t operator()(NodeStruct const &s) const {
        return s.name->name().getHashCache();
      }
    };

    struct intern_eq {
      bool operator()(NodeStruct const &LHS, NodeStruct const &RHS) const {
        auto lhs = LHS.name->name();
        auto rhs = RHS.name->name();
        return strncmp(lhs.begin(), rhs.begin(), lhs.size()) == 0;
      }
    };
  };

  struct NodeEnum {
    const Token *name = nullptr;
    ArenaVec<const Token *> constants;

    struct intern_hash {
      size_t operator()(NodeStruct const &s) const {
        return s.name->name().getHashCache();
      }
    };

    struct intern_eq {
      bool operator()(NodeStruct const &LHS, NodeStruct const &RHS) const {
        auto lhs = LHS.name->name();
        auto rhs = RHS.name->name();
        return strncmp(lhs.begin(), rhs.begin(), lhs.size()) == 0;
      }
    };
  };
}}



# endif //JET_AST_H