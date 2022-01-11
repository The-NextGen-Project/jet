# ifndef NEXTGEN_IO_H
# define NEXTGEN_IO_H
# include "allocator.h"

namespace nextgen::io {

  enum FileID {
    Jet_Source,
    C_Source,
    DynamicLib_Windows,
    DynamicLib_Mac,
    DynamicLib_Linux,
    StaticLib_Windows,
    StaticLib_Mac,
    StaticLib_Linux
  };

  struct File_Buf {
    char *file_buffer;
    FileID id;
  };


  static auto read_file(const char *FILE, FileID id) {
    std::ifstream in(FILE, std::ios::binary);

    (void) in.seekg(0, std::ios::end);
    auto len = in.tellg();
    (void) in.seekg(0, std::ios::beg);

    char *buffer = new char[static_cast<size_type>(len)];
    (void) in.rdbuf()->sgetn(buffer, len);
    buffer[len] = '\0';


    return new File_Buf {
      const_cast<char *>(buffer),
      id
    };
  }


  static auto output_file(const char *FILE) {
    return fmt::output_file(FILE);
  }


} // namespace nextgen::io




# endif //NEXTGEN_IO_H
