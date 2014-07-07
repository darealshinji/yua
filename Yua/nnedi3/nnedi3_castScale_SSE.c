const float sse_half[4] __attribute__((aligned(16))) = { 0.5f, 0.5f, 0.5f, 0.5f };

void castScale_SSE(const float *val, const float *scale, unsigned char *dstp) {
	__asm__ {
		mov rcx,val
		mov rax,scale
		movss xmm0,[rcx+12]
		mulss xmm0,[rax]
		addss xmm0,sse_half
		cvttss2si rax,xmm0
		mov rcx,dstp
		cmp rax,255
		jl b255
		mov rax,255
		jmp finish
b255:
		cmp rax,0
		jge finish
		xor rax,rax
finish:
		mov byte ptr[rcx],al
	}
}
