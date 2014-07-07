.globl _sse_half
	.data
	.align 16
_sse_half:
	.long	1056964608
	.long	1056964608
	.long	1056964608
	.long	1056964608
	.text
.globl _castScale_SSE
_castScale_SSE:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	# top of block
	.file "nnedi3_castScale_SSE_32.c"
	.line 5
	movl 8(%ebp), %ecx
	.file "nnedi3_castScale_SSE_32.c"
	.line 6
	movl 12(%ebp), %eax
	.file "nnedi3_castScale_SSE_32.c"
	.line 7
	movss 12(%ecx), %xmm0
	.file "nnedi3_castScale_SSE_32.c"
	.line 8
	mulss 0(%eax), %xmm0
	.file "nnedi3_castScale_SSE_32.c"
	.line 9
	addss _sse_half, %xmm0
	.file "nnedi3_castScale_SSE_32.c"
	.line 10
	cvttss2sil %xmm0, %eax
	.file "nnedi3_castScale_SSE_32.c"
	.line 11
	movl 16(%ebp), %ecx
	.file "nnedi3_castScale_SSE_32.c"
	.line 12
	cmpl $255, %eax
	.file "nnedi3_castScale_SSE_32.c"
	.line 13
	jl L2
	.file "nnedi3_castScale_SSE_32.c"
	.line 14
	movl $255, %eax
	.file "nnedi3_castScale_SSE_32.c"
	.line 15
	jmp L3
	L2: # LASM$b255
	.file "nnedi3_castScale_SSE_32.c"
	.line 17
	cmpl $0, %eax
	.file "nnedi3_castScale_SSE_32.c"
	.line 18
	jge L3
	.file "nnedi3_castScale_SSE_32.c"
	.line 19
	xorl %eax, %eax
	L3: # LASM$finish
	.file "nnedi3_castScale_SSE_32.c"
	.line 21
	movb %al, 0(%ecx)
	popl	%esi
	popl	%edi
	leave
	ret
	
