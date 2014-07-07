	.text
	.align 1,0x90
	.align 4,0x90
.globl __ZN13NNEDI3_Worker6step_2Ev
__ZN13NNEDI3_Worker6step_2Ev:
LFB9336:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	pushq	%r15
LCFI2:
	pushq	%r14
LCFI3:
	pushq	%r13
LCFI4:
	pushq	%r12
LCFI5:
	pushq	%rbx
LCFI6:
	subq	$216, %rsp
LCFI7:
	movq	%rdi, -200(%rbp)
	movq	16(%rdi), %rdx
	movq	192(%rdx), %rax
	movq	%rax, -192(%rbp)
	movq	200(%rdx), %r15
	movq	216(%rdx), %rcx
	movq	%rcx, -184(%rbp)
	movl	184(%rdx), %eax
	movl	%eax, -176(%rbp)
	movl	188(%rdx), %ecx
	movl	%ecx, -172(%rbp)
	movl	224(%rdx), %eax
	movl	%eax, -168(%rbp)
	movl	228(%rdx), %ecx
	movl	%ecx, -164(%rbp)
	movl	232(%rdx), %eax
	movl	%eax, -160(%rbp)
	movl	236(%rdx), %ecx
	movl	%ecx, -156(%rbp)
	cvtsi2ss	-172(%rbp), %xmm0
	movss	LC0(%rip), %xmm1
	divss	%xmm0, %xmm1
	movss	%xmm1, -52(%rbp)
	decl	%ecx
	movl	%ecx, -132(%rbp)
	sarl	%eax
	decl	%eax
	cltq
	movq	%rax, -128(%rbp)
	movl	-164(%rbp), %eax
	addl	%eax, %eax
	movl	%eax, -116(%rbp)
	movl	$0, -152(%rbp)
	leaq	-80(%rbp), %rcx
	movq	%rcx, -216(%rbp)
	leaq	-60(%rbp), %rax
	movq	%rax, -224(%rbp)
	leaq	-56(%rbp), %rcx
	movq	%rcx, -232(%rbp)
	leaq	-64(%rbp), %rax
	movq	%rax, -240(%rbp)
	leaq	-52(%rbp), %rcx
	movq	%rcx, -248(%rbp)
	movq	-216(%rbp), %rax
	addq	$8, %rax
	movq	%rax, -256(%rbp)
L2:
	movslq	-152(%rbp),%rax
	movq	16(%rdx,%rax,8), %r8
	movl	64(%rdx,%rax,4), %ecx
	movl	%ecx, -148(%rbp)
	movl	100(%rdx,%rax,4), %esi
	movq	40(%rdx,%rax,8), %r9
	movl	76(%rdx,%rax,4), %edi
	movl	136(%rdx,%rax,4), %ecx
	movl	148(%rdx,%rax,4), %eax
	movl	%eax, -144(%rbp)
	cmpl	%eax, %ecx
	jge	L3
	subl	$32, %esi
	movl	%esi, -120(%rbp)
	movl	-148(%rbp), %eax
	addl	%eax, %eax
	cltq
	movq	%rax, -112(%rbp)
	leal	6(%rcx), %eax
	imull	-148(%rbp), %eax
	cltq
	addq	%r8, %rax
	movl	-148(%rbp), %edx
	imull	-132(%rbp), %edx
	movslq	%edx,%rdx
	subq	%rdx, %rax
	subq	-128(%rbp), %rax
	addq	$32, %rax
	movq	%rax, -88(%rbp)
	leal	(%rdi,%rdi), %eax
	cltq
	movq	%rax, -104(%rbp)
	movl	%ecx, %eax
	imull	%edi, %eax
	cltq
	addq	%rax, %r9
	movq	%r9, -96(%rbp)
	movl	%ecx, -140(%rbp)
	movl	%esi, -204(%rbp)
L5:
	cmpl	$32, -120(%rbp)
	jle	L20
	movq	-96(%rbp), %r13
	movq	-88(%rbp), %r14
	movl	$32, -136(%rbp)
	jmp	L7
	.align 4,0x90
L8:
	incl	-136(%rbp)
	incq	%r13
	incq	%r14
	movl	-204(%rbp), %eax
	cmpl	%eax, -136(%rbp)
	je	L20
