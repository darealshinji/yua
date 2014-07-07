.globl _flt_epsilon_sse
	.data
	.align 16
_flt_epsilon_sse:
	.long	872415232
	.long	872415232
	.long	872415232
	.long	872415232
	.text
.globl _extract_m8_i16_SSE2
_extract_m8_i16_SSE2:
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$12, %esp
	call	L7

L7:
	popl	%ebx
	# top of block
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 7
	movl 32(%esp), %eax
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 8
	movl 36(%esp), %ebx
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 9
	movl 40(%esp), %edi
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 10
	movl 44(%esp), %ebp
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 11
	movl 52(%esp), %edx
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 12
	leal 0(%eax,%ebx,2), %esi
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 13
	pxor %xmm4, %xmm4
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 14
	pxor %xmm5, %xmm5
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 15
	pxor %xmm6, %xmm6
	L2: # LASM$yloop
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 17
	xorl %ecx, %ecx
	L3: # LASM$xloop
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 19
	movq 0(%ecx,%eax), %xmm0
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 20
	movq 0(%ecx,%esi), %xmm1
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 21
	movdqa %xmm0, %xmm2
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 22
	movdqa %xmm1, %xmm3
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 23
	punpcklbw %xmm6, %xmm0
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 24
	punpcklbw %xmm6, %xmm1
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 25
	psadbw %xmm6, %xmm2
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 26
	psadbw %xmm6, %xmm3
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 27
	movdqa %xmm0, 0(%edx)
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 28
	movdqa %xmm1, 0(%edx,%edi,2)
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 29
	pmaddwd %xmm0, %xmm0
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 30
	pmaddwd %xmm1, %xmm1
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 31
	paddd %xmm2, %xmm4
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 32
	paddd %xmm0, %xmm5
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 33
	paddd %xmm3, %xmm4
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 34
	paddd %xmm1, %xmm5
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 35
	addl $8, %ecx
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 36
	addl $16, %edx
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 37
	cmpl %edi, %ecx
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 38
	jl L3
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 39
	leal 0(%eax,%ebx,4), %eax
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 40
	leal 0(%esi,%ebx,4), %esi
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 41
	leal 0(%edx,%edi,2), %edx
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 42
	subl $2, %ebp
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 43
	jnz L2
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 44
	movhlps %xmm5, %xmm1
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 45
	movl 44(%esp), %eax
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 46
	paddd %xmm1, %xmm5
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 47
	mull %edi
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 48
	pshuflw $14, %xmm5, %xmm1
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 49
	cvtsi2ss %eax, %xmm7
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 50
	paddd %xmm1, %xmm5
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 51
	rcpss %xmm7, %xmm7
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 52
	cvtdq2ps %xmm4, %xmm4
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 53
	cvtdq2ps %xmm5, %xmm5
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 54
	movl 48(%esp), %eax
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 55
	mulss %xmm7, %xmm4
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 56
	mulss %xmm7, %xmm5
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 57
	movss %xmm4, 0(%eax)
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 58
	mulss %xmm4, %xmm4
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 59
	subss %xmm4, %xmm5
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 60
	comiss _flt_epsilon_sse, %xmm5
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 61
	jbe L4
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 62
	rsqrtss %xmm5, %xmm5
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 63
	rcpss %xmm5, %xmm4
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 64
	movss %xmm4, 4(%eax)
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 65
	movss %xmm5, 8(%eax)
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 66
	jmp L5
	L4: # LASM$novarjmp
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 68
	movss %xmm6, 4(%eax)
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 69
	movss %xmm6, 8(%eax)
	L5: # LASM$finish
	.file "nnedi3_extract_m8_i16_SSE2_32.c"
	.line 71
	movss %xmm6, 12(%eax)
	addl	$12, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	
