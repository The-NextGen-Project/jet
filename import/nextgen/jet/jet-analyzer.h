#ifndef JET_JET_ANALYZER_H
#define JET_JET_ANALYZER_H

#include "jet-syntax-nodes.h"
#include "../allocator.h"

namespace nextgen { namespace jet {

  class Analyzer {
    ObjectVector<SyntaxFunction> functions_to_analyze;
  };
}}



#endif //JET_JET_ANALYZER_H
