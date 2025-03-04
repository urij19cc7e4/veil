#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include "define.hpp"
#include "interpreter_type.hpp"
#include "op.hpp"
#include "stack_be.hpp"
#include "stack_le.hpp"

#ifdef MSVCROTATE
#include <intrin.h>
#endif

namespace interpreter
{
#ifdef MSVCROTATE
	template <VALUE V>
	struct msvcrotate;

	template <>
	struct msvcrotate<val64<>>
	{
		static inline uint64_t rotl(uint64_t value, uint8_t count)
		{
			return _rotl64(value, count);
		}

		static inline uint64_t rotr(uint64_t value, uint8_t count)
		{
			return _rotr64(value, count);
		}
	};

	template <>
	struct msvcrotate<val32<>>
	{
		static inline uint32_t rotl(uint32_t value, uint8_t count)
		{
			return _rotl(value, count);
		}

		static inline uint32_t rotr(uint32_t value, uint8_t count)
		{
			return _rotr(value, count);
		}
	};

	template <>
	struct msvcrotate<val16<>>
	{
		static inline uint16_t rotl(uint16_t value, uint8_t count)
		{
			return _rotl16(value, count);
		}

		static inline uint16_t rotr(uint16_t value, uint8_t count)
		{
			return _rotr16(value, count);
		}
	};

	template <>
	struct msvcrotate<val8<>>
	{
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
		static constexpr const char _err_msg_fetch_code[] = "Fetch out of code range";
		static constexpr const char _err_msg_wrong_fltp[] = "Wrong float class";
		static constexpr const char _err_msg_wrong_opnd[] = "Wrong operand";
		static constexpr const char _err_msg_wrong_type[] = "Wrong mask type";

	#ifdef FETCHCHECK
		const uint8_t* _code_beg;
		const uint8_t* _code_end;
	#endif

		stack<std::endian::native> _stack;
		const uint8_t* _opptr;
		state _state;

		template <typename T>
		inline T fetch()
		{
		#ifdef FETCHCHECK
			if (_opptr < _code_beg || _opptr + (ptrdiff_t)sizeof(T) > _code_end)
				throw std::runtime_error(_err_msg_fetch_code);
			else
			{
				T t = *((T*)_opptr);
				_opptr += (ptrdiff_t)sizeof(T);

				return t;
			}
		#else
			T t = *((T*)_opptr);
			_opptr += (ptrdiff_t)sizeof(T);

			return t;
		#endif
		}

		template <VALUE V>
		inline bool is_state_match_mask(V _mask, type _type)
		{
			switch (_type)
			{
			case type::v0:
			{
			#pragma loop(unroll)
				for (uint64_t i = 0ui64; i < _mask.v8_count; ++i)
					if ((_state.eval.byte.value | _mask.v8[i].ui) == _mask.v8[i].ui)
						return true;

				return false;
			}

			case type::v1:
			{
			#pragma loop(unroll)
				for (uint64_t i = 0ui64; i < _mask.v8_count; ++i)
					if ((_state.eval.byte.value & _mask.v8[i].ui) == _mask.v8[i].ui)
						return true;

				return false;
			}

			default:
			#ifdef MTYPECHECK
				throw std::runtime_error(_err_msg_wrong_type);
			#else
				__assume(false);
			#endif
				break;
			}
		}

		template <>
		inline bool is_state_match_mask<val8<>>(val8<> _mask, type _type)
		{
			switch (_type)
			{
			case type::v0:
				return (_state.eval.byte.value | _mask.ui) == _mask.ui;

			case type::v1:
				return (_state.eval.byte.value & _mask.ui) == _mask.ui;

			default:
			#ifdef MTYPECHECK
				throw std::runtime_error(_err_msg_wrong_type);
			#else
				__assume(false);
			#endif
				break;
			}
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
			type _type = fetch<type>();
			V _mask = fetch<V>();

			ptrdiff_t os = (ptrdiff_t)fetch<V>().si;

			if (is_state_match_mask(_mask, _type))
				_opptr += os;
		}

		template <VALUE V>
		inline void __l_load()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<int16_t>();

			_stack.load<V>(os);
		}

		template <VALUE V>
		inline void __l_store()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<int16_t>();

