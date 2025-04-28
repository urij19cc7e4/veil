#pragma once

#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>

#include "define.hpp"
#include "interpreter_type.hpp"
#include "native.hpp"
#include "op.hpp"
#include "stack_be.hpp"
#include "stack_le.hpp"

#ifdef MSVCROTATE
#include <intrin.h>
#endif

namespace interpreter
{
	template <VALUE V>
	struct match
	{
	public:
		static inline bool check(const state& state, V mask, type type)
		{
			switch (type)
			{
			case type::v0:
			{
			#pragma loop(unroll)
				for (uint64_t i = 0ui64; i < mask.v8_count; ++i)
					if ((state.eval.byte.value | mask.v8[i].ui) == mask.v8[i].ui)
						return true;

				return false;
			}

			case type::v1:
			{
			#pragma loop(unroll)
				for (uint64_t i = 0ui64; i < mask.v8_count; ++i)
					if ((state.eval.byte.value & mask.v8[i].ui) == mask.v8[i].ui)
						return true;

				return false;
			}

			default:
				__assume(false);
				break;
			}
		}
	};

	template <std::endian endianness>
	struct match<val8<endianness>>
	{
	public:
		static inline bool check(const state& state, val8<endianness> mask, type type)
		{
			switch (type)
			{
			case type::v0:
				return (state.eval.byte.value | mask.ui) == mask.ui;

			case type::v1:
				return (state.eval.byte.value & mask.ui) == mask.ui;

			default:
				__assume(false);
				break;
			}
		}
	};

	template <VALUE V>
	struct __mul;

	template <std::endian endianness>
	struct __mul<val64<endianness>>
	{
	private:
		static inline void __exumul
		(
			const uint32_t& o1_h,
			const uint32_t& o1_l,
			const uint32_t& o2_h,
			const uint32_t& o2_l,
			uint64_t& rh,
			uint64_t& rl
		)
		{
			const uint64_t hi = (uint64_t)o1_h * (uint64_t)o1_l;
			const uint64_t m1 = (uint64_t)o1_h * (uint64_t)o2_l;
			const uint64_t m2 = (uint64_t)o2_h * (uint64_t)o1_l;
			const uint64_t lo = (uint64_t)o2_h * (uint64_t)o2_l;

			const uint64_t __m1 = m1 + (lo >> 0x20ui8);
			const uint64_t __m2 = m2 + (uint64_t)((uint32_t)__m1);

			rh = hi + (__m1 >> 0x20ui8) + (__m2 >> 0x20ui8);
			rl = (uint64_t)((uint32_t)lo) | ((uint64_t)((uint32_t)__m2) << 0x20ui8);
		}

	public:
		static inline void smul(val64<endianness>& v, const int64_t& si, state& state)
		{
			// !!! ARM64EC also defines _M_X64 !!!
		#if (defined(_M_ARM64) || defined(_M_ARM64EC))
			const int64_t vsi_h = __mulh(v.si, si);
			v.si *= si;

			const int64_t ovf = (int64_t)(((uint64_t)vsi_h << 0x01ui8) | ((uint64_t)v.si >> 0x3Fui8));

			state.eval.bits.iovf = ovf > 0i64;
			state.eval.bits.iunf = ovf < -1i64;
		#elif (defined(_M_X64))
			int64_t vsi_h;
			v.si = _mul128(si, v.si, &vsi_h);

			const int64_t ovf = (int64_t)(((uint64_t)vsi_h << 0x01ui8) | ((uint64_t)v.si >> 0x3Fui8));

			state.eval.bits.iovf = ovf > 0i64;
			state.eval.bits.iunf = ovf < -1i64;
		#else
			constexpr uint32_t __2_pow_31 = 2147483648ui32;

			const bool vsi_sign = v.si < 0i64;
			const bool si_sign = si < 0i64;

			const uint64_t vsi_abs = (uint64_t)(vsi_sign ? -v.si : v.si);
			const uint64_t si_abs = (uint64_t)(si_sign ? -si : si);

			const uint32_t vsi_abs_h = (uint32_t)(vsi_abs >> 0x20ui8);
			const uint32_t vsi_abs_l = (uint32_t)vsi_abs;

			const uint32_t si_abs_h = (uint32_t)(si_abs >> 0x20ui8);
			const uint32_t si_abs_l = (uint32_t)si_abs;

			if (vsi_abs_h == 0ui32 && si_abs_h == 0ui32 && vsi_abs_l < __2_pow_31 && si_abs_l < __2_pow_31)
			{
				v.si = (int64_t)(int32_t)v.si * (int64_t)(int32_t)si;

				state.eval.bits.iovf = false;
				state.eval.bits.iunf = false;
			}
			else
			{
				uint64_t rh, rl;
				__exumul(vsi_abs_h, vsi_abs_l, si_abs_h, si_abs_l, rh, rl);

				v.si = (vsi_sign ^ si_sign) ? -(int64_t)rl : (int64_t)rl;
				const uint64_t ovf = (rh << 0x01ui8) | (rl >> 0x3Fui8);

				if (ovf != 0ui64)
				{
					state.eval.bits.iovf = !(vsi_sign ^ si_sign);
					state.eval.bits.iunf = vsi_sign ^ si_sign;
				}
				else
				{
					state.eval.bits.iovf = false;
					state.eval.bits.iunf = false;
				}
			}
		#endif
		}

