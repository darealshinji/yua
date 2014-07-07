	.text
.globl _dotProd_m32_m16_i16_SSE2
_dotProd_m32_m16_i16_SSE2:
LFB2:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	pushq	%rbx
LCFI2:
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	%rdx, -40(%rbp)
	movl	%ecx, -44(%rbp)
	movl	%r8d, -48(%rbp)
	movq	%r9, -56(%rbp)
	# top of block
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 4
	push %rbx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 6
	mov -32(%rbp), %rdi
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 7
	mov -40(%rbp), %rax
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 8
	movl -44(%rbp), %ebx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 9
	movl -48(%rbp), %esi
	L2: # LASM$nloop
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 11
	mov -24(%rbp), %rcx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 12
	pxor %xmm0, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 13
	pxor %xmm1, %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 14
	pxor %xmm2, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 15
	pxor %xmm3, %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 16
	mov %rsi, %rdx
	L3: # LASM$lloop
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 18
	movdqa 0(%rcx), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 19
	movdqa %xmm4, %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 20
	movdqa %xmm4, %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 21
	movdqa %xmm4, %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 22
	pmaddwd 0(%rdi), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 23
	pmaddwd 16(%rdi), %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 24
	pmaddwd 32(%rdi), %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 25
	pmaddwd 48(%rdi), %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 26
	paddd %xmm4, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 27
	paddd %xmm5, %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 28
	paddd %xmm6, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 29
	paddd %xmm7, %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 30
	movdqa 16(%rcx), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 31
	movdqa %xmm4, %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 32
	movdqa %xmm4, %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 33
	movdqa %xmm4, %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 34
	pmaddwd 64(%rdi), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 35
	pmaddwd 80(%rdi), %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 36
	pmaddwd 96(%rdi), %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 37
	pmaddwd 112(%rdi), %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 38
	paddd %xmm4, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 39
	paddd %xmm5, %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 40
	paddd %xmm6, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 41
	paddd %xmm7, %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 42
	movdqa 32(%rcx), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 43
	movdqa %xmm4, %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 44
	movdqa %xmm4, %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 45
	movdqa %xmm4, %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 46
	pmaddwd 128(%rdi), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 47
	pmaddwd 144(%rdi), %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 48
	pmaddwd 160(%rdi), %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 49
	pmaddwd 176(%rdi), %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 50
	paddd %xmm4, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 51
	paddd %xmm5, %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 52
	paddd %xmm6, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 53
	paddd %xmm7, %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 54
	movdqa 48(%rcx), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 55
	movdqa %xmm4, %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 56
	movdqa %xmm4, %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 57
	movdqa %xmm4, %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 58
	pmaddwd 192(%rdi), %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 59
	pmaddwd 208(%rdi), %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 60
	pmaddwd 224(%rdi), %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 61
	pmaddwd 240(%rdi), %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 62
	paddd %xmm4, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 63
	paddd %xmm5, %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 64
	paddd %xmm6, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 65
	paddd %xmm7, %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 66
	add $64, %rcx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 67
	add $256, %rdi
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 68
	sub $32, %rdx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 69
	jnz L3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 70
	movdqa %xmm0, %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 71
	movdqa %xmm2, %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 72
	punpcklqdq %xmm1, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 73
	punpcklqdq %xmm3, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 74
	punpckhqdq %xmm1, %xmm4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 75
	punpckhqdq %xmm3, %xmm5
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 76
	paddd %xmm4, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 77
	paddd %xmm5, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 78
	movdqa %xmm0, %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 79
	shufps $136, %xmm2, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 80
	shufps $221, %xmm2, %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 81
	paddd %xmm0, %xmm6
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 82
	movdqa %xmm6, 0(%rax)
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 83
	add $16, %rax
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 84
	sub $4, %rbx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 85
	jnz L2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 86
	mov -56(%rbp), %rcx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 87
	mov -40(%rbp), %rax
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 88
	movss 0(%rcx), %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 89
	movl -44(%rbp), %edx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 90
	pshufd $0, %xmm7, %xmm7
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 91
	xor %rcx, %rcx
	L4: # LASM$aloop
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 93
	movdqa 0(%rax,%rcx,4), %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 94
	movdqa 16(%rax,%rcx,4), %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 95
	movdqa 32(%rax,%rcx,4), %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 96
	movdqa 48(%rax,%rcx,4), %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 97
	cvtdq2ps %xmm0, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 98
	cvtdq2ps %xmm1, %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 99
	cvtdq2ps %xmm2, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 100
	cvtdq2ps %xmm3, %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 101
	mulps 0(%rdi,%rcx,8), %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 102
	mulps 32(%rdi,%rcx,8), %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 103
	mulps 64(%rdi,%rcx,8), %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 104
	mulps 96(%rdi,%rcx,8), %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 105
	mulps %xmm7, %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 106
	mulps %xmm7, %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 107
	mulps %xmm7, %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 108
	mulps %xmm7, %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 109
	addps 16(%rdi,%rcx,8), %xmm0
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 110
	addps 48(%rdi,%rcx,8), %xmm1
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 111
	addps 80(%rdi,%rcx,8), %xmm2
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 112
	addps 112(%rdi,%rcx,8), %xmm3
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 113
	movaps %xmm0, 0(%rax,%rcx,4)
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 114
	movaps %xmm1, 16(%rax,%rcx,4)
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 115
	movaps %xmm2, 32(%rax,%rcx,4)
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 116
	movaps %xmm3, 48(%rax,%rcx,4)
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 117
	add $16, %rcx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 118
	sub $16, %rdx
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 119
	jnz L4
	.file "nnedi3_dotProd_m32_m16_i16_SSE2.c"
	.line 121
	pop %rbx
	popq	%rbx
	leave
	ret
LFE2:
	.section __TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame1:
	.set L$set$0,LECIE1-LSCIE1
	.long L$set$0
LSCIE1:
	.long	0x0
	.byte	0x1
	.ascii "zR\0"
	.byte	0x1
	.byte	0x78
	.byte	0x10
	.byte	0x1
	.byte	0x10
	.byte	0xc
	.byte	0x7
	.byte	0x8
	.byte	0x90
	.byte	0x1
	.align 3
LECIE1:
.globl _dotProd_m32_m16_i16_SSE2.eh
_dotProd_m32_m16_i16_SSE2.eh:
LSFDE1:
	.set L$set$1,LEFDE1-LASFDE1
	.long L$set$1
LASFDE1:
	.long	LASFDE1-EH_frame1
	.quad	LFB2-.
	.set L$set$2,LFE2-LFB2
	.quad L$set$2
	.byte	0x0
	.byte	0x4
	.set L$set$3,LCFI0-LFB2
	.long L$set$3
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$4,LCFI1-LCFI0
	.long L$set$4
	.byte	0xd
	.byte	0x6
	.byte	0x4
	.set L$set$5,LCFI2-LCFI1
	.long L$set$5
	.byte	0x83
	.byte	0x3
	.align 3
LEFDE1:
	.subsections_via_symbols
