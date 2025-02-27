#pragma once

#include <cstdint>

namespace interpreter
{
	enum class op : uint8_t
	{
		// Pointer takes 64 bits on frame and stack on all systems

		// call_b - transfers control flow to the instruction by b-bit signed integer
		//          offset value from the instruction following the given instruction
		// 
		//          1. Control flow pointer is pushed to stack
		//          2. Control flow pointer is changed by offset value
		//          3. Stack frame pointer is pushed to stack
		//          4. Stack frame pointer is assigned current stack pointer

		call_64,			// [ op (8) ][ offset (64) ]
		call_32,			// [ op (8) ][ offset (32) ]
		call_16,			// [ op (8) ][ offset (16) ]
		call_8,				// [ op (8) ][ offset (8) ]

		// nativecall - not implemented

		nativecall,

		// ret - transfers control flow to the instruction pointer stored over stack frame
		// 
		//       1. Current stack pointer is assigned stack frame pointer
		//       2. Stack frame pointer is popped from stack
		//       3. Control flow pointer is popped from stack

		ret,				// [ op (8) ]

		// outerret - stops interpreter's work

		outerret,			// [ op (8) ]

		// l_alloc - allocates 16-bit unsigned integer byte count value on stack
		//           affected flags: none

		l_alloc,			// [ op (8) ][ offset (16) ]

		// s_alloc - allocates 8-bit unsigned integer byte count value on stack
		//           affected flags: none

		s_alloc,			// [ op (8) ][ offset (8) ]

		// l_allocz - allocates 16-bit unsigned integer byte count value on stack and zeroes it
		//            affected flags: none

		l_allocz,			// [ op (8) ][ offset (16) ]

		// s_allocz - allocates 8-bit unsigned integer byte count value on stack and zeroes it
		//            affected flags: none

		s_allocz,			// [ op (8) ][ offset (8) ]

		// l_dealloc - deallocates 16-bit unsigned integer byte count value on stack
		//             affected flags: none

		l_dealloc,			// [ op (8) ][ offset (16) ]

		// s_dealloc - deallocates 8-bit unsigned integer byte count value on stack
		//             affected flags: none

		s_dealloc,			// [ op (8) ][ offset (8) ]

		// push_flow - pushes current control flow pointer to stack
		//             affected flags: none

		push_flow,			// [ op (8) ]

		// push_frame - pushes current stack frame pointer to stack
		//              affected flags: none

		push_frame,			// [ op (8) ]

		// push_stack - pushes current stack pointer to stack (copy then allocate)
		//              affected flags: none

		push_stack,			// [ op (8) ]

		// push_state - pushes 16-bit current state value to stack
		//              affected flags: none

		push_state,			// [ op (8) ]

		// pop_flow - pops current control flow pointer from stack
		//            affected flags: none

		pop_flow,			// [ op (8) ]

		// pop_frame - pops current stack frame pointer from stack
		//             affected flags: none

		pop_frame,			// [ op (8) ]

		// pop_stack - pops current stack pointer from stack (deallocate then copy)
		//             affected flags: none

		pop_stack,			// [ op (8) ]

		// pop_state - pops 16-bit current state value from stack
		//             affected flags: none but all

		pop_state,			// [ op (8) ]

		// jmp_b - transfers control flow to the instruction by b-bit signed integer
		//         offset value from the instruction following the given instruction
		//         affected flags: none

		jmp_64,				// [ op (8) ][ offset (64) ]
		jmp_32,				// [ op (8) ][ offset (32) ]
		jmp_16,				// [ op (8) ][ offset (16) ]
		jmp_8,				// [ op (8) ][ offset (8) ]

		// jmp_a_b - transfers control flow to the instruction by b-bit signed integer
		//           offset value from the instruction following the given instruction
		//           if above is true and below is false (if above)
		//           affected flags: none

