// parser.c

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "parser.h"
#include "lexer.h"
#include "file.h"

enum BinaryOperators {
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_MOD,

  OP_DIV,

};

struct {
  unsigned char left;
  unsigned char right;
} priority[] = {
  {10, 10}, {10, 10},   // '+', '-'
  {11, 11}, {11, 11},   // '*', '%'
  {11, 11},             // '/'
};


static int blockEnd(struct Lexer* lexer); // Ends with '}'
static int tupleEnd(struct Lexer* lexer); // Ends with ')'

static void parseVariableDecl(struct Lexer* lexer);
static void parseExternStatement(struct Lexer* lexer);
static void parseInlineAssembly(struct Lexer* lexer);
static void parseExpression(struct Lexer* lexer, struct Expression* expr);
static void parseStatement(struct Lexer* lexer);
static void parseFunctionStatement(struct Lexer* lexer);
static void parseReturnStatement(struct Lexer* lexer);
static void parseFunctionArgs(struct Lexer* lexer);
static void parseFunctionBody(struct Lexer* lexer);

inline void printToken(struct Lexer* lexer) {
  if (lexer->token.stringLength) {
    printf("%.*s\n", lexer->token.stringLength, lexer->token.string);
  }
}

inline void printToken2(struct Token token) {
  printf("%.*s\n", token.stringLength, token.string);
}

static void errorMessage(struct Lexer* lexer, char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "ParseError (line:%i): ", lexer->currentLine);
  vfprintf(stderr, fmt, args);
  va_end(args);
  lexer->parser->error = 1;
}

static void warningMessage(struct Lexer* lexer, char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  printf("Warning (line:%i): ", lexer->currentLine);
  vprintf(fmt, args);
  va_end(args);
}

static void warningMessage2(struct Lexer* lexer, int line, char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  printf("Warning (line:%i): ", line);
  vprintf(fmt, args);
  va_end(args);
}

static int getBinaryOp(struct Token token) {
  switch (token.type) {
    case Token_plus:
      return OP_ADD;
      break;
    case Token_minus:
      return OP_SUB;
      break;
    case Token_mult:
      return OP_MUL;
      break;
    case Token_mod:
      return OP_MOD;
      break;
    case Token_div:
      return OP_DIV;
      break;
  }

  return -1;
}

static void writeOut(struct Lexer* lexer, char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(lexer->parser->out, fmt, args);
  va_end(args);
}

static inline int blockEnd(struct Lexer* lexer) {
  if (lexer->token.type == Token_EOF) {
    errorMessage(lexer, "%s\n", "Missing '}' block end");
    return 1;
  }
  return (lexer->token.type == Token_closebrace);
}

static inline int tupleEnd(struct Lexer* lexer) {
  if (lexer->token.type == Token_EOF) {
    errorMessage(lexer, "%s\n", "Missing ')'");
    return 1;
  }
  return (lexer->token.type == Token_closeparen);
}

// Variable decl
// <type> <identifier> ';'
static void parseVariableDecl(struct Lexer* lexer) {
  struct Token typeName = lexer->token;
  int size = sizeof(int);

  struct Token identifier = lexerGetToken(lexer);
  struct Function* func = &lexer->func;

  if (identifier.type == Token_identifier) {
    if (func->variableCount < array_size(func->variables)) {
      struct Variable var = {0};
      var.name = identifier;
      var.size = size;
      var.line = lexer->currentLine;
      var.offsetFromBasePointer = size * (func->variableCount + 1);
      func->variables[func->variableCount] = var;

      writeOut(lexer, "  movl $%i, -%i(%%ebp)", func->variableCount+1, var.offsetFromBasePointer);
      // writeOut(lexer, "  movl $%i, %i(%%esp)", func->variableCount+1, var.offsetFromBasePointer - 4);
      writeOut(lexer, " # Variable: %.*s\n", var.name.stringLength, var.name.string);
      // writeOut(lexer, "  movl -%i(%s), %%%s\n", var.offsetFromBasePointer, "%ebp", registerNames[func->variableCount]);

      func->variableCount++;
    } else {
      errorMessage(lexer, "%s\n", "Too many variables declared in this function");
    }

  } else {
    errorMessage(lexer, "%s\n", "Invalid variable name");
  }

}

static void parseExternStatement(struct Lexer* lexer) {
  struct Token token = lexerGetToken(lexer);

  if (token.type == Token_identifier) {
    writeOut(lexer, ".extern %.*s\n\n", token.stringLength, token.string);
    if (lexerRequireToken(lexer, Token_semicolon)) {
      return;
    } else {
      errorMessage(lexer, "%s\n", "Missing semicolon ';'");
    }

  } else {
    errorMessage(lexer, "%s\n", "Extern statement is missing symbol");
  }
}

// asm {} | asm ""
static void parseInlineAssembly(struct Lexer* lexer) {
  struct Token next = lexerGetToken(lexer);

  // asm '{' ... '}'
  if (next.type == Token_openbrace) {
    for (;;) {
      struct Token token = lexerGetToken(lexer);
      if (token.type == Token_string) {
        writeOut(lexer, "%.*s\n", token.stringLength, token.string);
      }
      if (blockEnd(lexer)) {
        writeOut(lexer, "\n");
        break;
      }
    }
  } else if (next.type == Token_string) {
    if (next.type == Token_string) {
      writeOut(lexer, "%.*s\n", next.stringLength, next.string);
    }
  } else {
    errorMessage(lexer, "%s\n", "Inline assembly statement is missing block or single string");
  }
}

// expr op expr | expr expr | op expr
// 5 + 5        | test()    | - (...)
//
// expression = identifier | number | '('...')'
//
static void parseExpression(struct Lexer* lexer, struct Expression* expr) {
  struct Token token = lexer->token;

  switch (token.type) {
    case Token_number: {
      struct Token number = token;
      printf("-> %.*s\n", number.stringLength, number.string);
      expr->token = token;
    }
      break;

    default:
      break;
  }
}

