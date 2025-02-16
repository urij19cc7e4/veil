#pragma once

#include <cstdint>

namespace interpreter
{
	enum class op : uint8_t
	{
		// 

		// call_b - transfers control flow to the instruction by b-bit signed integer
		//          offset value from the instruction following the given instruction
		// 
		//          1. Control flow pointer pushed to stack
		//          2. Control flow pointer increased by offset value
		//          3. Stack frame pointer pushed to stack
		//          4. Stack frame pointer assigned to current stack pointer

		call_64,			// [ op (8) ][ offset (64) ]
		call_32,			// [ op (8) ][ offset (32) ]
		call_16,			// [ op (8) ][ offset (16) ]
		call_8,				// [ op (8) ][ offset (8) ]

		// nativecall - 

		nativecall,

		// ret - transfers control flow to the instruction by b-bit signed integer
		//       offset value from the instruction following the given instruction
		// 
		//       3. Stack frame pointer popped from stack
		//       4. Stack frame pointer assigned to current stack pointer
		//       1. Control flow pointer popped from stack
		//       2. Control flow pointer increased by offset value

		ret,

		// jmp_b - transfers control flow to the instruction by b-bit signed integer
		//         offset value from the instruction following the given instruction

		jmp_64,				// [ op (8) ][ offset (64) ]
		jmp_32,				// [ op (8) ][ offset (32) ]
		jmp_16,				// [ op (8) ][ offset (16) ]
		jmp_8,				// [ op (8) ][ offset (8) ]

		// jmp_a_b - transfers control flow to the instruction by b-bit signed integer
		//           offset value from the instruction following the given instruction

		jmp_a_64,			// [ op (8) ][ offset (64) ]
		jmp_a_32,			// [ op (8) ][ offset (32) ]
		jmp_a_16,			// [ op (8) ][ offset (16) ]
		jmp_a_8,			// [ op (8) ][ offset (8) ]

		// jmp_ae_b - transfers control flow to the instruction by b-bit signed integer
		//            offset value from the instruction following the given instruction

		jmp_ae_64,			// [ op (8) ][ offset (64) ]
		jmp_ae_32,			// [ op (8) ][ offset (32) ]
		jmp_ae_16,			// [ op (8) ][ offset (16) ]
		jmp_ae_8,			// [ op (8) ][ offset (8) ]

		// jmp_b_b - transfers control flow to the instruction by b-bit signed integer
		//           offset value from the instruction following the given instruction

		jmp_b_64,			// [ op (8) ][ offset (64) ]
		jmp_b_32,			// [ op (8) ][ offset (32) ]
		jmp_b_16,			// [ op (8) ][ offset (16) ]
		jmp_b_8,			// [ op (8) ][ offset (8) ]

		// jmp_be_b - transfers control flow to the instruction by b-bit signed integer
		//            offset value from the instruction following the given instruction

		jmp_be_64,			// [ op (8) ][ offset (64) ]
		jmp_be_32,			// [ op (8) ][ offset (32) ]
		jmp_be_16,			// [ op (8) ][ offset (16) ]
		jmp_be_8,			// [ op (8) ][ offset (8) ]

		// jmp_e_b - transfers control flow to the instruction by b-bit signed integer
		//           offset value from the instruction following the given instruction

		jmp_e_64,			// [ op (8) ][ offset (64) ]
		jmp_e_32,			// [ op (8) ][ offset (32) ]
		jmp_e_16,			// [ op (8) ][ offset (16) ]
		jmp_e_8,			// [ op (8) ][ offset (8) ]

		// jmp_ne_b - transfers control flow to the instruction by b-bit signed integer
		//            offset value from the instruction following the given instruction

		jmp_ne_64,			// [ op (8) ][ offset (64) ]
		jmp_ne_32,			// [ op (8) ][ offset (32) ]
		jmp_ne_16,			// [ op (8) ][ offset (16) ]
		jmp_ne_8,			// [ op (8) ][ offset (8) ]

		// jmp_un_b - transfers control flow to the instruction by b-bit signed integer
		//            offset value from the instruction following the given instruction

