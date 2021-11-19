#include "str.h"

std::ostream &nextgen::operator<<(std::ostream &os, const string_buf &buf) {
  os.write(buf, std::streamsize(buf.size()));
  return os;
}
