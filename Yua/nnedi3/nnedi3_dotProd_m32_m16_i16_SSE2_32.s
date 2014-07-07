	.text
.globl _dotProd_m32_m16_i16_SSE2
_dotProd_m32_m16_i16_SSE2:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$12, %esp
	call	L6

L6:
	popl	%ebx
	# top of block
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 3
	pushl %edi
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 4
	pushl %esi
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 5
	pushl %ebx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 6
	movl 12(%ebp), %edi
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 7
	movl 16(%ebp), %eax
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 8
	movl 20(%ebp), %ebx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 9
	movl 24(%ebp), %esi
	L2: # LASM$nloop
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 11
	movl 8(%ebp), %ecx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 12
	pxor %xmm0, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 13
	pxor %xmm1, %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 14
	pxor %xmm2, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 15
	pxor %xmm3, %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 16
	movl %esi, %edx
	L3: # LASM$lloop
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 18
	movdqa 0(%ecx), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 19
	movdqa %xmm4, %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 20
	movdqa %xmm4, %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 21
	movdqa %xmm4, %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 22
	pmaddwd 0(%edi), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 23
	pmaddwd 16(%edi), %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 24
	pmaddwd 32(%edi), %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 25
	pmaddwd 48(%edi), %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 26
	paddd %xmm4, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 27
	paddd %xmm5, %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 28
	paddd %xmm6, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 29
	paddd %xmm7, %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 30
	movdqa 16(%ecx), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 31
	movdqa %xmm4, %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 32
	movdqa %xmm4, %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 33
	movdqa %xmm4, %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 34
	pmaddwd 64(%edi), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 35
	pmaddwd 80(%edi), %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 36
	pmaddwd 96(%edi), %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 37
	pmaddwd 112(%edi), %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 38
	paddd %xmm4, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 39
	paddd %xmm5, %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 40
	paddd %xmm6, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 41
	paddd %xmm7, %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 42
	movdqa 32(%ecx), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 43
	movdqa %xmm4, %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 44
	movdqa %xmm4, %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 45
	movdqa %xmm4, %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 46
	pmaddwd 128(%edi), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 47
	pmaddwd 144(%edi), %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 48
	pmaddwd 160(%edi), %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 49
	pmaddwd 176(%edi), %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 50
	paddd %xmm4, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 51
	paddd %xmm5, %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 52
	paddd %xmm6, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 53
	paddd %xmm7, %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 54
	movdqa 48(%ecx), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 55
	movdqa %xmm4, %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 56
	movdqa %xmm4, %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 57
	movdqa %xmm4, %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 58
	pmaddwd 192(%edi), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 59
	pmaddwd 208(%edi), %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 60
	pmaddwd 224(%edi), %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 61
	pmaddwd 240(%edi), %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 62
	paddd %xmm4, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 63
	paddd %xmm5, %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 64
	paddd %xmm6, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 65
	paddd %xmm7, %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 66
	addl $64, %ecx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 67
	addl $256, %edi
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 68
	subl $32, %edx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 69
	jnz L3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 70
	movdqa %xmm0, %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 71
	movdqa %xmm2, %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 72
	punpcklqdq %xmm1, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 73
	punpcklqdq %xmm3, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 74
	punpckhqdq %xmm1, %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 75
	punpckhqdq %xmm3, %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 76
	paddd %xmm4, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 77
	paddd %xmm5, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 78
	movdqa %xmm0, %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 79
	shufps $136, %xmm2, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 80
	shufps $221, %xmm2, %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 81
	paddd %xmm0, %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 82
	movdqa %xmm6, 0(%eax)
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 83
	addl $16, %eax
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 84
	subl $4, %ebx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 85
	jnz L2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 86
	movl 28(%ebp), %ecx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 87
	movl 16(%ebp), %eax
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 88
	movss 0(%ecx), %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 89
	movl 20(%ebp), %edx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 90
	pshufd $0, %xmm7, %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 91
	xorl %ecx, %ecx
	L4: # LASM$aloop
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 93
	movdqa 0(%eax,%ecx,4), %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 94
	movdqa 16(%eax,%ecx,4), %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 95
	movdqa 32(%eax,%ecx,4), %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 96
	movdqa 48(%eax,%ecx,4), %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 97
	cvtdq2ps %xmm0, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 98
	cvtdq2ps %xmm1, %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 99
	cvtdq2ps %xmm2, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 100
	cvtdq2ps %xmm3, %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 101
	mulps 0(%edi,%ecx,8), %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 102
	mulps 32(%edi,%ecx,8), %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 103
	mulps 64(%edi,%ecx,8), %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 104
	mulps 96(%edi,%ecx,8), %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 105
	mulps %xmm7, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 106
	mulps %xmm7, %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 107
	mulps %xmm7, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 108
	mulps %xmm7, %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 109
	addps 16(%edi,%ecx,8), %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 110
	addps 48(%edi,%ecx,8), %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 111
	addps 80(%edi,%ecx,8), %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 112
	addps 112(%edi,%ecx,8), %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 113
	movaps %xmm0, 0(%eax,%ecx,4)
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 114
	movaps %xmm1, 16(%eax,%ecx,4)
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 115
	movaps %xmm2, 32(%eax,%ecx,4)
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 116
	movaps %xmm3, 48(%eax,%ecx,4)
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 117
	addl $16, %ecx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 118
	subl $16, %edx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 119
	jnz L4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 120
	popl %ebx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 121
	popl %esi
	.file "nnedi3_dotProd_m32_m16_i16_SSE2_32.c"
	.line 122
	popl %edi
	addl	$12, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret
	
