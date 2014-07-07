	.text
.globl _uc2s64_SSE2
_uc2s64_SSE2:
LFB2:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	%rdx, -24(%rbp)
	# top of block
	.file "nnedi3_uc2s64_SSE2.c"
	.line 3
	mov -8(%rbp), %rax
	.file "nnedi3_uc2s64_SSE2.c"
	.line 4
	movl -12(%rbp), %ecx
	.file "nnedi3_uc2s64_SSE2.c"
	.line 5
	mov -24(%rbp), %rdx
	.file "nnedi3_uc2s64_SSE2.c"
	.line 6
	pxor %xmm7, %xmm7
	.file "nnedi3_uc2s64_SSE2.c"
	.line 7
	movq 0(%rax), %xmm0
	.file "nnedi3_uc2s64_SSE2.c"
	.line 8
	movq 8(%rax), %xmm1
	.file "nnedi3_uc2s64_SSE2.c"
	.line 9
	movq 0(%rax,%rcx,2), %xmm2
	.file "nnedi3_uc2s64_SSE2.c"
	.line 10
	movq 8(%rax,%rcx,2), %xmm3
	.file "nnedi3_uc2s64_SSE2.c"
	.line 11
	punpcklbw %xmm7, %xmm0
	.file "nnedi3_uc2s64_SSE2.c"
	.line 12
	punpcklbw %xmm7, %xmm1
	.file "nnedi3_uc2s64_SSE2.c"
	.line 13
	punpcklbw %xmm7, %xmm2
	.file "nnedi3_uc2s64_SSE2.c"
	.line 14
	punpcklbw %xmm7, %xmm3
	.file "nnedi3_uc2s64_SSE2.c"
	.line 15
	lea 0(%rax,%rcx,4), %rax
	.file "nnedi3_uc2s64_SSE2.c"
	.line 16
	movdqa %xmm0, 0(%rdx)
	.file "nnedi3_uc2s64_SSE2.c"
	.line 17
	movdqa %xmm1, 16(%rdx)
	.file "nnedi3_uc2s64_SSE2.c"
	.line 18
	movdqa %xmm2, 32(%rdx)
	.file "nnedi3_uc2s64_SSE2.c"
	.line 19
	movdqa %xmm3, 48(%rdx)
	.file "nnedi3_uc2s64_SSE2.c"
	.line 20
	movq 0(%rax), %xmm4
	.file "nnedi3_uc2s64_SSE2.c"
	.line 21
	movq 8(%rax), %xmm5
	.file "nnedi3_uc2s64_SSE2.c"
	.line 22
	movq 0(%rax,%rcx,2), %xmm6
	.file "nnedi3_uc2s64_SSE2.c"
	.line 23
	movq 8(%rax,%rcx,2), %xmm0
	.file "nnedi3_uc2s64_SSE2.c"
	.line 24
	punpcklbw %xmm7, %xmm4
	.file "nnedi3_uc2s64_SSE2.c"
	.line 25
	punpcklbw %xmm7, %xmm5
	.file "nnedi3_uc2s64_SSE2.c"
	.line 26
	punpcklbw %xmm7, %xmm6
	.file "nnedi3_uc2s64_SSE2.c"
	.line 27
	punpcklbw %xmm7, %xmm0
	.file "nnedi3_uc2s64_SSE2.c"
	.line 28
	movdqa %xmm4, 64(%rdx)
	.file "nnedi3_uc2s64_SSE2.c"
	.line 29
	movdqa %xmm5, 80(%rdx)
	.file "nnedi3_uc2s64_SSE2.c"
	.line 30
	movdqa %xmm6, 96(%rdx)
	.file "nnedi3_uc2s64_SSE2.c"
	.line 31
	movdqa %xmm0, 112(%rdx)
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
.globl _uc2s64_SSE2.eh
_uc2s64_SSE2.eh:
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
