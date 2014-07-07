const float e0_mult[4] __attribute__((aligned(16))) = { // (1.0/ln(2))*(2^23)
        12102203.161561486f, 12102203.161561486f, 12102203.161561486f, 12102203.161561486f };
const float e0_bias[4] __attribute__((aligned(16))) = { // (2^23)*127.0-486411.0
        1064866805.0f, 1064866805.0f, 1064866805.0f, 1064866805.0f };

const float exp_lo[4] __attribute__((aligned(16))) = { -80.0f, -80.0f, -80.0f, -80.0f };
const float exp_hi[4] __attribute__((aligned(16))) = { +80.0f, +80.0f, +80.0f, +80.0f };

void e0_m16_SSE2(float *s, const int n) {
	__asm__ {
		mov rax,s
                mov ecx,n
eloop16:
		movaps xmm0,[rax]
		movaps xmm1,[rax+16]
		movaps xmm2,[rax+32]
		movaps xmm3,[rax+48]
		minps xmm0,exp_hi
		minps xmm1,exp_hi
		minps xmm2,exp_hi
		minps xmm3,exp_hi
		maxps xmm0,exp_lo
		maxps xmm1,exp_lo
		maxps xmm2,exp_lo
		maxps xmm3,exp_lo
		mulps xmm0,e0_mult
		mulps xmm1,e0_mult
		mulps xmm2,e0_mult
		mulps xmm3,e0_mult
		addps xmm0,e0_bias
		addps xmm1,e0_bias
		addps xmm2,e0_bias
		addps xmm3,e0_bias
		cvtps2dq xmm0,xmm0
		cvtps2dq xmm1,xmm1
		cvtps2dq xmm2,xmm2
		cvtps2dq xmm3,xmm3
		movaps [rax],xmm0
		movaps [rax+16],xmm1
		movaps [rax+32],xmm2
		movaps [rax+48],xmm3
		add rax,64
		sub rcx,16
		jnz eloop16
	}
}
