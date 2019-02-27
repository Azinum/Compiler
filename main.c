// main.c

#include <stdio.h>

#include "parser.h"

/*

Functions:
.globl _function
_function:
  <FUNCTION BODY>


Return statement:
movl $3, %eax     # return 3
retl


Variable decl
movl $3, -4(%ebp) # int var = 3;
movl -4(%ebp), %eax  # put 'var' into register 'eax'
<type> <identifier> '=' <statement>

movl $3, -4(%ebp) # int a = 3;
movl $5, -8(%ebp) # int b = 5;


FUNCTIONS
<type> <identifier> '(' ')' '{' <expression> '}'

*/

int main(int argc, char** argv) {
  return parse(argc, argv);
}