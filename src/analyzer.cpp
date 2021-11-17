#include "analyzer.h"

using namespace nextgen::jet;

static const Type *TY_VOID = new Type(Type_Tag::Void, nullptr);
static const Type *TY_BOOL = new Type(Type_Tag::Bool, nullptr);
static const Type *TY_I8   = new Type(Type_Tag::I8, nullptr);
static const Type *TY_I16  = new Type(Type_Tag::I16, nullptr);
static const Type *TY_I32  = new Type(Type_Tag::I32, nullptr);
static const Type *TY_I64  = new Type(Type_Tag::I64, nullptr);
static const Type *TY_U8   = new Type(Type_Tag::U8, nullptr);
static const Type *TY_U16  = new Type(Type_Tag::U16, nullptr);
static const Type *TY_U32  = new Type(Type_Tag::U32, nullptr);
static const Type *TY_U64  = new Type(Type_Tag::U64, nullptr);
static const Type *TY_F32  = new Type(Type_Tag::F32, nullptr);
static const Type *TY_F64  = new Type(Type_Tag::F64, nullptr);
static const Type *TY_STR  = new Type(Type_Tag::Str, nullptr);


void
Analyzer::analyze(const Parse_Result output_to_analyze) {

  // Pass 1: Register Global Variables
  FOR(i, output_to_analyze.global_variables.size()) {
    auto var = output_to_analyze.global_variables[i];
    if (var->type.is_none()) {
      // TODO: Add error that global variables must have a type.
    }
    global_scope->variables[var->name->name()]
                    = Variable(this->resolve_type(var->type.unwrap()));
  }

  // Pass 2: Register All Function Signatures
  FOR(i, output_to_analyze.functions.size()) {
    auto func = output_to_analyze.functions[i];
    this->register_function(func);
  }

  // Pass 3: Analyze All Functions
  FOR(i, output_to_analyze.functions.size()) {
    auto func = output_to_analyze.functions[i];
    this->analyze_function(func);
  }


  // Pass 4: Lower Structs and Enums
}

void
Analyzer::analyze_function(const Syntax_Function *function) {
  auto stmts = function->body.statements;

  // Functions are defined in the global scope so this works
  scope_that_is_being_analyzed = new Scope(global_scope);


  // Analyze each statement's value, and then proceed if it is semantically valid.
  FOR(i, stmts.size()) {
    auto stmt = stmts[i];
    this->analyze_syntax_node(stmt);
  }
}


void
Analyzer::analyze_syntax_node(const Syntax_Node *node) {
  switch (node->kind) {
    case Literal_Value: {
      break;
    }
    case Binary:
      break;
    case Unary:
      break;
    case List:
      break;
    case If: {
      scope_that_is_being_analyzed = new Scope(scope_that_is_being_analyzed);
      auto if_stmt = (Syntax_If*) node;

      auto cond_type = resolve_type(if_stmt->condition);

      // Condition is expression is not a boolean, then we add an error
      if (*cond_type != *TY_BOOL) {
        // TODO: Add Error Here
      }
      break;
    }
    case Else:
      break;
    case Elif:
      break;
    case For:
      break;
    case While:
      break;
    case Function_Call:
      break;
    case Struct:
      break;
    case ForList:
      break;
    case ForRange:
      break;
    case Enum:
      break;
    case Variable_Assignment:
      this->register_variable((const Syntax_Variable_Assignment*)(node));
      break;
    case Variable_Value_Assignment:
      break;
    case Variable_Reassignment:
      break;
    case Array_Initialization:
      break;
    case Assert:
      break;
    case Function_Default:
      break;
    case Function_Struct_Property:
      break;
    case Match:
      break;
    case Path_Node:
      break;
    case Path_Struct:
      break;
    case Struct_Init:
      break;
  }
}

void
Analyzer::analyze_struct(const Syntax_Struct *structure) {

}

void
Analyzer::analyze_enum(const Syntax_Enum *enumeration) {

}

