.globl _flt_epsilon_sse
	
	.align 16
_flt_epsilon_sse:
	.long	872415232
	.long	872415232
	.long	872415232
	.long	872415232
	.text
.globl extract_m8_i16_SSE2
extract_m8_i16_SSE2:
LFB2:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	pushq	%rbx
LCFI2:
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movl	%edx, -32(%rbp)
	movl	%ecx, -36(%rbp)
	movq	%r8, -48(%rbp)
	movq	%r9, -56(%rbp)
	# top of block
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 7
	mov -24(%rbp), %rax
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 8
	movl -28(%rbp), %ebx
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 9
	movl -32(%rbp), %edi
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 10
	movl -36(%rbp), %edx
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 11
	mov -56(%rbp), %rsp
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 13
	lea 0(%rax,%rbx,2), %rsi
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 14
	pxor %xmm4, %xmm4
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 15
	pxor %xmm5, %xmm5
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 16
	pxor %xmm6, %xmm6
	L2: # LASM$yloop
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 18
	xor %rcx, %rcx
	L3: # LASM$xloop
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 20
	movq 0(%rcx,%rax), %xmm0
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 21
	movq 0(%rcx,%rsi), %xmm1
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 22
	movdqa %xmm0, %xmm2
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 23
	movdqa %xmm1, %xmm3
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 24
	punpcklbw %xmm6, %xmm0
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 25
	punpcklbw %xmm6, %xmm1
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 26
	psadbw %xmm6, %xmm2
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 27
	psadbw %xmm6, %xmm3
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 28
	movdqa %xmm0, 0(%rsp)
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 29
	movdqa %xmm1, 0(%rsp,%rdi,2)
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 30
	pmaddwd %xmm0, %xmm0
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 31
	pmaddwd %xmm1, %xmm1
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 32
	paddd %xmm2, %xmm4
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 33
	paddd %xmm0, %xmm5
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 34
	paddd %xmm3, %xmm4
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 35
	paddd %xmm1, %xmm5
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 36
	add $8, %rcx
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 37
	add $16, %rsp
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 38
	cmp %rdi, %rcx
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 39
	jl L3
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 41
	lea 0(%rax,%rbx,4), %rax
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 42
	lea 0(%rsi,%rbx,4), %rsi
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 43
	lea 0(%rsp,%rdi,2), %rsp
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 44
	subl $2, %edx
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 45
	jnz L2
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 47
	movhlps %xmm5, %xmm1
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 48
	movl -36(%rbp), %eax
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 49
	paddd %xmm1, %xmm5
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 50
	mul %rdi
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 51
	pshuflw $14, %xmm5, %xmm1
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 52
	cvtsi2ss %rax, %xmm7
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 53
	paddd %xmm1, %xmm5
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 54
	rcpss %xmm7, %xmm7
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 55
	cvtdq2ps %xmm4, %xmm4
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 56
	cvtdq2ps %xmm5, %xmm5
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 57
	mov -48(%rbp), %rax
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 58
	mulss %xmm7, %xmm4
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 59
	mulss %xmm7, %xmm5
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 60
	movss %xmm4, 0(%rax)
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 61
	mulss %xmm4, %xmm4
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 62
	subss %xmm4, %xmm5
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 63
	comiss _flt_epsilon_sse(%rip), %xmm5
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 64
	jbe L4
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 65
	rsqrtss %xmm5, %xmm5
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 66
	rcpss %xmm5, %xmm4
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 67
	movss %xmm4, 4(%rax)
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 68
	movss %xmm5, 8(%rax)
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 69
	jmp L5
	L4: # LASM$novarjmp
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 71
	movss %xmm6, 4(%rax)
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 72
	movss %xmm6, 8(%rax)
	L5: # LASM$finish
	.file "nnedi3extract_m8_i16_SSE2.c"
	.line 74
	movss %xmm6, 12(%rax)
	popq	%rbx
	leave
	ret
LFE2:
	.section __TEXT
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
	.align 16
LECIE1:
.globl extract_m8_i16_SSE2.eh
extract_m8_i16_SSE2.eh:
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
	.align 16
LEFDE1:
	
