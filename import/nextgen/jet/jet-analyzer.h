#ifndef JET_JET_ANALYZER_H
#define JET_JET_ANALYZER_H

#include "jet-parser.h"
#include "jet-ast.h"

namespace nextgen { namespace jet {


  class Analyzer {
    Map<Variable> global_variables;
    Map<NodeFunction> functions;
    Map<NodeStruct> structs;
    Map<NodeEnum> enums;
    Map<FuncSig> function_signatures;

    Scope *global_scope = new Scope(nullptr);
    Scope *scope_that_is_being_analyzed = nullptr;
  public:
    void analyze(const ParserOutput output_to_analyze);
    void analyze_function(const SyntaxFunction *function);
    void analyze_struct(const SyntaxStruct *structure);
    void analyze_enum(const SyntaxEnum *enumeration);
    void analyze_syntax_node(const SyntaxNode *node);

    void register_function(const SyntaxFunction *function);

    const Type *resolve_type(const Token *value);
    const Type *resolve_type(const SyntaxType *type);
    const Type *resolve_type(const SyntaxNode *value);
    const Type *resolve_type(SyntaxTypename name);
  };
}}



#endif //JET_JET_ANALYZER_H