const Type *
Analyzer::resolve_type(const Token *literal) {
  switch (literal->type()) {
    case Integer: return TY_I32;
    case String:  return TY_STR;
    case Decimal: return TY_F32;
    case Boolean: return TY_BOOL;
    case Char:    return TY_I8;
    case Keyword_True:  return TY_BOOL;
    case Keyword_False: return TY_BOOL;
    case Identifier: {
      auto var = scope_that_is_being_analyzed->variables.find(literal->name());
      if (var == scope_that_is_being_analyzed->variables.end()) {
        // TODO: Add Variable not found error here
      }
      return var->second.type;
    }
    default: UNREACHABLE;
  }
  UNREACHABLE;
  return TY_VOID;
}

const Type *
Analyzer::resolve_type(const Syntax_Type *type) {
  if (type->has_modifier()) {
    auto mod = type->modifier.unwrap();
    if (mod == Syntax_Type_Annotation::Pointer) {
      return (Type*) new Pointer_Type(resolve_type(type->type));
    }
    else if (mod == Syntax_Type_Annotation::ArrayType) {
      return (Type*) new Array_View_Type(resolve_type(type->type));
    }
  }
  else if (type->has_typename()) {
    return this->resolve_type(type->ty_name.unwrap());
  }
  UNREACHABLE;
  return nullptr;
}

const Type *
Analyzer::resolve_type(Syntax_Typename name) {
  switch (name.kind) {
    case Integer8:    return TY_I8;
    case Integer16:   return TY_I16;
    case Integer32:   return TY_I32;
    case Integer64:   return TY_I64;
    case UInteger8:   return TY_U8;
    case UInteger16:  return TY_U16;
    case UInteger32:  return TY_U32;
    case UInteger64:  return TY_U64;
    case Float32:     return TY_F32;
    case Float64:     return TY_F64;
    case String_Value:return TY_STR;
    case Boxed:       return new Type(Type_Tag::Smart_Pointer, nullptr);
    case User_Defined:return new Struct_Type(name.user_defined_typename.unwrap());
  }
  UNREACHABLE;
  return nullptr;
}

const Type *
Analyzer::resolve_type(const Syntax_Node *type) {
  switch (type->kind) {
    case Literal_Value:
      return this->resolve_type(((Syntax_Literal*)(type))->literal);
    case Binary:
      return this->validate_binary_expr(((Syntax_Binary*)(type)));
    case Unary:
      return this->validate_unary_expr((Syntax_Unary*)type);
    case List:
      return this->validate_list_expr((Syntax_List*) type);
    case If:
      return TY_VOID;
    case Else:
      return TY_VOID;
    case Elif:
      return TY_VOID;
    case For:
      return TY_VOID;
    case While:
      return TY_VOID;
    case Function_Call: {
      auto call = (Syntax_Function_Call*) type;
      auto sig_find = function_signatures.find(call->function_name->name());
      if (sig_find == function_signatures.end()) {
        // TODO: ERROR: Function has not been declared
      }
      return sig_find->second.return_type;
    }
    case Struct:
      return TY_VOID;
    case ForList:
      return TY_VOID;
    case ForRange:
      return TY_VOID;
    case Enum:
      return TY_VOID;
    case Variable_Assignment:
      return TY_VOID;
    case Variable_Value_Assignment:
      return TY_VOID;
    case Variable_Reassignment:
      return TY_VOID;
    case Array_Initialization:
      break;
    case Assert:
      break;
    case Function_Default:
      UNREACHABLE;
      break;
    case Function_Struct_Property:
      UNREACHABLE;
      break;
    case Match:
      return TY_VOID;
    case Path_Node:
      break;
    case Path_Struct:
      break;
    case Struct_Init:
      break;
    case Return_Value:
      return TY_VOID;
  }
  return nullptr;
}