static void parseStatement(struct Lexer* lexer) {
  struct Token token = lexerGetToken(lexer);

  switch (token.type) {
    case Token_function: {
      if (!lexer->isLocal) {
        parseFunctionStatement(lexer);
      } else {
        errorMessage(lexer, "%s\n", "Inline functions are not allowed");
      }
    }
      break;

    case Token_return: {
      if (lexer->isLocal) {
        lexerGetToken(lexer); // Skip 'return' token
        parseReturnStatement(lexer);
      } else {
        errorMessage(lexer, "%s\n", "Return statements are not allowed outside functions");
      }
    }
      break;

    // Function call: <identifier> '(' <args> ')'
    case Token_identifier: {
      struct Token next = lexerGetToken(lexer);

      // Function call
      if (next.type == Token_openparen) {
        if (lexer->isLocal) {
          lexerGetToken(lexer); // Eat '('
          while (!tupleEnd(lexer)) {
            // parseFunctionCallArgs(lexer);
            lexerGetToken(lexer);  // Eat function args
          }
          writeOut(lexer, "  subl $%i, %s\n", 24, "%esp");
          writeOut(lexer, "  calll _%.*s\n", token.stringLength, token.string);
          writeOut(lexer, "  addl $%i, %s\n", 24, "%esp");
        } else {
          errorMessage(lexer, "%s\n", "Cannot call functions in the global scope");
        }
      }
    }
      break;

    // TODO: Implement custom types
    case Token_variable: {
      if (lexer->isLocal) {
        parseVariableDecl(lexer);
      } else {
        errorMessage(lexer, "%s\n", "Variables must be declared locally");
      }
    }
      break;

    case Token_extern: {
      if (!lexer->isLocal) {
        parseExternStatement(lexer);
      } else {
        errorMessage(lexer, "%s\n", "Extern statement must be declared in the global scope");
      }
      parseStatement(lexer);  // Continue
    }
      break;

    case Token_inline_asm: {
      parseInlineAssembly(lexer);
      parseStatement(lexer);
    }
      break;

    case Token_EOF: {
      return;
    }
      break;

    case Token_semicolon: {
      parseStatement(lexer);  // Skip ';' unused
    }
      break;

    default: {
      // parseStatement(lexer);
    }
      break;
  }
}

// <function> <identifier> '(' <args> ')' '{' <statements> '}'
static void parseFunctionStatement(struct Lexer* lexer) {

  struct Token functionName = lexerGetToken(lexer);
  lexer->func.name = functionName;
  lexer->isLocal = 1;

  // TODO: check if function already exists
  if (functionName.type == Token_identifier) {
    parseFunctionArgs(lexer);
    writeOut(lexer, ".globl _%.*s\n", functionName.stringLength, functionName.string);
    writeOut(lexer, "_%.*s:\n", functionName.stringLength, functionName.string);
    writeOut(lexer, "%s\n", "  pushl %ebp \t# Save the old base pointer value");
    writeOut(lexer, "%s\n", "  movl  %esp, %ebp \t# Set new base pointer value");
    parseFunctionBody(lexer);
    
  } else {
    errorMessage(lexer, "%s\n", "Missing function name");
  }
}

// return <expr> ';'
static void parseReturnStatement(struct Lexer* lexer) {
  struct Expression expr;
  // parseStatement(lexer);
  struct Token token = lexer->token;
  if (token.type == Token_number) {
    parseExpression(lexer, &expr);
    writeOut(lexer, "  movl $%.*s, %%eax\n", expr.token.stringLength, expr.token.string);
    lexerGetToken(lexer);
  }
  writeOut(lexer, "%s\n",   "  popl  %ebp \t# Restore caller's base pointer value");
  writeOut(lexer, "%s\n\n", "  retl");

  if (lexer->token.type != Token_semicolon) {
    errorMessage(lexer, "%s\n", "Missing ';' semicolon");
    return;
  }
}

static void parseFunctionArgs(struct Lexer* lexer) {
  if (lexerRequireToken(lexer, Token_openparen)) {
    while (!tupleEnd(lexer)) {
      lexerGetToken(lexer); // Eat all args
    }
  } else {
    errorMessage(lexer, "%s\n", "Missing '(' open parentheses near function");
  }
}

static void parseFunctionBody(struct Lexer* lexer) {
  if (lexerRequireToken(lexer, Token_openbrace)) {
    while (!blockEnd(lexer)) {
      parseStatement(lexer);
    }

    for (int i = 0; i < lexer->func.variableCount; i++) {
      struct Variable var = lexer->func.variables[i];
      if (!var.isUsed) {
        warningMessage2(lexer, var.line, "Unused variable '%.*s'\n", var.name.stringLength, var.name.string);
      }
    }

    lexer->isLocal = 0;
    lexer->func.variableCount = 0;
    parseStatement(lexer);
  } else {
    errorMessage(lexer, "%s\n", "Missing '{' open brace near function");
  }
}

int parse(int argc, char** argv) {
  struct Parser parser = {0};
  struct Lexer lexer = {0};
  lexer.currentLine = 1;
  lexer.parser = &parser;
  
  if (argc > 2) {
    char* fileContents = readFile(argv[1]);
    if (fileContents) {
      lexer.index = fileContents;
      parser.lexer = &lexer;
      parser.out = fopen(argv[2], "w");
      parseStatement(&lexer);
      free(fileContents);
      fclose(parser.out);
    }
  } else {
    printf("%s\n", "Missing args: <input file> <output file>");
  }
  return parser.error == 1;
}
