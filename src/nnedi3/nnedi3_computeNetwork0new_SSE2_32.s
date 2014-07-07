.globl _sign_bits_f
	.data
	.align 16
_sign_bits_f:
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
.globl _sign_bits_f_zero_l
	.align 16
_sign_bits_f_zero_l:
	.long	0
	.long	2147483647
	.long	2147483647
	.long	2147483647
.globl _ones_f
	.align 16
_ones_f:
	.long	1065353216
	.long	1065353216
	.long	1065353216
	.long	1065353216
	.text
.globl _computeNetwork0new_SSE2
_computeNetwork0new_SSE2:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	# top of block
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 8
	movl 8(%ebp), %ecx
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 9
	movl 12(%ebp), %eax
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 10
	movdqa 0(%ecx), %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 11
	movdqa %xmm0, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 12
	movdqa %xmm0, %xmm2
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 13
	movdqa %xmm0, %xmm3
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 14
	pmaddwd 0(%eax), %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 15
	pmaddwd 16(%eax), %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 16
	pmaddwd 32(%eax), %xmm2
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 17
	pmaddwd 48(%eax), %xmm3
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 18
	movdqa 16(%ecx), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 19
	movdqa %xmm4, %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 20
	movdqa %xmm4, %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 21
	movdqa %xmm4, %xmm7
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 22
	pmaddwd 64(%eax), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 23
	pmaddwd 80(%eax), %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 24
	pmaddwd 96(%eax), %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 25
	pmaddwd 112(%eax), %xmm7
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 26
	paddd %xmm4, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 27
	paddd %xmm5, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 28
	paddd %xmm6, %xmm2
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 29
	paddd %xmm7, %xmm3
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 30
	movdqa 32(%ecx), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 31
	movdqa %xmm4, %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 32
	movdqa %xmm4, %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 33
	movdqa %xmm4, %xmm7
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 34
	pmaddwd 128(%eax), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 35
	pmaddwd 144(%eax), %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 36
	pmaddwd 160(%eax), %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 37
	pmaddwd 176(%eax), %xmm7
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 38
	paddd %xmm4, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 39
	paddd %xmm5, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 40
	paddd %xmm6, %xmm2
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 41
	paddd %xmm7, %xmm3
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 42
	movdqa 48(%ecx), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 43
	movdqa %xmm4, %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 44
	movdqa %xmm4, %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 45
	movdqa %xmm4, %xmm7
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 46
	pmaddwd 192(%eax), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 47
	pmaddwd 208(%eax), %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 48
	pmaddwd 224(%eax), %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 49
	pmaddwd 240(%eax), %xmm7
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 50
	paddd %xmm4, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 51
	paddd %xmm5, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 52
	paddd %xmm6, %xmm2
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 53
	paddd %xmm7, %xmm3
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 54
	movdqa 64(%ecx), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 55
	movdqa %xmm4, %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 56
	movdqa %xmm4, %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 57
	movdqa %xmm4, %xmm7
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 58
	pmaddwd 256(%eax), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 59
	pmaddwd 272(%eax), %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 60
	pmaddwd 288(%eax), %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 61
	pmaddwd 304(%eax), %xmm7
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 62
	paddd %xmm4, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 63
	paddd %xmm5, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 64
	paddd %xmm6, %xmm2
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 65
	paddd %xmm7, %xmm3
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 66
	movdqa 80(%ecx), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 67
	movdqa %xmm4, %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 68
	movdqa %xmm4, %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 69
	movdqa %xmm4, %xmm7
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 70
	pmaddwd 320(%eax), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 71
	pmaddwd 336(%eax), %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 72
	pmaddwd 352(%eax), %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 73
	pmaddwd 368(%eax), %xmm7
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 74
	paddd %xmm4, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 75
	paddd %xmm5, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 76
	paddd %xmm6, %xmm2
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 77
	paddd %xmm7, %xmm3
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 78
	movdqa 96(%ecx), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 79
	movdqa %xmm4, %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 80
	movdqa %xmm4, %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 81
	movdqa %xmm4, %xmm7
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 82
	pmaddwd 384(%eax), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 83
	pmaddwd 400(%eax), %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 84
	pmaddwd 416(%eax), %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 85
	pmaddwd 432(%eax), %xmm7
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 86
	paddd %xmm4, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 87
	paddd %xmm5, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 88
	paddd %xmm6, %xmm2
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 89
	paddd %xmm7, %xmm3
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 90
	movdqa 112(%ecx), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 91
	movdqa %xmm4, %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 92
	movdqa %xmm4, %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 93
	movdqa %xmm4, %xmm7
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 94
	pmaddwd 448(%eax), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 95
	pmaddwd 464(%eax), %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 96
	pmaddwd 480(%eax), %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 97
	pmaddwd 496(%eax), %xmm7
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 98
	paddd %xmm4, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 99
	paddd %xmm5, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 100
	paddd %xmm6, %xmm2
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 101
	paddd %xmm7, %xmm3
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 102
	movdqa %xmm0, %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 103
	movdqa %xmm2, %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 104
	punpcklqdq %xmm1, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 105
	punpcklqdq %xmm3, %xmm2
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 106
	punpckhqdq %xmm1, %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 107
	punpckhqdq %xmm3, %xmm5
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 108
	paddd %xmm4, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 109
	paddd %xmm5, %xmm2
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 110
	movdqa %xmm0, %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 111
	shufps $136, %xmm2, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 112
	shufps $221, %xmm2, %xmm6
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 113
	paddd %xmm6, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 114
	cvtdq2ps %xmm0, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 115
	mulps 512(%eax), %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 116
	addps 528(%eax), %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 117
	movaps %xmm0, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 118
	andps _sign_bits_f, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 119
	addps _ones_f, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 120
	rcpps %xmm0, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 121
	mulps %xmm1, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 122
	pshufd $0, %xmm0, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 123
	pshufd $85, %xmm0, %xmm2
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 124
	pshufd $170, %xmm0, %xmm3
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 125
	pshufd $255, %xmm0, %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 126
	mulps 544(%eax), %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 127
	mulps 560(%eax), %xmm2
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 128
	mulps 576(%eax), %xmm3
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 129
	mulps 592(%eax), %xmm4
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 130
	pxor %xmm0, %xmm0
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 131
	addps %xmm2, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 132
	addps %xmm4, %xmm3
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 133
	addps %xmm3, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 134
	movl 16(%ebp), %ecx
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 135
	addps 608(%eax), %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 136
	cmpps $1, %xmm0, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 137
	packssdw %xmm0, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 138
	packsswb %xmm0, %xmm1
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 139
	movd %xmm1, %eax
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 140
	xorl $4294967295, %eax
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 141
	andl $16843009, %eax
	.file "nnedi3_computeNetwork0new_SSE2_32.c"
	.line 142
	mov %eax, 0(%ecx)
	popl	%esi
	popl	%edi
	leave
	ret
	