const Type *
Analyzer::get_expected_type_from_op(Syntax_Binary_Op op) {
  switch (op) {
    case Modulus:           return TY_I32;
    case Greater:           return TY_BOOL;
    case Less:              return TY_BOOL;
    case Greater_Equal_To:  return TY_BOOL;
    case Less_Equal_To:     return TY_BOOL;
    case Boolean_Or:        return TY_BOOL;
    case Boolean_And:       return TY_BOOL;
    case Binary_ShLeft:     return TY_I32;
    case Binary_ShRight:    return TY_I32;
    case Binary_XOR:        return TY_I32;
    case Binary_AND:        return TY_I32;
    case Binary_OR:         return TY_I32;
    default: return nullptr;
  }
}

const Type *
Analyzer::get_expected_type_from_op(Syntax_Unary_Op op) {
  return nullptr;
}

const Type *
Analyzer::validate_binary_expr(const Syntax_Binary *expr) {
  auto checked_ty = this->get_expected_type_from_op(expr->operation);
  if (!checked_ty) {
    // Ambiguous number case. This is either Floating-Point or Integer operation. To resolve this,
    // we must determine if a floating-point number is within this expression. If so, then this 
    // operation is floating-point (F32 default) otherwise it is integer (I32 default).
    auto lhs_ty = this->resolve_type(expr->lhs);
    auto rhs_ty = this->resolve_type(expr->rhs);
    if (*lhs_ty != *TY_BOOL && *lhs_ty != *TY_F32) {
      // TODO: Type Error Case
    }
    if (*rhs_ty != *TY_BOOL && *rhs_ty != *TY_F32) {
      // TODO: Type Error Case
    }
    if (*lhs_ty == *TY_F32)
      checked_ty = TY_F32;
    else
      checked_ty = TY_I32;
  }
  return checked_ty;
}

const Type *
Analyzer::validate_list_expr(const Syntax_List *list) {
  auto list_ty = this->resolve_type(list->values[0]);
  for (const Syntax_Node *node : list->values) {
    if (*this->resolve_type(node) != *list_ty) {
      // TODO: Add Inconsistent List Type Error
    }
  }
  return list_ty;
}


const Type *
Analyzer::validate_unary_expr(const Syntax_Unary *unary) {
  return nullptr;
}

void
Analyzer::register_function(const Syntax_Function *function) {
  auto func_sig = AST::Func_Sig();
  auto begin = (const Type*) (mem::GLOBAL_OBJECT_ALLOC.current());

  // Resolve Parameter types
  for (auto param : function->parameters) {
    this->resolve_type(param.type);
  }
  func_sig.parameter_types = std::vector<Type>(begin, (const Type*) new char('a'));

  if (function->function_type.is_some()) {
    func_sig.return_type = resolve_type(function->function_type.unwrap());
  }
  else {
    // We have to infer the function's return type. This can be done by using the type of value
    // from the first return to infer the function type and confirm that all other returns are of
    // the same type.

    if (function->body.returns.size() == 0) {
      // The function never returns, so designate it as void.
      func_sig.return_type = new Type(Type_Tag::Void, nullptr);
    }
    else {
      func_sig.return_type = resolve_type(function->body.returns[0].value);
    }
  }
  // Check whether all the return values are the same type as the function
  for (const auto &ret : function->body.returns) {
    auto ret_ty = this->resolve_type(ret.value);
    if (ret_ty not_eq func_sig.return_type) {
      // TODO: Add Invalid Return Type Error
    }
  }
  this->function_signatures[function->function_name->name()] = func_sig;
}

void
Analyzer::register_variable(const Syntax_Variable_Assignment *var) {
  auto var_name = var->name->name();
  auto should_be_invalid = scope_that_is_being_analyzed->variables.find(var_name);
  if (should_be_invalid != scope_that_is_being_analyzed->variables.end()) {
    // TODO: Variable is being redeclared error
  }
  scope_that_is_being_analyzed->variables[var_name] = Variable(this->resolve_type(var->expression));
}