		jmp_a_64,			// [ op (8) ][ offset (64) ]
		jmp_a_32,			// [ op (8) ][ offset (32) ]
		jmp_a_16,			// [ op (8) ][ offset (16) ]
		jmp_a_8,			// [ op (8) ][ offset (8) ]

		// jmp_ae_b - transfers control flow to the instruction by b-bit signed integer
		//            offset value from the instruction following the given instruction
		//            if below is false (if above or equal)
		//            affected flags: none

		jmp_ae_64,			// [ op (8) ][ offset (64) ]
		jmp_ae_32,			// [ op (8) ][ offset (32) ]
		jmp_ae_16,			// [ op (8) ][ offset (16) ]
		jmp_ae_8,			// [ op (8) ][ offset (8) ]

		// jmp_b_b - transfers control flow to the instruction by b-bit signed integer
		//           offset value from the instruction following the given instruction
		//           if above is false and below is true (if below)
		//           affected flags: none

		jmp_b_64,			// [ op (8) ][ offset (64) ]
		jmp_b_32,			// [ op (8) ][ offset (32) ]
		jmp_b_16,			// [ op (8) ][ offset (16) ]
		jmp_b_8,			// [ op (8) ][ offset (8) ]

		// jmp_be_b - transfers control flow to the instruction by b-bit signed integer
		//            offset value from the instruction following the given instruction
		//            if above is false (if below or equal)
		//            affected flags: none

		jmp_be_64,			// [ op (8) ][ offset (64) ]
		jmp_be_32,			// [ op (8) ][ offset (32) ]
		jmp_be_16,			// [ op (8) ][ offset (16) ]
		jmp_be_8,			// [ op (8) ][ offset (8) ]

		// jmp_e_b - transfers control flow to the instruction by b-bit signed integer
		//           offset value from the instruction following the given instruction
		//           if above is false and below is false (if equal)
		//           affected flags: none

		jmp_e_64,			// [ op (8) ][ offset (64) ]
		jmp_e_32,			// [ op (8) ][ offset (32) ]
		jmp_e_16,			// [ op (8) ][ offset (16) ]
		jmp_e_8,			// [ op (8) ][ offset (8) ]

		// jmp_ne_b - transfers control flow to the instruction by b-bit signed integer
		//            offset value from the instruction following the given instruction
		//            if above is true xor below is true (if not equal)
		//            affected flags: none

		jmp_ne_64,			// [ op (8) ][ offset (64) ]
		jmp_ne_32,			// [ op (8) ][ offset (32) ]
		jmp_ne_16,			// [ op (8) ][ offset (16) ]
		jmp_ne_8,			// [ op (8) ][ offset (8) ]

		// jmp_un_b - transfers control flow to the instruction by b-bit signed integer
		//            offset value from the instruction following the given instruction
		//            if above is true and below is true (if unordered)
		//            affected flags: none

		jmp_un_64,			// [ op (8) ][ offset (64) ]
		jmp_un_32,			// [ op (8) ][ offset (32) ]
		jmp_un_16,			// [ op (8) ][ offset (16) ]
		jmp_un_8,			// [ op (8) ][ offset (8) ]

		// jmp_msk_b - transfers control flow to the instruction by b-bit signed integer
		//             offset value from the instruction following the given instruction
		//             if any of masks match by type with current state
		//             affected flags: none

		jmp_msk_64,			// [ op (8) ][ type (8) ][ mask (64) ][ offset (64) ]
		jmp_msk_32,			// [ op (8) ][ type (8) ][ mask (32) ][ offset (32) ]
		jmp_msk_16,			// [ op (8) ][ type (8) ][ mask (16) ][ offset (16) ]
		jmp_msk_8,			// [ op (8) ][ type (8) ][ mask (8) ][ offset (8) ]

		// l_load_b - pushes b-bit value
		//            from 16-bit signed integer offset value on stack frame to stack
		//            affected flags: none

