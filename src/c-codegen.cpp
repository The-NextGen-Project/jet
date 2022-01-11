#include "c-codegen.h"

using namespace nextgen::jet;

static void
gen_type(C_Codegen &CG, const Type *type) {
  switch (type->tag) {
    case Void: CG.writer.appendf("void"); break;
    case I8:   CG.writer.appendf("i8"); break;
    case I16:  CG.writer.appendf("i16"); break;
    case I32:  CG.writer.appendf("i32"); break;
    case I64:  CG.writer.appendf("i64"); break;
    case U8:   CG.writer.appendf("u8"); break;
    case U16:  CG.writer.appendf("u16"); break;
    case U32:  CG.writer.appendf("u32"); break;
    case U64:  CG.writer.appendf("u64"); break;
    case F32:  CG.writer.appendf("f32"); break;
    case F64:  CG.writer.appendf("f64"); break;
    case Str:  CG.writer.appendf("str"); break;
    case Bool: CG.writer.appendf("bool"); break;
    case Pointer_Ty:
      gen_type(CG, static_cast<const Pointer_Type*>(type)->pointer_of);
      CG.writer.appendf("*");
      break;
    case Array_View: // TODO: Fix pointer in the name issue for array views
      CG.writer.appendf("$array_view__");
      gen_type(CG, CAST(const Array_View_Type*, type)->array_of);
      break;
    case Smart_Pointer: // TODO: Add this
      break;
    case User_Defined_Type:
      CG.writer.appendf("{}", static_cast<const Struct_Type*>(type)->name);
      break;
  }
}


static void 
gen_struct(C_Codegen &CG) {
  auto structs = CG.analyzer.structs;

  auto gen_field = [&](AST::Struct_Member member) {
    gen_type(CG, member.type);
    CG.writer.appendf(" {};\n", member.name->name().data());
  };

  for (auto const & [name, struct_node] : structs) {
    CG.writer.appendf("struct {} {}", name, "{");
    (void) std::for_each(struct_node.members.begin(), struct_node.members.end(), gen_field);
    CG.writer.appendf("{}\n", "}");
  }
}


static void
gen_node(C_Codegen &CG, const AST_Node &node) {
  switch(node.kind) {
    case Binary_Expr_Node:
      break;
    case Literal_Expr_Node:
      break;
    case If_Node:
      break;
    case Else_Node:
      break;
    case Elif_Node:
      break;
    case While_Node:
      break;
    case ForList_Node:
      break;
    case ForRange_Node:
      break;
    case Match_Node:
      break;
    default:
      UNREACHABLE;
      break;
  }
}

static void
gen_func(C_Codegen &CG) {
  auto funcs = CG.analyzer.functions;

  auto gen_param = [&](AST::Func_Param param) {
    gen_type(CG, &param.type);
    CG.writer.appendf("{}", param.param_name->name());
  };

  for (auto [name, func_node] : funcs) {
    auto function_signature = CG.analyzer.function_signatures.find(name)->second;

    // Gen Proto
    gen_type(CG, function_signature.return_type);
    CG.writer.appendf("{}(", name);
    (void) std::for_each(func_node.parameters.begin(), func_node.parameters.end(), gen_param);
    CG.writer.appendf(") {}", "{");

    // Gen Body
    for (auto node : func_node.statements) {
      gen_node(CG, node);
    }
    CG.writer.appendf("{}\n", "}");
  }
}
