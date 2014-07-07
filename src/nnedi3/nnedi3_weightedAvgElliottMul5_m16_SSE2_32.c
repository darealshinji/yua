float min_weight_sum[4] __attribute__((aligned(16))) = { 1e-10f, 1e-10f, 1e-10f, 1e-10f };
float five_f[4] __attribute__((aligned(16))) = { 5.0f, 5.0f, 5.0f, 5.0f };
float ones_f2[4] __attribute__((aligned(16))) = { 1.0f, 1.0f, 1.0f, 1.0f };
long long sign_bits_f2[2] __attribute__((aligned(16))) = { 0x7FFFFFFF7FFFFFFFll, 0x7FFFFFFF7FFFFFFFll };

void weightedAvgElliottMul5_m16_SSE2(float *w, int n, float *mstd) {
	__asm__ {
		push edi
		mov eax,w
		mov ecx,n
		lea edx,[eax+ecx*4]
		xor edi,edi
		xorps xmm0,xmm0 // sum w
		xorps xmm1,xmm1 // sum w*v
nloop:
		movaps xmm2,[eax+edi*4]
		movaps xmm3,[eax+edi*4+16]
		movaps xmm4,[edx+edi*4]
		movaps xmm5,[edx+edi*4+16]
		addps xmm0,xmm2
		movaps xmm6,xmm4
		movaps xmm7,xmm5
		addps xmm0,xmm3
		andps xmm4,sign_bits_f2
		andps xmm5,sign_bits_f2
		addps xmm4,ones_f2
		addps xmm5,ones_f2
		rcpps xmm4,xmm4
		rcpps xmm5,xmm5
		mulps xmm6,xmm4
		mulps xmm7,xmm5
		mulps xmm6,xmm2
		mulps xmm7,xmm3
		addps xmm1,xmm6
		addps xmm1,xmm7
		movaps xmm2,[eax+edi*4+32]
		movaps xmm3,[eax+edi*4+48]
		movaps xmm4,[edx+edi*4+32]
		movaps xmm5,[edx+edi*4+48]
		addps xmm0,xmm2
		movaps xmm6,xmm4
		movaps xmm7,xmm5
		addps xmm0,xmm3
		andps xmm4,sign_bits_f2
		andps xmm5,sign_bits_f2
		addps xmm4,ones_f2
		addps xmm5,ones_f2
		rcpps xmm4,xmm4
		rcpps xmm5,xmm5
		mulps xmm6,xmm4
		mulps xmm7,xmm5
		mulps xmm6,xmm2
		mulps xmm7,xmm3
		addps xmm1,xmm6
		addps xmm1,xmm7
		add edi,16
		sub ecx,16
		jnz nloop
		movhlps xmm2,xmm0
		movhlps xmm3,xmm1
		addps xmm0,xmm2
		addps xmm1,xmm3
		pshuflw xmm2,xmm0,14
		pshuflw xmm3,xmm1,14
		addss xmm0,xmm2
		addss xmm1,xmm3
		comiss xmm0,min_weight_sum
		jbe nodiv
		mulss xmm1,five_f
		rcpss xmm0,xmm0
		mulss xmm1,xmm0
		jmp finish
nodiv:
		xorps xmm1,xmm1
finish:
		mov eax,mstd
		mulss xmm1,[eax+4]
		addss xmm1,[eax]
		addss xmm1,[eax+12]
		movss [eax+12],xmm1
		pop edi
	}
}
