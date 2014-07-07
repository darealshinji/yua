const float min_weight_sum[4] __attribute__((aligned(16))) = { 1e-10f, 1e-10f, 1e-10f, 1e-10f };
const float five_f[4] __attribute__((aligned(16))) = { 5.0f, 5.0f, 5.0f, 5.0f };
const float ones_f2[4] __attribute__((aligned(16))) = { 1.0f, 1.0f, 1.0f, 1.0f };
const long long sign_bits_f2[2] __attribute__((aligned(16))) = { 0x7FFFFFFF7FFFFFFF, 0x7FFFFFFF7FFFFFFF };

void weightedAvgElliottMul5_m16_SSE2(const float *w, const int n, float *mstd) {
	__asm__ {
		mov rax,w
                mov ecx,n
		lea rdx,[rax+rcx*4]
		xor rdi,rdi
		xorps xmm0,xmm0 // sum w
		xorps xmm1,xmm1 // sum w*v
nloop:
		movaps xmm2,[rax+rdi*4]
		movaps xmm3,[rax+rdi*4+16]
		movaps xmm4,[rdx+rdi*4]
		movaps xmm5,[rdx+rdi*4+16]
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
		movaps xmm2,[rax+rdi*4+32]
		movaps xmm3,[rax+rdi*4+48]
		movaps xmm4,[rdx+rdi*4+32]
		movaps xmm5,[rdx+rdi*4+48]
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
		add rdi,16
		sub rcx,16
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
		mov rax,mstd
		mulss xmm1,[rax+4]
		addss xmm1,[rax]
		addss xmm1,[rax+12]
		movss [rax+12],xmm1
	}
}
