	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 13
	.globl	_a                      ## -- Begin function a
	.p2align	4, 0x90
_a:                                     ## @a
## BB#0:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$12, %esp
	movl	16(%ebp), %eax
	movl	12(%ebp), %ecx
	movl	8(%ebp), %edx
	movl	%eax, -4(%ebp)          ## 4-byte Spill
	movl	%ecx, -8(%ebp)          ## 4-byte Spill
	movl	%edx, -12(%ebp)         ## 4-byte Spill
	addl	$12, %esp
	popl	%ebp
	retl
                                        ## -- End function
	.globl	_start                  ## -- Begin function start
	.p2align	4, 0x90
_start:                                 ## @start
## BB#0:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$1, %eax
	movl	$2, %ecx
	movl	$3, %edx
	movl	$1, (%esp)
	movl	$2, 4(%esp)
	movl	$3, 8(%esp)
	movl	%eax, -4(%ebp)          ## 4-byte Spill
	movl	%ecx, -8(%ebp)          ## 4-byte Spill
	movl	%edx, -12(%ebp)         ## 4-byte Spill
	calll	_a
	addl	$24, %esp
	popl	%ebp
	retl
                                        ## -- End function

.subsections_via_symbols
