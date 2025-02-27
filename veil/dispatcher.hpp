#pragma once

#include <cstddef>
#include <cstdint>

#include "op.hpp"
#include "stack_be.hpp"
#include "stack_le.hpp"

#define MSVCROTATE

#ifdef MSVCROTATE
#include <intrin.h>
#endif

namespace interpreter
{
#ifdef MSVCROTATE
	template <VALUE V>
	struct msvcrotate;

	template <>
	struct msvcrotate<val64>
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
	struct msvcrotate<val32>
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
	struct msvcrotate<val16>
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
	struct msvcrotate<val8>
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
		stack<std::endian::native> _stack;
		const uint8_t* _opptr;
		state _state;

		template <VALUE V>
		inline bool __mask_cmp(V _mask, type _type)
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
				throw;
			}
		}

		template <>
		inline bool __mask_cmp<val8>(val8 _mask, type _type)
		{
			switch (_type)
			{
			case type::v0:
				return (_state.eval.byte.value | _mask.ui) == _mask.ui;

			case type::v1:
				return (_state.eval.byte.value & _mask.ui) == _mask.ui;

			default:
				throw;
			}
		}

		template <VALUE V>
		inline void __and()
		{
			auto ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			v.ui &= ui;
		}

		template <VALUE V>
		inline void __call()
		{
			using vsi_t = decltype(V::si);
			ptrdiff_t os = (ptrdiff_t) * ((vsi_t*)_opptr);
			_opptr += (ptrdiff_t)sizeof(vsi_t);

			_stack.push_frame(_opptr);
			_opptr += os;
		}

		template <VALUE V>
		inline void __fadd()
		{
			auto f = _stack.pop<V>().f;
			V& v = _stack.top<V>();

			v.f += f;
		}

		template <VALUE V>
		inline void __fcmp()
		{
			auto f_1 = _stack.pop<V>().f;
			auto f_2 = _stack.pop<V>().f;

			_state.comp.bits.above = !(f_1 <= f_2);
			_state.comp.bits.below = !(f_1 >= f_2);
		}

		template <VALUE V>
		inline void __fdiv()
		{
			auto f = _stack.pop<V>().f;
			V& v = _stack.top<V>();

			v.f /= f;
		}

		template <VALUE V>
		inline void __fevl()
		{
			auto f = _stack.pop<V>().f;

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
				throw;
				break;
			}
		}

		template <VALUE V>
		inline void __fmod()
		{
			auto f = _stack.pop<V>().f;
			V& v = _stack.top<V>();

			v.f = fmod(v.f, f);
		}

		template <VALUE V>
		inline void __fmul()
		{
			auto f = _stack.pop<V>().f;
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
			auto f = _stack.pop<V>().f;
			V& v = _stack.top<V>();

			v.f -= f;
		}

		template <VALUE V>
		inline void __jmp()
		{
			using vsi_t = decltype(V::si);
			ptrdiff_t os = (ptrdiff_t) * ((vsi_t*)_opptr);
			_opptr += (ptrdiff_t)sizeof(vsi_t);

			_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_a()
		{
			using vsi_t = decltype(V::si);
			ptrdiff_t os = (ptrdiff_t) * ((vsi_t*)_opptr);
			_opptr += (ptrdiff_t)sizeof(vsi_t);

			if (_state.comp.bits.above && !_state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_ae()
		{
			using vsi_t = decltype(V::si);
			ptrdiff_t os = (ptrdiff_t) * ((vsi_t*)_opptr);
			_opptr += (ptrdiff_t)sizeof(vsi_t);

			if (!_state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_b()
		{
			using vsi_t = decltype(V::si);
			ptrdiff_t os = (ptrdiff_t) * ((vsi_t*)_opptr);
			_opptr += (ptrdiff_t)sizeof(vsi_t);

			if (!_state.comp.bits.above && _state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_be()
		{
			using vsi_t = decltype(V::si);
			ptrdiff_t os = (ptrdiff_t) * ((vsi_t*)_opptr);
			_opptr += (ptrdiff_t)sizeof(vsi_t);

			if (!_state.comp.bits.above)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_e()
		{
			using vsi_t = decltype(V::si);
			ptrdiff_t os = (ptrdiff_t) * ((vsi_t*)_opptr);
			_opptr += (ptrdiff_t)sizeof(vsi_t);

			if (!_state.comp.bits.above && !_state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_ne()
		{
			using vsi_t = decltype(V::si);
			ptrdiff_t os = (ptrdiff_t) * ((vsi_t*)_opptr);
			_opptr += (ptrdiff_t)sizeof(vsi_t);

			if ((bool)(_state.comp.bits.above ^ _state.comp.bits.below))
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_un()
		{
			using vsi_t = decltype(V::si);
			ptrdiff_t os = (ptrdiff_t) * ((vsi_t*)_opptr);
			_opptr += (ptrdiff_t)sizeof(vsi_t);

			if (_state.comp.bits.above && _state.comp.bits.below)
				_opptr += os;
		}

		template <VALUE V>
		inline void __jmp_msk()
		{
			using vsi_t = decltype(V::si);

			type _type = *((type*)_opptr);
			_opptr += (ptrdiff_t)sizeof(type);

			V _mask = *((V*)_opptr);
			_opptr += (ptrdiff_t)sizeof(V);

			ptrdiff_t os = (ptrdiff_t) * ((vsi_t*)_opptr);
			_opptr += (ptrdiff_t)sizeof(vsi_t);

			if (__mask_cmp(_mask, _type))
				_opptr += os;
		}

		template <VALUE V>
		inline void __l_load()
		{
			ptrdiff_t os = (ptrdiff_t) * ((int16_t*)_opptr);
			_opptr += (ptrdiff_t)sizeof(int16_t);

			_stack.load<V>(os);
		}

		template <VALUE V>
		inline void __l_store()
		{
			ptrdiff_t os = (ptrdiff_t) * ((int16_t*)_opptr);
			_opptr += (ptrdiff_t)sizeof(int16_t);

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
			auto ui = _stack.pop<V>().ui;
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
			V val = *((V*)_opptr);
			_opptr += (ptrdiff_t)sizeof(V);

			_stack.push<V>(val);
		}

		template <VALUE V>
		inline void __rotl()
		{
			uint8_t c = _stack.pop<val8>().ui;
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
			uint8_t c = _stack.pop<val8>().ui;
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
			ptrdiff_t os = (ptrdiff_t) * ((int8_t*)_opptr);
			_opptr += (ptrdiff_t)sizeof(int8_t);

			_stack.load<V>(os);
		}

		template <VALUE V>
		inline void __s_store()
		{
			ptrdiff_t os = (ptrdiff_t) * ((int8_t*)_opptr);
			_opptr += (ptrdiff_t)sizeof(int8_t);

			_stack.store<V>(os);
		}

		template <VALUE V>
		inline void __sadd()
		{
			using vsi_t = decltype(V::si);
			auto si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

			_state.eval.bits.iovf = si > (vsi_t)0 && v.si > numeric_limits<vsi_t>::max() - si;
			_state.eval.bits.iunf = si < (vsi_t)0 && v.si < numeric_limits<vsi_t>::min() - si;

			v.si += si;
		}

		template <VALUE V>
		inline void __scmp()
		{
			auto si_1 = _stack.pop<V>().si;
			auto si_2 = _stack.pop<V>().si;

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
			auto si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

			bool zero = si == (decltype(V::si))0;
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
			auto si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

			bool zero = si == (decltype(V::si))0;
			_state.eval.bits.ierr = zero;

			if (!zero)
				v.si %= si;
		}

		template <VALUE V>
		inline void __smul()
		{
			using vsi_t = decltype(V::si);
			auto si = _stack.pop<V>().si;
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
			uint8_t c = _stack.pop<val8>().ui;
			V& v = _stack.top<V>();

			v.si <<= c;
		}

		template <VALUE V>
		inline void __sshr()
		{
			uint8_t c = _stack.pop<val8>().ui;
			V& v = _stack.top<V>();

			v.si >>= c;
		}

		template <VALUE V>
		inline void __ssub()
		{
			using vsi_t = decltype(V::si);
			auto si = _stack.pop<V>().si;
			V& v = _stack.top<V>();

			_state.eval.bits.iovf = si < (vsi_t)0 && v.si > numeric_limits<vsi_t>::max() + si;
			_state.eval.bits.iunf = si > (vsi_t)0 && v.si < numeric_limits<vsi_t>::min() + si;

			v.si -= si;
		}

		template <VALUE V>
		inline void __uadd()
		{
			auto ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			_state.eval.bits.iovf = v.ui > numeric_limits<decltype(V::ui)>::max() - ui;
			_state.eval.bits.iunf = false;

			v.ui += ui;
		}

		template <VALUE V>
		inline void __ucmp()
		{
			auto ui_1 = _stack.pop<V>().ui;
			auto ui_2 = _stack.pop<V>().ui;

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
			auto ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			bool zero = ui == (decltype(V::ui))0;
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
			auto ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			bool zero = ui == (decltype(V::ui))0;
			_state.eval.bits.ierr = zero;

			if (!zero)
				v.ui %= ui;
		}

		template <VALUE V>
		inline void __umul()
		{
			using vui_t = decltype(V::ui);
			auto ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			_state.eval.bits.iovf = (v.si & si) != (vui_t)0 && v.si > numeric_limits<vui_t>::max() / si;
			_state.eval.bits.iunf = false;

			v.ui *= ui;
		}

		template <VALUE V>
		inline void __ushl()
		{
			uint8_t c = _stack.pop<val8>().ui;
			V& v = _stack.top<V>();

			v.ui <<= c;
		}

		template <VALUE V>
		inline void __ushr()
		{
			uint8_t c = _stack.pop<val8>().ui;
			V& v = _stack.top<V>();

			v.ui >>= c;
		}

		template <VALUE V>
		inline void __usub()
		{
			auto ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			_state.eval.bits.iovf = false;
			_state.eval.bits.iunf = v.ui < ui;

			v.ui -= ui;
		}

		template <VALUE V>
		inline void __xor()
		{
			auto ui = _stack.pop<V>().ui;
			V& v = _stack.top<V>();

			v.ui ^= ui;
		}

	public:
		dispatcher() = delete;
		dispatcher(const uint8_t* code, uint64_t size = (1ui64 << 20ui64));
		dispatcher(const dispatcher& o) = delete;
		dispatcher(dispatcher&& o);
		~dispatcher();

		void loop();
	};
}