float sse_half[4] __attribute__((aligned(16))) = { 0.5f, 0.5f, 0.5f, 0.5f };

void castScale_SSE(float *val, float *scale, unsigned char *dstp) {
	__asm__ {
		mov ecx,val
		mov eax,scale
		movss xmm0,[ecx+12]
		mulss xmm0,[eax]
		addss xmm0,sse_half
		cvttss2si eax,xmm0
		mov ecx,dstp
		cmp eax,255
		jl b255
		mov eax,255
		jmp finish
b255:
		cmp eax,0
		jge finish
		xor eax,eax
finish:
		mov byte ptr[ecx],al
	}
}
