# include <nextgen/Support/IO.h>


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
void FileBuf::Output(FileBuf *files, FileID output) {

}

FileBuf nextgen::io::CreateFileBuffer(const char *FILE, FileID ID) {
  auto read = std::ifstream(FILE, std::ios::binary);
  auto buf  = read.rdbuf();
  auto size = read.tellg();

  auto block = static_cast<char*>(nextgen::mem::os::malloc(size));
  buf->sgetn(block, size);

  return { block, ID };
}
