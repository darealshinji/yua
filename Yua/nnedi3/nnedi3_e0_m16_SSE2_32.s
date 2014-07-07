.globl _e0_mult
	.data
	.align 16
_e0_mult:
	.long	1262004795
	.long	1262004795
	.long	1262004795
	.long	1262004795
.globl _e0_bias
	.align 16
_e0_bias:
	.long	1316872784
	.long	1316872784
	.long	1316872784
	.long	1316872784
.globl _exp_lo
	.align 16
_exp_lo:
	.long	3265265664
	.long	3265265664
	.long	3265265664
	.long	3265265664
.globl _exp_hi
	.align 16
_exp_hi:
	.long	1117782016
	.long	1117782016
	.long	1117782016
	.long	1117782016
	.text
.globl _e0_m16_SSE2
_e0_m16_SSE2:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	# top of block
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 11
	movl 8(%ebp), %eax
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 12
	movl 12(%ebp), %ecx
	L2: # LASM$eloop16
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 14
	movaps 0(%eax), %xmm0
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 15
	movaps 16(%eax), %xmm1
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 16
	movaps 32(%eax), %xmm2
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 17
	movaps 48(%eax), %xmm3
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 18
	minps _exp_hi, %xmm0
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 19
	minps _exp_hi, %xmm1
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 20
	minps _exp_hi, %xmm2
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 21
	minps _exp_hi, %xmm3
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 22
	maxps _exp_lo, %xmm0
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 23
	maxps _exp_lo, %xmm1
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 24
	maxps _exp_lo, %xmm2
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 25
	maxps _exp_lo, %xmm3
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 26
	mulps _e0_mult, %xmm0
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 27
	mulps _e0_mult, %xmm1
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 28
	mulps _e0_mult, %xmm2
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 29
	mulps _e0_mult, %xmm3
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 30
	addps _e0_bias, %xmm0
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 31
	addps _e0_bias, %xmm1
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 32
	addps _e0_bias, %xmm2
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 33
	addps _e0_bias, %xmm3
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 34
	cvtps2dq %xmm0, %xmm0
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 35
	cvtps2dq %xmm1, %xmm1
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 36
	cvtps2dq %xmm2, %xmm2
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 37
	cvtps2dq %xmm3, %xmm3
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 38
	movaps %xmm0, 0(%eax)
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 39
	movaps %xmm1, 16(%eax)
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 40
	movaps %xmm2, 32(%eax)
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 41
	movaps %xmm3, 48(%eax)
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 42
	addl $64, %eax
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 43
	subl $16, %ecx
	.file "nnedi3_e0_m16_SSE2_32.c"
	.line 44
	jnz L2
	popl	%esi
	popl	%edi
	leave
	ret
	