		jmp_un_64,			// [ op (8) ][ offset (64) ]
		jmp_un_32,			// [ op (8) ][ offset (32) ]
		jmp_un_16,			// [ op (8) ][ offset (16) ]
		jmp_un_8,			// [ op (8) ][ offset (8) ]

		// jmp_err_b - transfers control flow to the instruction by b-bit signed integer
		//             offset value from the instruction following the given instruction

		jmp_err_64,			// [ op (8) ][ offset (64) ]
		jmp_err_32,			// [ op (8) ][ offset (32) ]
		jmp_err_16,			// [ op (8) ][ offset (16) ]
		jmp_err_8,			// [ op (8) ][ offset (8) ]

		// jmp_ovf_b - transfers control flow to the instruction by b-bit signed integer
		//             offset value from the instruction following the given instruction

		jmp_ovf_64,			// [ op (8) ][ offset (64) ]
		jmp_ovf_32,			// [ op (8) ][ offset (32) ]
		jmp_ovf_16,			// [ op (8) ][ offset (16) ]
		jmp_ovf_8,			// [ op (8) ][ offset (8) ]

		// jmp_unf_b - transfers control flow to the instruction by b-bit signed integer
		//             offset value from the instruction following the given instruction

		jmp_unf_64,			// [ op (8) ][ offset (64) ]
		jmp_unf_32,			// [ op (8) ][ offset (32) ]
		jmp_unf_16,			// [ op (8) ][ offset (16) ]
		jmp_unf_8,			// [ op (8) ][ offset (8) ]

		// l_load_b - pushes b-bit value
		//            from 16-bit signed integer offset value on stack frame to stack

		l_load_64,			// [ op (8) ][ offset (16) ]
		l_load_32,			// [ op (8) ][ offset (16) ]
		l_load_16,			// [ op (8) ][ offset (16) ]
		l_load_8,			// [ op (8) ][ offset (16) ]

		// l_store_b - pops b-bit value
		//             from stack to 16-bit signed integer offset value on stack frame

		l_store_64,			// [ op (8) ][ offset (16) ]
		l_store_32,			// [ op (8) ][ offset (16) ]
		l_store_16,			// [ op (8) ][ offset (16) ]
		l_store_8,			// [ op (8) ][ offset (16) ]

		// s_load_b - pushes b-bit value
		//            from 8-bit signed integer offset value on stack frame to stack

		s_load_64,			// [ op (8) ][ offset (8) ]
		s_load_32,			// [ op (8) ][ offset (8) ]
		s_load_16,			// [ op (8) ][ offset (8) ]
		s_load_8,			// [ op (8) ][ offset (8) ]

		// s_store_b - pops b-bit value
		//             from stack to 8-bit signed integer offset value on stack frame

		s_store_64,			// [ op (8) ][ offset (8) ]
		s_store_32,			// [ op (8) ][ offset (8) ]
		s_store_16,			// [ op (8) ][ offset (8) ]
		s_store_8,			// [ op (8) ][ offset (8) ]

		// push_b - pushes b-bit value to stack

		push_64,			// [ op (8) ][ value (64) ]
		push_32,			// [ op (8) ][ value (32) ]
		push_16,			// [ op (8) ][ value (16) ]
		push_8,				// [ op (8) ][ value (8) ]

		// pop_b - pops b-bit value from stack

		pop_64,				// [ op (8) ]
		pop_32,				// [ op (8) ]
		pop_16,				// [ op (8) ]
		pop_8,				// [ op (8) ]

		// dup_p - duplicates b-bit value on stack

		dup_64,				// [ op (8) ]
		dup_32,				// [ op (8) ]
		dup_16,				// [ op (8) ]
		dup_8,				// [ op (8) ]

		// memcpy_b - pops destination memory region pointer, source memory region pointer,
		//            b-bit unsigned integer memory region size value
		//            from stack in the specified order and
		//            copies source memory region content to destination memory region