		l_load_64,			// [ op (8) ][ offset (16) ]
		l_load_32,			// [ op (8) ][ offset (16) ]
		l_load_16,			// [ op (8) ][ offset (16) ]
		l_load_8,			// [ op (8) ][ offset (16) ]

		// l_store_b - pops b-bit value
		//             from stack to 16-bit signed integer offset value on stack frame
		//             affected flags: none

		l_store_64,			// [ op (8) ][ offset (16) ]
		l_store_32,			// [ op (8) ][ offset (16) ]
		l_store_16,			// [ op (8) ][ offset (16) ]
		l_store_8,			// [ op (8) ][ offset (16) ]

		// s_load_b - pushes b-bit value
		//            from 8-bit signed integer offset value on stack frame to stack
		//            affected flags: none

		s_load_64,			// [ op (8) ][ offset (8) ]
		s_load_32,			// [ op (8) ][ offset (8) ]
		s_load_16,			// [ op (8) ][ offset (8) ]
		s_load_8,			// [ op (8) ][ offset (8) ]

		// s_store_b - pops b-bit value
		//             from stack to 8-bit signed integer offset value on stack frame
		//             affected flags: none

		s_store_64,			// [ op (8) ][ offset (8) ]
		s_store_32,			// [ op (8) ][ offset (8) ]
		s_store_16,			// [ op (8) ][ offset (8) ]
		s_store_8,			// [ op (8) ][ offset (8) ]

		// push_b - pushes b-bit value to stack
		//          affected flags: none

		push_64,			// [ op (8) ][ value (64) ]
		push_32,			// [ op (8) ][ value (32) ]
		push_16,			// [ op (8) ][ value (16) ]
		push_8,				// [ op (8) ][ value (8) ]

		// pop_b - pops b-bit value from stack
		//         affected flags: none

		pop_64,				// [ op (8) ]
		pop_32,				// [ op (8) ]
		pop_16,				// [ op (8) ]
		pop_8,				// [ op (8) ]

		// dup_p - duplicates b-bit value on stack
		//         affected flags: none

		dup_64,				// [ op (8) ]
		dup_32,				// [ op (8) ]
		dup_16,				// [ op (8) ]
		dup_8,				// [ op (8) ]

		// memcpy - pops destination memory region pointer, source memory region pointer,
		//          64-bit unsigned integer memory region size value
		//          from stack in the specified order and
		//          copies source memory region content to destination memory region
		//          affected flags: none

		memcpy,				// [ op (8) ]

		// memset - pops destination memory region pointer, 8-bit fill value,
		//          64-bit unsigned integer memory region size value
		//          from stack in the specified order and
		//          fills destination memory region with fill value
		//          affected flags: none

		memset,				// [ op (8) ]

		// ptrcpy_b - pops pointer #1 and pointer #2 from stack in the specified order
		//            and moves pointer #1 b-bit value to pointer #2 b-bit value
		//            affected flags: none

		ptrcpy_64,			// [ op (8) ]
		ptrcpy_32,			// [ op (8) ]
		ptrcpy_16,			// [ op (8) ]
		ptrcpy_8, 			// [ op (8) ]

		// ptrget_b - pops pointer from stack and pushes pointer value to stack
		//            affected flags: none

		ptrget_64,			// [ op (8) ]
		ptrget_32,			// [ op (8) ]
		ptrget_16,			// [ op (8) ]
		ptrget_8,			// [ op (8) ]

		// ptrset_b - pops pointer from stack and pops pointer value from stack
		//            affected flags: none

		ptrset_64,			// [ op (8) ]
		ptrset_32,			// [ op (8) ]
		ptrset_16,			// [ op (8) ]
		ptrset_8,			// [ op (8) ]

		// neg_b - arithmetic negates b-bit signed integer value on stack
		//         affected flags: ierr

		neg_64,				// [ op (8) ]
		neg_32,				// [ op (8) ]
		neg_16,				// [ op (8) ]
		neg_8,				// [ op (8) ]

		// not_b - boolean inverts b-bit value bits on stack
		//         affected flags: none

