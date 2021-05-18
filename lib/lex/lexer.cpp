# include <nextgen/jet/lex/lexer.h>

using namespace nextgen::jet;

// Peek `n` number of characters in the file buffer.
void Lexer::Peek(size_t n) {
  this->pos += n;
  this->c = *(this->pos);
}

// Get current character that the lexer is at.
char Lexer::Curr() const {
  return this->c;
}

Result<nextgen::mem::list<Token>, LexError> Lexer::Lex() {
  return Ok<nextgen::mem::list<Token>, LexError>(mem::list<Token>{});
}


