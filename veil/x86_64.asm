.code

; FASTCALL_WIN_X86_64
; ARG: rcx - val64<>* v
; ARG: rdx - int64_t si
; RET: rax - bool overflow/underflow
; caller-care registers: rax, rcx, rdx, r8, r9
___asm_long_smul_64 proc
	mov rax, [rcx]
	xor r8, r8
	imul rdx
	seto r8b
	mov [rcx], rax
	mov rax, r8
	ret
___asm_long_smul_64 endp

; FASTCALL_WIN_X86_64
; ARG: rcx - val64<>* v
; ARG: rdx - uint64_t ui
; RET: rax - bool overflow
; caller-care registers: rax, rcx, rdx, r8, r9
___asm_long_umul_64 proc
	mov rax, [rcx]
	xor r8, r8
	mul rdx
	setc r8b
	mov [rcx], rax
	mov rax, r8
	ret
___asm_long_umul_64 endp

end