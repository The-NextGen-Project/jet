#include "c-codegen.h"

using namespace nextgen::jet;

void
C_Codegen::gen_struct() {
  auto structs = this->analyzer.structs;

  auto gen_field = [&](AST::Struct_Member member) {
    this->gen_type(member.type);
    this->writer.appendf(" {};\n", member.name->name());
  };

  for (auto const & [name, struct_node] : structs) {
    this->writer.appendf("struct {} {}", name, "{");
    std::for_each(struct_node.members.begin(), struct_node.members.end(), gen_field);
    this->writer.appendf("{}\n", "}");
  }
}

void
C_Codegen::gen_enum() {
  auto enums = this->analyzer.enums;

}

void
C_Codegen::gen_func() {
  auto funcs = this->analyzer.functions;

  auto gen_param = [&](AST::Func_Param param) {
    this->gen_type(&param.type);
    this->writer.appendf("{}", param.param_name->name());
  };

  for (auto [name, func_node] : funcs) {
    auto function_signature = this->analyzer.function_signatures.find(name)->second;

    // Gen Proto
    this->gen_type(function_signature.return_type);
    this->writer.appendf("{}(", name);
    std::for_each(func_node.parameters.begin(), func_node.parameters.end(), gen_param);
    this->writer.appendf(") {}", "{");

    // Gen Body
    for (auto node : func_node.statements) {
      this->gen_node(node);
    }
    this->writer.appendf("{}\n", "}");
  }
}

void
C_Codegen::gen_node(const AST_Node &node) {
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
    case Structure:
      break;
    case Function_Node:
      break;
    case Match_Node:
      break;
  }
}

void
C_Codegen::gen_type(const Type *type) {
  switch (type->tag) {
    case Void: this->writer.appendf("void"); break;
    case I8:   this->writer.appendf("i8"); break;
    case I16:  this->writer.appendf("i16"); break;
    case I32:  this->writer.appendf("i32"); break;
    case I64:  this->writer.appendf("i64"); break;
    case U8:   this->writer.appendf("u8"); break;
    case U16:  this->writer.appendf("u16"); break;
    case U32:  this->writer.appendf("u32"); break;
    case U64:  this->writer.appendf("u64"); break;
    case F32:  this->writer.appendf("f32"); break;
    case F64:  this->writer.appendf("f64"); break;
    case Str:  this->writer.appendf("str"); break;
    case Bool: this->writer.appendf("bool"); break;
    case Pointer_Ty:
      this->gen_type(CAST(Pointer_Type*, type)->pointer_of);
      this->writer.appendf("*");
      break;
    case Array_View: // TODO: Fix pointer in the name issue for array views
      this->writer.appendf("$array_view__");
      this->gen_type(CAST(Array_View_Type*, type)->array_of);
      break;
    case Smart_Pointer: // TODO: Add this
      break;
    case User_Defined_Type:
      break;
  }
}