		memcpy_64,			// [ op (8) ]
		memcpy_32,			// [ op (8) ]
		memcpy_16,			// [ op (8) ]
		memcpy_8,			// [ op (8) ]

		// memset_b - pops destination memory region pointer, 8-bit fill value,
		//            b-bit unsigned integer memory region size value
		//            from stack in the specified order and
		//            fills destination memory region with fill value

		memset_64,			// [ op (8) ]
		memset_32,			// [ op (8) ]
		memset_16,			// [ op (8) ]
		memset_8,			// [ op (8) ]

		// ptrcpy_b - pops pointer #1 and pointer #2 from stack in the specified order
		//            and moves pointer #1 b-bit value to pointer #2 b-bit value

		ptrcpy_64,			// [ op (8) ]
		ptrcpy_32,			// [ op (8) ]
		ptrcpy_16,			// [ op (8) ]
		ptrcpy_8, 			// [ op (8) ]

		// ptrget_b - pops pointer from stack and pushes pointer value to stack

		ptrget_64,			// [ op (8) ]
		ptrget_32,			// [ op (8) ]
		ptrget_16,			// [ op (8) ]
		ptrget_8,			// [ op (8) ]

		// ptrset_b - pops pointer from stack and pops pointer value from stack

		ptrset_64,			// [ op (8) ]
		ptrset_32,			// [ op (8) ]
		ptrset_16,			// [ op (8) ]
		ptrset_8,			// [ op (8) ]

		// push_flow - pushes current control flow pointer to stack

		push_flow,			// [ op (8) ]

		// push_frame - pushes current stack frame pointer to stack

		push_frame,			// [ op (8) ]

		// push_stack - pushes current stack pointer to stack (copy then allocate)

		push_stack,			// [ op (8) ]

		// pop_flow - pops current control flow pointer from stack

		pop_flow,			// [ op (8) ]

		// pop_frame - pops current stack frame pointer from stack

		pop_frame,			// [ op (8) ]

		// pop_stack - pops current stack pointer from stack (deallocate then copy)

		pop_stack,			// [ op (8) ]

		// neg_b - arithmetic negates b-bit signed integer value on stack

		neg_64,				// [ op (8) ]
		neg_32,				// [ op (8) ]
		neg_16,				// [ op (8) ]
		neg_8,				// [ op (8) ]

		// not_b - boolean inverts b-bit value bits on stack

		not_64,				// [ op (8) ]
		not_32,				// [ op (8) ]
		not_16,				// [ op (8) ]
		not_8,				// [ op (8) ]

		// sinc_b - arithmetic increments b-bit signed integer value on stack

		sinc_64,			// [ op (8) ]
		sinc_32,			// [ op (8) ]
		sinc_16,			// [ op (8) ]
		sinc_8,				// [ op (8) ]

		// uinc_b - arithmetic increments b-bit unsigned integer value on stack

		uinc_64,			// [ op (8) ]
		uinc_32,			// [ op (8) ]
		uinc_16,			// [ op (8) ]
		uinc_8,				// [ op (8) ]

		// sdec_b - arithmetic decrements b-bit signed integer value on stack

		sdec_64,			// [ op (8) ]
		sdec_32,			// [ op (8) ]
		sdec_16,			// [ op (8) ]
		sdec_8,				// [ op (8) ]

		// udec_b - arithmetic decrements b-bit unsigned integer value on stack

		udec_64,			// [ op (8) ]
		udec_32,			// [ op (8) ]
		udec_16,			// [ op (8) ]
		udec_8,				// [ op (8) ]

		// and_b - pops b-bit value from stack and boolean
		//         ANDs it to the next b-bit value on stack

		and_64,				// [ op (8) ]
		and_32,				// [ op (8) ]
		and_16,				// [ op (8) ]
		and_8,				// [ op (8) ]

		// or_b - pops b-bit value from stack and boolean
		//        ORs it to the next b-bit value on stack

		or_64,				// [ op (8) ]
		or_32,				// [ op (8) ]
		or_16,				// [ op (8) ]
		or_8,				// [ op (8) ]

		// xor_b - pops b-bit value from stack and boolean
		//         XORs it to the next b-bit value on stack

