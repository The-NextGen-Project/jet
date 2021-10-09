#include <nextgen/jet/jet-analyzer.h>

using namespace nextgen::jet;


void Analyzer::analyze(const ParserOutput output_to_analyze) {
  FOR(i, output_to_analyze.global_variables.size()) {
    auto var = output_to_analyze.global_variables[i];
    if (var->type.isNone()) {
      // TODO: Add error that global variables must have a type.
    }
    global_scope->variables[var->name->name()] = Variable(var->name,
                                                          resolve_type
                                                          (var->type.unwrap()));
  }
}

void Analyzer::analyze_function(const SyntaxFunction *function) {
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

const Type
Analyzer::resolve_type(const Token *literal) {
  switch (literal->getKind()) {
    case Integer:
      return Type(TypeTag::I32, nullptr);
    case String:
      return Type(TypeTag::Str, nullptr);
    case Decimal:
      return Type(TypeTag::F32, nullptr);
    case Boolean:
      return Type(TypeTag::Bool, nullptr);
    case Identifier: {
        auto var = scope_that_is_being_analyzed->variables
          .find(literal->name());
        if (var == scope_that_is_being_analyzed->variables.end()) {
          // TODO: Add Variable not found error here
        }
        return var->second.type;
    }
    case Char:
      return Type(TypeTag::I8, nullptr);
    default:
      // TODO: Add Error Here
      break;
  }
  UNREACHABLE;
}

const Type
Analyzer::resolve_type(const SyntaxType *type) {
  if (type->has_modifier()) {
    auto mod = type->modifier.unwrap();
    if (mod == SyntaxTypeAnnotation::Pointer) {
      return PointerType(resolve_type(type->type));
    }
    else if (mod == SyntaxTypeAnnotation::ArrayType) {
      return ArrayViewType(resolve_type(type->type));
    }
  }
  else if (type->has_typename()) {
    return resolve_type(type->ty_name.unwrap());
  }
  return Type();
}

const Type
Analyzer::resolve_type(SyntaxTypename name) {
  switch (name.kind) {
    case Integer8:
      return Type(TypeTag::I8, nullptr);
    case Integer16:
      return Type(TypeTag::I16, nullptr);
    case Integer32:
      return Type(TypeTag::I32, nullptr);
    case Integer64:
      return Type(TypeTag::I64, nullptr);
    case UInteger8:
      return Type(TypeTag::U8, nullptr);
    case UInteger16:
      return Type(TypeTag::U16, nullptr);
    case UInteger32:
      return Type(TypeTag::U32, nullptr);
    case UInteger64:
      return Type(TypeTag::U64, nullptr);
    case Boxed:
      return Type(TypeTag::SmartPointer, nullptr);
    case Float32:
      return Type(TypeTag::F32, nullptr);
    case Float64:
      return Type(TypeTag::F64, nullptr);
    case StringValue:
      return Type(TypeTag::Str, nullptr);
    case UserDefined:
      return StructType(name.user_defined_typename.unwrap());
  }
  UNREACHABLE;
}
