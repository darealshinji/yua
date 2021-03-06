#include <inttypes.h>

const unsigned char ub_1[16] __attribute__((aligned(16))) = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
const int16_t w_19[8] __attribute__((aligned(16))) = { 19, 19, 19, 19, 19, 19, 19, 19 };
const int16_t w_3[8] __attribute__((aligned(16))) = { 3, 3, 3, 3, 3, 3, 3, 3 };
const int16_t w_254[8] __attribute__((aligned(16))) = { 254, 254, 254, 254, 254, 254, 254, 254 };
const uint16_t uw_16[8] __attribute__((aligned(16))) = { 16, 16, 16, 16, 16, 16, 16, 16 };

#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#define CB2(n) MAX(MIN((n),254),0)

int processLine0_SSE2(const unsigned char *tempu, int width, unsigned char *dstp, const unsigned char *src3p, const int src_pitch) {
	int count;
	const int remain = width&15;
	width -= remain;
        if (width) {
//		goto skipasm;

	__asm__ {
		push rbx
		
                mov rax,tempu
		mov rbx,src3p
                mov ecx,width
                mov edx,src_pitch
		mov rsi,dstp
		lea rdi,[rbx+rdx*4]
		pxor xmm6,xmm6
		pxor xmm7,xmm7
xloop:
		movdqa xmm0,[rbx+rdx*2]
		movdqa xmm1,[rdi]
		movdqa xmm2,xmm0
		movdqa xmm3,xmm1
		punpcklbw xmm0,xmm7
		punpckhbw xmm2,xmm7
		punpcklbw xmm1,xmm7
		punpckhbw xmm3,xmm7
		paddw xmm0,xmm1
		paddw xmm2,xmm3
		pmullw xmm0,w_19
		pmullw xmm2,w_19
		movdqa xmm1,[rbx]
		movdqa xmm3,[rdi+rdx*2]
		movdqa xmm4,xmm1
		movdqa xmm5,xmm3
		punpcklbw xmm1,xmm7
		punpckhbw xmm4,xmm7
		punpcklbw xmm3,xmm7
		punpckhbw xmm5,xmm7
		paddw xmm1,xmm3
		paddw xmm4,xmm5
		pmullw xmm1,w_3
		pmullw xmm4,w_3
		movdqa xmm3,[rax]
		psubusw xmm0,xmm1
		psubusw xmm2,xmm4
		pcmpeqb xmm3,ub_1
		paddusw xmm0,uw_16
		paddusw xmm2,uw_16
		movdqa xmm1,xmm3
		pcmpeqb xmm4,xmm4
		psrlw xmm0,5
		psrlw xmm2,5
		pxor xmm1,xmm4
		pminsw xmm0,w_254
		pminsw xmm2,w_254
		movdqa xmm5,xmm1
		packuswb xmm0,xmm2
		pand xmm5,ub_1
		pand xmm0,xmm3
		psadbw xmm5,xmm7
		por xmm0,xmm1
		movdqa xmm2,xmm5
		psrldq xmm5,8
		movdqa [rsi],xmm0
		paddusw xmm5,xmm2
		paddusw xmm6,xmm5
		add rbx,16
		add rdi,16
		add rax,16
		add rsi,16
		sub rcx,16
		jnz xloop
		movd count,xmm6
		
		pop rbx
        }
                }
	
//skipasm:
                int x;
        for (x=width; x<width+remain; ++x) {
                if (tempu[x])
                        dstp[x] = CB2((19*(src3p[x+src_pitch*2]+src3p[x+src_pitch*4])-
                                3*(src3p[x]+src3p[x+src_pitch*6])+16)>>5);
                else {
                        dstp[x] = 255;
                        ++count;
                }
        }
        return count;
}