		xor_64,				// [ op (8) ]
		xor_32,				// [ op (8) ]
		xor_16,				// [ op (8) ]
		xor_8,				// [ op (8) ]

		// sadd_b - pops b-bit signed integer value from stack and arithmetic
		//          adds it to the next b-bit signed integer value on stack

		sadd_64,			// [ op (8) ]
		sadd_32,			// [ op (8) ]
		sadd_16,			// [ op (8) ]
		sadd_8,				// [ op (8) ]

		// uadd_b - pops b-bit unsigned integer value from stack and arithmetic
		//          adds it to the next b-bit unsigned integer value on stack

		uadd_64,			// [ op (8) ]
		uadd_32,			// [ op (8) ]
		uadd_16,			// [ op (8) ]
		uadd_8,				// [ op (8) ]

		// ssub_b - pops b-bit signed integer value from stack and arithmetic
		//          subtracts it from the next b-bit signed integer value on stack

		ssub_64,			// [ op (8) ]
		ssub_32,			// [ op (8) ]
		ssub_16,			// [ op (8) ]
		ssub_8,				// [ op (8) ]

		// usub_b - pops b-bit unsigned integer value from stack and arithmetic
		//          subtracts it from the next b-bit unsigned integer value on stack

		usub_64,			// [ op (8) ]
		usub_32,			// [ op (8) ]
		usub_16,			// [ op (8) ]
		usub_8,				// [ op (8) ]

		// smul_b - pops b-bit signed integer value from stack and arithmetic
		//          multiplies the next b-bit signed integer value on stack by it

		smul_64,			// [ op (8) ]
		smul_32,			// [ op (8) ]
		smul_16,			// [ op (8) ]
		smul_8,				// [ op (8) ]

		// umul_b - pops b-bit unsigned integer value from stack and arithmetic
		//          multiplies the next b-bit unsigned integer value on stack by it

		umul_64,			// [ op (8) ]
		umul_32,			// [ op (8) ]
		umul_16,			// [ op (8) ]
		umul_8,				// [ op (8) ]

		// smod_b - pops b-bit signed integer value from stack and computes arithmetic
		//          modulo of the next b-bit signed integer value on stack by it

		smod_64,			// [ op (8) ]
		smod_32,			// [ op (8) ]
		smod_16,			// [ op (8) ]
		smod_8,				// [ op (8) ]

		// umod_b - pops b-bit unsigned integer value from stack and computes arithmetic
		//          modulo of the next b-bit unsigned integer value on stack by it

		umod_64,			// [ op (8) ]
		umod_32,			// [ op (8) ]
		umod_16,			// [ op (8) ]
		umod_8,				// [ op (8) ]

		// sdiv_b - pops b-bit signed integer value from stack and arithmetic
		//          divides the next b-bit signed integer value on stack by it

		sdiv_64,			// [ op (8) ]
		sdiv_32,			// [ op (8) ]
		sdiv_16,			// [ op (8) ]
		sdiv_8,				// [ op (8) ]

		// udiv_b - pops b-bit unsigned integer value from stack and arithmetic
		//          divides the next b-bit unsigned integer value on stack by it

		udiv_64,			// [ op (8) ]
		udiv_32,			// [ op (8) ]
		udiv_16,			// [ op (8) ]
		udiv_8,				// [ op (8) ]

		// scmp_b - pops b-bit signed integer value #1 and b-bit signed integer value #2
		//          from stack in the specified order and arithmetic compares them

		scmp_64,			// [ op (8) ]
		scmp_32,			// [ op (8) ]
		scmp_16,			// [ op (8) ]
		scmp_8,				// [ op (8) ]

		// ucmp_b - pops b-bit unsigned integer value #1 and b-bit unsigned integer value #2
		//          from stack in the specified order and arithmetic compares them

		ucmp_64,			// [ op (8) ]
		ucmp_32,			// [ op (8) ]
		ucmp_16,			// [ op (8) ]
		ucmp_8,				// [ op (8) ]

