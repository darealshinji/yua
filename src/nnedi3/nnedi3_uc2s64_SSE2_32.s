	.text
.globl _uc2s64_SSE2
_uc2s64_SSE2:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	# top of block
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 3
	movl 8(%ebp), %eax
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 4
	movl 12(%ebp), %ecx
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 5
	movl 16(%ebp), %edx
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 6
	pxor %xmm7, %xmm7
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 7
	movq 0(%eax), %xmm0
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 8
	movq 8(%eax), %xmm1
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 9
	movq 0(%eax,%ecx,2), %xmm2
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 10
	movq 8(%eax,%ecx,2), %xmm3
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 11
	punpcklbw %xmm7, %xmm0
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 12
	punpcklbw %xmm7, %xmm1
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 13
	punpcklbw %xmm7, %xmm2
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 14
	punpcklbw %xmm7, %xmm3
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 15
	leal 0(%eax,%ecx,4), %eax
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 16
	movdqa %xmm0, 0(%edx)
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 17
	movdqa %xmm1, 16(%edx)
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 18
	movdqa %xmm2, 32(%edx)
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 19
	movdqa %xmm3, 48(%edx)
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 20
	movq 0(%eax), %xmm4
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 21
	movq 8(%eax), %xmm5
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 22
	movq 0(%eax,%ecx,2), %xmm6
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 23
	movq 8(%eax,%ecx,2), %xmm0
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 24
	punpcklbw %xmm7, %xmm4
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 25
	punpcklbw %xmm7, %xmm5
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 26
	punpcklbw %xmm7, %xmm6
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 27
	punpcklbw %xmm7, %xmm0
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 28
	movdqa %xmm4, 64(%edx)
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 29
	movdqa %xmm5, 80(%edx)
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 30
	movdqa %xmm6, 96(%edx)
	.file "nnedi3_uc2s64_SSE2_32.c"
	.line 31
	movdqa %xmm0, 112(%edx)
	popl	%esi
	popl	%edi
	leave
	ret
	