		not_64,				// [ op (8) ]
		not_32,				// [ op (8) ]
		not_16,				// [ op (8) ]
		not_8,				// [ op (8) ]

		// sinc_b - arithmetic increments b-bit signed integer value on stack
		//          affected flags: iovf, iunf

		sinc_64,			// [ op (8) ]
		sinc_32,			// [ op (8) ]
		sinc_16,			// [ op (8) ]
		sinc_8,				// [ op (8) ]

		// uinc_b - arithmetic increments b-bit unsigned integer value on stack
		//          affected flags: iovf, iunf

		uinc_64,			// [ op (8) ]
		uinc_32,			// [ op (8) ]
		uinc_16,			// [ op (8) ]
		uinc_8,				// [ op (8) ]

		// sdec_b - arithmetic decrements b-bit signed integer value on stack
		//          affected flags: iovf, iunf

		sdec_64,			// [ op (8) ]
		sdec_32,			// [ op (8) ]
		sdec_16,			// [ op (8) ]
		sdec_8,				// [ op (8) ]

		// udec_b - arithmetic decrements b-bit unsigned integer value on stack
		//          affected flags: iovf, iunf

		udec_64,			// [ op (8) ]
		udec_32,			// [ op (8) ]
		udec_16,			// [ op (8) ]
		udec_8,				// [ op (8) ]

		// and_b - pops b-bit value from stack and boolean
		//         ANDs it to the next b-bit value on stack
		//         affected flags: none

		and_64,				// [ op (8) ]
		and_32,				// [ op (8) ]
		and_16,				// [ op (8) ]
		and_8,				// [ op (8) ]

		// or_b - pops b-bit value from stack and boolean
		//        ORs it to the next b-bit value on stack
		//        affected flags: none

		or_64,				// [ op (8) ]
		or_32,				// [ op (8) ]
		or_16,				// [ op (8) ]
		or_8,				// [ op (8) ]

		// xor_b - pops b-bit value from stack and boolean
		//         XORs it to the next b-bit value on stack
		//         affected flags: none

		xor_64,				// [ op (8) ]
		xor_32,				// [ op (8) ]
		xor_16,				// [ op (8) ]
		xor_8,				// [ op (8) ]

		// sadd_b - pops b-bit signed integer value from stack and arithmetic
		//          adds it to the next b-bit signed integer value on stack
		//          affected flags: iovf, iunf

		sadd_64,			// [ op (8) ]
		sadd_32,			// [ op (8) ]
		sadd_16,			// [ op (8) ]
		sadd_8,				// [ op (8) ]

		// uadd_b - pops b-bit unsigned integer value from stack and arithmetic
		//          adds it to the next b-bit unsigned integer value on stack
		//          affected flags: iovf, iunf

		uadd_64,			// [ op (8) ]
		uadd_32,			// [ op (8) ]
		uadd_16,			// [ op (8) ]
		uadd_8,				// [ op (8) ]

		// ssub_b - pops b-bit signed integer value from stack and arithmetic
		//          subtracts it from the next b-bit signed integer value on stack
		//          affected flags: iovf, iunf

		ssub_64,			// [ op (8) ]
		ssub_32,			// [ op (8) ]
		ssub_16,			// [ op (8) ]
		ssub_8,				// [ op (8) ]

		// usub_b - pops b-bit unsigned integer value from stack and arithmetic
		//          subtracts it from the next b-bit unsigned integer value on stack
		//          affected flags: iovf, iunf

		usub_64,			// [ op (8) ]
		usub_32,			// [ op (8) ]
		usub_16,			// [ op (8) ]
		usub_8,				// [ op (8) ]

		// smul_b - pops b-bit signed integer value from stack and arithmetic
		//          multiplies the next b-bit signed integer value on stack by it
		//          affected flags: iovf, iunf

