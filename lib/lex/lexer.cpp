# include <nextgen/jet/lex/lexer.h>

using namespace nextgen::jet;

// Peek `n` number of characters in the file buffer.
void Lexer::peek(size_t n) {
  this->pos += n;
  this->c = *(this->pos);
}

// Get current character that the lexer is at.
char Lexer::curr() const {
  return this->c;
}

auto Lexer::lex() -> Result<List<Token>, LexError> {
  auto tokens = List<Token> {};
  do {

  } while (this->clz != CharacterClass::Error);

  return Ok<List<Token>, LexError>(tokens);
}


