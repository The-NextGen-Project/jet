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


const Type *resolve_type(Analyzer &A, const Syntax_Node *);
void analyze_node(Analyzer &A, const Syntax_Node *);

static const Type *
get_expected_type_from_op(Syntax_Binary_Op op) {
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

// TODO: This likely needs a reference to the value that it is being performed on to determine accurately the type
//  that is created.
static const Type *
get_expected_type_from_op(Syntax_Unary_Op op) {
  switch (op) {
    case Bitwise_Not:
      break;
    case Positive:
      break;
    case Negative:
      break;
    case Conditional_Not:
      break;
    case Memory_Reference:
      break;
    case Inc:
      break;
    case Dec:
      break;
  }
  return nullptr;
}

static const Type *
resolve_type(const Token *lit) {
  switch (lit->type()) {
    case Integer: return TY_I32;
    case String:  return TY_STR;
    case Decimal: return TY_F64;
    case Keyword_True: case Keyword_False: return TY_BOOL;
    default: UNREACHABLE;
  }
  return nullptr;
}

static const Type *
resolve_type(const Syntax_Typename& name) {
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

static const Type *
resolve_type(Analyzer &A, const Syntax_Type *type) {
//  if (type->has_modifier()) {
//    auto mod = type->modifier.unwrap();
//    if (mod is Syntax_Type_Annotation::Pointer) {
//      return new Pointer_Type(resolve_type(A, type->type));
//    }
//    else if (mod is Syntax_Type_Annotation::ArrayType) {
//      return new Array_View_Type(resolve_type(A,type->type));
//    }
//  }
//  else if (type->has_typename()) {
//    return resolve_type(type->ty_name.unwrap());
//  }
//  UNREACHABLE;
//  return nullptr;

  auto result_type = type;

  while (true) {
    if (result_type->has_modifier()) {
      auto modifier = result_type->modifier.unwrap();
      result_type = result_type->type;
      if (modifier is Pointer) {

      }
      else if (modifier is ArrayType) {

      }
    }
    else if (result_type->has_typename()) {
      return resolve_type(result_type->ty_name.unwrap());
    }
  }
  return nullptr;
}

static const Type *
resolve_expr(Analyzer &A, Syntax_Binary *expr) {
  auto checked_ty = get_expected_type_from_op(expr->operation);
  if (!checked_ty) {
    // Ambiguous number case. This is either Floating-Point or Integer operation. To resolve this,
    // we must determine if a floating-point number is within this expression. If so, then this
    // operation is floating-point (F32 default) otherwise it is integer (I32 default).
    auto lhs_ty = resolve_type(A, expr->lhs);
    auto rhs_ty = resolve_type(A, expr->rhs);
    if (*lhs_ty != *TY_BOOL && *lhs_ty != *TY_F32) {
      // TODO: Type Error Case
    }
    if (*rhs_ty != *TY_BOOL && *rhs_ty != *TY_F32) {
      // TODO: Type Error Case
    }
    if (*lhs_ty is *TY_F32)
      checked_ty = TY_F32;
    else
      checked_ty = TY_I32;
  }
  return checked_ty;
}

static const Type *
resolve_expr(Analyzer &A, Syntax_Unary *expr) {
  return nullptr;
}

static const Type *
resolve_expr(Analyzer &A, Syntax_List *expr) {
  return nullptr;
}


static const Type *
resolve_expr(Analyzer &A, Syntax_Struct_Instantiation *expr) {
  return nullptr;
}


static const Type *
resolve_expr(Analyzer &A, Syntax_Function_Call *expr) {
  auto sig_find = A.function_signatures.find(expr->function_name->name());
  if (sig_find is A.function_signatures.end()) {
    // TODO: ERROR: Function has not been declared
  }
  return sig_find->second.return_type;
}


const Type *
resolve_type(Analyzer &A, const Syntax_Node *type) {
  auto node = const_cast<Syntax_Node*>(type);
  switch (type->kind) {
    case Literal_Value: return resolve_type(reinterpret_cast<const Syntax_Literal*>(type)->literal);
    case Binary:        return resolve_expr(A, reinterpret_cast<Syntax_Binary*>(node));
    case Unary:         return resolve_expr(A, reinterpret_cast<Syntax_Unary*>(node));
    case List:          return resolve_expr(A, reinterpret_cast<Syntax_List*>(node));
    case Function_Call: return resolve_expr(A, reinterpret_cast<Syntax_Function_Call*>(node));
    case Struct_Init:   return resolve_expr(A, reinterpret_cast<Syntax_Struct_Instantiation*>(node));
    case Return_Value:  return resolve_type(A, reinterpret_cast<Return*>(node)->value);

    case If:
    case Else:
    case Elif:
    case For:
    case While:
    case Struct:
    case For_List:
    case For_Range:
    case Enum:
    case Variable_Assignment:
    case Variable_Value_Assignment:
    case Variable_Reassignment:
    case Match:
      return TY_VOID;
    default:
      UNREACHABLE;
      break;
  }
  return nullptr;
}

static void
register_function(Analyzer &A, const Syntax_Function *function) {
  auto func_sig = AST::Func_Sig();
  auto begin = (const Type*) (nextgen::mem::GLOBAL_OBJECT_ALLOC.current());

  // Resolve Parameter types
  for (auto param : function->parameters) {
    resolve_type(A, param.type);
  }
  func_sig.parameter_types = std::vector<Type>(begin, (const Type*) new char('a'));

  if (function->function_type.is_some()) {
    if (function->body.returns.empty()) {
      // TODO: Missing return value for function ERROR
    }

    func_sig.return_type = resolve_type(A, function->function_type.unwrap());
  }
  else {
    // We have to infer the function's return type. This can be done by using the type of value
    // from the first return to infer the function type and confirm that all other returns are of
    // the same type.

    if (function->body.returns.empty()) {
      // The function never returns, so designate it as void.
      func_sig.return_type = new Type(Type_Tag::Void, nullptr);
    }
    else {
      func_sig.return_type = resolve_type(A, function->body.returns[0].value);
    }
  }
  // Check whether all the return values are the same type as the function
  for (const auto &ret : function->body.returns) {
    auto ret_ty = resolve_type(A, ret.value);
    if (ret_ty != func_sig.return_type) {
      // TODO: Add Invalid Return Type Error
    }
  }
  A.function_signatures[function->function_name->name()] = func_sig;
}

static void
register_variable(Analyzer &A, const Syntax_Variable_Assignment *var) {
  auto variable_name = var->name->name();
  auto should_be_invalid = A.scope_that_is_being_analyzed->variables.find(variable_name);
  if (should_be_invalid != A.scope_that_is_being_analyzed->variables.end()) {
    // TODO: Variable is being redeclared error
  }
  A.scope_that_is_being_analyzed->variables[variable_name] = Variable(resolve_type(A, var->expression));
}

static void
analyze_block(Analyzer &A, Syntax_Block &block) {
  auto initial_scope = A.scope_that_is_being_analyzed;
  A.scope_that_is_being_analyzed = new Scope(initial_scope);
  for(const auto &node : block.statements) {
    analyze_node(A, node);
  }
  A.scope_that_is_being_analyzed = initial_scope;
}

static void
analyze_if(Analyzer &A, Syntax_If *if_expr) {
  auto condition_type = resolve_type(A, if_expr->condition);
  if (*condition_type != *TY_BOOL) {
    // TODO: Type Error Here
  }
  analyze_block(A, if_expr->body);
}

void
analyze_node(Analyzer &A, const Syntax_Node *node) {
  auto mut_node = const_cast<Syntax_Node*>(node);
  switch (node->kind) {
    case Literal_Value: break;
    case Binary:  (void) resolve_type(A, reinterpret_cast<Syntax_Binary*>(mut_node)); break;
    case Unary:   (void) resolve_type(A, reinterpret_cast<Syntax_Unary*>(mut_node)); break;
    case List:    (void) resolve_type(A, reinterpret_cast<Syntax_List*>(mut_node)); break;
    case If:      analyze_if(A, reinterpret_cast<Syntax_If*>(mut_node)); break;
    case For:
      break;
    case While:
      break;
    case Function_Call:
      break;
    case Struct:
      break;
    case For_List:
      break;
    case For_Range:
      break;
    case Enum:
      break;
    case Variable_Assignment:
      register_variable(A, (const Syntax_Variable_Assignment*)(node));
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
    default:
      UNREACHABLE;
      break;
  }
}


static void
analyze_function(Analyzer &A, const Syntax_Function *function) {
  auto stmts = function->body.statements;

  // Functions are defined in the global scope so this works
  A.scope_that_is_being_analyzed = new Scope(A.global_scope);


  // Analyze each statement's value, and then proceed if it is semantically valid.
  for (const auto &stmt : stmts) {
    analyze_node(A, stmt);
  }
}


static void
analyze(Analyzer &A, const Parse_Result &output_to_analyze) {


  // Pass 1: Register Global Variables
  for(const auto &var : output_to_analyze.global_variables) {
    if (var->type.is_none()) {
      // TODO: Add error that global variables must have a type.
    }
    A.global_scope->variables[var->name->name()] = Variable(resolve_type(A, var->type.unwrap()));
  }

  // Pass 2: Register All Function Signatures
  for(const auto &func : output_to_analyze.functions) {
    register_function(A, func);
  }

  // Pass 3: Analyze All Functions
  for(const auto &func : output_to_analyze.functions) {
    analyze_function(A, func);
  }


  // Pass 4: Lower Structs and Enums
}