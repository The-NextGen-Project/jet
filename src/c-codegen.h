#ifndef JET_C_CODEGEN_H
#define JET_C_CODEGEN_H

#include "analyzer.h"

namespace nextgen::jet {

  struct C_Codegen {
    Analyzer analyzer;
    nextgen::string_buf writer = nextgen::string_buf(10);
  };

}


#endif //JET_C_CODEGEN_H
