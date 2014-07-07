void uc2s64_SSE2(const unsigned char *t, const int pitch, float *p) {
	__asm__ {
		mov rax,t
                mov ecx,pitch
		mov rdx,p
		pxor xmm7,xmm7
		movq xmm0,QWORD PTR[rax]
		movq xmm1,QWORD PTR[rax+8]
		movq xmm2,QWORD PTR[rax+rcx*2]
		movq xmm3,QWORD PTR[rax+rcx*2+8]
		punpcklbw xmm0,xmm7
		punpcklbw xmm1,xmm7
		punpcklbw xmm2,xmm7
		punpcklbw xmm3,xmm7
		lea rax,[rax+rcx*4]
		movdqa [rdx],xmm0
		movdqa [rdx+16],xmm1
		movdqa [rdx+32],xmm2
		movdqa [rdx+48],xmm3
		movq xmm4,QWORD PTR[rax]
		movq xmm5,QWORD PTR[rax+8]
		movq xmm6,QWORD PTR[rax+rcx*2]
		movq xmm0,QWORD PTR[rax+rcx*2+8]
		punpcklbw xmm4,xmm7
		punpcklbw xmm5,xmm7
		punpcklbw xmm6,xmm7
		punpcklbw xmm0,xmm7
		movdqa [rdx+64],xmm4
		movdqa [rdx+80],xmm5
		movdqa [rdx+96],xmm6
		movdqa [rdx+112],xmm0
	}
}
