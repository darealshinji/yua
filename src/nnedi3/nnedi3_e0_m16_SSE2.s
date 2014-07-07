.globl _e0_mult
	.const
	.align 4
_e0_mult:
	.long	1262004795
	.long	1262004795
	.long	1262004795
	.long	1262004795
.globl _e0_bias
	.align 4
_e0_bias:
	.long	1316872784
	.long	1316872784
	.long	1316872784
	.long	1316872784
.globl _exp_lo
	.align 4
_exp_lo:
	.long	3265265664
	.long	3265265664
	.long	3265265664
	.long	3265265664
.globl _exp_hi
	.align 4
_exp_hi:
	.long	1117782016
	.long	1117782016
	.long	1117782016
	.long	1117782016
	.text
.globl _e0_m16_SSE2
_e0_m16_SSE2:
LFB2:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	# top of block
	.file "nnedi3_e0_m16_SSE2.c"
	.line 11
	mov -8(%rbp), %rax
	.file "nnedi3_e0_m16_SSE2.c"
	.line 12
	movl -12(%rbp), %ecx
	L2: # LASM$eloop16
	.file "nnedi3_e0_m16_SSE2.c"
	.line 14
	movaps 0(%rax), %xmm0
	.file "nnedi3_e0_m16_SSE2.c"
	.line 15
	movaps 16(%rax), %xmm1
	.file "nnedi3_e0_m16_SSE2.c"
	.line 16
	movaps 32(%rax), %xmm2
	.file "nnedi3_e0_m16_SSE2.c"
	.line 17
	movaps 48(%rax), %xmm3
	.file "nnedi3_e0_m16_SSE2.c"
	.line 18
	minps _exp_hi(%rip), %xmm0
	.file "nnedi3_e0_m16_SSE2.c"
	.line 19
	minps _exp_hi(%rip), %xmm1
	.file "nnedi3_e0_m16_SSE2.c"
	.line 20
	minps _exp_hi(%rip), %xmm2
	.file "nnedi3_e0_m16_SSE2.c"
	.line 21
	minps _exp_hi(%rip), %xmm3
	.file "nnedi3_e0_m16_SSE2.c"
	.line 22
	maxps _exp_lo(%rip), %xmm0
	.file "nnedi3_e0_m16_SSE2.c"
	.line 23
	maxps _exp_lo(%rip), %xmm1
	.file "nnedi3_e0_m16_SSE2.c"
	.line 24
	maxps _exp_lo(%rip), %xmm2
	.file "nnedi3_e0_m16_SSE2.c"
	.line 25
	maxps _exp_lo(%rip), %xmm3
	.file "nnedi3_e0_m16_SSE2.c"
	.line 26
	mulps _e0_mult(%rip), %xmm0
	.file "nnedi3_e0_m16_SSE2.c"
	.line 27
	mulps _e0_mult(%rip), %xmm1
	.file "nnedi3_e0_m16_SSE2.c"
	.line 28
	mulps _e0_mult(%rip), %xmm2
	.file "nnedi3_e0_m16_SSE2.c"
	.line 29
	mulps _e0_mult(%rip), %xmm3
	.file "nnedi3_e0_m16_SSE2.c"
	.line 30
	addps _e0_bias(%rip), %xmm0
	.file "nnedi3_e0_m16_SSE2.c"
	.line 31
	addps _e0_bias(%rip), %xmm1
	.file "nnedi3_e0_m16_SSE2.c"
	.line 32
	addps _e0_bias(%rip), %xmm2
	.file "nnedi3_e0_m16_SSE2.c"
	.line 33
	addps _e0_bias(%rip), %xmm3
	.file "nnedi3_e0_m16_SSE2.c"
	.line 34
	cvtps2dq %xmm0, %xmm0
	.file "nnedi3_e0_m16_SSE2.c"
	.line 35
	cvtps2dq %xmm1, %xmm1
	.file "nnedi3_e0_m16_SSE2.c"
	.line 36
	cvtps2dq %xmm2, %xmm2
	.file "nnedi3_e0_m16_SSE2.c"
	.line 37
	cvtps2dq %xmm3, %xmm3
	.file "nnedi3_e0_m16_SSE2.c"
	.line 38
	movaps %xmm0, 0(%rax)
	.file "nnedi3_e0_m16_SSE2.c"
	.line 39
	movaps %xmm1, 16(%rax)
	.file "nnedi3_e0_m16_SSE2.c"
	.line 40
	movaps %xmm2, 32(%rax)
	.file "nnedi3_e0_m16_SSE2.c"
	.line 41
	movaps %xmm3, 48(%rax)
	.file "nnedi3_e0_m16_SSE2.c"
	.line 42
	add $64, %rax
	.file "nnedi3_e0_m16_SSE2.c"
	.line 43
	sub $16, %rcx
	.file "nnedi3_e0_m16_SSE2.c"
	.line 44
	jnz L2
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
.globl _e0_m16_SSE2.eh
_e0_m16_SSE2.eh:
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
	.align 3
LEFDE1:
	.subsections_via_symbols
