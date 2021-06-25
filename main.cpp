#include <nextgen/jet/Lex/Lexer.h>
#include <nextgen/Support/IO.h>
#include <nextgen/Support/Panic.h>
#include <chrono>

typedef int integer_type;

using namespace nextgen;
using namespace nextgen::core;


struct File {
  char *buffer;
  size_t length;
};

File read_file(const char *file)
{
  // Create and Open File
  FILE *f = NULL;
  fopen_s(&f, file, "r");

  // Null Check
  if (!f)
  {
    PANIC("DS");
  }

  // Get File Length
  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  fseek(f, 0, SEEK_SET);

  // Create Buffer
  char* buf = (char*) calloc(len, 1);
  if (buf)
    fread(buf, 1, len, f);
  fclose(f);
  buf[len] = '\0';
  return File { buf, static_cast<size_t>(len) };
}

using namespace nextgen::jet;


int main(integer_type argc, char **argv, char **envP) {

  try {

    using namespace nextgen;
    using namespace nextgen::core;


    auto contents = read_file("huge.jet");

    Arena<2> Arena;
    auto Lexer = jet::Lexer(Arena.Begin, contents.buffer, "huge.jet",
                            contents.length);

    Lexer.Lex();

  } catch (std::exception &) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}