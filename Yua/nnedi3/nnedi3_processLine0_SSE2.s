.globl _ub_1
	.const
	.align 4
_ub_1:
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
.globl _w_19
	.align 4
_w_19:
	.word	19
	.word	19
	.word	19
	.word	19
	.word	19
	.word	19
	.word	19
	.word	19
.globl _w_3
	.align 4
_w_3:
	.word	3
	.word	3
	.word	3
	.word	3
	.word	3
	.word	3
	.word	3
	.word	3
.globl _w_254
	.align 4
_w_254:
	.word	254
	.word	254
	.word	254
	.word	254
	.word	254
	.word	254
	.word	254
	.word	254
.globl _uw_16
	.align 4
_uw_16:
	.word	16
	.word	16
	.word	16
	.word	16
	.word	16
	.word	16
	.word	16
	.word	16
	.text
.globl _processLine0_SSE2
_processLine0_SSE2:
LFB2:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	pushq	%rbx
LCFI2:
	movq	%rdi, -56(%rbp)
	movl	%esi, -60(%rbp)
	movq	%rdx, -72(%rbp)
	movq	%rcx, -80(%rbp)
	movl	%r8d, -84(%rbp)
	movl	-60(%rbp), %eax
	andl	$15, %eax
	movl	%eax, -24(%rbp)
	movl	-60(%rbp), %eax
	subl	-24(%rbp), %eax
	movl	%eax, -60(%rbp)
	movl	-60(%rbp), %eax
	testl	%eax, %eax
	je	L2
	# top of block
	.file "nnedi3_processLine0_SSE2.c"
	.line 27
	push %rbx
	.file "nnedi3_processLine0_SSE2.c"
	.line 29
	mov -56(%rbp), %rax
	.file "nnedi3_processLine0_SSE2.c"
	.line 30
	mov -80(%rbp), %rbx
	.file "nnedi3_processLine0_SSE2.c"
	.line 31
	movl -60(%rbp), %ecx
	.file "nnedi3_processLine0_SSE2.c"
	.line 32
	movl -84(%rbp), %edx
	.file "nnedi3_processLine0_SSE2.c"
	.line 33
	mov -72(%rbp), %rsi
	.file "nnedi3_processLine0_SSE2.c"
	.line 34
	lea 0(%rbx,%rdx,4), %rdi
	.file "nnedi3_processLine0_SSE2.c"
	.line 35
	pxor %xmm6, %xmm6
	.file "nnedi3_processLine0_SSE2.c"
	.line 36
	pxor %xmm7, %xmm7
	L4: # LASM$xloop
	.file "nnedi3_processLine0_SSE2.c"
	.line 38
	movdqa 0(%rbx,%rdx,2), %xmm0
	.file "nnedi3_processLine0_SSE2.c"
	.line 39
	movdqa 0(%rdi), %xmm1
	.file "nnedi3_processLine0_SSE2.c"
	.line 40
	movdqa %xmm0, %xmm2
	.file "nnedi3_processLine0_SSE2.c"
	.line 41
	movdqa %xmm1, %xmm3
	.file "nnedi3_processLine0_SSE2.c"
	.line 42
	punpcklbw %xmm7, %xmm0
	.file "nnedi3_processLine0_SSE2.c"
	.line 43
	punpckhbw %xmm7, %xmm2
	.file "nnedi3_processLine0_SSE2.c"
	.line 44
	punpcklbw %xmm7, %xmm1
	.file "nnedi3_processLine0_SSE2.c"
	.line 45
	punpckhbw %xmm7, %xmm3
	.file "nnedi3_processLine0_SSE2.c"
	.line 46
	paddw %xmm1, %xmm0
	.file "nnedi3_processLine0_SSE2.c"
	.line 47
	paddw %xmm3, %xmm2
	.file "nnedi3_processLine0_SSE2.c"
	.line 48
	pmullw _w_19(%rip), %xmm0
	.file "nnedi3_processLine0_SSE2.c"
	.line 49
	pmullw _w_19(%rip), %xmm2
	.file "nnedi3_processLine0_SSE2.c"
	.line 50
	movdqa 0(%rbx), %xmm1
	.file "nnedi3_processLine0_SSE2.c"
	.line 51
	movdqa 0(%rdi,%rdx,2), %xmm3
	.file "nnedi3_processLine0_SSE2.c"
	.line 52
	movdqa %xmm1, %xmm4
	.file "nnedi3_processLine0_SSE2.c"
	.line 53
	movdqa %xmm3, %xmm5
	.file "nnedi3_processLine0_SSE2.c"
	.line 54
	punpcklbw %xmm7, %xmm1
	.file "nnedi3_processLine0_SSE2.c"
	.line 55
	punpckhbw %xmm7, %xmm4
	.file "nnedi3_processLine0_SSE2.c"
	.line 56
	punpcklbw %xmm7, %xmm3
	.file "nnedi3_processLine0_SSE2.c"
	.line 57
	punpckhbw %xmm7, %xmm5
	.file "nnedi3_processLine0_SSE2.c"
	.line 58
	paddw %xmm3, %xmm1
	.file "nnedi3_processLine0_SSE2.c"
	.line 59
	paddw %xmm5, %xmm4
	.file "nnedi3_processLine0_SSE2.c"
	.line 60
	pmullw _w_3(%rip), %xmm1
	.file "nnedi3_processLine0_SSE2.c"
	.line 61
	pmullw _w_3(%rip), %xmm4
	.file "nnedi3_processLine0_SSE2.c"
	.line 62
	movdqa 0(%rax), %xmm3
	.file "nnedi3_processLine0_SSE2.c"
	.line 63
	psubusw %xmm1, %xmm0
	.file "nnedi3_processLine0_SSE2.c"
	.line 64
	psubusw %xmm4, %xmm2
	.file "nnedi3_processLine0_SSE2.c"
	.line 65
	pcmpeqb _ub_1(%rip), %xmm3
	.file "nnedi3_processLine0_SSE2.c"
	.line 66
	paddusw _uw_16(%rip), %xmm0
	.file "nnedi3_processLine0_SSE2.c"
	.line 67
	paddusw _uw_16(%rip), %xmm2
	.file "nnedi3_processLine0_SSE2.c"
	.line 68
	movdqa %xmm3, %xmm1
	.file "nnedi3_processLine0_SSE2.c"
	.line 69
	pcmpeqb %xmm4, %xmm4
	.file "nnedi3_processLine0_SSE2.c"
	.line 70
	psrlw $5, %xmm0
	.file "nnedi3_processLine0_SSE2.c"
	.line 71
	psrlw $5, %xmm2
	.file "nnedi3_processLine0_SSE2.c"
	.line 72
	pxor %xmm4, %xmm1
	.file "nnedi3_processLine0_SSE2.c"
	.line 73
	pminsw _w_254(%rip), %xmm0
	.file "nnedi3_processLine0_SSE2.c"
	.line 74
	pminsw _w_254(%rip), %xmm2
	.file "nnedi3_processLine0_SSE2.c"
	.line 75
	movdqa %xmm1, %xmm5
	.file "nnedi3_processLine0_SSE2.c"
	.line 76
	packuswb %xmm2, %xmm0
	.file "nnedi3_processLine0_SSE2.c"
	.line 77
	pand _ub_1(%rip), %xmm5
	.file "nnedi3_processLine0_SSE2.c"
	.line 78
	pand %xmm3, %xmm0
	.file "nnedi3_processLine0_SSE2.c"
	.line 79
	psadbw %xmm7, %xmm5
	.file "nnedi3_processLine0_SSE2.c"
	.line 80
	por %xmm1, %xmm0
	.file "nnedi3_processLine0_SSE2.c"
	.line 81
	movdqa %xmm5, %xmm2
	.file "nnedi3_processLine0_SSE2.c"
	.line 82
	psrldq $8, %xmm5
	.file "nnedi3_processLine0_SSE2.c"
	.line 83
	movdqa %xmm0, 0(%rsi)
	.file "nnedi3_processLine0_SSE2.c"
	.line 84
	paddusw %xmm2, %xmm5
	.file "nnedi3_processLine0_SSE2.c"
	.line 85
	paddusw %xmm5, %xmm6
	.file "nnedi3_processLine0_SSE2.c"
	.line 86
	add $16, %rbx
	.file "nnedi3_processLine0_SSE2.c"
	.line 87
	add $16, %rdi
	.file "nnedi3_processLine0_SSE2.c"
	.line 88
	add $16, %rax
	.file "nnedi3_processLine0_SSE2.c"
	.line 89
	add $16, %rsi
	.file "nnedi3_processLine0_SSE2.c"
	.line 90
	sub $16, %rcx
	.file "nnedi3_processLine0_SSE2.c"
	.line 91
	jnz L4
	.file "nnedi3_processLine0_SSE2.c"
	.line 92
	movd %xmm6, -20(%rbp)
	.file "nnedi3_processLine0_SSE2.c"
	.line 94
	pop %rbx
