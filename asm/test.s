# test.s

# int test0() {
#   return 2;
# }
.globl _test0
_test0:
  pushl %ebp
  movl %esp, %ebp
  movl $2, %eax
  popl %ebp
  retl
# function (test0)


# int test1() {
#   return 20;
# }
.globl _test
_test1:
  pushl %ebp
  movl %esp, %ebp
  movl $20, %eax
  popl %ebp
  retl
