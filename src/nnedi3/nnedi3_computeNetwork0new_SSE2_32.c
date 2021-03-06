long long sign_bits_f[2] __attribute__((aligned(16))) = { 0x7FFFFFFF7FFFFFFFll, 0x7FFFFFFF7FFFFFFFll };
long long sign_bits_f_zero_l[2] __attribute__((aligned(16))) = { 0x7FFFFFFF00000000ll, 0x7FFFFFFF7FFFFFFFll };

float ones_f[4] __attribute__((aligned(16))) = { 1.0f, 1.0f, 1.0f, 1.0f };

void computeNetwork0new_SSE2(float *datai, float *weights, unsigned char *d) {
	__asm__ {
		mov ecx,datai
		mov eax,weights
		movdqa xmm0,[ecx]
		movdqa xmm1,xmm0
		movdqa xmm2,xmm0
		movdqa xmm3,xmm0
		pmaddwd xmm0,[eax]
		pmaddwd xmm1,[eax+16]
		pmaddwd xmm2,[eax+32]
		pmaddwd xmm3,[eax+48]
		movdqa xmm4,[ecx+16]
		movdqa xmm5,xmm4
		movdqa xmm6,xmm4
		movdqa xmm7,xmm4
		pmaddwd xmm4,[eax+64]
		pmaddwd xmm5,[eax+80]
		pmaddwd xmm6,[eax+96]
		pmaddwd xmm7,[eax+112]
		paddd xmm0,xmm4
		paddd xmm1,xmm5
		paddd xmm2,xmm6
		paddd xmm3,xmm7
		movdqa xmm4,[ecx+32]
		movdqa xmm5,xmm4
		movdqa xmm6,xmm4
		movdqa xmm7,xmm4
		pmaddwd xmm4,[eax+128]
		pmaddwd xmm5,[eax+144]
		pmaddwd xmm6,[eax+160]
		pmaddwd xmm7,[eax+176]
		paddd xmm0,xmm4
		paddd xmm1,xmm5
		paddd xmm2,xmm6
		paddd xmm3,xmm7
		movdqa xmm4,[ecx+48]
		movdqa xmm5,xmm4
		movdqa xmm6,xmm4
		movdqa xmm7,xmm4
		pmaddwd xmm4,[eax+192]
		pmaddwd xmm5,[eax+208]
		pmaddwd xmm6,[eax+224]
		pmaddwd xmm7,[eax+240]
		paddd xmm0,xmm4
		paddd xmm1,xmm5
		paddd xmm2,xmm6
		paddd xmm3,xmm7
		movdqa xmm4,[ecx+64]
		movdqa xmm5,xmm4
		movdqa xmm6,xmm4
		movdqa xmm7,xmm4
		pmaddwd xmm4,[eax+256]
		pmaddwd xmm5,[eax+272]
		pmaddwd xmm6,[eax+288]
		pmaddwd xmm7,[eax+304]
		paddd xmm0,xmm4
		paddd xmm1,xmm5
		paddd xmm2,xmm6
		paddd xmm3,xmm7
		movdqa xmm4,[ecx+80]
		movdqa xmm5,xmm4
		movdqa xmm6,xmm4
		movdqa xmm7,xmm4
		pmaddwd xmm4,[eax+320]
		pmaddwd xmm5,[eax+336]
		pmaddwd xmm6,[eax+352]
		pmaddwd xmm7,[eax+368]
		paddd xmm0,xmm4
		paddd xmm1,xmm5
		paddd xmm2,xmm6
		paddd xmm3,xmm7
		movdqa xmm4,[ecx+96]
		movdqa xmm5,xmm4
		movdqa xmm6,xmm4
		movdqa xmm7,xmm4
		pmaddwd xmm4,[eax+384]
		pmaddwd xmm5,[eax+400]
		pmaddwd xmm6,[eax+416]
		pmaddwd xmm7,[eax+432]
		paddd xmm0,xmm4
		paddd xmm1,xmm5
		paddd xmm2,xmm6
		paddd xmm3,xmm7
		movdqa xmm4,[ecx+112]
		movdqa xmm5,xmm4
		movdqa xmm6,xmm4
		movdqa xmm7,xmm4
		pmaddwd xmm4,[eax+448]
		pmaddwd xmm5,[eax+464]
		pmaddwd xmm6,[eax+480]
		pmaddwd xmm7,[eax+496]
		paddd xmm0,xmm4
		paddd xmm1,xmm5
		paddd xmm2,xmm6
		paddd xmm3,xmm7
		movdqa xmm4,xmm0
		movdqa xmm5,xmm2
		punpcklqdq xmm0,xmm1
		punpcklqdq xmm2,xmm3
		punpckhqdq xmm4,xmm1
		punpckhqdq xmm5,xmm3
		paddd xmm0,xmm4
		paddd xmm2,xmm5
		movdqa xmm6,xmm0
		shufps xmm0,xmm2,136
		shufps xmm6,xmm2,221
		paddd xmm0,xmm6
		cvtdq2ps xmm0,xmm0
		mulps xmm0,[eax+512]
		addps xmm0,[eax+528]
		movaps xmm1,xmm0
		andps xmm0,sign_bits_f
		addps xmm0,ones_f
		rcpps xmm0,xmm0
		mulps xmm0,xmm1
		pshufd xmm1,xmm0,0
		pshufd xmm2,xmm0,85
		pshufd xmm3,xmm0,170
		pshufd xmm4,xmm0,255
		mulps xmm1,[eax+544]
		mulps xmm2,[eax+560]
		mulps xmm3,[eax+576]
		mulps xmm4,[eax+592]
		pxor xmm0,xmm0
		addps xmm1,xmm2
		addps xmm3,xmm4
		addps xmm1,xmm3
		mov ecx,d
		addps xmm1,[eax+608]
		cmpps xmm1,xmm0,1
		packssdw xmm1,xmm0
		packsswb xmm1,xmm0
		movd eax,xmm1
		xor eax,0xFFFFFFFF
		and eax,0x01010101
		mov [ecx],eax
	}
}
