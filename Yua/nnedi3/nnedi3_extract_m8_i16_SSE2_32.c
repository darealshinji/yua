#include <float.h>

float flt_epsilon_sse[4] __attribute__((aligned(16))) = { FLT_EPSILON, FLT_EPSILON, FLT_EPSILON, FLT_EPSILON };

void extract_m8_i16_SSE2(unsigned char *srcp, int stride, int xdia, int ydia, float *mstd, float *inputf) {
	__asm__ {
		mov eax,[esp+20+12]
		mov ebx,[esp+24+12]
		mov edi,[esp+28+12]
		mov ebp,[esp+32+12]
		mov edx,[esp+40+12] //this is offset 0x34
		lea esi,[eax+ebx*2]
		pxor xmm4,xmm4 // sum
		pxor xmm5,xmm5 // sumsq
		pxor xmm6,xmm6
yloop:
		xor ecx,ecx
xloop:
		movq xmm0,QWORD PTR[eax+ecx]
		movq xmm1,QWORD PTR[esi+ecx]
		movdqa xmm2,xmm0
		movdqa xmm3,xmm1
		punpcklbw xmm0,xmm6
		punpcklbw xmm1,xmm6
		psadbw xmm2,xmm6
		psadbw xmm3,xmm6
		movdqa [edx],xmm0
		movdqa [edx+edi*2],xmm1
		pmaddwd xmm0,xmm0
		pmaddwd xmm1,xmm1
		paddd xmm4,xmm2
		paddd xmm5,xmm0
		paddd xmm4,xmm3
		paddd xmm5,xmm1
		add ecx,8
		add edx,16
		cmp ecx,edi
		jl xloop
		lea eax,[eax+ebx*4]
		lea esi,[esi+ebx*4]
		lea edx,[edx+edi*2]
		sub ebp,2
		jnz yloop
		movhlps xmm1,xmm5
		mov eax,[esp+32+12]
		paddd xmm5,xmm1
		mul edi
		pshuflw xmm1,xmm5,14
		cvtsi2ss xmm7,eax
		paddd xmm5,xmm1
		rcpss xmm7,xmm7 // scale
		cvtdq2ps xmm4,xmm4
		cvtdq2ps xmm5,xmm5
		mov eax,[esp+36+12]
		mulss xmm4,xmm7 // mean
		mulss xmm5,xmm7
		movss [eax],xmm4
		mulss xmm4,xmm4
		subss xmm5,xmm4 // var
		comiss xmm5,flt_epsilon_sse
		jbe novarjmp
		rsqrtss xmm5,xmm5 // 1.0/std
		rcpss xmm4,xmm5 // std
		movss [eax+4],xmm4
		movss [eax+8],xmm5
		jmp finish
novarjmp:
		movss [eax+4],xmm6
		movss [eax+8],xmm6
finish:
		movss [eax+12],xmm6
	}
}