		static inline void umul(val64<endianness>& v, const uint64_t& ui, state& state)
		{
			// !!! ARM64EC also defines _M_X64 !!!
		#if (defined(_M_ARM64) || defined(_M_ARM64EC))
			const uint64_t vui_h = __umulh(v.ui, ui);
			v.ui *= ui;

			state.eval.bits.iovf = vui_h != 0ui64;
			state.eval.bits.iunf = false;
		#elif (defined(_M_X64))
			uint64_t vui_h;
			v.ui = _umul128(ui, v.ui, &vui_h);

			state.eval.bits.iovf = vui_h != 0ui64;
			state.eval.bits.iunf = false;
		#else
			const uint32_t vui_h = (uint32_t)(v.ui >> 0x20ui8);
			const uint32_t vui_l = (uint32_t)v.ui;

			const uint32_t ui_h = (uint32_t)(ui >> 0x20ui8);
			const uint32_t ui_l = (uint32_t)ui;

			if ((vui_h | ui_h) == 0ui32)
			{
				v.ui = (uint64_t)vui_l * (uint64_t)ui_l;

				state.eval.bits.iovf = false;
				state.eval.bits.iunf = false;
			}
			else
			{
				uint64_t rh, rl;
				__exumul(vui_h, vui_l, ui_h, ui_l, rh, rl);

				v.ui = rl;

				state.eval.bits.iovf = rh != 0ui64;
				state.eval.bits.iunf = false;
			}
		#endif
		}
	};

	template <std::endian endianness>
	struct __mul<val32<endianness>>
	{
	public:
		static inline void smul(val32<endianness>& v, const int32_t& si, state& state)
		{
			val64<endianness> ext_v;

			ext_v.si = (int64_t)v.si * (int64_t)si;
			v.si = ext_v.half.lo.si;

			state.eval.bits.iovf = ext_v.half.hi.si > 0i32;
			state.eval.bits.iunf = ext_v.half.hi.si < -1i32;
		}

		static inline void umul(val32<endianness>& v, const uint32_t& ui, state& state)
		{
			val64<endianness> ext_v;

			ext_v.ui = (uint64_t)v.ui * (uint64_t)ui;
			v.ui = ext_v.half.lo.ui;

			state.eval.bits.iovf = ext_v.half.hi.ui != 0ui32;
			state.eval.bits.iunf = false;
		}
	};

	template <std::endian endianness>
	struct __mul<val16<endianness>>
	{
	public:
		static inline void smul(val16<endianness>& v, const int16_t& si, state& state)
		{
			val32<endianness> ext_v;

			ext_v.si = (int32_t)v.si * (int32_t)si;
			v.si = ext_v.half.lo.si;

			state.eval.bits.iovf = ext_v.half.hi.si > 0i16;
			state.eval.bits.iunf = ext_v.half.hi.si < -1i16;
		}

		static inline void umul(val16<endianness>& v, const uint16_t& ui, state& state)
		{
			val32<endianness> ext_v;

			ext_v.ui = (uint32_t)v.ui * (uint32_t)ui;
			v.ui = ext_v.half.lo.ui;

			state.eval.bits.iovf = ext_v.half.hi.ui != 0ui16;
			state.eval.bits.iunf = false;
		}
	};

	template <std::endian endianness>
	struct __mul<val8<endianness>>
	{
	public:
		static inline void smul(val8<endianness>& v, const int8_t& si, state& state)
		{
			val16<endianness> ext_v;

			ext_v.si = (int16_t)v.si * (int16_t)si;
			v.si = ext_v.half.lo.si;

			state.eval.bits.iovf = ext_v.half.hi.si > 0i8;
			state.eval.bits.iunf = ext_v.half.hi.si < -1i8;
		}

		static inline void umul(val8<endianness>& v, const uint8_t& ui, state& state)
		{
			val16<endianness> ext_v;

			ext_v.ui = (uint16_t)v.ui * (uint16_t)ui;
			v.ui = ext_v.half.lo.ui;

			state.eval.bits.iovf = ext_v.half.hi.ui != 0ui8;
			state.eval.bits.iunf = false;
		}
	};

#ifdef MSVCROTATE
	template <VALUE V>
	struct msvcrotate;

	template <std::endian endianness>
	struct msvcrotate<val64<endianness>>
	{
	public:
		static inline uint64_t rotl(uint64_t value, uint8_t count)
		{
			return _rotl64(value, count);
		}

		static inline uint64_t rotr(uint64_t value, uint8_t count)
		{
			return _rotr64(value, count);
		}
	};

	template <std::endian endianness>
	struct msvcrotate<val32<endianness>>
	{
	public:
		static inline uint32_t rotl(uint32_t value, uint8_t count)
		{
			return _rotl(value, count);
		}

		static inline uint32_t rotr(uint32_t value, uint8_t count)
		{
			return _rotr(value, count);
		}
	};

