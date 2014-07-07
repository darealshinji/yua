.globl _sse_half
	.const
	.align 4
_sse_half:
	.long	1056964608
	.long	1056964608
	.long	1056964608
	.long	1056964608
	.text
.globl _castScale_SSE
_castScale_SSE:
LFB2:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	# top of block
	.file "nnedi3_castScale_SSE.c"
	.line 5
	mov -8(%rbp), %rcx
	.file "nnedi3_castScale_SSE.c"
	.line 6
	mov -16(%rbp), %rax
	.file "nnedi3_castScale_SSE.c"
	.line 7
	movss 12(%rcx), %xmm0
	.file "nnedi3_castScale_SSE.c"
	.line 8
	mulss 0(%rax), %xmm0
	.file "nnedi3_castScale_SSE.c"
	.line 9
	addss _sse_half(%rip), %xmm0
	.file "nnedi3_castScale_SSE.c"
	.line 10
	cvttss2si %xmm0, %rax
	.file "nnedi3_castScale_SSE.c"
	.line 11
	mov -24(%rbp), %rcx
	.file "nnedi3_castScale_SSE.c"
	.line 12
	cmp $255, %rax
	.file "nnedi3_castScale_SSE.c"
	.line 13
	jl L2
	.file "nnedi3_castScale_SSE.c"
	.line 14
	mov $255, %rax
	.file "nnedi3_castScale_SSE.c"
	.line 15
	jmp L3
	L2: # LASM$b255
	.file "nnedi3_castScale_SSE.c"
	.line 17
	cmp $0, %rax
	.file "nnedi3_castScale_SSE.c"
	.line 18
	jge L3
	.file "nnedi3_castScale_SSE.c"
	.line 19
	xor %rax, %rax
	L3: # LASM$finish
	.file "nnedi3_castScale_SSE.c"
	.line 21
	movb %al, 0(%rcx)
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
.globl _castScale_SSE.eh
_castScale_SSE.eh:
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