		// rotl_b - pops 8-bit unsigned integer value from stack and rotates left
		//          the next b-bit unsigned integer value on stack by given bit count

		rotl_64,			// [ op (8) ]
		rotl_32,			// [ op (8) ]
		rotl_16,			// [ op (8) ]
		rotl_8,				// [ op (8) ]

		// rotr_b - pops 8-bit unsigned integer value from stack and rotates right
		//          the next b-bit unsigned integer value on stack by given bit count

		rotr_64,			// [ op (8) ]
		rotr_32,			// [ op (8) ]
		rotr_16,			// [ op (8) ]
		rotr_8,				// [ op (8) ]

		// sshl_b - pops 8-bit unsigned integer value from stack and shifts left
		//          the next b-bit signed integer value on stack by given bit count

		sshl_64,			// [ op (8) ]
		sshl_32,			// [ op (8) ]
		sshl_16,			// [ op (8) ]
		sshl_8,				// [ op (8) ]

		// ushl_b - pops 8-bit unsigned integer value from stack and shifts left
		//          the next b-bit unsigned integer value on stack by given bit count

		ushl_64,			// [ op (8) ]
		ushl_32,			// [ op (8) ]
		ushl_16,			// [ op (8) ]
		ushl_8,				// [ op (8) ]

		// sshr_b - pops 8-bit unsigned integer value from stack and shifts right
		//          the next b-bit signed integer value on stack by given bit count

		sshr_64,			// [ op (8) ]
		sshr_32,			// [ op (8) ]
		sshr_16,			// [ op (8) ]
		sshr_8,				// [ op (8) ]

		// ushr_b - pops 8-bit unsigned integer value from stack and shifts right
		//          the next b-bit unsigned integer value on stack by given bit count

		ushr_64,			// [ op (8) ]
		ushr_32,			// [ op (8) ]
		ushr_16,			// [ op (8) ]
		ushr_8,				// [ op (8) ]

		// fadd_b - pops b-bit float value from stack and arithmetic
		//          adds it to the next b-bit float value on stack

		fadd_64,			// [ op (8) ]
		fadd_32,			// [ op (8) ]

		// fadd_b - pops b-bit float value from stack and arithmetic
		//          subtracts it from the next b-bit float value on stack

		fsub_64,			// [ op (8) ]
		fsub_32,			// [ op (8) ]

		// fmul_b - pops b-bit float value from stack and arithmetic
		//          multiplies the next b-bit float value on stack by it

		fmul_64,			// [ op (8) ]
		fmul_32,			// [ op (8) ]

		// fmod_b - pops b-bit float value from stack and computes arithmetic
		//          modulo of the next b-bit float value on stack by it

		fmod_64,			// [ op (8) ]
		fmod_32,			// [ op (8) ]

		// fdiv_b - pops b-bit float value from stack and arithmetic
		//          divides the next b-bit float value on stack by it

		fdiv_64,			// [ op (8) ]
		fdiv_32,			// [ op (8) ]

		// fcmp_b - pops b-bit float value #1 and b-bit float value #2
		//          from stack in the specified order and arithmetic compares them

		fcmp_64,			// [ op (8) ]
		fcmp_32,			// [ op (8) ]

		// sib1_2_fb2 - converts b1-bit signed integer value to b2-bit float value on stack

		si64_2_f64,			// [ op (8) ]
		si64_2_f32,			// [ op (8) ]
		si32_2_f64,			// [ op (8) ]
		si32_2_f32,			// [ op (8) ]

		// uib1_2_fb2 - converts b1-bit unsigned integer value to b2-bit float value on stack

		ui64_2_f64,			// [ op (8) ]
		ui64_2_f32,			// [ op (8) ]
		ui32_2_f64,			// [ op (8) ]
		ui32_2_f32,			// [ op (8) ]

		// fb1_2_sib2 - converts b1-bit float value to b2-bit signed integer value on stack

		f64_2_si64,			// [ op (8) ]
		f64_2_si32,			// [ op (8) ]
		f32_2_si64,			// [ op (8) ]
		f32_2_si32,			// [ op (8) ]