	template <std::endian endianness>
	struct msvcrotate<val16<endianness>>
	{
	public:
		static inline uint16_t rotl(uint16_t value, uint8_t count)
		{
			return _rotl16(value, count);
		}

		static inline uint16_t rotr(uint16_t value, uint8_t count)
		{
			return _rotr16(value, count);
		}
	};

	template <std::endian endianness>
	struct msvcrotate<val8<endianness>>
	{
	public:
		static inline uint8_t rotl(uint8_t value, uint8_t count)
		{
			return _rotl8(value, count);
		}

		static inline uint8_t rotr(uint8_t value, uint8_t count)
		{
			return _rotr8(value, count);
		}
	};
#endif

	class dispatcher
	{
	private:
		using stack_type = stack<std::endian::native>;

		static constexpr const char _err_msg_fetch_code[] = "Fetch out of code range";
		static constexpr const char _err_msg_wrong_fltp[] = "Wrong float class";
		static constexpr const char _err_msg_wrong_opnd[] = "Wrong operand";
		static constexpr const char _err_msg_wrong_type[] = "Wrong mask type";

	#ifdef FETCHCHECK
		const uint8_t* _code_beg;
		const uint8_t* _code_end;
	#endif

		nativehub _nativehub;
		stack_type _stack;
		const uint8_t* _opptr;
		state _state;

		template <VALUE V>
		static inline V dil_fetch(const uint8_t*& opptr)
		{
			V t = *((V*)opptr);
			opptr += (ptrdiff_t)sizeof(V);

			return t;
		}

		template <VALUE V>
		static inline void __dil_and(uintptr_t& stop)
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = stack_type::dil_pop<V>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

			v.ui &= ui;
		}

		template <VALUE V>
		static inline void __dil_call(const uint8_t*& opptr, uintptr_t& ftop, uintptr_t& stop)
		{
			const ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			stack_type::dil_push_frame(ftop, stop, (uintptr_t)opptr);
			opptr += os;
		}

		template <VALUE V>
		static inline void __dil_fadd(uintptr_t& stop)
		{
			using vf_t = decltype(V::f);
			const vf_t f = stack_type::dil_pop<V>(stop).f;
			V& v = stack_type::dil_top<V>(stop);

			v.f += f;
		}

		template <VALUE V>
		static inline void __dil_fcmp(state& state, uintptr_t& stop)
		{
			using vf_t = decltype(V::f);
			const vf_t f_1 = stack_type::dil_pop<V>(stop).f;
			const vf_t f_2 = stack_type::dil_pop<V>(stop).f;

			state.comp.bits.above = !(f_1 <= f_2);
			state.comp.bits.below = !(f_1 >= f_2);
		}

		template <VALUE V>
		static inline void __dil_fdiv(uintptr_t& stop)
		{
			using vf_t = decltype(V::f);
			const vf_t f = stack_type::dil_pop<V>(stop).f;
			V& v = stack_type::dil_top<V>(stop);

			v.f /= f;
		}

		template <VALUE V>
		static inline void __dil_fevl(state& state, uintptr_t& stop)
		{
			using vf_t = decltype(V::f);
			const vf_t f = stack_type::dil_pop<V>(stop).f;

			state.eval.bits.fzer = false;
			state.eval.bits.fsub = false;
			state.eval.bits.finf = false;
			state.eval.bits.fnan = false;
			state.eval.bits.fnrm = false;

			switch (fpclassify(f))
			{
			case FP_ZERO:
				state.eval.bits.fzer = true;
				break;

			case FP_SUBNORMAL:
				state.eval.bits.fsub = true;
				break;

			case FP_INFINITE:
				state.eval.bits.finf = true;
				break;

			case FP_NAN:
				state.eval.bits.fnan = true;
				break;

			case FP_NORMAL:
				state.eval.bits.fnrm = true;
				break;

			default:
			#ifdef FLOATCHECK
				throw std::runtime_error(_err_msg_wrong_fltp);
			#else
				__assume(false);
			#endif
				break;
			}
		}

		template <VALUE V>
		static inline void __dil_fmod(uintptr_t& stop)
		{
			using vf_t = decltype(V::f);
			const vf_t f = stack_type::dil_pop<V>(stop).f;
			V& v = stack_type::dil_top<V>(stop);

			v.f = fmod(v.f, f);
		}

		template <VALUE V>
		static inline void __dil_fmul(uintptr_t& stop)
		{
			using vf_t = decltype(V::f);
			const vf_t f = stack_type::dil_pop<V>(stop).f;
			V& v = stack_type::dil_top<V>(stop);

			v.f *= f;
		}

		template <VALUE V>
		static inline void __dil_fneg(const uintptr_t& stop)
		{
			V& v = stack_type::dil_top<V>(stop);

			v.f = -v.f;
		}

		template <VALUE V>
		static inline void __dil_fsub(uintptr_t& stop)
		{
			using vf_t = decltype(V::f);
			const vf_t f = stack_type::dil_pop<V>(stop).f;
			V& v = stack_type::dil_top<V>(stop);

			v.f -= f;
		}

