// main.c

#include <stdio.h>

extern void start();

void print() {
  printf("%s\n", "Printing here!");
}

void print1(int a) {
  printf("a: %i\n", a);
}

void print2(int a, int b) {
  printf("a:%i, b:%i\n", a, b);
}

void print3(int a, int b, int c) {
  printf("%s: %i, %i, %i\n", __FUNCTION__, a, b, c);
}

void ptr(int a) {
  printf("a:%i\n", a);
}

int add(int a, int b) {
  int result = a + b;
  printf("%i + %i = %i\n", a, b, a + b);
  return a + b;
}

void manyParams(int a, int b, int c, int d, int e) {
  printf("%i, %i, %i, %i, %i\n", a, b, c, d, e);
}

int main(int argc, char** argv) {
  start();
  return 0;
}