		// fb1_2_sib2 - converts b1-bit float value to b2-bit unsigned integer value on stack

		f64_2_ui64,			// [ op (8) ]
		f64_2_ui32,			// [ op (8) ]
		f32_2_ui64,			// [ op (8) ]
		f32_2_ui32,			// [ op (8) ]

		// r2r_b - moves b-bit value from src rg to dst rg

		r2r_64,		// [ op (8) ][ rg (8) ]
		r2r_32,		// [ op (8) ][ rg (8) ]
		r2r_16,		// [ op (8) ][ rg (8) ]
		r2r_8,		// [ op (8) ][ rg (8) ]

		// r2rp_b - moves b-bit value from src rg to dst rg ptr

		r2rp_64,	// [ op (8) ][ rg (8) ]
		r2rp_32,	// [ op (8) ][ rg (8) ]
		r2rp_16,	// [ op (8) ][ rg (8) ]
		r2rp_8,		// [ op (8) ][ rg (8) ]

		// r2s_b - pushes b-bit value from src rg to stack

		r2s_64,		// [ op (8) ][ rg (8) ]
		r2s_32,		// [ op (8) ][ rg (8) ]
		r2s_16,		// [ op (8) ][ rg (8) ]
		r2s_8,		// [ op (8) ][ rg (8) ]

		// r2sp_b - moves b-bit value from src rg to stack ptr

		r2sp_64,	// [ op (8) ][ rg (8) ]
		r2sp_32,	// [ op (8) ][ rg (8) ]
		r2sp_16,	// [ op (8) ][ rg (8) ]
		r2sp_8,		// [ op (8) ][ rg (8) ]

		// rp2r_b - moves b-bit value from src rg ptr to dst rg

		rp2r_64,	// [ op (8) ][ rg (8) ]
		rp2r_32,	// [ op (8) ][ rg (8) ]
		rp2r_16,	// [ op (8) ][ rg (8) ]
		rp2r_8,		// [ op (8) ][ rg (8) ]

		// rp2rp_b - moves b-bit value from src rg ptr to dst rg ptr

		rp2rp_64,	// [ op (8) ][ rg (8) ]
		rp2rp_32,	// [ op (8) ][ rg (8) ]
		rp2rp_16,	// [ op (8) ][ rg (8) ]
		rp2rp_8,	// [ op (8) ][ rg (8) ]

		// rp2s_b - pushes b-bit value from src rg ptr to stack

		rp2s_64,	// [ op (8) ][ rg (8) ]
		rp2s_32,	// [ op (8) ][ rg (8) ]
		rp2s_16,	// [ op (8) ][ rg (8) ]
		rp2s_8,		// [ op (8) ][ rg (8) ]

		// rp2sp_b - pushes b-bit value from src rg ptr to stack ptr

		rp2sp_64,	// [ op (8) ][ rg (8) ]
		rp2sp_32,	// [ op (8) ][ rg (8) ]
		rp2sp_16,	// [ op (8) ][ rg (8) ]
		rp2sp_8,	// [ op (8) ][ rg (8) ]

		// s2r_b - pops b-bit value from stack to dst rg

		s2r_64,		// [ op (8) ][ rg (8) ]
		s2r_32,		// [ op (8) ][ rg (8) ]
		s2r_16,		// [ op (8) ][ rg (8) ]
		s2r_8,		// [ op (8) ][ rg (8) ]

		// s2rp_b - pops b-bit value from stack to dst rg ptr

		s2rp_64,	// [ op (8) ][ rg (8) ]
		s2rp_32,	// [ op (8) ][ rg (8) ]
		s2rp_16,	// [ op (8) ][ rg (8) ]
		s2rp_8,		// [ op (8) ][ rg (8) ]

		// s2s_b - pushes b-bit value from stack to stack

		s2s_64,		// [ op (8) ]
		s2s_32,		// [ op (8) ]
		s2s_16,		// [ op (8) ]
		s2s_8,		// [ op (8) ]

		// s2sp_b - pops b-bit value from stack to stack ptr

