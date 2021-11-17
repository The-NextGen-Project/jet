#ifndef JET_ANALYZER_TYPES_H
#define JET_ANALYZER_TYPES_H

namespace nextgen { namespace jet {

  struct AST_Node;
  using Statements = std::vector<AST_Node>;
  using Node = const AST_Node *;

  template<typename T>
  using Map = std::unordered_map<std::string_view, T, string_view_hash::hash, string_view_hash::eq>;

  enum AST_Kind {
    Binary_Expr_Node,
    Literal_Expr_Node,
    If_Node,
    Else_Node,
    Elif_Node,
    While_Node,
    ForList_Node,
    ForRange_Node,
    Structure,
    Function_Node,
    Match_Node
  };


  struct AST_Node {
    AST_Kind kind;
  };


  enum Type_Tag {
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
    Pointer_Ty,
    Array_View,
    Smart_Pointer,
    User_Defined_Type
  };

  struct Type {
    Type_Tag tag;
    Type    *ref;
  public: // Constructors

    Type() = default;
    Type(Type_Tag tag, Type *ref) : tag(tag), ref(ref) {}
    Type(const Type& other) : tag(other.tag), ref(other.ref) {}
  public: // Operator Overloads

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

  struct Pointer_Type : public Type {
    const Type *pointer_of;
    explicit Pointer_Type(const Type *p) : pointer_of(p) {
      this->tag = Type_Tag::Pointer_Ty;
    }

    bool operator==(const Pointer_Type &other) const {
      return *pointer_of == *other.pointer_of;
    }

  };

  struct Array_View_Type : public Type {
    const Type *array_of;
    explicit Array_View_Type(const Type *a) : array_of(a) {
      this->tag = Type_Tag::Array_View;
    }
    bool operator==(const Array_View_Type &other) {
      return *array_of == *other.array_of;
    }
  };

  struct Struct_Type : public Type {
    const std::string_view name;
    explicit Struct_Type(const std::string_view &name) : name(name) {
      this->tag = Type_Tag::User_Defined_Type;
    }
    bool operator==(const Struct_Type &other) {
      return this->name == other.name;
    }
  };


  struct Variable {
    const Type  *type;
    bool        is_mutable = false;
  public: // Constructors
    Variable() = default;
    Variable(const Type *type) : type(type) {}
  };

  struct Scope {
    Map<Variable> variables;
    Scope         *parent;
  public: // Constructor
    Scope() = default;
    Scope(Scope *parent) : parent(parent) {}
  };


  struct AST {

    struct Lit : public AST_Node {
      const Token *literal_token;
      const Type  *type;
    public: // Constructors
      Lit(const Token *literal_token, const Type *type) : literal_token(literal_token),
                                                          type(type) {}
    };

    struct Unary : public AST_Node {
      Node expression;
      enum Op {
        Unary_Inc,
        Unary_Dec,
        Unary_Positive,
        Unary_Negative,
        Unary_Memory_Reference,
        Unary_Boolean_Not,
        Unary_Dereference
      } op;
    public: // Constructors
      Unary(const AST_Node *expression, Op op) : expression(expression), op(op) {}
    };

    struct Binary : public AST_Node {
      Node lhs = nullptr;
      Node rhs = nullptr;
      enum Op {
        Member_Get,
        Add,
        Sub,
        Div,
        Mul,
        Power,
        Gtr,
        GtrEq,
        Less_Than_Value,
        LessEq,
        LShft,
        LShftEq,
        RShft,
        RShftEq,
        Bin_AND,
        Bin_OR,
        Bin_XOR
      } op;

    public: // Constructors
      Binary(const AST_Node *lhs, const AST_Node *rhs, Op op) : lhs(lhs), rhs(rhs), op(op) {}
    };

    struct Block : public AST_Node {
      Statements statements;
      Scope      *scope;
    };


    struct While : public AST_Node {
      Statements statements;
      Node              cond;
    public: // Constructors
      While(const Statements &statements, const AST_Node *cond) : statements(statements),
                                                                  cond(cond) {}
    };

    struct Else : public AST_Node {
      Statements statements;
    public: // Constructors
      explicit Else(const Statements &statements) : statements(statements) {}
    };

    struct Elif : public AST_Node {
      Statements statements;
    public: // Constructors
      explicit Elif(const Statements &statements) : statements(statements) {}
    };

    struct If : public AST_Node {
      Option<std::vector<Elif>> elif_branches;
      Else                       *else_;
      Node                       cond;
    public: // Constructors
      If(const Option <std::vector<Elif>> &elif_branches, Else * else_, const AST_Node *cond)
        :elif_branches(elif_branches), else_(else_), cond(cond){}
    };

    struct Func_Param {
      const Token *param_name;
      const Type type;
    };

    struct Func {
      const Token *name = nullptr;
      std::vector<Func_Param> parameters;
      Statements statements;
    public: // Constructors
      Func(const Token *name, const std::vector <Func_Param> &parameters,
           const Statements &statements) : name(name), parameters(parameters),
                                           statements(statements) {}
    };

    struct Func_Sig {
      const Type *return_type;
      std::vector<Type> parameter_types;
    };

    struct Struct_Member {
      const Token *name = nullptr;
      const Type *type;
    };

    struct Struct {
      std::vector<Struct_Member> members;
    };

    struct Enum {
      std::vector<const Token *> constants;
    };
  };

}}



#endif //JET_ANALYZER_TYPES_H