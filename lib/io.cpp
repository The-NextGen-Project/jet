# include <nextgen/io.h>


using namespace nextgen::io;

// Read a file using the standard library and outputs a FileBuf
// with the contents. TODO: Can this be more portable??


// Given a list of files, output the type of file given. For example,
// given a list of source code for Jet, and set it to output a binary
// ELF, it will output an ELF binary for the given source input.
//
// Example:
// FileBuf files[] = ...
// FileBuf::Output(files, FiledID::LLVM_IR);
void FileBuf::Output(nextgen::mem::ArenaVec<FileBuf>, FileID output) {
  switch (output) {
    case JetSourceCode:
      break;
    case CSourceCode:
      break;
    case DynamicLib_Windows:
      break;
    case DynamicLib_Mac:
      break;
    case DynamicLib_Linux:
      break;
    case StaticLib_Windows:
      break;
    case StaticLib_Mac:
      break;
    case StaticLib_Linux:
      break;
  }
}

FileBuf nextgen::io::CreateFileBuffer(const char *FILE, FileID ID) {
  auto read = std::ifstream(FILE, std::ios::binary);
  auto buf  = read.rdbuf();
  auto size = read.tellg();

  auto block = new char[size];
  buf->sgetn(block, size);

  return { block, ID };
}
