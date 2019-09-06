.section .data
  array:
	.long 12,4,6,7,80,34,54,46,3,23
  output:
	.ascii "The minium value is %d.\nThe mean value is %d.\n"

.section .bss
	.lcomm buffer,100

.section .text
.globl _start
_start:
	movl $0,%edi
	movl array(,%edi,4),%esi
	movl %esi,%eax
	movl %esi,%ebx

loop:
	cmpl $9,%edi
	je end_loop
	incl %edi
	movl array(,%edi,4),%esi
	addl %esi,%eax
	cmpl %esi,%ebx
	jl loop
	movl %esi,%ebx
	jmp loop

end_loop:
	movl $10,%ecx
	movl $0,%edx
	div %ecx
	pushl %eax
	pushl %ebx
	pushl $output
	call printf
	addl $8,%esp
	pushl $0
	call exit

