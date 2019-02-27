# Compiler

A small compiler made in C. Generates assembly code.

## Features

```
// Single line comments
/* Multi-line
  comments */

// Extern statement
extern symbol;

// Functions
func thing() {
  return;
}

func hello() {
  int a;  // Variable declaration
  asm "movl $5, %eax"; // Inline assembly
  asm { // Multi-line inline assembly
    "movl $12, %eax"
    "movl %eax, %ecx"
  };
  thing();  // Function calls
  return; // Void return statements
}
```