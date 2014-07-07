.globl _ub_1
	.data
	.align 16
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
	.align 16
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
	.align 16
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
	.align 16
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
	.align 16
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
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$44, %esp
	call	L12

L12:
	popl	%ebx
	movl	12(%ebp), %eax
	andl	$15, %eax
	movl	%eax, -32(%ebp)
	movl	12(%ebp), %eax
	subl	-32(%ebp), %eax
	movl	%eax, 12(%ebp)
	movl	12(%ebp), %eax
	testl	%eax, %eax
	je	L2
	# top of block
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 27
	movl 8(%ebp), %eax
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 28
	movl 20(%ebp), %ebx
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 29
	movl 12(%ebp), %ecx
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 30
	movl 24(%ebp), %edx
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 31
	movl 16(%ebp), %esi
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 32
	leal 0(%ebx,%edx,4), %edi
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 33
	pxor %xmm6, %xmm6
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 34
	pxor %xmm7, %xmm7
	L4: # LASM$xloop
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 36
	movdqa 0(%ebx,%edx,2), %xmm0
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 37
	movdqa 0(%edi), %xmm1
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 38
	movdqa %xmm0, %xmm2
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 39
	movdqa %xmm1, %xmm3
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 40
	punpcklbw %xmm7, %xmm0
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 41
	punpckhbw %xmm7, %xmm2
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 42
	punpcklbw %xmm7, %xmm1
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 43
	punpckhbw %xmm7, %xmm3
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 44
	paddw %xmm1, %xmm0
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 45
	paddw %xmm3, %xmm2
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 46
	pmullw _w_19, %xmm0
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 47
	pmullw _w_19, %xmm2
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 48
	movdqa 0(%ebx), %xmm1
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 49
	movdqa 0(%edi,%edx,2), %xmm3
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 50
	movdqa %xmm1, %xmm4
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 51
	movdqa %xmm3, %xmm5
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 52
	punpcklbw %xmm7, %xmm1
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 53
	punpckhbw %xmm7, %xmm4
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 54
	punpcklbw %xmm7, %xmm3
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 55
	punpckhbw %xmm7, %xmm5
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 56
	paddw %xmm3, %xmm1
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 57
	paddw %xmm5, %xmm4
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 58
	pmullw _w_3, %xmm1
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 59
	pmullw _w_3, %xmm4
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 60
	movdqa 0(%eax), %xmm3
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 61
	psubusw %xmm1, %xmm0
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 62
	psubusw %xmm4, %xmm2
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 63
	pcmpeqb _ub_1, %xmm3
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 64
	paddusw _uw_16, %xmm0
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 65
	paddusw _uw_16, %xmm2
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 66
	movdqa %xmm3, %xmm1
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 67
	pcmpeqb %xmm4, %xmm4
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 68
	psrlw $5, %xmm0
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 69
	psrlw $5, %xmm2
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 70
	pxor %xmm4, %xmm1
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 71
	pminsw _w_254, %xmm0
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 72
	pminsw _w_254, %xmm2
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 73
	movdqa %xmm1, %xmm5
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 74
	packuswb %xmm2, %xmm0
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 75
	pand _ub_1, %xmm5
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 76
	pand %xmm3, %xmm0
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 77
	psadbw %xmm7, %xmm5
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 78
	por %xmm1, %xmm0
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 79
	movdqa %xmm5, %xmm2
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 80
	psrldq $8, %xmm5
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 81
	movdqa %xmm0, 0(%esi)
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 82
	paddusw %xmm2, %xmm5
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 83
	paddusw %xmm5, %xmm6
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 84
	addl $16, %ebx
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 85
	addl $16, %edi
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 86
	addl $16, %eax
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 87
	addl $16, %esi
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 88
	subl $16, %ecx
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 89
	jnz L4
	.file "nnedi3_processLine0_SSE2_32.c"
	.line 90
	movd %xmm6, -28(%ebp)
L2:
	movl	12(%ebp), %eax
	movl	%eax, -36(%ebp)
	jmp	L5
L6:
	movl	8(%ebp), %edx
	movl	-36(%ebp), %eax
	leal	(%edx,%eax), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	je	L7
	movl	16(%ebp), %edx
	movl	-36(%ebp), %eax
	leal	(%edx,%eax), %esi
	movl	24(%ebp), %eax
	addl	%eax, %eax
	addl	-36(%ebp), %eax
	movl	%eax, %edx
	movl	20(%ebp), %eax
	leal	(%edx,%eax), %eax
	movzbl	(%eax), %eax
	movzbl	%al, %ecx
	movl	24(%ebp), %eax
	sall	$2, %eax
	addl	-36(%ebp), %eax
	movl	%eax, %edx
	movl	20(%ebp), %eax
	leal	(%edx,%eax), %eax
	movzbl	(%eax), %eax
	movzbl	%al, %eax
	leal	(%ecx,%eax), %edx
	movl	%edx, %eax
	sall	$3, %eax
	addl	%edx, %eax
	addl	%eax, %eax
	leal	(%eax,%edx), %ecx
	movl	20(%ebp), %edx
	movl	-36(%ebp), %eax
	leal	(%edx,%eax), %eax
	movzbl	(%eax), %eax
	movzbl	%al, %edi
	movl	24(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	addl	%eax, %eax
	addl	-36(%ebp), %eax
	movl	%eax, %edx
	movl	20(%ebp), %eax
	leal	(%edx,%eax), %eax
	movzbl	(%eax), %eax
	movzbl	%al, %eax
	leal	(%edi,%eax), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	movl	%ecx, %edx
	subl	%eax, %edx
	movl	%edx, %eax
	addl	$16, %eax
	sarl	$5, %eax
	movl	%eax, -44(%ebp)
	movl	$254, -48(%ebp)
	movl	-44(%ebp), %eax
	cmpl	%eax, -48(%ebp)
	cmovle	-48(%ebp), %eax
	movl	%eax, -40(%ebp)
	movl	$0, -52(%ebp)
	movl	-40(%ebp), %eax
	cmpl	%eax, -52(%ebp)
	cmovge	-52(%ebp), %eax
	movb	%al, (%esi)
	jmp	L9
L7:
	movl	16(%ebp), %edx
	movl	-36(%ebp), %eax
	leal	(%edx,%eax), %eax
	movb	$-1, (%eax)
	movl	-28(%ebp), %eax
	incl	%eax
	movl	%eax, -28(%ebp)
L9:
	incl	-36(%ebp)
L5:
	movl	12(%ebp), %eax
	addl	-32(%ebp), %eax
	cmpl	-36(%ebp), %eax
	jg	L6
	movl	-28(%ebp), %eax
	addl	$44, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	leave
	ret
	
