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
    case VariableAssignment: {
      break;
    }
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

void Analyzer::analyze_struct(const SyntaxStruct *structure) {

}

void Analyzer::analyze_enum(const SyntaxEnum *enumeration) {

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
}

const Type *
Analyzer::resolve_type(const SyntaxNode *type) {
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