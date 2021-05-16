# ifndef NEXTGEN_IO_H
# define NEXTGEN_IO_H
# include "core.h"
# include "allocator.h"

namespace nextgen { namespace io {

  enum FileID {
    JetSourceCode,
    CSourceCode,
    LLVM_IR,
    LLVM_BC,
    Mach_O,
    ELF,
    PE
  };

  class FileBuf {
  public:
    using Files = FileBuf[];

    FileBuf(char *buffer, FileID id) : buffer(buffer), id(id) {}

    FileID getFileID() const {
      return id;
    }

    char *getFileBuffer() const {
      return buffer;
    }


    // Compile a file buffer to a format specified by FileID.
    void compile();

    // Given a list of files, output the type of file given. For example,
    // given a list of source code for Jet, and set it to output a binary
    // ELF, it will output an ELF binary for the given source input.
    //
    // Example:
    // FileBuf files[] = ...
    // FileBuf::Output(files, FiledID::LLVM_IR);
    //
    // NOTE: The types of files need to be able to be linked together to form
    // the resulting FileID file type.
    static void Output(Files files, FileID output);
  private:
    char *buffer; // Buffer pointer to the source text of a file
    FileID id; // File Type to know how file should be read
  };


  FileBuf get_file_buf(const char *FILE, FileID id);


}} // namespace nextgen::io




# endif //NEXTGEN_IO_H