		smul_64,			// [ op (8) ]
		smul_32,			// [ op (8) ]
		smul_16,			// [ op (8) ]
		smul_8,				// [ op (8) ]

		// umul_b - pops b-bit unsigned integer value from stack and arithmetic
		//          multiplies the next b-bit unsigned integer value on stack by it
		//          affected flags: iovf, iunf

		umul_64,			// [ op (8) ]
		umul_32,			// [ op (8) ]
		umul_16,			// [ op (8) ]
		umul_8,				// [ op (8) ]

		// smod_b - pops b-bit signed integer value from stack and computes arithmetic
		//          modulo of the next b-bit signed integer value on stack by it
		//          affected flags: ierr

		smod_64,			// [ op (8) ]
		smod_32,			// [ op (8) ]
		smod_16,			// [ op (8) ]
		smod_8,				// [ op (8) ]

		// umod_b - pops b-bit unsigned integer value from stack and computes arithmetic
		//          modulo of the next b-bit unsigned integer value on stack by it
		//          affected flags: ierr

		umod_64,			// [ op (8) ]
		umod_32,			// [ op (8) ]
		umod_16,			// [ op (8) ]
		umod_8,				// [ op (8) ]

		// sdiv_b - pops b-bit signed integer value from stack and arithmetic
		//          divides the next b-bit signed integer value on stack by it
		//          affected flags: ierr

		sdiv_64,			// [ op (8) ]
		sdiv_32,			// [ op (8) ]
		sdiv_16,			// [ op (8) ]
		sdiv_8,				// [ op (8) ]

		// udiv_b - pops b-bit unsigned integer value from stack and arithmetic
		//          divides the next b-bit unsigned integer value on stack by it
		//          affected flags: ierr

		udiv_64,			// [ op (8) ]
		udiv_32,			// [ op (8) ]
		udiv_16,			// [ op (8) ]
		udiv_8,				// [ op (8) ]

		// scmp_b - pops b-bit signed integer value #1 and b-bit signed integer value #2
		//          from stack in the specified order and arithmetic compares them
		//          affected flags: above, below

		scmp_64,			// [ op (8) ]
		scmp_32,			// [ op (8) ]
		scmp_16,			// [ op (8) ]
		scmp_8,				// [ op (8) ]

		// ucmp_b - pops b-bit unsigned integer value #1 and b-bit unsigned integer value #2
		//          from stack in the specified order and arithmetic compares them
		//          affected flags: above, below

		ucmp_64,			// [ op (8) ]
		ucmp_32,			// [ op (8) ]
		ucmp_16,			// [ op (8) ]
		ucmp_8,				// [ op (8) ]

		// rotl_b - pops 8-bit unsigned integer value from stack and rotates left
		//          the next b-bit unsigned integer value on stack by given bit count
		//          affected flags: none

		rotl_64,			// [ op (8) ]
		rotl_32,			// [ op (8) ]
		rotl_16,			// [ op (8) ]
		rotl_8,				// [ op (8) ]

		// rotr_b - pops 8-bit unsigned integer value from stack and rotates right
		//          the next b-bit unsigned integer value on stack by given bit count
		//          affected flags: none

		rotr_64,			// [ op (8) ]
		rotr_32,			// [ op (8) ]
		rotr_16,			// [ op (8) ]
		rotr_8,				// [ op (8) ]

		// sshl_b - pops 8-bit unsigned integer value from stack and shifts left
		//          the next b-bit signed integer value on stack by given bit count
		//          affected flags: none

		sshl_64,			// [ op (8) ]
		sshl_32,			// [ op (8) ]
		sshl_16,			// [ op (8) ]
		sshl_8,				// [ op (8) ]

		// ushl_b - pops 8-bit unsigned integer value from stack and shifts left
		//          the next b-bit unsigned integer value on stack by given bit count
		//          affected flags: none

		ushl_64,			// [ op (8) ]
		ushl_32,			// [ op (8) ]
		ushl_16,			// [ op (8) ]
		ushl_8,				// [ op (8) ]

