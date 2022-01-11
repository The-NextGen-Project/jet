#ifndef JET_ANALYZER_H
#define JET_ANALYZER_H

#include "parser.h"
#include "types/analyzer-types.h"

namespace nextgen::jet {

  struct Analyzer {
    Map<Variable>       global_variables;
    Map<AST::Func>      functions;
    Map<AST::Struct>    structs;
    Map<AST::Enum>      enums;
    Map<AST::Func_Sig>  function_signatures;

    Scope *global_scope = new Scope(nullptr);
    Scope *scope_that_is_being_analyzed = global_scope;
  };
}


#endif //JET_ANALYZER_H