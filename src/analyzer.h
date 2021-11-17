#ifndef JET_ANALYZER_H
#define JET_ANALYZER_H

#include "parser.h"
#include "types/analyzer-types.h"

namespace nextgen { namespace jet {

  struct Analyzer {
    Map<Variable>       global_variables;
    Map<AST::Func>      functions;
    Map<AST::Struct>    structs;
    Map<AST::Enum>      enums;
    Map<AST::Func_Sig>  function_signatures;

    Scope *global_scope = new Scope(nullptr);
    Scope *scope_that_is_being_analyzed = global_scope;
  public:
    void analyze(const Parse_Result output_to_analyze);
    void analyze_function(const Syntax_Function *function);
    void analyze_struct(const Syntax_Struct *structure);
    void analyze_enum(const Syntax_Enum *enumeration);
    void analyze_syntax_node(const Syntax_Node *node);

    void register_function(const Syntax_Function *function);
    void register_variable(const Syntax_Variable_Assignment *var);

    const Type *validate_binary_expr(const Syntax_Binary *expr);
    const Type *validate_list_expr(const Syntax_List *list);
    const Type *validate_unary_expr(const Syntax_Unary *unary);

    const Type *resolve_type(const Token *value);
    const Type *resolve_type(const Syntax_Type *type);
    const Type *resolve_type(const Syntax_Node *value);
    const Type *resolve_type(Syntax_Typename name);

    const Type *get_expected_type_from_op(Syntax_Binary_Op op);
    const Type *get_expected_type_from_op(Syntax_Unary_Op op);
  };
}}



#endif //JET_ANALYZER_H
