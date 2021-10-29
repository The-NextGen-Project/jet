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
  std::ifstream in(FILE, std::ios::binary);
  std::string contents((std::istreambuf_iterator<char>(in)),
                       std::istreambuf_iterator<char>());

  return new FileBuf {const_cast<char *>(contents.c_str()), id };
}
