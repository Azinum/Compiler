// lexer.h

#ifndef _LEXER_H
#define _LEXER_H

#define array_size(arr) ((sizeof(arr)) / (sizeof(arr[0])))

struct Parser;

enum TokenTypes {
  Token_unknown = 0,

  Token_colon,
  Token_semicolon,
  Token_comma,

  Token_openparen,
  Token_closeparen,
  Token_openbrace,    // {
  Token_closebrace,   // }
  Token_openbracket,  // [
  Token_closebracket, // ]

  Token_plus,
  Token_minus,
  Token_mult,
  Token_mod,
  Token_div,
  Token_equals,

  Token_string,
  Token_identifier,
  Token_number,
  Token_type,
  Token_variable,
  Token_function,
  Token_return,
  Token_extern,
  Token_inline_asm,

  Token_EOF
};

static const char* lexerTokenTypes[] = {
  "Unknown",

  ":",
  ";",
  ",",

  "(",
  ")",
  "{",
  "}",
  "[",
  "]",

  "+",
  "-",
  "*",
  "%",
  "/",
  "="

  "String",
  "Identifier",
  "Number",
  "Type",
  "Variable",
  "Function",
  "Return",
  "Extern",
  "Inline assembly",

  "EOF",
};

static const char* registerNames[] = {
  "eax",
  "ecx",
  "edx",
  "esi",
  "edi",
  "ebx"
};

struct Type {
  int size;
};

struct Token {
  int type;
  int stringLength;
  char* string;
};

struct Variable {
  struct Token name;
  int size;   // Default: 4 (integer)
  int offsetFromBasePointer;  // -var_count*type_size(%ebp)
  int isUsed;
  int line;
};

struct Function {
  struct Token name;
  int variableCount;
  struct Variable variables[12];
};

struct Lexer {
  char* index;
  int currentLine;
  struct Token token;
  struct Function func; // Current function
  int isLocal;  // Are we inside a function scope?
  struct Parser* parser;
};

struct Token lexerGetToken(struct Lexer* lexer);

int lexerTokenEquals(struct Token token, char* toMatch);

int lexerRequireToken(struct Lexer* lexer, int type);

int lexerRequireCurrentToken(struct Lexer* lexer, int type);

#endif