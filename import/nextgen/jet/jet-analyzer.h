#ifndef JET_JET_ANALYZER_H
#define JET_JET_ANALYZER_H

#include "jet-parser.h"
#include "jet-ast.h"

namespace nextgen { namespace jet {


  class Analyzer {
    VariableMap global_variables;
    FunctionMap functions;
  public:
    void analyze(const ParserOutput output_to_analyze);
    void analyze_function(const SyntaxFunction *function);
    void analyze_syntax_node(const SyntaxNode *node);
  };
}}



#endif //JET_JET_ANALYZER_H
