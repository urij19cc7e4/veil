#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

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
		static inline V dil_fetch(const uint8_t*&opptr)
		{
			V t = *((V*)opptr);
			opptr += (ptrdiff_t)sizeof(V);

			return t;
		}

		template <VALUE V>
		static inline void __dil_and(uintptr_t& stop)
		{
			decltype(V::ui) ui = stack_type::dil_pop<V>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

			v.ui &= ui;
		}

		template <VALUE V>
		static inline void __dil_call(const uint8_t*& opptr, uintptr_t& ftop, uintptr_t& stop)
		{
			ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			stack_type::dil_push_frame(ftop, stop, (uintptr_t)opptr);
			opptr += os;
		}

		template <VALUE V>
		static inline void __dil_fadd(uintptr_t& stop)
		{
			decltype(V::f) f = stack_type::dil_pop<V>(stop).f;
			V& v = stack_type::dil_top<V>(stop);

			v.f += f;
		}

		template <VALUE V>
		static inline void __dil_fcmp(state& state, uintptr_t& stop)
		{
			decltype(V::f) f_1 = stack_type::dil_pop<V>(stop).f;
			decltype(V::f) f_2 = stack_type::dil_pop<V>(stop).f;

			state.comp.bits.above = !(f_1 <= f_2);
			state.comp.bits.below = !(f_1 >= f_2);
		}

		template <VALUE V>
		static inline void __dil_fdiv(uintptr_t& stop)
		{
			decltype(V::f) f = stack_type::dil_pop<V>(stop).f;
			V& v = stack_type::dil_top<V>(stop);

			v.f /= f;
		}

		template <VALUE V>
		static inline void __dil_fevl(state& state, uintptr_t& stop)
		{
			decltype(V::f) f = stack_type::dil_pop<V>(stop).f;

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
			decltype(V::f) f = stack_type::dil_pop<V>(stop).f;
			V& v = stack_type::dil_top<V>(stop);

			v.f = fmod(v.f, f);
		}

		template <VALUE V>
		static inline void __dil_fmul(uintptr_t& stop)
		{
			decltype(V::f) f = stack_type::dil_pop<V>(stop).f;
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
			decltype(V::f) f = stack_type::dil_pop<V>(stop).f;
			V& v = stack_type::dil_top<V>(stop);

			v.f -= f;
		}

		template <VALUE V>
		static inline void __dil_jmp(const uint8_t*& opptr)
		{
			ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_a(const uint8_t*& opptr, const state& state)
		{
			ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if (state.comp.bits.above && !state.comp.bits.below)
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_ae(const uint8_t*& opptr, const state& state)
		{
			ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if (!state.comp.bits.below)
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_b(const uint8_t*& opptr, const state& state)
		{
			ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if (!state.comp.bits.above && state.comp.bits.below)
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_be(const uint8_t*& opptr, const state& state)
		{
			ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if (!state.comp.bits.above)
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_e(const uint8_t*& opptr, const state& state)
		{
			ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if (!state.comp.bits.above && !state.comp.bits.below)
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_ne(const uint8_t*& opptr, const state& state)
		{
			ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if ((bool)(state.comp.bits.above ^ state.comp.bits.below))
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_un(const uint8_t*& opptr, const state& state)
		{
			ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if (state.comp.bits.above && state.comp.bits.below)
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_jmp_msk(const uint8_t*& opptr, const state& state)
		{
			type _type = (type)dil_fetch<val8<>>(opptr).ui;
			V _mask = dil_fetch<V>(opptr);

			ptrdiff_t os = (ptrdiff_t)dil_fetch<V>(opptr).si;

			if (match<V>::check(state, _mask, _type))
				opptr += os;
		}

		template <VALUE V>
		static inline void __dil_l_load(const uint8_t*& opptr, const uintptr_t& ftop, uintptr_t& stop)
		{
			ptrdiff_t os = (ptrdiff_t)dil_fetch<val16<>>(opptr).si;

			stack_type::dil_load<V>(ftop, stop, os);
		}

		template <VALUE V>
		static inline void __dil_l_store(const uint8_t*& opptr, const uintptr_t& ftop, uintptr_t& stop)
		{
			ptrdiff_t os = (ptrdiff_t)dil_fetch<val16<>>(opptr).si;

			stack_type::dil_store<V>(ftop, stop, os);
		}

		template <VALUE V>
		static inline void __dil_neg(state& state, const uintptr_t& stop)
		{
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.ierr = v.si == numeric_limits<decltype(V::si)>::min();
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
			decltype(V::ui) ui = stack_type::dil_pop<V>(stop).ui;
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
			ptrdiff_t os = (ptrdiff_t)dil_fetch<val8<>>(opptr).si;

			stack_type::dil_load<V>(ftop, stop, os);
		}

		template <VALUE V>
		static inline void __dil_s_store(const uint8_t*& opptr, const uintptr_t& ftop, uintptr_t& stop)
		{
			ptrdiff_t os = (ptrdiff_t)dil_fetch<val8<>>(opptr).si;

			stack_type::dil_store<V>(ftop, stop, os);
		}

		template <VALUE V>
		static inline void __dil_sadd(state& state, uintptr_t& stop)
		{
			using vsi_t = decltype(V::si);
			vsi_t si = stack_type::dil_pop<V>(stop).si;
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
			decltype(V::si) si_1 = stack_type::dil_pop<V>(stop).si;
			decltype(V::si) si_2 = stack_type::dil_pop<V>(stop).si;

			state.comp.bits.above = si_1 > si_2;
			state.comp.bits.below = si_1 < si_2;
		}

		template <VALUE V>
		static inline void __dil_sdec(state& state, const uintptr_t& stop)
		{
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.iovf = false;
			state.eval.bits.iunf = v.si == numeric_limits<decltype(V::si)>::min();
		#endif

			--v.si;
		}

		template <VALUE V>
		static inline void __dil_sdiv(state& state, uintptr_t& stop)
		{
			using vsi_t = decltype(V::si);
			vsi_t si = stack_type::dil_pop<V>(stop).si;
			V& v = stack_type::dil_top<V>(stop);

			bool zero = si == (vsi_t)0;
		#ifdef OPINTCHECK
			state.eval.bits.ierr = zero;
		#endif

			if (!zero)
				v.si /= si;
		}

		template <VALUE V>
		static inline void __dil_sinc(state& state, const uintptr_t& stop)
		{
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.iovf = v.si == numeric_limits<decltype(V::si)>::max();
			state.eval.bits.iunf = false;
		#endif

			++v.si;
		}

		template <VALUE V>
		static inline void __dil_smod(state& state, uintptr_t& stop)
		{
			using vsi_t = decltype(V::si);
			vsi_t si = stack_type::dil_pop<V>(stop).si;
			V& v = stack_type::dil_top<V>(stop);

			bool zero = si == (vsi_t)0;
		#ifdef OPINTCHECK
			state.eval.bits.ierr = zero;
		#endif

			if (!zero)
				v.si %= si;
		}

		template <VALUE V>
		static inline void __dil_smul(state& state, uintptr_t& stop)
		{
			using vsi_t = decltype(V::si);
			vsi_t si = stack_type::dil_pop<V>(stop).si;
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.iovf = false;
			state.eval.bits.iunf = false;

			if ((v.si & si) != (vsi_t)0)
			{
				if ((v.si ^ si) >= (vsi_t)0)
					state.eval.bits.iovf = v.si > numeric_limits<vsi_t>::max() / si;
				else
					state.eval.bits.iunf = v.si < numeric_limits<vsi_t>::min() / si;
			}
		#endif

			v.si *= si;
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
			vsi_t si = stack_type::dil_pop<V>(stop).si;
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
			vui_t ui = stack_type::dil_pop<V>(stop).ui;
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
			decltype(V::ui) ui_1 = stack_type::dil_pop<V>(stop).ui;
			decltype(V::ui) ui_2 = stack_type::dil_pop<V>(stop).ui;

			state.comp.bits.above = ui_1 > ui_2;
			state.comp.bits.below = ui_1 < ui_2;
		}

		template <VALUE V>
		static inline void __dil_udec(state& state, const uintptr_t& stop)
		{
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.iovf = false;
			state.eval.bits.iunf = v.ui == numeric_limits<decltype(V::ui)>::min();
		#endif

			--v.ui;
		}

		template <VALUE V>
		static inline void __dil_udiv(state& state, uintptr_t& stop)
		{
			using vui_t = decltype(V::ui);
			vui_t ui = stack_type::dil_pop<V>(stop).ui;
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
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.iovf = v.ui == numeric_limits<decltype(V::ui)>::max();
			state.eval.bits.iunf = false;
		#endif

			++v.ui;
		}

		template <VALUE V>
		static inline void __dil_umod(state& state, uintptr_t& stop)
		{
			using vui_t = decltype(V::ui);
			vui_t ui = stack_type::dil_pop<V>(stop).ui;
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
			vui_t ui = stack_type::dil_pop<V>(stop).ui;
			V& v = stack_type::dil_top<V>(stop);

		#ifdef OPINTCHECK
			state.eval.bits.iovf = (v.ui & ui) != (vui_t)0 && v.ui > numeric_limits<vui_t>::max() / ui;
			state.eval.bits.iunf = false;
		#endif

			v.ui *= ui;
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
			decltype(V::ui) ui = stack_type::dil_pop<V>(stop).ui;
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
			decltype(V::ui) ui = stack_type::dil_pop<V>(stop).ui;
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
			decltype(V::ui) ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			v.ui &= ui;
		}

		template <VALUE V>
		inline void __call()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			_stack.push_frame((uintptr_t)_opptr);
			_opptr += os;
		}

		template <VALUE V>
		inline void __fadd()
		{
			decltype(V::f) f = _stack.pop<V>().f;
			V& v = _stack.top<V>();

			v.f += f;
		}

		template <VALUE V>
		inline void __fcmp()
		{
			decltype(V::f) f_1 = _stack.pop<V>().f;
			decltype(V::f) f_2 = _stack.pop<V>().f;

			_state.comp.bits.above = !(f_1 <= f_2);
			_state.comp.bits.below = !(f_1 >= f_2);
		}

		template <VALUE V>
		inline void __fdiv()
		{
			decltype(V::f) f = _stack.pop<V>().f;
			V& v = _stack.top<V>();

			v.f /= f;
		}

		template <VALUE V>
		inline void __fevl()
		{
			decltype(V::f) f = _stack.pop<V>().f;

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
			decltype(V::f) f = _stack.pop<V>().f;
			V& v = _stack.top<V>();

			v.f = fmod(v.f, f);
		}

		template <VALUE V>
		inline void __fmul()
		{
			decltype(V::f) f = _stack.pop<V>().f;
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
			decltype(V::f) f = _stack.pop<V>().f;
			V& v = _stack.top<V>();

			v.f -= f;
		}

		template <VALUE V>
		inline void __jmp()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_a()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (_state.comp.bits.above && !_state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_ae()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (!_state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_b()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (!_state.comp.bits.above && _state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_be()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (!_state.comp.bits.above)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_e()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (!_state.comp.bits.above && !_state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_ne()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if ((bool)(_state.comp.bits.above ^ _state.comp.bits.below))
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_un()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (_state.comp.bits.above && _state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_msk()
		{
			type _type = (type)fetch<val8<>>().ui;
			V _mask = fetch<V>();

			ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (match<V>::check(_state, _mask, _type))
				_opptr += os;
		}

		template <VALUE V>
		inline void __l_load()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<val16<>>().si;

			_stack.load<V>(os);
		}

		template <VALUE V>
		inline void __l_store()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<val16<>>().si;

			_stack.store<V>(os);
		}

		template <VALUE V>
		inline void __neg()
		{
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.ierr = v.si == numeric_limits<decltype(V::si)>::min();
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
			decltype(V::ui) ui = _stack.pop<V>().ui;
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
			ptrdiff_t os = (ptrdiff_t)fetch<val8<>>().si;

			_stack.load<V>(os);
		}

		template <VALUE V>
		inline void __s_store()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<val8<>>().si;

			_stack.store<V>(os);
		}

		template <VALUE V>
		inline void __sadd()
		{
			using vsi_t = decltype(V::si);
			vsi_t si = _stack.pop<V>().si;
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
			decltype(V::si) si_1 = _stack.pop<V>().si;
			decltype(V::si) si_2 = _stack.pop<V>().si;

			_state.comp.bits.above = si_1 > si_2;
			_state.comp.bits.below = si_1 < si_2;
		}

		template <VALUE V>
		inline void __sdec()
		{
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.iovf = false;
			_state.eval.bits.iunf = v.si == numeric_limits<decltype(V::si)>::min();
		#endif

			--v.si;
		}

		template <VALUE V>
		inline void __sdiv()
		{
			using vsi_t = decltype(V::si);
			vsi_t si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

			bool zero = si == (vsi_t)0;
		#ifdef OPINTCHECK
			_state.eval.bits.ierr = zero;
		#endif

			if (!zero)
				v.si /= si;
		}

		template <VALUE V>
		inline void __sinc()
		{
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.iovf = v.si == numeric_limits<decltype(V::si)>::max();
			_state.eval.bits.iunf = false;
		#endif

			++v.si;
		}

		template <VALUE V>
		inline void __smod()
		{
			using vsi_t = decltype(V::si);
			vsi_t si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

			bool zero = si == (vsi_t)0;
		#ifdef OPINTCHECK
			_state.eval.bits.ierr = zero;
		#endif

			if (!zero)
				v.si %= si;
		}

		template <VALUE V>
		inline void __smul()
		{
			using vsi_t = decltype(V::si);
			vsi_t si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.iovf = false;
			_state.eval.bits.iunf = false;

			if ((v.si & si) != (vsi_t)0)
			{
				if ((v.si ^ si) >= (vsi_t)0)
					_state.eval.bits.iovf = v.si > numeric_limits<vsi_t>::max() / si;
				else
					_state.eval.bits.iunf = v.si < numeric_limits<vsi_t>::min() / si;
			}
		#endif

			v.si *= si;
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
			vsi_t si = _stack.pop<V>().si;
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
			vui_t ui = _stack.pop<V>().ui;
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
			decltype(V::ui) ui_1 = _stack.pop<V>().ui;
			decltype(V::ui) ui_2 = _stack.pop<V>().ui;

			_state.comp.bits.above = ui_1 > ui_2;
			_state.comp.bits.below = ui_1 < ui_2;
		}

		template <VALUE V>
		inline void __udec()
		{
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.iovf = false;
			_state.eval.bits.iunf = v.ui == numeric_limits<decltype(V::ui)>::min();
		#endif

			--v.ui;
		}

		template <VALUE V>
		inline void __udiv()
		{
			using vui_t = decltype(V::ui);
			vui_t ui = _stack.pop<V>().ui;
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
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.iovf = v.ui == numeric_limits<decltype(V::ui)>::max();
			_state.eval.bits.iunf = false;
		#endif

			++v.ui;
		}

		template <VALUE V>
		inline void __umod()
		{
			using vui_t = decltype(V::ui);
			vui_t ui = _stack.pop<V>().ui;
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
			vui_t ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

		#ifdef OPINTCHECK
			_state.eval.bits.iovf = (v.ui & ui) != (vui_t)0 && v.ui > numeric_limits<vui_t>::max() / ui;
			_state.eval.bits.iunf = false;
		#endif

			v.ui *= ui;
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
			decltype(V::ui) ui = _stack.pop<V>().ui;
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
			decltype(V::ui) ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			v.ui ^= ui;
		}

	public:
		static void rev_endian(uint8_t* code, uint64_t size);
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
		~dispatcher() noexcept=default;

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