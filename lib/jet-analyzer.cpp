#include <nextgen/jet/jet-analyzer.h>

using namespace nextgen::jet;


void
Analyzer::analyze(const ParserOutput output_to_analyze) {

  // Pass 1: Register Global Variables
  FOR(i, output_to_analyze.global_variables.size()) {
    auto var = output_to_analyze.global_variables[i];
    if (var->type.isNone()) {
      // TODO: Add error that global variables must have a type.
    }
    global_scope->variables[var->name->name()] = Variable(var->name,
                                                          resolve_type
                                                          (var->type.unwrap()));
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
Analyzer::analyze_function(const SyntaxFunction *function) {
  auto stmts = function->body.statements;

  // Functions are defined in the global scope so this works
  scope_that_is_being_analyzed = new Scope(global_scope);


  // Analyze each statement's value, and then proceed if it is
  // semantically valid.
  FOR(i, stmts.size()) {
    auto stmt = stmts[i];
    this->analyze_syntax_node(stmt);
  }
}


void Analyzer::analyze_syntax_node(const SyntaxNode *node) {
  switch (node->kind) {
    case LiteralValue:
      break;
    case Binary:
      break;
    case Unary:
      break;
    case List:
      break;
    case If: {
      scope_that_is_being_analyzed = new Scope(scope_that_is_being_analyzed);
      auto if_stmt = (SyntaxIf*) node;

      auto cond_type = resolve_type(if_stmt->condition);

      // Condition is expression is not a boolean, then we add an error
      if (*cond_type != Type(TypeTag::Bool, nullptr)) {
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
    case FunctionCall:
      break;
    case Struct:
      break;
    case ForList:
      break;
    case ForRange:
      break;
    case Enum:
      break;
    case VariableAssignment:
      this->register_variable((const SyntaxVariableAssignment*)(node));
      break;
    case VariableValueAssignment:
      break;
    case VariableReassignment:
      break;
    case ArrayInitialization:
      break;
    case Assert:
      break;
    case FunctionDefault:
      break;
    case FunctionStructProperty:
      break;
    case Match:
      break;
    case PathNode:
      break;
    case PathStruct:
      break;
    case StructInit:
      break;
  }
}

void
Analyzer::analyze_struct(const SyntaxStruct *structure) {

}

void
Analyzer::analyze_enum(const SyntaxEnum *enumeration) {

}

const Type *
Analyzer::resolve_type(const Token *literal) {
  switch (literal->getKind()) {
    case Integer:
      return new Type(TypeTag::I32, nullptr);
    case String:
      return new Type(TypeTag::Str, nullptr);
    case Decimal:
      return new Type(TypeTag::F32, nullptr);
    case Boolean:
      return new Type(TypeTag::Bool, nullptr);
    case Identifier: {
        auto var = scope_that_is_being_analyzed->variables
          .find(literal->name());
        if (var == scope_that_is_being_analyzed->variables.end()) {
          // TODO: Add Variable not found error here
        }
        return var->second.type;
    }
    case Char:
      return new Type(TypeTag::I8, nullptr);
    default:
      // TODO: Add Error Here
      break;
  }
  UNREACHABLE;
}

const Type *
Analyzer::resolve_type(const SyntaxType *type) {
  if (type->has_modifier()) {
    auto mod = type->modifier.unwrap();
    if (mod == SyntaxTypeAnnotation::Pointer) {
      return (Type*) new PointerType(resolve_type(type->type));
    }
    else if (mod == SyntaxTypeAnnotation::ArrayType) {
      return (Type*) new ArrayViewType(resolve_type(type->type));
    }
  }
  else if (type->has_typename()) {
    return resolve_type(type->ty_name.unwrap());
  }
  UNREACHABLE;
  return nullptr;
}

const Type *
Analyzer::resolve_type(SyntaxTypename name) {
  switch (name.kind) {
    case Integer8:
      return new Type(TypeTag::I8, nullptr);
    case Integer16:
      return new Type(TypeTag::I16, nullptr);
    case Integer32:
      return new Type(TypeTag::I32, nullptr);
    case Integer64:
      return new Type(TypeTag::I64, nullptr);
    case UInteger8:
      return new Type(TypeTag::U8, nullptr);
    case UInteger16:
      return new Type(TypeTag::U16, nullptr);
    case UInteger32:
      return new Type(TypeTag::U32, nullptr);
    case UInteger64:
      return new Type(TypeTag::U64, nullptr);
    case Boxed:
      return new Type(TypeTag::SmartPointer, nullptr);
    case Float32:
      return new Type(TypeTag::F32, nullptr);
    case Float64:
      return new Type(TypeTag::F64, nullptr);
    case StringValue:
      return new Type(TypeTag::Str, nullptr);
    case UserDefined:
      return new StructType(name.user_defined_typename.unwrap());
  }
  UNREACHABLE;
  return nullptr;
}

const Type *
Analyzer::resolve_type(const SyntaxNode *type) {
  switch (type->kind) {
    case LiteralValue:
      return resolve_type(((SyntaxLiteral*)(type))->literal);
    case Binary:
      return validate_binary_expr(((SyntaxBinary*)(type)));
    case Unary:
      return validate_unary_expr((SyntaxUnary*)type);
    case List:
      return validate_list_expr((SyntaxList*) type);
    case If:
      return new Type(TypeTag::Void, nullptr);
    case Else:
      return new Type(TypeTag::Void, nullptr);
    case Elif:
      return new Type(TypeTag::Void, nullptr);
    case For:
      return new Type(TypeTag::Void, nullptr);
    case While:
      return new Type(TypeTag::Void, nullptr);
    case FunctionCall: {
      auto call = (SyntaxFunctionCall*) type;
      auto sig_find = function_signatures.find(call->function_name->name());
      if (sig_find == function_signatures.end()) {
        // TODO: Function has not been declared
      }
      return sig_find->second.return_type;
    }
    case Struct:
      return new Type(TypeTag::Void, nullptr);
    case ForList:
      return new Type(TypeTag::Void, nullptr);
    case ForRange:
      return new Type(TypeTag::Void, nullptr);
    case Enum:
      return new Type(TypeTag::Void, nullptr);
    case VariableAssignment:
      return new Type(TypeTag::Void, nullptr);
    case VariableValueAssignment:
      return new Type(TypeTag::Void, nullptr);
    case VariableReassignment:
      return new Type(TypeTag::Void, nullptr);
    case ArrayInitialization:
      break;
    case Assert:
      break;
    case FunctionDefault:
      UNREACHABLE;
      break;
    case FunctionStructProperty:
      UNREACHABLE;
      break;
    case Match:
      return new Type(TypeTag::Void, nullptr);
    case PathNode:
      break;
    case PathStruct:
      break;
    case StructInit:
      break;
    case ReturnValue:
      return new Type(TypeTag::Void, nullptr);
  }
  return nullptr;
}

const Type *
Analyzer::validate_binary_expr(const SyntaxBinary *expr) {
  return nullptr;
}

const Type *
Analyzer::validate_list_expr(const SyntaxList *list) {
  return nullptr;
}


const Type *
Analyzer::validate_unary_expr(const SyntaxUnary *unary) {
  return nullptr;
}

void
Analyzer::register_function(const SyntaxFunction *function) {
  FuncSig sig;
  sig.name = function->function_name->name();
  sig.parameter_types.begin = (const Type*)
    (mem::GLOBAL_OBJECT_ALLOC.current());


  FOR(i, function->parameters.len()) {
    auto param = function->parameters[i];
    sig.parameter_types.end = resolve_type(param->type);
  }

  if (function->function_type.isSome()) {
    sig.return_type = resolve_type(function->function_type.unwrap());

    // Check whether all the return values are the same type as the function
    // type.
    FOR(i, function->body.returns.size()) {
      auto ret = function->body.returns[i];
      auto ret_type = resolve_type(ret.value);

      if (ret_type != sig.return_type) {
        // TODO: Add Invalid Return Type Error
      }
    }
  }
  this->function_signatures[sig.name] = sig;
}

void
Analyzer::register_variable(const SyntaxVariableAssignment *var) {
  auto var_name = var->name;

  auto should_be_invalid = scope_that_is_being_analyzed->variables.find
    (var_name->name());

  if (should_be_invalid != scope_that_is_being_analyzed->variables.end()) {
    // TODO: Variable is being redeclared error
  }

  scope_that_is_being_analyzed->variables.insert(
  { var_name->name(),
    Variable(var_name, resolve_type(var->expression))
  });
}


