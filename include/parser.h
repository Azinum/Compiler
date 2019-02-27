// parser.h

#ifndef _PARSER_H
#define _PARSER_H

#include "lexer.h"

struct Expression {
  struct Token token;
  union {
    int i;
    double d;
    float f;
  } value;
  int type;
};

struct Parser {
  struct Lexer* lexer;
  int error;
  FILE* out;
};

int parse(int argc, char** argv);

#endif