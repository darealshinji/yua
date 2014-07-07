.globl _min_weight_sum
	.data
	.align 16
_min_weight_sum:
	.long	786163455
	.long	786163455
	.long	786163455
	.long	786163455
.globl _five_f
	.align 16
_five_f:
	.long	1084227584
	.long	1084227584
	.long	1084227584
	.long	1084227584
.globl _ones_f2
	.align 16
_ones_f2:
	.long	1065353216
	.long	1065353216
	.long	1065353216
	.long	1065353216
.globl _sign_bits_f2
	.align 16
_sign_bits_f2:
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.long	2147483647
	.text
.globl _weightedAvgElliottMul5_m16_SSE2
_weightedAvgElliottMul5_m16_SSE2:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	# top of block
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 8
	pushl %edi
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 9
	movl 8(%ebp), %eax
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 10
	movl 12(%ebp), %ecx
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 11
	leal 0(%eax,%ecx,4), %edx
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 12
	xorl %edi, %edi
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 13
	xorps %xmm0, %xmm0
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 14
	xorps %xmm1, %xmm1
	L2: # LASM$nloop
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 16
	movaps 0(%eax,%edi,4), %xmm2
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 17
	movaps 16(%eax,%edi,4), %xmm3
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 18
	movaps 0(%edx,%edi,4), %xmm4
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 19
	movaps 16(%edx,%edi,4), %xmm5
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 20
	addps %xmm2, %xmm0
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 21
	movaps %xmm4, %xmm6
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 22
	movaps %xmm5, %xmm7
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 23
	addps %xmm3, %xmm0
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 24
	andps _sign_bits_f2, %xmm4
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 25
	andps _sign_bits_f2, %xmm5
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 26
	addps _ones_f2, %xmm4
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 27
	addps _ones_f2, %xmm5
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 28
	rcpps %xmm4, %xmm4
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 29
	rcpps %xmm5, %xmm5
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 30
	mulps %xmm4, %xmm6
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 31
	mulps %xmm5, %xmm7
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 32
	mulps %xmm2, %xmm6
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 33
	mulps %xmm3, %xmm7
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 34
	addps %xmm6, %xmm1
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 35
	addps %xmm7, %xmm1
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 36
	movaps 32(%eax,%edi,4), %xmm2
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 37
	movaps 48(%eax,%edi,4), %xmm3
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 38
	movaps 32(%edx,%edi,4), %xmm4
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 39
	movaps 48(%edx,%edi,4), %xmm5
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 40
	addps %xmm2, %xmm0
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 41
	movaps %xmm4, %xmm6
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 42
	movaps %xmm5, %xmm7
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 43
	addps %xmm3, %xmm0
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 44
	andps _sign_bits_f2, %xmm4
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 45
	andps _sign_bits_f2, %xmm5
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 46
	addps _ones_f2, %xmm4
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 47
	addps _ones_f2, %xmm5
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 48
	rcpps %xmm4, %xmm4
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 49
	rcpps %xmm5, %xmm5
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 50
	mulps %xmm4, %xmm6
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 51
	mulps %xmm5, %xmm7
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 52
	mulps %xmm2, %xmm6
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 53
	mulps %xmm3, %xmm7
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 54
	addps %xmm6, %xmm1
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 55
	addps %xmm7, %xmm1
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 56
	addl $16, %edi
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 57
	subl $16, %ecx
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 58
	jnz L2
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 59
	movhlps %xmm0, %xmm2
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 60
	movhlps %xmm1, %xmm3
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 61
	addps %xmm2, %xmm0
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 62
	addps %xmm3, %xmm1
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 63
	pshuflw $14, %xmm0, %xmm2
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 64
	pshuflw $14, %xmm1, %xmm3
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 65
	addss %xmm2, %xmm0
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 66
	addss %xmm3, %xmm1
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 67
	comiss _min_weight_sum, %xmm0
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 68
	jbe L3
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 69
	mulss _five_f, %xmm1
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 70
	rcpss %xmm0, %xmm0
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 71
	mulss %xmm0, %xmm1
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 72
	jmp L4
	L3: # LASM$nodiv
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 74
	xorps %xmm1, %xmm1
	L4: # LASM$finish
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 76
	movl 16(%ebp), %eax
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 77
	mulss 4(%eax), %xmm1
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 78
	addss 0(%eax), %xmm1
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 79
	addss 12(%eax), %xmm1
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 80
	movss %xmm1, 12(%eax)
	.file "nnedi3_weightedAvgElliottMul5_m16_SSE2_32.c"
	.line 81
	popl %edi
	popl	%esi
	popl	%edi
	leave
	ret
	
