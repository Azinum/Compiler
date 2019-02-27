.globl _thing
_thing:
  pushl %ebp 	# Save the old base pointer value
  movl  %esp, %ebp 	# Set new base pointer value
  movl $7, %eax
  popl  %ebp 	# Restore caller's base pointer value
  retl

.globl _start
_start:
  pushl %ebp 	# Save the old base pointer value
  movl  %esp, %ebp 	# Set new base pointer value
  subl $24, %esp
  calll _thing
  addl $24, %esp
  popl  %ebp 	# Restore caller's base pointer value
  retl

