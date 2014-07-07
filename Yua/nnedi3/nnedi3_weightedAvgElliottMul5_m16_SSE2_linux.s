.globl _min_weight_sum
	
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
	.quad	9223372034707292159
	.quad	9223372034707292159
	.text
.globl weightedAvgElliottMul5_m16_SSE2
weightedAvgElliottMul5_m16_SSE2:
LFB2:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	%rdx, -24(%rbp)
	# top of block
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 8
	mov -8(%rbp), %rax
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 9
	movl -12(%rbp), %ecx
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 10
	lea 0(%rax,%rcx,4), %rdx
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 11
	xor %rdi, %rdi
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 12
	xorps %xmm0, %xmm0
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 13
	xorps %xmm1, %xmm1
	L2: # LASM$nloop
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 15
	movaps 0(%rax,%rdi,4), %xmm2
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 16
	movaps 16(%rax,%rdi,4), %xmm3
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 17
	movaps 0(%rdx,%rdi,4), %xmm4
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 18
	movaps 16(%rdx,%rdi,4), %xmm5
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 19
	addps %xmm2, %xmm0
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 20
	movaps %xmm4, %xmm6
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 21
	movaps %xmm5, %xmm7
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 22
	addps %xmm3, %xmm0
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 23
	andps _sign_bits_f2(%rip), %xmm4
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 24
	andps _sign_bits_f2(%rip), %xmm5
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 25
	addps _ones_f2(%rip), %xmm4
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 26
	addps _ones_f2(%rip), %xmm5
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 27
	rcpps %xmm4, %xmm4
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 28
	rcpps %xmm5, %xmm5
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 29
	mulps %xmm4, %xmm6
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 30
	mulps %xmm5, %xmm7
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 31
	mulps %xmm2, %xmm6
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 32
	mulps %xmm3, %xmm7
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 33
	addps %xmm6, %xmm1
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 34
	addps %xmm7, %xmm1
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 35
	movaps 32(%rax,%rdi,4), %xmm2
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 36
	movaps 48(%rax,%rdi,4), %xmm3
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 37
	movaps 32(%rdx,%rdi,4), %xmm4
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 38
	movaps 48(%rdx,%rdi,4), %xmm5
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 39
	addps %xmm2, %xmm0
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 40
	movaps %xmm4, %xmm6
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 41
	movaps %xmm5, %xmm7
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 42
	addps %xmm3, %xmm0
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 43
	andps _sign_bits_f2(%rip), %xmm4
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 44
	andps _sign_bits_f2(%rip), %xmm5
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 45
	addps _ones_f2(%rip), %xmm4
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 46
	addps _ones_f2(%rip), %xmm5
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 47
	rcpps %xmm4, %xmm4
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 48
	rcpps %xmm5, %xmm5
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 49
	mulps %xmm4, %xmm6
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 50
	mulps %xmm5, %xmm7
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 51
	mulps %xmm2, %xmm6
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 52
	mulps %xmm3, %xmm7
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 53
	addps %xmm6, %xmm1
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 54
	addps %xmm7, %xmm1
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 55
	add $16, %rdi
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 56
	sub $16, %rcx
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 57
	jnz L2
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 58
	movhlps %xmm0, %xmm2
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 59
	movhlps %xmm1, %xmm3
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 60
	addps %xmm2, %xmm0
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 61
	addps %xmm3, %xmm1
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 62
	pshuflw $14, %xmm0, %xmm2
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 63
	pshuflw $14, %xmm1, %xmm3
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 64
	addss %xmm2, %xmm0
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 65
	addss %xmm3, %xmm1
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 66
	comiss _min_weight_sum(%rip), %xmm0
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 67
	jbe L3
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 68
	mulss _five_f(%rip), %xmm1
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 69
	rcpss %xmm0, %xmm0
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 70
	mulss %xmm0, %xmm1
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 71
	jmp L4
	L3: # LASM$nodiv
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 73
	xorps %xmm1, %xmm1
	L4: # LASM$finish
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 75
	mov -24(%rbp), %rax
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 76
	mulss 4(%rax), %xmm1
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 77
	addss 0(%rax), %xmm1
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 78
	addss 12(%rax), %xmm1
	.file "nnedi3weightedAvgElliottMul5_m16_SSE2.c"
	.line 79
	movss %xmm1, 12(%rax)
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
.globl weightedAvgElliottMul5_m16_SSE2.eh
weightedAvgElliottMul5_m16_SSE2.eh:
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
	.align 16
LEFDE1:
	
