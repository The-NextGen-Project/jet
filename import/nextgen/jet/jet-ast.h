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

    bool operator==(const Type &other) const {
      auto check1 = other.tag == this->tag;
      bool check2 = true;
      if (ref) {
        if (!other.ref) {
          check2 = false;
        } else {
          check2 = *ref == *other.ref;
        }
      }
      return check1 && check2;
    }

    bool operator!=(const Type &other) const {
      return !(this->operator==(other));
    }
  };

  struct PointerType : public Type {
    const Type *pointer_of;
    explicit PointerType(const Type *p) : pointer_of(p) {
      this->tag = TypeTag::PointerTy;
    }

    bool operator==(const PointerType &other) const {
      return *pointer_of == *other.pointer_of;
    }

  };

  struct ArrayViewType : public Type {
    const Type *array_of;
    explicit ArrayViewType(const Type *a) : array_of(a) {
      this->tag = TypeTag::ArrayView;
    }
    bool operator==(const ArrayViewType &other) {
      return *array_of == *other.array_of;
    }
  };

  struct StructType : public Type {
    const str name = nullptr;
    explicit StructType(const str &name) : name(name) {
      this->tag = TypeTag::UserDefinedType;
    }
    bool operator==(const StructType &other) {
      return strncmp(name.begin(), other.name.begin(), name.size()) == 0;
    }
  };


  struct Variable {
    const Token *initial_decl;
    const Type *type;
    bool is_mutable = false;

    Variable() = default;
    Variable(const Token *decl,
             const Type *type)
             : initial_decl(decl), type(type) {}
  };

  struct Scope {
    Scope *parent;
    Map<Variable> variables;
    Scope() = default;
    Scope(Scope *parent) : parent(parent) {}
  };

  struct NodeLiteral {
    Type *type;
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

  struct NodeWhile : public AST {
    const AST *cond;
    ArenaVec<AST> statements;
  };

  struct NodeElse : public AST {
    ArenaVec<AST> statements;
  };

  struct NodeElif : public AST {
    ArenaVec<AST> statements;
  };

  struct NodeIf : public AST {
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
  };

  struct FuncSig {
    str name;
    const Type *return_type;
    ArenaVec<const Type> parameter_types;
  };

  struct StructMember {
    const Token *name = nullptr;
    const Type *type;
  };

  struct NodeStruct {
    const Token *name = nullptr;
    ArenaVec<StructMember> members;
  };

  struct NodeEnum {
    const Token *name = nullptr;
    ArenaVec<const Token *> constants;
  };
}}



# endif //JET_AST_H