		template <VALUE V>
		static inline void __dil_jmp(const uint8_t*& opptr)
		{
			const ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_a(const uint8_t*& opptr, const state& state)
		{
			const ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if (state.comp.bits.above && !state.comp.bits.below)
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_ae(const uint8_t*& opptr, const state& state)
		{
			const ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if (!state.comp.bits.below)
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_b(const uint8_t*& opptr, const state& state)
		{
			const ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if (!state.comp.bits.above && state.comp.bits.below)
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_be(const uint8_t*& opptr, const state& state)
		{
			const ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if (!state.comp.bits.above)
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_e(const uint8_t*& opptr, const state& state)
		{
			const ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if (!state.comp.bits.above && !state.comp.bits.below)
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_ne(const uint8_t*& opptr, const state& state)
		{
			const ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if ((bool)(state.comp.bits.above ^ state.comp.bits.below))
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_un(const uint8_t*& opptr, const state& state)
		{
			const ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if (state.comp.bits.above && state.comp.bits.below)
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_msk(const uint8_t*& opptr, const state& state)
		{
			type _type = (type)dil_fetch<val8<>>(opptr).ui;
			V _mask = dil_fetch<V>(opptr);

			const ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if (match<V>::check(state, _mask, _type))
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_l_load(const uint8_t*& opptr, const uintptr_t& ftop, uintptr_t& stop)
		{
			const ptrdiff_t os = (ptrdiff_t)dil_fetch<val16<>>(opptr).si;

			stack_type::dil_load<V>(ftop, stop, os);
		}

		template <VALUE V>
		static inline void __dil_l_store(const uint8_t*& opptr, const uintptr_t& ftop, uintptr_t& stop)
		{
			const ptrdiff_t os = (ptrdiff_t)dil_fetch<val16<>>(opptr).si;

			stack_type::dil_store<V>(ftop, stop, os);
		}

		template <VALUE V>
		static inline void __dil_neg(state& state, const uintptr_t& stop)
		{
			using vsi_t = decltype(V::si);
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.ierr = v.si == numeric_limits<vsi_t>::min();
		#endif

			v.si = -v.si;
		}

		template <VALUE V>
		static inline void __dil_not(const uintptr_t& stop)
		{
			V& v = stack_type::dil_top<V>(stop);

			v.ui = ~v.ui;
		}

		template <VALUE V>
		static inline void __dil_or(uintptr_t& stop)
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = stack_type::dil_pop<V>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

			v.ui |= ui;
		}

		template <VALUE V>
		static inline void __dil_ptrcpy(uintptr_t& stop)
		{
			V* ptr_1 = (V*)stack_type::dil_pop_ptr(stop);
			V* ptr_2 = (V*)stack_type::dil_pop_ptr(stop);

			*ptr_2 = *ptr_1;
		}

		template <VALUE V>
		static inline void __dil_ptrget(uintptr_t& stop)
		{
			V* ptr = (V*)stack_type::dil_pop_ptr(stop);
			V val = *ptr;

			stack_type::dil_push<V>(stop, val);
		}

		template <VALUE V>
		static inline void __dil_ptrset(uintptr_t& stop)
		{
			V* ptr = (V*)stack_type::dil_pop_ptr(stop);
			V val = stack_type::dil_pop<V>(stop);

			*ptr = val;
		}

		template <VALUE V>
		static inline void __dil_push(const uint8_t*& opptr, uintptr_t& stop)
		{
			V val = dil_fetch<V>(opptr);

			stack_type::dil_push<V>(stop, val);
		}

		template <VALUE V>
		static inline void __dil_rotl(uintptr_t& stop)
		{
			uint8_t c = stack_type::dil_pop<val8<>>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

		#ifdef MSVCROTATE
			v.ui = msvcrotate<V>::rotl(v.ui, c);
		#else
			v.ui = (v.ui << c) | (v.ui >> ((uint8_t)sizeof(V) * 8ui8 - c));
		#endif
		}

		template <VALUE V>
		static inline void __dil_rotr(uintptr_t& stop)
		{
			uint8_t c = stack_type::dil_pop<val8<>>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

		#ifdef MSVCROTATE
			v.ui = msvcrotate<V>::rotr(v.ui, c);
		#else
			v.ui = (v.ui >> c) | (v.ui << ((uint8_t)sizeof(V) * 8ui8 - c));
		#endif
		}

		template <VALUE V>
		static inline void __dil_s_load(const uint8_t*& opptr, const uintptr_t& ftop, uintptr_t& stop)
		{
			const ptrdiff_t os = (ptrdiff_t)dil_fetch<val8<>>(opptr).si;

			stack_type::dil_load<V>(ftop, stop, os);
		}

		template <VALUE V>
		static inline void __dil_s_store(const uint8_t*& opptr, const uintptr_t& ftop, uintptr_t& stop)
		{
			const ptrdiff_t os = (ptrdiff_t)dil_fetch<val8<>>(opptr).si;

			stack_type::dil_store<V>(ftop, stop, os);
		}

		template <VALUE V>
		static inline void __dil_sadd(state& state, uintptr_t& stop)
		{
			using vsi_t = decltype(V::si);
			const vsi_t si = stack_type::dil_pop<V>(stop).si;
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.iovf = si > (vsi_t)0 && v.si > numeric_limits<vsi_t>::max() - si;
			state.eval.bits.iunf = si < (vsi_t)0 && v.si < numeric_limits<vsi_t>::min() - si;
		#endif

			v.si += si;
		}

		template <VALUE V>
		static inline void __dil_scmp(state& state, uintptr_t& stop)
		{
			using vsi_t = decltype(V::si);
			vsi_t si_1 = stack_type::dil_pop<V>(stop).si;
			vsi_t si_2 = stack_type::dil_pop<V>(stop).si;

			state.comp.bits.above = si_1 > si_2;
			state.comp.bits.below = si_1 < si_2;
		}

		template <VALUE V>
		static inline void __dil_sdec(state& state, const uintptr_t& stop)
		{
			using vsi_t = decltype(V::si);
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.iovf = false;
			state.eval.bits.iunf = v.si == numeric_limits<vsi_t>::min();
		#endif

			--v.si;
		}

		template <VALUE V>
		static inline void __dil_sdiv(state& state, uintptr_t& stop)
		{
			using vsi_t = decltype(V::si);
			const vsi_t si = stack_type::dil_pop<V>(stop).si;
			V& v = stack_type::dil_top<V>(stop);

			bool zeromirror = si == (vsi_t)0 || (si == (vsi_t)-1 && v.si == numeric_limits<vsi_t>::min());
		#ifdef OPINTCHECK
			state.eval.bits.ierr = zeromirror;
		#endif

			if (!zeromirror)
				v.si /= si;
		}

		template <VALUE V>
		static inline void __dil_sinc(state& state, const uintptr_t& stop)
		{
			using vsi_t = decltype(V::si);
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.iovf = v.si == numeric_limits<vsi_t>::max();
			state.eval.bits.iunf = false;
		#endif

			++v.si;
		}

		template <VALUE V>
		static inline void __dil_smod(state& state, uintptr_t& stop)
		{
			using vsi_t = decltype(V::si);
			const vsi_t si = stack_type::dil_pop<V>(stop).si;
			V& v = stack_type::dil_top<V>(stop);

			bool zero = si == (vsi_t)0;
		#ifdef OPINTCHECK
			state.eval.bits.ierr = zero;
		#endif

			if (!zero)
			{
				if (si == (vsi_t)1 || si == (vsi_t)-1)
					v.si = (vsi_t)0;
				else
					v.si %= si;
			}
		}

		template <VALUE V>
		static inline void __dil_smul(state& state, uintptr_t& stop)
		{
			using vsi_t = decltype(V::si);
			const vsi_t si = stack_type::dil_pop<V>(stop).si;
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			__mul<V>::smul(v, si, state);
		#else
			v.si *= si;
		#endif
		}

		template <VALUE V>
		static inline void __dil_sshl(uintptr_t& stop)
		{
			uint8_t c = stack_type::dil_pop<val8<>>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

			v.si <<= c;
		}

		template <VALUE V>
		static inline void __dil_sshr(uintptr_t& stop)
		{
			uint8_t c = stack_type::dil_pop<val8<>>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

			v.si >>= c;
		}

		template <VALUE V>
		static inline void __dil_ssub(state& state, uintptr_t& stop)
		{
			using vsi_t = decltype(V::si);
			const vsi_t si = stack_type::dil_pop<V>(stop).si;
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.iovf = si < (vsi_t)0 && v.si > numeric_limits<vsi_t>::max() + si;
			state.eval.bits.iunf = si > (vsi_t)0 && v.si < numeric_limits<vsi_t>::min() + si;
		#endif

			v.si -= si;
		}

		template <VALUE V>
		static inline void __dil_uadd(state& state, uintptr_t& stop)
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = stack_type::dil_pop<V>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.iovf = v.ui > numeric_limits<vui_t>::max() - ui;
			state.eval.bits.iunf = false;
		#endif

			v.ui += ui;
		}

		template <VALUE V>
		static inline void __dil_ucmp(state& state, uintptr_t& stop)
		{
			using vui_t = decltype(V::ui);
			vui_t ui_1 = stack_type::dil_pop<V>(stop).ui;
			vui_t ui_2 = stack_type::dil_pop<V>(stop).ui;

			state.comp.bits.above = ui_1 > ui_2;
			state.comp.bits.below = ui_1 < ui_2;
		}

		template <VALUE V>
		static inline void __dil_udec(state& state, const uintptr_t& stop)
		{
			using vui_t = decltype(V::ui);
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.iovf = false;
			state.eval.bits.iunf = v.ui == numeric_limits<vui_t>::min();
		#endif

			--v.ui;
		}

		template <VALUE V>
		static inline void __dil_udiv(state& state, uintptr_t& stop)
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = stack_type::dil_pop<V>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

			bool zero = ui == (vui_t)0;
		#ifdef OPINTCHECK
			state.eval.bits.ierr = zero;
		#endif

			if (!zero)
				v.ui /= ui;
		}

		template <VALUE V>
		static inline void __dil_uinc(state& state, const uintptr_t& stop)
		{
			using vui_t = decltype(V::ui);
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.iovf = v.ui == numeric_limits<vui_t>::max();
			state.eval.bits.iunf = false;
		#endif

			++v.ui;
		}

		template <VALUE V>
		static inline void __dil_umod(state& state, uintptr_t& stop)
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = stack_type::dil_pop<V>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

			bool zero = ui == (vui_t)0;
		#ifdef OPINTCHECK
			state.eval.bits.ierr = zero;
		#endif

			if (!zero)
				v.ui %= ui;
		}

		template <VALUE V>
		static inline void __dil_umul(state& state, uintptr_t& stop)
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = stack_type::dil_pop<V>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			__mul<V>::umul(v, ui, state);
		#else
			v.ui *= ui;
		#endif
		}

		template <VALUE V>
		static inline void __dil_ushl(uintptr_t& stop)
		{
			uint8_t c = stack_type::dil_pop<val8<>>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

			v.ui <<= c;
		}

		template <VALUE V>
		static inline void __dil_ushr(uintptr_t& stop)
		{
			uint8_t c = stack_type::dil_pop<val8<>>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

			v.ui >>= c;
		}

		template <VALUE V>
		static inline void __dil_usub(state& state, uintptr_t& stop)
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = stack_type::dil_pop<V>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.iovf = false;
			state.eval.bits.iunf = v.ui < ui;
		#endif

			v.ui -= ui;
		}

		template <VALUE V>
		static inline void __dil_xor(uintptr_t& stop)
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = stack_type::dil_pop<V>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

			v.ui ^= ui;
		}

		template <VALUE V>
		inline V fetch()
		{
		#ifdef FETCHCHECK
			if (_opptr < _code_beg || _opptr + (ptrdiff_t)sizeof(V) > _code_end)
				throw std::runtime_error(_err_msg_fetch_code);
			else
			{
				V t = *((V*)_opptr);
				_opptr += (ptrdiff_t)sizeof(V);

				return t;
			}
		#else
			V t = *((V*)_opptr);
			_opptr += (ptrdiff_t)sizeof(V);

			return t;
		#endif
		}

		template <VALUE V>
		inline void __and()
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			v.ui &= ui;
		}

		template <VALUE V>
		inline void __call()
		{
			const ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			_stack.push_frame((uintptr_t)_opptr);
			_opptr += os;
		}

		template <VALUE V>
		inline void __fadd()
		{
			using vf_t = decltype(V::f);
			const vf_t f = _stack.pop<V>().f;
			V& v = _stack.top<V>();

			v.f += f;
		}

		template <VALUE V>
		inline void __fcmp()
		{
			using vf_t = decltype(V::f);
			const vf_t f_1 = _stack.pop<V>().f;
			const vf_t f_2 = _stack.pop<V>().f;

			_state.comp.bits.above = !(f_1 <= f_2);
			_state.comp.bits.below = !(f_1 >= f_2);
		}

		template <VALUE V>
		inline void __fdiv()
		{
			using vf_t = decltype(V::f);
			const vf_t f = _stack.pop<V>().f;
			V& v = _stack.top<V>();

			v.f /= f;
		}

		template <VALUE V>
		inline void __fevl()
		{
			using vf_t = decltype(V::f);
			const vf_t f = _stack.pop<V>().f;

			_state.eval.bits.fzer = false;
			_state.eval.bits.fsub = false;
			_state.eval.bits.finf = false;
			_state.eval.bits.fnan = false;
			_state.eval.bits.fnrm = false;

			switch (fpclassify(f))
			{
			case FP_ZERO:
				_state.eval.bits.fzer = true;
				break;

			case FP_SUBNORMAL:
				_state.eval.bits.fsub = true;
				break;

			case FP_INFINITE:
				_state.eval.bits.finf = true;
				break;

			case FP_NAN:
				_state.eval.bits.fnan = true;
				break;

			case FP_NORMAL:
				_state.eval.bits.fnrm = true;
				break;

			default:
			#ifdef FLOATCHECK
				throw std::runtime_error(_err_msg_wrong_fltp);
			#else
				__assume(false);
			#endif
				break;
			}
		}

		template <VALUE V>
		inline void __fmod()
		{
			using vf_t = decltype(V::f);
			const vf_t f = _stack.pop<V>().f;
			V& v = _stack.top<V>();

			v.f = fmod(v.f, f);
		}

		template <VALUE V>
		inline void __fmul()
		{
			using vf_t = decltype(V::f);
			const vf_t f = _stack.pop<V>().f;
			V& v = _stack.top<V>();

			v.f *= f;
		}

		template <VALUE V>
		inline void __fneg()
		{
			V& v = _stack.top<V>();

			v.f = -v.f;
		}

		template <VALUE V>
		inline void __fsub()
		{
			using vf_t = decltype(V::f);
			const vf_t f = _stack.pop<V>().f;
			V& v = _stack.top<V>();

			v.f -= f;
		}

		template <VALUE V>
		inline void __jmp()
		{
			const ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_a()
		{
			const ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (_state.comp.bits.above && !_state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_ae()
		{
			const ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (!_state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_b()
		{
			const ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (!_state.comp.bits.above && _state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_be()
		{
			const ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (!_state.comp.bits.above)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_e()
		{
			const ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (!_state.comp.bits.above && !_state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_ne()
		{
			const ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if ((bool)(_state.comp.bits.above ^ _state.comp.bits.below))
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_un()
		{
			const ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (_state.comp.bits.above && _state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_msk()
		{
			type _type = (type)fetch<val8<>>().ui;
			V _mask = fetch<V>();

			const ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (match<V>::check(_state, _mask, _type))
				_opptr += os;
		}

		template <VALUE V>
		inline void __l_load()
		{
			const ptrdiff_t os = (ptrdiff_t)fetch<val16<>>().si;

			_stack.load<V>(os);
		}

		template <VALUE V>
		inline void __l_store()
		{
			const ptrdiff_t os = (ptrdiff_t)fetch<val16<>>().si;

			_stack.store<V>(os);
		}

		template <VALUE V>
		inline void __neg()
		{
			using vsi_t = decltype(V::si);
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.ierr = v.si == numeric_limits<vsi_t>::min();
		#endif

			v.si = -v.si;
		}

		template <VALUE V>
		inline void __not()
		{
			V& v = _stack.top<V>();

			v.ui = ~v.ui;
		}

		template <VALUE V>
		inline void __or()
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			v.ui |= ui;
		}

		template <VALUE V>
		inline void __ptrcpy()
		{
			V* ptr_1 = (V*)_stack.pop_ptr();
			V* ptr_2 = (V*)_stack.pop_ptr();

			*ptr_2 = *ptr_1;
		}

		template <VALUE V>
		inline void __ptrget()
		{
			V* ptr = (V*)_stack.pop_ptr();
			V val = *ptr;

			_stack.push<V>(val);
		}

		template <VALUE V>
		inline void __ptrset()
		{
			V* ptr = (V*)_stack.pop_ptr();
			V val = _stack.pop<V>();

			*ptr = val;
		}

		template <VALUE V>
		inline void __push()
		{
			V val = fetch<V>();

			_stack.push<V>(val);
		}

		template <VALUE V>
		inline void __rotl()
		{
			uint8_t c = _stack.pop<val8<>>().ui;
			V& v = _stack.top<V>();

		#ifdef MSVCROTATE
			v.ui = msvcrotate<V>::rotl(v.ui, c);
		#else
			v.ui = (v.ui << c) | (v.ui >> ((uint8_t)sizeof(V) * 8ui8 - c));
		#endif
		}

		template <VALUE V>
		inline void __rotr()
		{
			uint8_t c = _stack.pop<val8<>>().ui;
			V& v = _stack.top<V>();

		#ifdef MSVCROTATE
			v.ui = msvcrotate<V>::rotr(v.ui, c);
		#else
			v.ui = (v.ui >> c) | (v.ui << ((uint8_t)sizeof(V) * 8ui8 - c));
		#endif
		}

		template <VALUE V>
		inline void __s_load()
		{
			const ptrdiff_t os = (ptrdiff_t)fetch<val8<>>().si;

			_stack.load<V>(os);
		}

		template <VALUE V>
		inline void __s_store()
		{
			const ptrdiff_t os = (ptrdiff_t)fetch<val8<>>().si;

			_stack.store<V>(os);
		}

		template <VALUE V>
		inline void __sadd()
		{
			using vsi_t = decltype(V::si);
			const vsi_t si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.iovf = si > (vsi_t)0 && v.si > numeric_limits<vsi_t>::max() - si;
			_state.eval.bits.iunf = si < (vsi_t)0 && v.si < numeric_limits<vsi_t>::min() - si;
		#endif

			v.si += si;
		}

		template <VALUE V>
		inline void __scmp()
		{
			using vsi_t = decltype(V::si);
			vsi_t si_1 = _stack.pop<V>().si;
			vsi_t si_2 = _stack.pop<V>().si;

			_state.comp.bits.above = si_1 > si_2;
			_state.comp.bits.below = si_1 < si_2;
		}

		template <VALUE V>
		inline void __sdec()
		{
			using vsi_t = decltype(V::si);
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.iovf = false;
			_state.eval.bits.iunf = v.si == numeric_limits<vsi_t>::min();
		#endif

			--v.si;
		}

		template <VALUE V>
		inline void __sdiv()
		{
			using vsi_t = decltype(V::si);
			const vsi_t si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

			bool zeromirror = si == (vsi_t)0 || (si == (vsi_t)-1 && v.si == numeric_limits<vsi_t>::min());
		#ifdef OPINTCHECK
			_state.eval.bits.ierr = zeromirror;
		#endif

			if (!zeromirror)
				v.si /= si;
		}

		template <VALUE V>
		inline void __sinc()
		{
			using vsi_t = decltype(V::si);
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.iovf = v.si == numeric_limits<vsi_t>::max();
			_state.eval.bits.iunf = false;
		#endif

			++v.si;
		}

		template <VALUE V>
		inline void __smod()
		{
			using vsi_t = decltype(V::si);
			const vsi_t si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

			bool zero = si == (vsi_t)0;
		#ifdef OPINTCHECK
			_state.eval.bits.ierr = zero;
		#endif

			if (!zero)
			{
				if (si == (vsi_t)1 || si == (vsi_t)-1)
					v.si = (vsi_t)0;
				else
					v.si %= si;
			}
		}

		template <VALUE V>
		inline void __smul()
		{
			using vsi_t = decltype(V::si);
			const vsi_t si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			__mul<V>::smul(v, si, _state);
		#else
			v.si *= si;
		#endif
		}

		template <VALUE V>
		inline void __sshl()
		{
			uint8_t c = _stack.pop<val8<>>().ui;
			V& v = _stack.top<V>();

			v.si <<= c;
		}

		template <VALUE V>
		inline void __sshr()
		{
			uint8_t c = _stack.pop<val8<>>().ui;
			V& v = _stack.top<V>();

			v.si >>= c;
		}

		template <VALUE V>
		inline void __ssub()
		{
			using vsi_t = decltype(V::si);
			const vsi_t si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.iovf = si < (vsi_t)0 && v.si > numeric_limits<vsi_t>::max() + si;
			_state.eval.bits.iunf = si > (vsi_t)0 && v.si < numeric_limits<vsi_t>::min() + si;
		#endif

			v.si -= si;
		}

		template <VALUE V>
		inline void __uadd()
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.iovf = v.ui > numeric_limits<vui_t>::max() - ui;
			_state.eval.bits.iunf = false;
		#endif

			v.ui += ui;
		}

		template <VALUE V>
		inline void __ucmp()
		{
			using vui_t = decltype(V::ui);
			vui_t ui_1 = _stack.pop<V>().ui;
			vui_t ui_2 = _stack.pop<V>().ui;

			_state.comp.bits.above = ui_1 > ui_2;
			_state.comp.bits.below = ui_1 < ui_2;
		}

		template <VALUE V>
		inline void __udec()
		{
			using vui_t = decltype(V::ui);
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.iovf = false;
			_state.eval.bits.iunf = v.ui == numeric_limits<vui_t>::min();
		#endif

			--v.ui;
		}

		template <VALUE V>
		inline void __udiv()
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			bool zero = ui == (vui_t)0;
		#ifdef OPINTCHECK
			_state.eval.bits.ierr = zero;
		#endif

			if (!zero)
				v.ui /= ui;
		}

		template <VALUE V>
		inline void __uinc()
		{
			using vui_t = decltype(V::ui);
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.iovf = v.ui == numeric_limits<vui_t>::max();
			_state.eval.bits.iunf = false;
		#endif

			++v.ui;
		}

		template <VALUE V>
		inline void __umod()
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			bool zero = ui == (vui_t)0;
		#ifdef OPINTCHECK
			_state.eval.bits.ierr = zero;
		#endif

			if (!zero)
				v.ui %= ui;
		}

		template <VALUE V>
		inline void __umul()
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			__mul<V>::umul(v, ui, _state);
		#else
			v.ui *= ui;
		#endif
		}

		template <VALUE V>
		inline void __ushl()
		{
			uint8_t c = _stack.pop<val8<>>().ui;
			V& v = _stack.top<V>();

			v.ui <<= c;
		}

		template <VALUE V>
		inline void __ushr()
		{
			uint8_t c = _stack.pop<val8<>>().ui;
			V& v = _stack.top<V>();

			v.ui >>= c;
		}

		template <VALUE V>
		inline void __usub()
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.iovf = false;
			_state.eval.bits.iunf = v.ui < ui;
		#endif

			v.ui -= ui;
		}

		template <VALUE V>
		inline void __xor()
		{
			using vui_t = decltype(V::ui);
			const vui_t ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			v.ui ^= ui;
		}

	public:
		static void rev_endian(uint8_t* code, uint64_t code_size);
		static void rev_endian(uint8_t* bcode, const uint8_t* ecode);

		dispatcher() = delete;
	#ifdef FETCHCHECK
		dispatcher(const uint8_t* code, uint64_t code_size, uint64_t cache_size, uint64_t stack_size);
		dispatcher(const uint8_t* bcode, const uint8_t* ecode, uint64_t cache_size, uint64_t stack_size);
	#else
		dispatcher(const uint8_t* code, uint64_t cache_size, uint64_t stack_size);
	#endif
		dispatcher(const dispatcher& o) = delete;
		dispatcher(dispatcher&& o) noexcept;
		~dispatcher() noexcept = default;

		void exit();
		void init();
		void loop();

		void push_ptr(uintptr_t value);
		uintptr_t pop_ptr();

		template <VALUE V>
		void push_val(V value)
		{
			_stack.push(value);
		}

		template <VALUE V>
		V pop_val()
		{
			return _stack.pop<V>();
		}

		dispatcher& operator=(const dispatcher& o) = delete;
		dispatcher& operator=(dispatcher&& o) noexcept;
	};
}