		// sshr_b - pops 8-bit unsigned integer value from stack and shifts right
		//          the next b-bit signed integer value on stack by given bit count
		//          affected flags: none

		sshr_64,			// [ op (8) ]
		sshr_32,			// [ op (8) ]
		sshr_16,			// [ op (8) ]
		sshr_8,				// [ op (8) ]

		// ushr_b - pops 8-bit unsigned integer value from stack and shifts right
		//          the next b-bit unsigned integer value on stack by given bit count
		//          affected flags: none

		ushr_64,			// [ op (8) ]
		ushr_32,			// [ op (8) ]
		ushr_16,			// [ op (8) ]
		ushr_8,				// [ op (8) ]

		// fneg_b - arithmetic negates b-bit float value on stack
		//          affected flags: none

		fneg_64,			// [ op (8) ]
		fneg_32,			// [ op (8) ]

		// fevl_b - pops b-bit float value from stack and evaluates it
		//          affected flags: fzer, fsub, finf, fnan, fnrm

		fevl_64,			// [ op (8) ]
		fevl_32,			// [ op (8) ]

		// fadd_b - pops b-bit float value from stack and arithmetic
		//          adds it to the next b-bit float value on stack
		//          affected flags: none

		fadd_64,			// [ op (8) ]
		fadd_32,			// [ op (8) ]

		// fadd_b - pops b-bit float value from stack and arithmetic
		//          subtracts it from the next b-bit float value on stack
		//          affected flags: none

		fsub_64,			// [ op (8) ]
		fsub_32,			// [ op (8) ]

		// fmul_b - pops b-bit float value from stack and arithmetic
		//          multiplies the next b-bit float value on stack by it
		//          affected flags: none

		fmul_64,			// [ op (8) ]
		fmul_32,			// [ op (8) ]

		// fmod_b - pops b-bit float value from stack and computes arithmetic
		//          modulo of the next b-bit float value on stack by it
		//          affected flags: none

		fmod_64,			// [ op (8) ]
		fmod_32,			// [ op (8) ]

		// fdiv_b - pops b-bit float value from stack and arithmetic
		//          divides the next b-bit float value on stack by it
		//          affected flags: none

		fdiv_64,			// [ op (8) ]
		fdiv_32,			// [ op (8) ]

		// fcmp_b - pops b-bit float value #1 and b-bit float value #2
		//          from stack in the specified order and arithmetic compares them
		//          affected flags: above, below

		fcmp_64,			// [ op (8) ]
		fcmp_32,			// [ op (8) ]

		// sib1_2_fb2 - converts b1-bit signed integer value to b2-bit float value on stack
		//              affected flags: none

		si64_2_f64,			// [ op (8) ]
		si64_2_f32,			// [ op (8) ]
		si32_2_f64,			// [ op (8) ]
		si32_2_f32,			// [ op (8) ]

		// uib1_2_fb2 - converts b1-bit unsigned integer value to b2-bit float value on stack
		//              affected flags: none

		ui64_2_f64,			// [ op (8) ]
		ui64_2_f32,			// [ op (8) ]
		ui32_2_f64,			// [ op (8) ]
		ui32_2_f32,			// [ op (8) ]

		// fb1_2_sib2 - converts b1-bit float value to b2-bit signed integer value on stack
		//              affected flags: ierr, iovf, iunf

		f64_2_si64,			// [ op (8) ]
		f64_2_si32,			// [ op (8) ]
		f32_2_si64,			// [ op (8) ]
		f32_2_si32,			// [ op (8) ]

		// fb1_2_uib2 - converts b1-bit float value to b2-bit unsigned integer value on stack
		//              affected flags: ierr, iovf, iunf

		f64_2_ui64,			// [ op (8) ]
		f64_2_ui32,			// [ op (8) ]
		f32_2_ui64,			// [ op (8) ]
		f32_2_ui32,			// [ op (8) ]
	};
}