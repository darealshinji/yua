#include <float.h>

const float flt_epsilon_sse[4] __attribute__((aligned(16))) = { FLT_EPSILON, FLT_EPSILON, FLT_EPSILON, FLT_EPSILON };

void extract_m8_i16_SSE2(const unsigned char *srcp, const int stride, const int xdia, const int ydia, float *mstd, float *inputf) {
	__asm__ {
                mov rax,srcp
                mov ebx,stride
                mov edi,xdia
                mov edx,ydia
                mov rsp,inputf
		
		lea rsi,[rax+rbx*2]
		pxor xmm4,xmm4 // sum
		pxor xmm5,xmm5 // sumsq
		pxor xmm6,xmm6
yloop:
		xor rcx,rcx
xloop:
		movq xmm0,QWORD PTR[rax+rcx]
		movq xmm1,QWORD PTR[rsi+rcx]
		movdqa xmm2,xmm0
		movdqa xmm3,xmm1
		punpcklbw xmm0,xmm6
		punpcklbw xmm1,xmm6
		psadbw xmm2,xmm6
		psadbw xmm3,xmm6
                movdqa [rsp],xmm0
                movdqa [rsp+rdi*2],xmm1
		pmaddwd xmm0,xmm0
		pmaddwd xmm1,xmm1
		paddd xmm4,xmm2
		paddd xmm5,xmm0
		paddd xmm4,xmm3
		paddd xmm5,xmm1
		add rcx,8
                add rsp,16
		cmp rcx,rdi
		jl xloop
		
		lea rax,[rax+rbx*4]
		lea rsi,[rsi+rbx*4]
                lea rsp,[rsp+rdi*2]
                sub edx,2
		jnz yloop
		
		movhlps xmm1,xmm5
                mov eax,ydia
		paddd xmm5,xmm1
		mul rdi
		pshuflw xmm1,xmm5,14
		cvtsi2ss xmm7,rax
		paddd xmm5,xmm1
		rcpss xmm7,xmm7 // scale
		cvtdq2ps xmm4,xmm4
		cvtdq2ps xmm5,xmm5
                mov rax,mstd
		mulss xmm4,xmm7 // mean
		mulss xmm5,xmm7
		movss [rax],xmm4
		mulss xmm4,xmm4
		subss xmm5,xmm4 // var
		comiss xmm5,flt_epsilon_sse
		jbe novarjmp
		rsqrtss xmm5,xmm5 // 1.0/std
		rcpss xmm4,xmm5 // std
		movss [rax+4],xmm4
		movss [rax+8],xmm5
		jmp finish
novarjmp:
		movss [rax+4],xmm6
		movss [rax+8],xmm6
finish:
		movss [rax+12],xmm6
        }
}
