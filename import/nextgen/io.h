# ifndef NEXTGEN_IO_H
# define NEXTGEN_IO_H
# include "allocator.h"

namespace nextgen { namespace io {

  enum FileID {
    JetSourceCode,
    CSourceCode,
    DynamicLib_Windows,
    DynamicLib_Mac,
    DynamicLib_Linux,
    StaticLib_Windows,
    StaticLib_Mac,
    StaticLib_Linux
  };


  class FileBuf {
  public:
    using Files = mem::ArenaVec<FileBuf>;

    FileBuf(char *buffer, FileID id) : buffer(buffer), id(id) {}

    FileID getFileID() const {
      return id;
    }

    char *getFileBuffer() const {
      return buffer;
    }


    // Given a list of files, output the type of file given. For example,
    // given a list of source code for Jet, and set it to output a binary
    // ELF, it will output an ELF binary for the given source input.
    //
    // Example:
    // FileBuf files[] = ...
    // FileBuf::Output(files, FileID::CSourceCode);
    //
    // NOTE: The types of files need to be able to be linked together to form
    // the resulting FileID file type.
    static void Output(Files files, FileID output);
  private:
    char *buffer; // Buffer pointer to the source text of a file
    FileID id; // File Type to know how file should be read
  };

  class FileWriter {

  };


  FileBuf CreateFileBuffer(const char *FILE, FileID id);



}} // namespace nextgen::io




# endif //NEXTGEN_IO_H
