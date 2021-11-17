#ifndef JET_C_CODEGEN_H
#define JET_C_CODEGEN_H

#include "analyzer.h"

namespace nextgen { namespace jet {

  class C_Codegen {
    Analyzer analyzer;
    fmt::ostream file = fmt::output_file("program.c");
    nextgen::string_buf writer = nextgen::string_buf(10);
  public: // Constructors
    void gen_func();
    void gen_struct();
    void gen_enum();
    void gen_node(const AST_Node &node);
    void gen_binary(const AST::Binary &binary);
    void gen_unary(const AST::Unary &unary);
    void gen_if(const AST::If &if_node);
    void gen_while(const AST::While &while_node);
    void gen_for_list();
    void gen_type(const Type *type);
  };

}}


#endif //JET_C_CODEGEN_H
