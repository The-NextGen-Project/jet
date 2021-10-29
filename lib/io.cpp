# include <nextgen/io.h>


using namespace nextgen::io;


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

FileBuf *nextgen::io::create_file_buffer(const char *FILE, FileID id) {
  auto read = std::ifstream(FILE, std::ios::binary);
  auto buf  = read.rdbuf();
  auto size = read.tellg();

  auto block = new char[size];
  buf->sgetn(block, size);

  Console::Log("DSD: ", block, "\n");

  return new FileBuf {block, id };
}
