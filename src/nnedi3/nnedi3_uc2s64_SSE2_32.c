void uc2s64_SSE2(unsigned char *t, int pitch, float *p) {
	__asm__ {
		mov eax,t
		mov ecx,pitch
		mov edx,p
		pxor xmm7,xmm7
		movq xmm0,QWORD PTR[eax]
		movq xmm1,QWORD PTR[eax+8]
		movq xmm2,QWORD PTR[eax+ecx*2]
		movq xmm3,QWORD PTR[eax+ecx*2+8]
		punpcklbw xmm0,xmm7
		punpcklbw xmm1,xmm7
		punpcklbw xmm2,xmm7
		punpcklbw xmm3,xmm7
		lea eax,[eax+ecx*4]
		movdqa [edx],xmm0
		movdqa [edx+16],xmm1
		movdqa [edx+32],xmm2
		movdqa [edx+48],xmm3
		movq xmm4,QWORD PTR[eax]
		movq xmm5,QWORD PTR[eax+8]
		movq xmm6,QWORD PTR[eax+ecx*2]
		movq xmm0,QWORD PTR[eax+ecx*2+8]
		punpcklbw xmm4,xmm7
		punpcklbw xmm5,xmm7
		punpcklbw xmm6,xmm7
		punpcklbw xmm0,xmm7
		movdqa [edx+64],xmm4
		movdqa [edx+80],xmm5
		movdqa [edx+96],xmm6
		movdqa [edx+112],xmm0
	}
}