L7:
	cmpb	$-1, (%r13)
	jne	L8
	movq	-192(%rbp), %r9
	movq	-216(%rbp), %r8
	movl	-156(%rbp), %ecx
	movl	-160(%rbp), %edx
	movl	-148(%rbp), %esi
	movq	%r14, %rdi
	call	_extract_m8_i16_SSE2
	movl	-172(%rbp), %eax
	testl	%eax, %eax
	jle	L10
	movq	-184(%rbp), %rbx
	xorl	%r12d, %r12d
	.align 4,0x90
L12:
	movq	-256(%rbp), %r9
	movl	-168(%rbp), %r8d
	movl	-116(%rbp), %ecx
	movq	%r15, %rdx
	movq	(%rbx), %rsi
	movq	-192(%rbp), %rdi
	call	_dotProd_m32_m16_i16_SSE2
	movl	-164(%rbp), %esi
	movq	%r15, %rdi
	call	_e0_m16_SSE2
	movq	-216(%rbp), %rdx
	movl	-164(%rbp), %esi
	movq	%r15, %rdi
	call	_weightedAvgElliottMul5_m16_SSE2
	incl	%r12d
	addq	$8, %rbx
	cmpl	-172(%rbp), %r12d
	jne	L12
L10:
	cmpl	$1, -176(%rbp)
	jle	L13
	movq	%r13, %rdx
	movq	-248(%rbp), %rsi
	movq	-216(%rbp), %rdi
	call	_castScale_SSE
	jmp	L8
L20:
	addl	$2, -140(%rbp)
	movq	-112(%rbp), %rcx
	addq	%rcx, -88(%rbp)
	movq	-104(%rbp), %rax
	addq	%rax, -96(%rbp)
	movl	-140(%rbp), %ecx
	cmpl	%ecx, -144(%rbp)
	jg	L5
L3:
	incl	-152(%rbp)
	cmpl	$3, -152(%rbp)
	je	L27
	movq	-200(%rbp), %rcx
	movq	16(%rcx), %rdx
	jmp	L2
L13:
	movl	$255, -64(%rbp)
	movl	$0, -60(%rbp)
	movss	-68(%rbp), %xmm0
	mulss	-52(%rbp), %xmm0
	addss	LC1(%rip), %xmm0
	cvttss2si	%xmm0, %eax
	movl	%eax, -56(%rbp)
	testl	%eax, %eax
	movq	-224(%rbp), %rax
	cmovns	-232(%rbp), %rax
	cmpl	$256, (%rax)
	cmovge	-240(%rbp), %rax
	movl	(%rax), %eax
	movb	%al, (%r13)
	jmp	L8
L27:
	movq	-200(%rbp), %rdi
	call	__ZN13NNEDI3_Worker15step_2_finishedEv
	addq	$216, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	leave
	ret
LFE9336:
	.literal4
	.align 2
LC0:
	.long	1065353216
	.align 2
LC1:
	.long	1056964608
	.section __TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame1:
	.set L$set$0,LECIE1-LSCIE1
	.long L$set$0
LSCIE1:
	.long	0x0
	.byte	0x1
	.ascii "zPR\0"
	.byte	0x1
	.byte	0x78
	.byte	0x10
	.byte	0x6
	.byte	0x9b
	.long	___gxx_personality_v0+4@GOTPCREL
	.byte	0x10
	.byte	0xc
	.byte	0x7
	.byte	0x8
	.byte	0x90
	.byte	0x1
	.align 3
LECIE1:
.globl __ZN13NNEDI3_Worker6step_2Ev.eh
__ZN13NNEDI3_Worker6step_2Ev.eh:
LSFDE1:
	.set L$set$1,LEFDE1-LASFDE1
	.long L$set$1
LASFDE1:
	.long	LASFDE1-EH_frame1
	.quad	LFB9336-.
	.set L$set$2,LFE9336-LFB9336
	.quad L$set$2
	.byte	0x0
	.byte	0x4
	.set L$set$3,LCFI0-LFB9336
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
	.set L$set$5,LCFI7-LCFI1
	.long L$set$5
	.byte	0x83
	.byte	0x7
	.byte	0x8c
	.byte	0x6
	.byte	0x8d
	.byte	0x5
	.byte	0x8e
	.byte	0x4
	.byte	0x8f
	.byte	0x3
	.align 3
LEFDE1:
	.constructor
	.destructor
	.align 1
	.subsections_via_symbols
