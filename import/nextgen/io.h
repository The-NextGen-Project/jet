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

  struct FileBuf {
    using Files = mem::ArenaVec<FileBuf>;
  public:
    char *file_buffer;
    FileID id;
    static void Output(Files files, FileID output);
  };


  struct File_Writer {
    const char *file_name;
    std::ofstream file;
  public:
    explicit File_Writer(const char *file) : file_name(file) {
      this->file.open(file_name);
    }

    template<typename ... Args>
    void write(Args&& ... args) {
      write(args...);
    }

    template<typename T>
    void write(T value) {
      file << value;
    }

    void close() {
      file.close();
    }

  };


  FileBuf *create_file_buffer(const char *FILE, FileID id);



}} // namespace nextgen::io




# endif //NEXTGEN_IO_H
