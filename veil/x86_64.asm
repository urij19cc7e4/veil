.code

asm_long_smul_64@@16 proc
	mov rax, rcx
	imul rdx
	movq xmm0, rax
	movq xmm1, rdx
	ret
asm_long_smul_64@@16 endp

asm_long_smul_32@@16 proc
	mov eax, ecx
	imul edx
	movq xmm0, rax
	movq xmm1, rdx
	ret
asm_long_smul_32@@16 endp

asm_long_smul_16@@16 proc
	mov ax, cx
	imul dx
	movq xmm0, rax
	movq xmm1, rdx
	ret
asm_long_smul_16@@16 endp

asm_long_smul_8@@16 proc
	mov al, cl
	imul dl
	mov dl, ah
	movq xmm0, rax
	movq xmm1, rdx
	ret
asm_long_smul_8@@16 endp

asm_long_umul_64@@16 proc
	mov rax, rcx
	mul rdx
	movq xmm0, rax
	movq xmm1, rdx
	ret
asm_long_umul_64@@16 endp

asm_long_umul_32@@16 proc
	mov eax, ecx
	mul edx
	movq xmm0, rax
	movq xmm1, rdx
	ret
asm_long_umul_32@@16 endp

asm_long_umul_16@@16 proc
	mov ax, cx
	mul dx
	movq xmm0, rax
	movq xmm1, rdx
	ret
asm_long_umul_16@@16 endp

asm_long_umul_8@@16 proc
	mov al, cl
	mul dl
	mov dl, ah
	movq xmm0, rax
	movq xmm1, rdx
	ret
asm_long_umul_8@@16 endp

end