L2:
	movl	-60(%rbp), %eax
	movl	%eax, -28(%rbp)
	jmp	L5
L6:
	movl	-28(%rbp), %eax
	cltq
	movq	%rax, %rdx
	movq	-56(%rbp), %rax
	leaq	(%rdx,%rax), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	L7
	movl	-28(%rbp), %eax
	cltq
	movq	%rax, %rdx
	movq	-72(%rbp), %rax
	leaq	(%rdx,%rax), %rsi
	movl	-84(%rbp), %eax
	addl	%eax, %eax
	addl	-28(%rbp), %eax
	cltq
	movq	%rax, %rdx
	movq	-80(%rbp), %rax
	leaq	(%rdx,%rax), %rax
	movzbl	(%rax), %eax
	movzbl	%al, %ecx
	movl	-84(%rbp), %eax
	sall	$2, %eax
	addl	-28(%rbp), %eax
	cltq
	movq	%rax, %rdx
	movq	-80(%rbp), %rax
	leaq	(%rdx,%rax), %rax
	movzbl	(%rax), %eax
	movzbl	%al, %eax
	leal	(%rcx,%rax), %edx
	movl	%edx, %eax
	sall	$3, %eax
	addl	%edx, %eax
	addl	%eax, %eax
	leal	(%rax,%rdx), %ecx
	movl	-28(%rbp), %eax
	cltq
	movq	%rax, %rdx
	movq	-80(%rbp), %rax
	leaq	(%rdx,%rax), %rax
	movzbl	(%rax), %eax
	movzbl	%al, %edi
	movl	-84(%rbp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	addl	%eax, %eax
	addl	-28(%rbp), %eax
	cltq
	movq	%rax, %rdx
	movq	-80(%rbp), %rax
	leaq	(%rdx,%rax), %rax
	movzbl	(%rax), %eax
	movzbl	%al, %eax
	leal	(%rdi,%rax), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	movl	%ecx, %edx
	subl	%eax, %edx
	movl	%edx, %eax
	addl	$16, %eax
	sarl	$5, %eax
	movl	%eax, -36(%rbp)
	movl	$254, -40(%rbp)
	movl	-36(%rbp), %eax
	cmpl	%eax, -40(%rbp)
	cmovle	-40(%rbp), %eax
	movl	%eax, -32(%rbp)
	movl	$0, -44(%rbp)
	movl	-32(%rbp), %eax
	cmpl	%eax, -44(%rbp)
	cmovge	-44(%rbp), %eax
	movb	%al, (%rsi)
	jmp	L9
L7:
	movl	-28(%rbp), %eax
	cltq
	movq	%rax, %rdx
	movq	-72(%rbp), %rax
	leaq	(%rdx,%rax), %rax
	movb	$-1, (%rax)
	movl	-20(%rbp), %eax
	incl	%eax
	movl	%eax, -20(%rbp)
L9:
	incl	-28(%rbp)
L5:
	movl	-60(%rbp), %eax
	addl	-24(%rbp), %eax
	cmpl	-28(%rbp), %eax
	jg	L6
	movl	-20(%rbp), %eax
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
.globl _processLine0_SSE2.eh
_processLine0_SSE2.eh:
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
