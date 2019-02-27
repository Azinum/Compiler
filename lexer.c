// lexer.c

#include <stdio.h>

#include "lexer.h"

static inline int isEndOfLine(char c) {
  return (c == '\n' || c == '\r');
}

static inline int isWhiteSpace(char c) {
  return (
    c == '\t' ||
    c == '\v' ||
    c == '\f' ||
    c == ' '
  );
}

static inline int isAlphabetical(char c) {
  return ((c >= 'a') && (c <= 'z')) ||
         ((c >= 'A') && (c <= 'Z'));
}

static inline int isNumber(char c) {
  return ((c >= '0') && (c <= '9'));
}

static void removeWhiteSpaces(struct Lexer* lexer) {
  while (1) {
    if (isWhiteSpace(lexer->index[0])) {
      lexer->index++;
    }
    else if (isEndOfLine(lexer->index[0])) {
      lexer->index++;
      lexer->currentLine++;
    }
    else if (lexer->index[0] == '/' && lexer->index[1] == '/') {  // Comment
      lexer->index += 2;
      while (lexer->index[0] && !isEndOfLine(lexer->index[0])) {
        lexer->index++;
      }
    }
    else if (lexer->index[0] == '#') {  // Skip macros
      lexer->index++;
      while (lexer->index[0] && !isEndOfLine(lexer->index[0])) {
        if (lexer->index[0] == '\\') {
          lexer->index++;
        }
        lexer->index++;
      }
    }
    else if (lexer->index[0] == '/' && lexer->index[1] == '*') {  // Multi-line comment
      lexer->index += 2;
      while ((lexer->index[0] && !(lexer->index[0] == '*' && lexer->index[1] == '/'))) {
        if (isEndOfLine(lexer->index[0])) {
          lexer->currentLine++;
        }
        lexer->index++;
      }
      if (lexer->index[0] == '*') {
        lexer->index += 2;
      }
    }
    else {
      break;
    }
  }
}

struct Token lexerGetToken(struct Lexer* lexer) {
  removeWhiteSpaces(lexer);

  struct Token token = {0};

  token.stringLength = 1;
  token.string = lexer->index;

  char c = lexer->index[0];

  lexer->index++;

  switch (c) {
    case ':': {
      token.type = Token_colon;
    } break;

    case ';': {
      token.type = Token_semicolon;
    } break;

    case ',': {
      token.type = Token_comma;
    } break;

    case '(': {
      token.type = Token_openparen;
    } break;

    case ')': {
      token.type = Token_closeparen;
    } break;

    case '{': {
      token.type = Token_openbrace;
    } break;

    case '}': {
      token.type = Token_closebrace;
    } break;

    case '[': {
      token.type = Token_openbracket;
    } break;

    case ']': {
      token.type = Token_closebracket;
    } break;

    case '*': {
      token.type = Token_mult;
    } break;

    case '/': {
      token.type = Token_div;
    } break;

    case '+': {
      token.type = Token_plus;
    } break;

    case '-': {
      token.type = Token_minus;
    } break;

    case '=': {
      token.type = Token_equals;
    } break;

    case '"': {
      token.type = Token_string;
      token.string = lexer->index;

      while (lexer->index[0] && lexer->index[0] != '"') {
        if (lexer->index[0] == '\\' && lexer->index[1]) {
          lexer->index++;
        }
        lexer->index++;
      }

      token.stringLength = lexer->index - token.string;
      if (lexer->index[0] == '"') {
        lexer->index++;
      }
    }
      break;

    case 0: { // End of file / end of stream
      token.type = Token_EOF;
    }
      break;

    default: {
      if (isAlphabetical(c) || c == '_') {
        while (
          isAlphabetical(lexer->index[0]) ||
          isNumber(lexer->index[0]) ||
          lexer->index[0] == '_'
        ) {
          lexer->index++;
        }

        token.type = Token_identifier;
        token.stringLength = lexer->index - token.string;

        if (lexerTokenEquals(token, "var")) {
          token.type = Token_variable;
        }
        else if (lexerTokenEquals(token, "func")) {
          token.type = Token_function;
        }
        else if (lexerTokenEquals(token, "return")) {
          token.type = Token_return;
        }
        else if (lexerTokenEquals(token, "extern")) {
          token.type = Token_extern;
        }
        else if (lexerTokenEquals(token, "asm")) {
          token.type = Token_inline_asm;
        }
      }
      else if (isNumber(c)) {
        while (isNumber(lexer->index[0]) || lexer->index[0] == '.') {
          lexer->index++;
        }
        token.type = Token_number;
        token.stringLength = lexer->index - token.string;
      }
      else {
        token.type = Token_unknown;
      }
    }
      break;
  }
  lexer->token = token;
  return token;
}

int lexerTokenEquals(struct Token token, char* toMatch) {
  char* index = toMatch;
  for (int i = 0; i < token.stringLength; i++, index++) {
    if ((*index == '\0') || token.string[i] != *index) {
      return 0;
    }
  }
  return (*index == 0);
}

int lexerRequireToken(struct Lexer* lexer, int type) {
  struct Token token = lexerGetToken(lexer);
  return (token.type == type);
}

int lexerRequireCurrentToken(struct Lexer* lexer, int type) {
  return (lexer->token.type == type);
}
