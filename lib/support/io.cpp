# include <nextgen/support/io.h>

// Reads a file that is given and returns an allocated block (char*) that is
// owned by the caller. To make this faster, we restrain from using any IO
// that is C-related.
char *get_file_buf(const char *FILE) {
  auto read = std::ifstream(FILE, std::ios::binary);
  auto buf  = read.rdbuf();
  auto size = read.tellg();

  auto block = static_cast<char*>(nextgen::mem::os::malloc(size));
  buf->sgetn(block, size);

  return block;
}