		s2sp_64,	// [ op (8) ]
		s2sp_32,	// [ op (8) ]
		s2sp_16,	// [ op (8) ]
		s2sp_8,		// [ op (8) ]

		// sp2r_b - moves b-bit value from stack ptr to dst rg

		sp2r_64,	// [ op (8) ][ rg (8) ]
		sp2r_32,	// [ op (8) ][ rg (8) ]
		sp2r_16,	// [ op (8) ][ rg (8) ]
		sp2r_8,		// [ op (8) ][ rg (8) ]

		// sp2rp_b - moves b-bit value from stack ptr to dst rg ptr

		sp2rp_64,	// [ op (8) ][ rg (8) ]
		sp2rp_32,	// [ op (8) ][ rg (8) ]
		sp2rp_16,	// [ op (8) ][ rg (8) ]
		sp2rp_8,	// [ op (8) ][ rg (8) ]

		// sp2s_b - pushes b-bit value from stack ptr to stack

		sp2s_64,	// [ op (8) ]
		sp2s_32,	// [ op (8) ]
		sp2s_16,	// [ op (8) ]
		sp2s_8,		// [ op (8) ]

		// sp2sp_b - pushes b-bit value from stack ptr to stack ptr

		sp2sp_64,	// [ op (8) ]
		sp2sp_32,	// [ op (8) ]
		sp2sp_16,	// [ op (8) ]
		sp2sp_8,	// [ op (8) ]

		// v2r_b - moves b-bit value from val to dst rg

		v2r_64,		// [ op (8) ][ rg (8) ][ val64 (64) ]
		v2r_32,		// [ op (8) ][ rg (8) ][ val32 (32) ]
		v2r_16,		// [ op (8) ][ rg (8) ][ val16 (16) ]
		v2r_8,		// [ op (8) ][ rg (8) ][ val8 (8) ]

		// v2rp_b - moves b-bit value from val to dst rg ptr

		v2rp_64,	// [ op (8) ][ rg (8) ][ val64 (64) ]
		v2rp_32,	// [ op (8) ][ rg (8) ][ val32 (32) ]
		v2rp_16,	// [ op (8) ][ rg (8) ][ val16 (16) ]
		v2rp_8,		// [ op (8) ][ rg (8) ][ val8 (8) ]

		// v2s_b - pushes b-bit value from val to stack

		v2s_64,		// [ op (8) ][ val64 (64) ]
		v2s_32,		// [ op (8) ][ val32 (32) ]
		v2s_16,		// [ op (8) ][ val16 (16) ]
		v2s_8,		// [ op (8) ][ val8 (8) ]

		// v2sp_b - moves b-bit value from val to stack ptr

		v2sp_64,	// [ op (8) ][ val64 (64) ]
		v2sp_32,	// [ op (8) ][ val32 (32) ]
		v2sp_16,	// [ op (8) ][ val16 (16) ]
		v2sp_8,		// [ op (8) ][ val8 (8) ]

		// r_add_r_b - adds b-bit src rg val to dst rg val

		r_add_r_64,
		r_add_r_32,
		r_add_r_16,
		r_add_r_8,

		r_add_s_64,
		r_add_s_32,
		r_add_s_16,
		r_add_s_8,

		s_add_r_64,
		s_add_r_32,
		s_add_r_16,
		s_add_r_8,

		s_add_s_64,
		s_add_s_32,
		s_add_s_16,
		s_add_s_8,
	};

	struct state
	{
		bool above : 1;
		bool below : 1;

		bool ierr : 1;
		bool iovf : 1;
		bool iunf : 1;

		bool fzer : 1;
		bool fsub : 1;
		bool finf : 1;
		bool fnan : 1;
		bool fnrm : 1;
	};

	struct rg
	{
		uint8_t src : 4;
		uint8_t dst : 4;
	};

	struct val64
	{
		uint64_t value;
	};

	struct val32
	{
		uint32_t value;
	};

	struct val16
	{
		uint16_t value;
	};

	struct val8
	{
		uint8_t value;
	};
}