			_stack.store<V>(os);
		}

		template <VALUE V>
		inline void __neg()
		{
			V& v = _stack.top<V>();

			_state.eval.bits.ierr = v.si == numeric_limits<decltype(V::si)>::min();

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
			ptrdiff_t os = (ptrdiff_t)fetch<int8_t>();

			_stack.load<V>(os);
		}

		template <VALUE V>
		inline void __s_store()
		{
			ptrdiff_t os = (ptrdiff_t)fetch<int8_t>();

			_stack.store<V>(os);
		}

		template <VALUE V>
		inline void __sadd()
		{
			using vsi_t = decltype(V::si);
			vsi_t si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

			_state.eval.bits.iovf = si > (vsi_t)0 && v.si > numeric_limits<vsi_t>::max() - si;
			_state.eval.bits.iunf = si < (vsi_t)0 && v.si < numeric_limits<vsi_t>::min() - si;

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

			_state.eval.bits.iovf = false;
			_state.eval.bits.iunf = v.si == numeric_limits<decltype(V::si)>::min();

			--v.si;
		}

		template <VALUE V>
		inline void __sdiv()
		{
			using vsi_t = decltype(V::si);
			vsi_t si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

			bool zero = si == (vsi_t)0;
			_state.eval.bits.ierr = zero;

			if (!zero)
				v.si /= si;
		}

		template <VALUE V>
		inline void __sinc()
		{
			V& v = _stack.top<V>();

			_state.eval.bits.iovf = v.si == numeric_limits<decltype(V::si)>::max();
			_state.eval.bits.iunf = false;

			++v.si;
		}

		template <VALUE V>
		inline void __smod()
		{
			using vsi_t = decltype(V::si);
			vsi_t si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

			bool zero = si == (vsi_t)0;
			_state.eval.bits.ierr = zero;

			if (!zero)
				v.si %= si;
		}

		template <VALUE V>
		inline void __smul()
		{
			using vsi_t = decltype(V::si);
			vsi_t si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

			_state.eval.bits.iovf = false;
			_state.eval.bits.iunf = false;

			if ((v.si & si) != (vsi_t)0)
			{
				if ((v.si ^ si) >= (vsi_t)0)
					_state.eval.bits.iovf = v.si > numeric_limits<vsi_t>::max() / si;
				else
					_state.eval.bits.iunf = v.si < numeric_limits<vsi_t>::min() / si;
			}

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

			_state.eval.bits.iovf = si < (vsi_t)0 && v.si > numeric_limits<vsi_t>::max() + si;
			_state.eval.bits.iunf = si > (vsi_t)0 && v.si < numeric_limits<vsi_t>::min() + si;

			v.si -= si;
		}

		template <VALUE V>
		inline void __uadd()
		{
			using vui_t = decltype(V::ui);
			vui_t ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			_state.eval.bits.iovf = v.ui > numeric_limits<vui_t>::max() - ui;
			_state.eval.bits.iunf = false;

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

			_state.eval.bits.iovf = false;
			_state.eval.bits.iunf = v.ui == numeric_limits<decltype(V::ui)>::min();

			--v.ui;
		}

		template <VALUE V>
		inline void __udiv()
		{
			using vui_t = decltype(V::ui);
			vui_t ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			bool zero = ui == (vui_t)0;
			_state.eval.bits.ierr = zero;

			if (!zero)
				v.ui /= ui;
		}

		template <VALUE V>
		inline void __uinc()
		{
			V& v = _stack.top<V>();

			_state.eval.bits.iovf = v.ui == numeric_limits<decltype(V::ui)>::max();
			_state.eval.bits.iunf = false;

			++v.ui;
		}

		template <VALUE V>
		inline void __umod()
		{
			using vui_t = decltype(V::ui);
			vui_t ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			bool zero = ui == (vui_t)0;
			_state.eval.bits.ierr = zero;

			if (!zero)
				v.ui %= ui;
		}

		template <VALUE V>
		inline void __umul()
		{
			using vui_t = decltype(V::ui);
			vui_t ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			_state.eval.bits.iovf = (v.ui & ui) != (vui_t)0 && v.ui > numeric_limits<vui_t>::max() / ui;
			_state.eval.bits.iunf = false;

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

			_state.eval.bits.iovf = false;
			_state.eval.bits.iunf = v.ui < ui;

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

		dispatcher() = delete;
	#ifdef FETCHCHECK
		dispatcher(const uint8_t* bcode, const uint8_t* ecode, uint64_t size);
	#else
		dispatcher(const uint8_t* code, uint64_t size);
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