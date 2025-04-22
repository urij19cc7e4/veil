#include "dispatcher.hpp"

#include <cmath>

using namespace interpreter;
using namespace std;

#define __2_POW_64 18446744073709551616.0
#define __2_POW_63 9223372036854775808.0
#define __2_POW_32 4294967296.0
#define __2_POW_31 2147483648.0
#define __0 0.0

namespace interpreter
{
	template <VALUE V>
	struct byteswap;

	template <endian endianness>
	struct byteswap<val64<endianness>>
	{
	public:
		static inline uint64_t bs(uint64_t value)
		{
		#ifdef MSVCBTSWAP
			return _byteswap_uint64(value);
		#else
			return (((value >> 0x38ui8) & 0xFFui64) << 0x00ui8)
				| (((value >> 0x30ui8) & 0xFFui64) << 0x08ui8)
				| (((value >> 0x28ui8) & 0xFFui64) << 0x10ui8)
				| (((value >> 0x20ui8) & 0xFFui64) << 0x18ui8)
				| (((value >> 0x18ui8) & 0xFFui64) << 0x20ui8)
				| (((value >> 0x10ui8) & 0xFFui64) << 0x28ui8)
				| (((value >> 0x08ui8) & 0xFFui64) << 0x30ui8)
				| (((value >> 0x00ui8) & 0xFFui64) << 0x38ui8);
		#endif
		}
	};

	template <endian endianness>
	struct byteswap<val32<endianness>>
	{
	public:
		static inline uint32_t bs(uint32_t value)
		{
		#ifdef MSVCBTSWAP
			return _byteswap_ulong(value);
		#else
			return (((value >> 0x18ui8) & 0xFFui32) << 0x00ui8)
				| (((value >> 0x10ui8) & 0xFFui32) << 0x08ui8)
				| (((value >> 0x08ui8) & 0xFFui32) << 0x10ui8)
				| (((value >> 0x00ui8) & 0xFFui32) << 0x18ui8);
		#endif
		}
	};

	template <endian endianness>
	struct byteswap<val16<endianness>>
	{
	public:
		static inline uint16_t bs(uint16_t value)
		{
		#ifdef MSVCBTSWAP
			return _byteswap_ushort(value);
		#else
			return (((value >> 0x08ui8) & 0xFFui16) << 0x00ui8)
				| (((value >> 0x00ui8) & 0xFFui16) << 0x08ui8);
		#endif
		}
	};

	template <VALUE V>
	struct fbyteswap
	{
	public:
		static inline bool fbs(uint8_t*& code, const uint8_t* ecode)
		{
			if (code + (ptrdiff_t)sizeof(V) > ecode)
				return true;
			else
			{
				((V*)code)->ui = byteswap<V>::bs(((V*)code)->ui);
				code += (ptrdiff_t)sizeof(V);

				return false;
			}
		}
	};

	template <endian endianness>
	struct fbyteswap<val8<endianness>>
	{
	public:
		static inline bool fbs(uint8_t*& code, const uint8_t* ecode)
		{
			if (code + (ptrdiff_t)sizeof(val8<endianness>) > ecode)
				return true;
			else
			{
				code += (ptrdiff_t)sizeof(val8<endianness>);

				return false;
			}
		}
	};
}

void dispatcher::rev_endian(uint8_t* code, uint64_t size)
{
	rev_endian(code, (const uint8_t*)(code + (ptrdiff_t)size));
}

void dispatcher::rev_endian(uint8_t* bcode, const uint8_t* ecode)
{
	while (true)
	{
		op* opptr = (op*)bcode;

		if (fbyteswap<val8<>>::fbs(bcode, ecode))
			return;

		switch (*opptr)
		{
		case op::nativecall:
		{
			throw;
			break;
		}

		case op::call_64:
		case op::jmp_64:
		case op::jmp_a_64:
		case op::jmp_ae_64:
		case op::jmp_b_64:
		case op::jmp_be_64:
		case op::jmp_e_64:
		case op::jmp_ne_64:
		case op::jmp_un_64:
		case op::push_64:
		{
			if (fbyteswap<val64<>>::fbs(bcode, ecode))
				return;

			break;
		}

		case op::call_32:
		case op::jmp_32:
		case op::jmp_a_32:
		case op::jmp_ae_32:
		case op::jmp_b_32:
		case op::jmp_be_32:
		case op::jmp_e_32:
		case op::jmp_ne_32:
		case op::jmp_un_32:
		case op::push_32:
		{
			if (fbyteswap<val32<>>::fbs(bcode, ecode))
				return;

			break;
		}

		case op::call_16:
		case op::jmp_16:
		case op::jmp_a_16:
		case op::jmp_ae_16:
		case op::jmp_b_16:
		case op::jmp_be_16:
		case op::jmp_e_16:
		case op::jmp_ne_16:
		case op::jmp_un_16:
		case op::push_16:
		case op::l_alloc:
		case op::l_allocz:
		case op::l_dealloc:
		case op::l_load_64:
		case op::l_load_32:
		case op::l_load_16:
		case op::l_load_8:
		case op::l_store_64:
		case op::l_store_32:
		case op::l_store_16:
		case op::l_store_8:
		{
			if (fbyteswap<val16<>>::fbs(bcode, ecode))
				return;

			break;
		}

		case op::call_8:
		case op::jmp_8:
		case op::jmp_a_8:
		case op::jmp_ae_8:
		case op::jmp_b_8:
		case op::jmp_be_8:
		case op::jmp_e_8:
		case op::jmp_ne_8:
		case op::jmp_un_8:
		case op::push_8:
		case op::s_alloc:
		case op::s_allocz:
		case op::s_dealloc:
		case op::s_load_64:
		case op::s_load_32:
		case op::s_load_16:
		case op::s_load_8:
		case op::s_store_64:
		case op::s_store_32:
		case op::s_store_16:
		case op::s_store_8:
		{
			if (fbyteswap<val8<>>::fbs(bcode, ecode))
				return;

			break;
		}

		case op::jmp_msk_64:
		{
			if (fbyteswap<val8<>>::fbs(bcode, ecode))
				return;

			if (fbyteswap<val64<>>::fbs(bcode, ecode))
				return;

			if (fbyteswap<val64<>>::fbs(bcode, ecode))
				return;

			break;
		}

		case op::jmp_msk_32:
		{
			if (fbyteswap<val8<>>::fbs(bcode, ecode))
				return;

			if (fbyteswap<val32<>>::fbs(bcode, ecode))
				return;

			if (fbyteswap<val32<>>::fbs(bcode, ecode))
				return;

			break;
		}

		case op::jmp_msk_16:
		{
			if (fbyteswap<val8<>>::fbs(bcode, ecode))
				return;

			if (fbyteswap<val16<>>::fbs(bcode, ecode))
				return;

			if (fbyteswap<val16<>>::fbs(bcode, ecode))
				return;

			break;
		}

		case op::jmp_msk_8:
		{
			if (fbyteswap<val8<>>::fbs(bcode, ecode))
				return;

			if (fbyteswap<val8<>>::fbs(bcode, ecode))
				return;

			if (fbyteswap<val8<>>::fbs(bcode, ecode))
				return;

			break;
		}

		default:
			break;
		}
	}
}

#ifdef FETCHCHECK
dispatcher::dispatcher(const uint8_t* code, uint64_t code_size, uint64_t cache_size, uint64_t stack_size)
	: dispatcher(code, code + (ptrdiff_t)code_size, cache_size, stack_size) {}

dispatcher::dispatcher(const uint8_t* bcode, const uint8_t* ecode, uint64_t cache_size, uint64_t stack_size)
	: _code_beg(bcode), _code_end(ecode), _nativehub(cache_size), _stack(stack_size), _opptr(bcode), _state() {}

dispatcher::dispatcher(dispatcher&& o) noexcept : _code_beg(move(o._code_beg)), _code_end(move(o._code_end)),
	_nativehub(move(o._nativehub)), _stack(move(o._stack)), _opptr(move(o._opptr)), _state(move(o._state))
{
	o._code_beg = (const uint8_t*)nullptr;
	o._code_end = (const uint8_t*)nullptr;
	o._opptr = (const uint8_t*)nullptr;
	o._state = move(state());
}
#else
dispatcher::dispatcher(const uint8_t* code, uint64_t cache_size, uint64_t stack_size)
	: _nativehub(cache_size), _stack(stack_size), _opptr(code), _state() {}

dispatcher::dispatcher(dispatcher&& o) noexcept
	: _nativehub(move(o._nativehub)), _stack(move(o._stack)), _opptr(move(o._opptr)), _state(move(o._state))
{
	o._opptr = (const uint8_t*)nullptr;
	o._state = move(state());
}
#endif

void dispatcher::exit()
{
	_stack.pop_frame();
}

void dispatcher::init()
{
	_stack.push_frame((uintptr_t)nullptr);
}

void dispatcher::loop()
{
#ifdef DEEPINLINE
	uint8_t* data = _stack.get_data();
	uint64_t size = _stack.get_size();
	uintptr_t sbeg = _stack.get_sbeg();
	uintptr_t send = _stack.get_send();
	uintptr_t ftop = _stack.get_ftop();
	uintptr_t stop = _stack.get_stop();

	const uint8_t* opptr = _opptr;
	state state = _state;
#endif

	while (true)
	{
	#ifdef DEEPINLINE
		op op_code = (op)dil_fetch<val8<>>(opptr).ui;
	#else
		op op_code = (op)fetch<val8<>>().ui;
	#endif

		switch (op_code)
		{
		case op::nop:
			break;

		case op::exit:
			return;

		case op::nativecall:
		{
		#ifdef DEEPINLINE
			_nativehub.call(opptr, stop);
		#else
		#ifdef FETCHCHECK
			_nativehub.call(_opptr, _stack, _code_beg, _code_end);
		#else
			_nativehub.call(_opptr, _stack);
		#endif
		#endif
			break;
		}

		case op::ret:
		{
		#ifdef DEEPINLINE
			opptr = (const uint8_t*)stack_type::dil_pop_frame(ftop, stop);
		#else
			_opptr = (const uint8_t*)_stack.pop_frame();
		#endif
			break;
		}

		case op::call_64:
		{
		#ifdef DEEPINLINE
			__dil_call<val64<>>(opptr, ftop, stop);
		#else
			__call<val64<>>();
		#endif
			break;
		}

		case op::call_32:
		{
		#ifdef DEEPINLINE
			__dil_call<val32<>>(opptr, ftop, stop);
		#else
			__call<val32<>>();
		#endif
			break;
		}

		case op::call_16:
		{
		#ifdef DEEPINLINE
			__dil_call<val16<>>(opptr, ftop, stop);
		#else
			__call<val16<>>();
		#endif
			break;
		}

		case op::call_8:
		{
		#ifdef DEEPINLINE
			__dil_call<val8<>>(opptr, ftop, stop);
		#else
			__call<val8<>>();
		#endif
			break;
		}

		case op::memcpy:
		{
		#ifdef DEEPINLINE
			uint8_t* dst = (uint8_t*)stack_type::dil_pop_ptr(stop);
			uint8_t* src = (uint8_t*)stack_type::dil_pop_ptr(stop);
			uint64_t c = stack_type::dil_pop<val64<>>(stop).ui;

			memcpy(dst, src, c);
		#else
			uint8_t* dst = (uint8_t*)_stack.pop_ptr();
			uint8_t* src = (uint8_t*)_stack.pop_ptr();
			uint64_t c = _stack.pop<val64<>>().ui;

			memcpy(dst, src, c);
		#endif
			break;
		}

		case op::memset:
		{
		#ifdef DEEPINLINE
			uint8_t* dst = (uint8_t*)stack_type::dil_pop_ptr(stop);
			uint8_t val = stack_type::dil_pop<val8<>>(stop).ui;
			uint64_t c = stack_type::dil_pop<val64<>>(stop).ui;

			memset(dst, val, c);
		#else
			uint8_t* dst = (uint8_t*)_stack.pop_ptr();
			uint8_t val = _stack.pop<val8<>>().ui;
			uint64_t c = _stack.pop<val64<>>().ui;

			memset(dst, val, c);
		#endif
			break;
		}

		case op::l_alloc:
		{
		#ifdef DEEPINLINE
			stack_type::dil_alloc(stop, (uint64_t)dil_fetch<val16<>>(opptr).ui);
		#else
			_stack.alloc((uint64_t)fetch<val16<>>().ui);
		#endif
			break;
		}

		case op::s_alloc:
		{
		#ifdef DEEPINLINE
			stack_type::dil_alloc(stop, (uint64_t)dil_fetch<val8<>>(opptr).ui);
		#else
			_stack.alloc((uint64_t)fetch<val8<>>().ui);
		#endif
			break;
		}

		case op::l_allocz:
		{
		#ifdef DEEPINLINE
			stack_type::dil_allocz(stop, (uint64_t)dil_fetch<val16<>>(opptr).ui);
		#else
			_stack.allocz((uint64_t)fetch<val16<>>().ui);
		#endif
			break;
		}

		case op::s_allocz:
		{
		#ifdef DEEPINLINE
			stack_type::dil_allocz(stop, (uint64_t)dil_fetch<val8<>>(opptr).ui);
		#else
			_stack.allocz((uint64_t)fetch<val8<>>().ui);
		#endif
			break;
		}

		case op::l_dealloc:
		{
		#ifdef DEEPINLINE
			stack_type::dil_dealloc(stop, (uint64_t)dil_fetch<val16<>>(opptr).ui);
		#else
			_stack.dealloc((uint64_t)fetch<val16<>>().ui);
		#endif
			break;
		}

		case op::s_dealloc:
		{
		#ifdef DEEPINLINE
			stack_type::dil_dealloc(stop, (uint64_t)dil_fetch<val8<>>(opptr).ui);
		#else
			_stack.dealloc((uint64_t)fetch<val8<>>().ui);
		#endif
			break;
		}

		case op::push_flow:
		{
		#ifdef DEEPINLINE
			stack_type::dil_push_ptr(stop, (uintptr_t)opptr);
		#else
			_stack.push_ptr((uintptr_t)_opptr);
		#endif
			break;
		}

		case op::push_frame:
		{
		#ifdef DEEPINLINE
			stack_type::dil_store_frame(ftop, stop);
		#else
			_stack.store_frame();
		#endif
			break;
		}

		case op::push_stack:
		{
		#ifdef DEEPINLINE
			stack_type::dil_store_stack(stop);
		#else
			_stack.store_stack();
		#endif
			break;
		}

		case op::push_state:
		{
		#ifdef DEEPINLINE
			stack_type::dil_push(stop, (state_val<>(state)).__value);
		#else
			_stack.push((state_val<>(_state)).__value);
		#endif
			break;
		}

		case op::pop_flow:
		{
		#ifdef DEEPINLINE
			opptr = (const uint8_t*)stack_type::dil_pop_ptr(stop);
		#else
			_opptr = (const uint8_t*)_stack.pop_ptr();
		#endif
			break;
		}

		case op::pop_frame:
		{
		#ifdef DEEPINLINE
			stack_type::dil_load_frame(ftop, stop);
		#else
			_stack.load_frame();
		#endif
			break;
		}

		case op::pop_stack:
		{
		#ifdef DEEPINLINE
			stack_type::dil_load_stack(stop);
		#else
			_stack.load_stack();
		#endif
			break;
		}

		case op::pop_state:
		{
		#ifdef DEEPINLINE
			state = (state_val<>(stack_type::dil_pop<val16<>>(stop))).__state;
		#else
			_state = (state_val<>(_stack.pop<val16<>>())).__state;
		#endif
			break;
		}

		case op::jmp_64:
		{
		#ifdef DEEPINLINE
			__dil_jmp<val64<>>(opptr);
		#else
			__jmp<val64<>>();
		#endif
			break;
		}

		case op::jmp_32:
		{
		#ifdef DEEPINLINE
			__dil_jmp<val32<>>(opptr);
		#else
			__jmp<val32<>>();
		#endif
			break;
		}

		case op::jmp_16:
		{
		#ifdef DEEPINLINE
			__dil_jmp<val16<>>(opptr);
		#else
			__jmp<val16<>>();
		#endif
			break;
		}

		case op::jmp_8:
		{
		#ifdef DEEPINLINE
			__dil_jmp<val8<>>(opptr);
		#else
			__jmp<val8<>>();
		#endif
			break;
		}

		case op::jmp_a_64:
		{
		#ifdef DEEPINLINE
			__dil_jmp_a<val64<>>(opptr, state);
		#else
			__jmp_a<val64<>>();
		#endif
			break;
		}

		case op::jmp_a_32:
		{
		#ifdef DEEPINLINE
			__dil_jmp_a<val32<>>(opptr, state);
		#else
			__jmp_a<val32<>>();
		#endif
			break;
		}

		case op::jmp_a_16:
		{
		#ifdef DEEPINLINE
			__dil_jmp_a<val16<>>(opptr, state);
		#else
			__jmp_a<val16<>>();
		#endif
			break;
		}

		case op::jmp_a_8:
		{
		#ifdef DEEPINLINE
			__dil_jmp_a<val8<>>(opptr, state);
		#else
			__jmp_a<val8<>>();
		#endif
			break;
		}

		case op::jmp_ae_64:
		{
		#ifdef DEEPINLINE
			__dil_jmp_ae<val64<>>(opptr, state);
		#else
			__jmp_ae<val64<>>();
		#endif
			break;
		}

		case op::jmp_ae_32:
		{
		#ifdef DEEPINLINE
			__dil_jmp_ae<val32<>>(opptr, state);
		#else
			__jmp_ae<val32<>>();
		#endif
			break;
		}

		case op::jmp_ae_16:
		{
		#ifdef DEEPINLINE
			__dil_jmp_ae<val16<>>(opptr, state);
		#else
			__jmp_ae<val16<>>();
		#endif
			break;
		}

		case op::jmp_ae_8:
		{
		#ifdef DEEPINLINE
			__dil_jmp_ae<val8<>>(opptr, state);
		#else
			__jmp_ae<val8<>>();
		#endif
			break;
		}

		case op::jmp_b_64:
		{
		#ifdef DEEPINLINE
			__dil_jmp_b<val64<>>(opptr, state);
		#else
			__jmp_b<val64<>>();
		#endif
			break;
		}

		case op::jmp_b_32:
		{
		#ifdef DEEPINLINE
			__dil_jmp_b<val32<>>(opptr, state);
		#else
			__jmp_b<val32<>>();
		#endif
			break;
		}

		case op::jmp_b_16:
		{
		#ifdef DEEPINLINE
			__dil_jmp_b<val16<>>(opptr, state);
		#else
			__jmp_b<val16<>>();
		#endif
			break;
		}

		case op::jmp_b_8:
		{
		#ifdef DEEPINLINE
			__dil_jmp_b<val8<>>(opptr, state);
		#else
			__jmp_b<val8<>>();
		#endif
			break;
		}

		case op::jmp_be_64:
		{
		#ifdef DEEPINLINE
			__dil_jmp_be<val64<>>(opptr, state);
		#else
			__jmp_be<val64<>>();
		#endif
			break;
		}

		case op::jmp_be_32:
		{
		#ifdef DEEPINLINE
			__dil_jmp_be<val32<>>(opptr, state);
		#else
			__jmp_be<val32<>>();
		#endif
			break;
		}

		case op::jmp_be_16:
		{
		#ifdef DEEPINLINE
			__dil_jmp_be<val16<>>(opptr, state);
		#else
			__jmp_be<val16<>>();
		#endif
			break;
		}

		case op::jmp_be_8:
		{
		#ifdef DEEPINLINE
			__dil_jmp_be<val8<>>(opptr, state);
		#else
			__jmp_be<val8<>>();
		#endif
			break;
		}

		case op::jmp_e_64:
		{
		#ifdef DEEPINLINE
			__dil_jmp_e<val64<>>(opptr, state);
		#else
			__jmp_e<val64<>>();
		#endif
			break;
		}

		case op::jmp_e_32:
		{
		#ifdef DEEPINLINE
			__dil_jmp_e<val32<>>(opptr, state);
		#else
			__jmp_e<val32<>>();
		#endif
			break;
		}

		case op::jmp_e_16:
		{
		#ifdef DEEPINLINE
			__dil_jmp_e<val16<>>(opptr, state);
		#else
			__jmp_e<val16<>>();
		#endif
			break;
		}

		case op::jmp_e_8:
		{
		#ifdef DEEPINLINE
			__dil_jmp_e<val8<>>(opptr, state);
		#else
			__jmp_e<val8<>>();
		#endif
			break;
		}

		case op::jmp_ne_64:
		{
		#ifdef DEEPINLINE
			__dil_jmp_ne<val64<>>(opptr, state);
		#else
			__jmp_ne<val64<>>();
		#endif
			break;
		}

		case op::jmp_ne_32:
		{
		#ifdef DEEPINLINE
			__dil_jmp_ne<val32<>>(opptr, state);
		#else
			__jmp_ne<val32<>>();
		#endif
			break;
		}

		case op::jmp_ne_16:
		{
		#ifdef DEEPINLINE
			__dil_jmp_ne<val16<>>(opptr, state);
		#else
			__jmp_ne<val16<>>();
		#endif
			break;
		}

		case op::jmp_ne_8:
		{
		#ifdef DEEPINLINE
			__dil_jmp_ne<val8<>>(opptr, state);
		#else
			__jmp_ne<val8<>>();
		#endif
			break;
		}

		case op::jmp_un_64:
		{
		#ifdef DEEPINLINE
			__dil_jmp_un<val64<>>(opptr, state);
		#else
			__jmp_un<val64<>>();
		#endif
			break;
		}

		case op::jmp_un_32:
		{
		#ifdef DEEPINLINE
			__dil_jmp_un<val32<>>(opptr, state);
		#else
			__jmp_un<val32<>>();
		#endif
			break;
		}

		case op::jmp_un_16:
		{
		#ifdef DEEPINLINE
			__dil_jmp_un<val16<>>(opptr, state);
		#else
			__jmp_un<val16<>>();
		#endif
			break;
		}

		case op::jmp_un_8:
		{
		#ifdef DEEPINLINE
			__dil_jmp_un<val8<>>(opptr, state);
		#else
			__jmp_un<val8<>>();
		#endif
			break;
		}

		case op::jmp_msk_64:
		{
		#ifdef DEEPINLINE
			__dil_jmp_msk<val64<>>(opptr, state);
		#else
			__jmp_msk<val64<>>();
		#endif
			break;
		}

		case op::jmp_msk_32:
		{
		#ifdef DEEPINLINE
			__dil_jmp_msk<val32<>>(opptr, state);
		#else
			__jmp_msk<val32<>>();
		#endif
			break;
		}

		case op::jmp_msk_16:
		{
		#ifdef DEEPINLINE
			__dil_jmp_msk<val16<>>(opptr, state);
		#else
			__jmp_msk<val16<>>();
		#endif
			break;
		}

		case op::jmp_msk_8:
		{
		#ifdef DEEPINLINE
			__dil_jmp_msk<val8<>>(opptr, state);
		#else
			__jmp_msk<val8<>>();
		#endif
			break;
		}

		case op::l_load_64:
		{
		#ifdef DEEPINLINE
			__dil_l_load<val64<>>(opptr, ftop, stop);
		#else
			__l_load<val64<>>();
		#endif
			break;
		}

		case op::l_load_32:
		{
		#ifdef DEEPINLINE
			__dil_l_load<val32<>>(opptr, ftop, stop);
		#else
			__l_load<val32<>>();
		#endif
			break;
		}

		case op::l_load_16:
		{
		#ifdef DEEPINLINE
			__dil_l_load<val16<>>(opptr, ftop, stop);
		#else
			__l_load<val16<>>();
		#endif
			break;
		}

		case op::l_load_8:
		{
		#ifdef DEEPINLINE
			__dil_l_load<val8<>>(opptr, ftop, stop);
		#else
			__l_load<val8<>>();
		#endif
			break;
		}

		case op::l_store_64:
		{
		#ifdef DEEPINLINE
			__dil_l_store<val64<>>(opptr, ftop, stop);
		#else
			__l_store<val64<>>();
		#endif
			break;
		}

		case op::l_store_32:
		{
		#ifdef DEEPINLINE
			__dil_l_store<val32<>>(opptr, ftop, stop);
		#else
			__l_store<val32<>>();
		#endif
			break;
		}

		case op::l_store_16:
		{
		#ifdef DEEPINLINE
			__dil_l_store<val16<>>(opptr, ftop, stop);
		#else
			__l_store<val16<>>();
		#endif
			break;
		}

		case op::l_store_8:
		{
		#ifdef DEEPINLINE
			__dil_l_store<val8<>>(opptr, ftop, stop);
		#else
			__l_store<val8<>>();
		#endif
			break;
		}

		case op::s_load_64:
		{
		#ifdef DEEPINLINE
			__dil_s_load<val64<>>(opptr, ftop, stop);
		#else
			__s_load<val64<>>();
		#endif
			break;
		}

		case op::s_load_32:
		{
		#ifdef DEEPINLINE
			__dil_s_load<val32<>>(opptr, ftop, stop);
		#else
			__s_load<val32<>>();
		#endif
			break;
		}

		case op::s_load_16:
		{
		#ifdef DEEPINLINE
			__dil_s_load<val16<>>(opptr, ftop, stop);
		#else
			__s_load<val16<>>();
		#endif
			break;
		}

		case op::s_load_8:
		{
		#ifdef DEEPINLINE
			__dil_s_load<val8<>>(opptr, ftop, stop);
		#else
			__s_load<val8<>>();
		#endif
			break;
		}

		case op::s_store_64:
		{
		#ifdef DEEPINLINE
			__dil_s_store<val64<>>(opptr, ftop, stop);
		#else
			__s_store<val64<>>();
		#endif
			break;
		}

		case op::s_store_32:
		{
		#ifdef DEEPINLINE
			__dil_s_store<val32<>>(opptr, ftop, stop);
		#else
			__s_store<val32<>>();
		#endif
			break;
		}

		case op::s_store_16:
		{
		#ifdef DEEPINLINE
			__dil_s_store<val16<>>(opptr, ftop, stop);
		#else
			__s_store<val16<>>();
		#endif
			break;
		}

		case op::s_store_8:
		{
		#ifdef DEEPINLINE
			__dil_s_store<val8<>>(opptr, ftop, stop);
		#else
			__s_store<val8<>>();
		#endif
			break;
		}

		case op::push_64:
		{
		#ifdef DEEPINLINE
			__dil_push<val64<>>(opptr, stop);
		#else
			__push<val64<>>();
		#endif
			break;
		}

		case op::push_32:
		{
		#ifdef DEEPINLINE
			__dil_push<val32<>>(opptr, stop);
		#else
			__push<val32<>>();
		#endif
			break;
		}

		case op::push_16:
		{
		#ifdef DEEPINLINE
			__dil_push<val16<>>(opptr, stop);
		#else
			__push<val16<>>();
		#endif
			break;
		}

		case op::push_8:
		{
		#ifdef DEEPINLINE
			__dil_push<val8<>>(opptr, stop);
		#else
			__push<val8<>>();
		#endif
			break;
		}

		case op::pop_64:
		{
		#ifdef DEEPINLINE
			stack_type::dil_rem<val64<>>(stop);
		#else
			_stack.rem<val64<>>();
		#endif
			break;
		}

		case op::pop_32:
		{
		#ifdef DEEPINLINE
			stack_type::dil_rem<val32<>>(stop);
		#else
			_stack.rem<val32<>>();
		#endif
			break;
		}

		case op::pop_16:
		{
		#ifdef DEEPINLINE
			stack_type::dil_rem<val16<>>(stop);
		#else
			_stack.rem<val16<>>();
		#endif
			break;
		}

		case op::pop_8:
		{
		#ifdef DEEPINLINE
			stack_type::dil_rem<val8<>>(stop);
		#else
			_stack.rem<val8<>>();
		#endif
			break;
		}

		case op::dup_64:
		{
		#ifdef DEEPINLINE
			stack_type::dil_dup<val64<>>(stop);
		#else
			_stack.dup<val64<>>();
		#endif
			break;
		}

		case op::dup_32:
		{
		#ifdef DEEPINLINE
			stack_type::dil_dup<val32<>>(stop);
		#else
			_stack.dup<val32<>>();
		#endif
			break;
		}

		case op::dup_16:
		{
		#ifdef DEEPINLINE
			stack_type::dil_dup<val16<>>(stop);
		#else
			_stack.dup<val16<>>();
		#endif
			break;
		}

		case op::dup_8:
		{
		#ifdef DEEPINLINE
			stack_type::dil_dup<val8<>>(stop);
		#else
			_stack.dup<val8<>>();
		#endif
			break;
		}

		case op::ptrcpy_64:
		{
		#ifdef DEEPINLINE
			__dil_ptrcpy<val64<>>(stop);
		#else
			__ptrcpy<val64<>>();
		#endif
			break;
		}

		case op::ptrcpy_32:
		{
		#ifdef DEEPINLINE
			__dil_ptrcpy<val32<>>(stop);
		#else
			__ptrcpy<val32<>>();
		#endif
			break;
		}

		case op::ptrcpy_16:
		{
		#ifdef DEEPINLINE
			__dil_ptrcpy<val16<>>(stop);
		#else
			__ptrcpy<val16<>>();
		#endif
			break;
		}

		case op::ptrcpy_8:
		{
		#ifdef DEEPINLINE
			__dil_ptrcpy<val8<>>(stop);
		#else
			__ptrcpy<val8<>>();
		#endif
			break;
		}

		case op::ptrget_64:
		{
		#ifdef DEEPINLINE
			__dil_ptrget<val64<>>(stop);
		#else
			__ptrget<val64<>>();
		#endif
			break;
		}

		case op::ptrget_32:
		{
		#ifdef DEEPINLINE
			__dil_ptrget<val32<>>(stop);
		#else
			__ptrget<val32<>>();
		#endif
			break;
		}

		case op::ptrget_16:
		{
		#ifdef DEEPINLINE
			__dil_ptrget<val16<>>(stop);
		#else
			__ptrget<val16<>>();
		#endif
			break;
		}

		case op::ptrget_8:
		{
		#ifdef DEEPINLINE
			__dil_ptrget<val8<>>(stop);
		#else
			__ptrget<val8<>>();
		#endif
			break;
		}

		case op::ptrset_64:
		{
		#ifdef DEEPINLINE
			__dil_ptrset<val64<>>(stop);
		#else
			__ptrset<val64<>>();
		#endif
			break;
		}

		case op::ptrset_32:
		{
		#ifdef DEEPINLINE
			__dil_ptrset<val32<>>(stop);
		#else
			__ptrset<val32<>>();
		#endif
			break;
		}

		case op::ptrset_16:
		{
		#ifdef DEEPINLINE
			__dil_ptrset<val16<>>(stop);
		#else
			__ptrset<val16<>>();
		#endif
			break;
		}

		case op::ptrset_8:
		{
		#ifdef DEEPINLINE
			__dil_ptrset<val8<>>(stop);
		#else
			__ptrset<val8<>>();
		#endif
			break;
		}

		case op::neg_64:
		{
		#ifdef DEEPINLINE
			__dil_neg<val64<>>(state, stop);
		#else
			__neg<val64<>>();
		#endif
			break;
		}

		case op::neg_32:
		{
		#ifdef DEEPINLINE
			__dil_neg<val32<>>(state, stop);
		#else
			__neg<val32<>>();
		#endif
			break;
		}

		case op::neg_16:
		{
		#ifdef DEEPINLINE
			__dil_neg<val16<>>(state, stop);
		#else
			__neg<val16<>>();
		#endif
			break;
		}

		case op::neg_8:
		{
		#ifdef DEEPINLINE
			__dil_neg<val8<>>(state, stop);
		#else
			__neg<val8<>>();
		#endif
			break;
		}

		case op::not_64:
		{
		#ifdef DEEPINLINE
			__dil_not<val64<>>(stop);
		#else
			__not<val64<>>();
		#endif
			break;
		}

		case op::not_32:
		{
		#ifdef DEEPINLINE
			__dil_not<val32<>>(stop);
		#else
			__not<val32<>>();
		#endif
			break;
		}

		case op::not_16:
		{
		#ifdef DEEPINLINE
			__dil_not<val16<>>(stop);
		#else
			__not<val16<>>();
		#endif
			break;
		}

		case op::not_8:
		{
		#ifdef DEEPINLINE
			__dil_not<val8<>>(stop);
		#else
			__not<val8<>>();
		#endif
			break;
		}

		case op::sinc_64:
		{
		#ifdef DEEPINLINE
			__dil_sinc<val64<>>(state, stop);
		#else
			__sinc<val64<>>();
		#endif
			break;
		}

		case op::sinc_32:
		{
		#ifdef DEEPINLINE
			__dil_sinc<val32<>>(state, stop);
		#else
			__sinc<val32<>>();
		#endif
			break;
		}

		case op::sinc_16:
		{
		#ifdef DEEPINLINE
			__dil_sinc<val16<>>(state, stop);
		#else
			__sinc<val16<>>();
		#endif
			break;
		}

		case op::sinc_8:
		{
		#ifdef DEEPINLINE
			__dil_sinc<val8<>>(state, stop);
		#else
			__sinc<val8<>>();
		#endif
			break;
		}

		case op::uinc_64:
		{
		#ifdef DEEPINLINE
			__dil_uinc<val64<>>(state, stop);
		#else
			__uinc<val64<>>();
		#endif
			break;
		}

		case op::uinc_32:
		{
		#ifdef DEEPINLINE
			__dil_uinc<val32<>>(state, stop);
		#else
			__uinc<val32<>>();
		#endif
			break;
		}

		case op::uinc_16:
		{
		#ifdef DEEPINLINE
			__dil_uinc<val16<>>(state, stop);
		#else
			__uinc<val16<>>();
		#endif
			break;
		}

		case op::uinc_8:
		{
		#ifdef DEEPINLINE
			__dil_uinc<val8<>>(state, stop);
		#else
			__uinc<val8<>>();
		#endif
			break;
		}

		case op::sdec_64:
		{
		#ifdef DEEPINLINE
			__dil_sdec<val64<>>(state, stop);
		#else
			__sdec<val64<>>();
		#endif
			break;
		}

		case op::sdec_32:
		{
		#ifdef DEEPINLINE
			__dil_sdec<val32<>>(state, stop);
		#else
			__sdec<val32<>>();
		#endif
			break;
		}

		case op::sdec_16:
		{
		#ifdef DEEPINLINE
			__dil_sdec<val16<>>(state, stop);
		#else
			__sdec<val16<>>();
		#endif
			break;
		}

		case op::sdec_8:
		{
		#ifdef DEEPINLINE
			__dil_sdec<val8<>>(state, stop);
		#else
			__sdec<val8<>>();
		#endif
			break;
		}

		case op::udec_64:
		{
		#ifdef DEEPINLINE
			__dil_udec<val64<>>(state, stop);
		#else
			__udec<val64<>>();
		#endif
			break;
		}

		case op::udec_32:
		{
		#ifdef DEEPINLINE
			__dil_udec<val32<>>(state, stop);
		#else
			__udec<val32<>>();
		#endif
			break;
		}

		case op::udec_16:
		{
		#ifdef DEEPINLINE
			__dil_udec<val16<>>(state, stop);
		#else
			__udec<val16<>>();
		#endif
			break;
		}

		case op::udec_8:
		{
		#ifdef DEEPINLINE
			__dil_udec<val8<>>(state, stop);
		#else
			__udec<val8<>>();
		#endif
			break;
		}

		case op::and_64:
		{
		#ifdef DEEPINLINE
			__dil_and<val64<>>(stop);
		#else
			__and<val64<>>();
		#endif
			break;
		}

		case op::and_32:
		{
		#ifdef DEEPINLINE
			__dil_and<val32<>>(stop);
		#else
			__and<val32<>>();
		#endif
			break;
		}

		case op::and_16:
		{
		#ifdef DEEPINLINE
			__dil_and<val16<>>(stop);
		#else
			__and<val16<>>();
		#endif
			break;
		}

		case op::and_8:
		{
		#ifdef DEEPINLINE
			__dil_and<val8<>>(stop);
		#else
			__and<val8<>>();
		#endif
			break;
		}

		case op::or_64:
		{
		#ifdef DEEPINLINE
			__dil_or<val64<>>(stop);
		#else
			__or<val64<>>();
		#endif
			break;
		}

		case op::or_32:
		{
		#ifdef DEEPINLINE
			__dil_or<val32<>>(stop);
		#else
			__or<val32<>>();
		#endif
			break;
		}

		case op::or_16:
		{
		#ifdef DEEPINLINE
			__dil_or<val16<>>(stop);
		#else
			__or<val16<>>();
		#endif
			break;
		}

		case op::or_8:
		{
		#ifdef DEEPINLINE
			__dil_or<val8<>>(stop);
		#else
			__or<val8<>>();
		#endif
			break;
		}

		case op::xor_64:
		{
		#ifdef DEEPINLINE
			__dil_xor<val64<>>(stop);
		#else
			__xor<val64<>>();
		#endif
			break;
		}

		case op::xor_32:
		{
		#ifdef DEEPINLINE
			__dil_xor<val32<>>(stop);
		#else
			__xor<val32<>>();
		#endif
			break;
		}

		case op::xor_16:
		{
		#ifdef DEEPINLINE
			__dil_xor<val16<>>(stop);
		#else
			__xor<val16<>>();
		#endif
			break;
		}

		case op::xor_8:
		{
		#ifdef DEEPINLINE
			__dil_xor<val8<>>(stop);
		#else
			__xor<val8<>>();
		#endif
			break;
		}

		case op::sadd_64:
		{
		#ifdef DEEPINLINE
			__dil_sadd<val64<>>(state, stop);
		#else
			__sadd<val64<>>();
		#endif
			break;
		}

		case op::sadd_32:
		{
		#ifdef DEEPINLINE
			__dil_sadd<val32<>>(state, stop);
		#else
			__sadd<val32<>>();
		#endif
			break;
		}

		case op::sadd_16:
		{
		#ifdef DEEPINLINE
			__dil_sadd<val16<>>(state, stop);
		#else
			__sadd<val16<>>();
		#endif
			break;
		}

		case op::sadd_8:
		{
		#ifdef DEEPINLINE
			__dil_sadd<val8<>>(state, stop);
		#else
			__sadd<val8<>>();
		#endif
			break;
		}

		case op::uadd_64:
		{
		#ifdef DEEPINLINE
			__dil_uadd<val64<>>(state, stop);
		#else
			__uadd<val64<>>();
		#endif
			break;
		}

		case op::uadd_32:
		{
		#ifdef DEEPINLINE
			__dil_uadd<val32<>>(state, stop);
		#else
			__uadd<val32<>>();
		#endif
			break;
		}

		case op::uadd_16:
		{
		#ifdef DEEPINLINE
			__dil_uadd<val16<>>(state, stop);
		#else
			__uadd<val16<>>();
		#endif
			break;
		}

		case op::uadd_8:
		{
		#ifdef DEEPINLINE
			__dil_uadd<val8<>>(state, stop);
		#else
			__uadd<val8<>>();
		#endif
			break;
		}

		case op::ssub_64:
		{
		#ifdef DEEPINLINE
			__dil_ssub<val64<>>(state, stop);
		#else
			__ssub<val64<>>();
		#endif
			break;
		}

		case op::ssub_32:
		{
		#ifdef DEEPINLINE
			__dil_ssub<val32<>>(state, stop);
		#else
			__ssub<val32<>>();
		#endif
			break;
		}

		case op::ssub_16:
		{
		#ifdef DEEPINLINE
			__dil_ssub<val16<>>(state, stop);
		#else
			__ssub<val16<>>();
		#endif
			break;
		}

		case op::ssub_8:
		{
		#ifdef DEEPINLINE
			__dil_ssub<val8<>>(state, stop);
		#else
			__ssub<val8<>>();
		#endif
			break;
		}

		case op::usub_64:
		{
		#ifdef DEEPINLINE
			__dil_usub<val64<>>(state, stop);
		#else
			__usub<val64<>>();
		#endif
			break;
		}

		case op::usub_32:
		{
		#ifdef DEEPINLINE
			__dil_usub<val32<>>(state, stop);
		#else
			__usub<val32<>>();
		#endif
			break;
		}

		case op::usub_16:
		{
		#ifdef DEEPINLINE
			__dil_usub<val16<>>(state, stop);
		#else
			__usub<val16<>>();
		#endif
			break;
		}

		case op::usub_8:
		{
		#ifdef DEEPINLINE
			__dil_usub<val8<>>(state, stop);
		#else
			__usub<val8<>>();
		#endif
			break;
		}

		case op::smul_64:
		{
		#ifdef DEEPINLINE
			__dil_smul<val64<>>(state, stop);
		#else
			__smul<val64<>>();
		#endif
			break;
		}

		case op::smul_32:
		{
		#ifdef DEEPINLINE
			__dil_smul<val32<>>(state, stop);
		#else
			__smul<val32<>>();
		#endif
			break;
		}

		case op::smul_16:
		{
		#ifdef DEEPINLINE
			__dil_smul<val16<>>(state, stop);
		#else
			__smul<val16<>>();
		#endif
			break;
		}

		case op::smul_8:
		{
		#ifdef DEEPINLINE
			__dil_smul<val8<>>(state, stop);
		#else
			__smul<val8<>>();
		#endif
			break;
		}

		case op::umul_64:
		{
		#ifdef DEEPINLINE
			__dil_umul<val64<>>(state, stop);
		#else
			__umul<val64<>>();
		#endif
			break;
		}

		case op::umul_32:
		{
		#ifdef DEEPINLINE
			__dil_umul<val32<>>(state, stop);
		#else
			__umul<val32<>>();
		#endif
			break;
		}

		case op::umul_16:
		{
		#ifdef DEEPINLINE
			__dil_umul<val16<>>(state, stop);
		#else
			__umul<val16<>>();
		#endif
			break;
		}

		case op::umul_8:
		{
		#ifdef DEEPINLINE
			__dil_umul<val8<>>(state, stop);
		#else
			__umul<val8<>>();
		#endif
			break;
		}

		case op::smod_64:
		{
		#ifdef DEEPINLINE
			__dil_smod<val64<>>(state, stop);
		#else
			__smod<val64<>>();
		#endif
			break;
		}

		case op::smod_32:
		{
		#ifdef DEEPINLINE
			__dil_smod<val32<>>(state, stop);
		#else
			__smod<val32<>>();
		#endif
			break;
		}

		case op::smod_16:
		{
		#ifdef DEEPINLINE
			__dil_smod<val16<>>(state, stop);
		#else
			__smod<val16<>>();
		#endif
			break;
		}

		case op::smod_8:
		{
		#ifdef DEEPINLINE
			__dil_smod<val8<>>(state, stop);
		#else
			__smod<val8<>>();
		#endif
			break;
		}

		case op::umod_64:
		{
		#ifdef DEEPINLINE
			__dil_umod<val64<>>(state, stop);
		#else
			__umod<val64<>>();
		#endif
			break;
		}

		case op::umod_32:
		{
		#ifdef DEEPINLINE
			__dil_umod<val32<>>(state, stop);
		#else
			__umod<val32<>>();
		#endif
			break;
		}

		case op::umod_16:
		{
		#ifdef DEEPINLINE
			__dil_umod<val16<>>(state, stop);
		#else
			__umod<val16<>>();
		#endif
			break;
		}

		case op::umod_8:
		{
		#ifdef DEEPINLINE
			__dil_umod<val8<>>(state, stop);
		#else
			__umod<val8<>>();
		#endif
			break;
		}

		case op::sdiv_64:
		{
		#ifdef DEEPINLINE
			__dil_sdiv<val64<>>(state, stop);
		#else
			__sdiv<val64<>>();
		#endif
			break;
		}

		case op::sdiv_32:
		{
		#ifdef DEEPINLINE
			__dil_sdiv<val32<>>(state, stop);
		#else
			__sdiv<val32<>>();
		#endif
			break;
		}

		case op::sdiv_16:
		{
		#ifdef DEEPINLINE
			__dil_sdiv<val16<>>(state, stop);
		#else
			__sdiv<val16<>>();
		#endif
			break;
		}

		case op::sdiv_8:
		{
		#ifdef DEEPINLINE
			__dil_sdiv<val8<>>(state, stop);
		#else
			__sdiv<val8<>>();
		#endif
			break;
		}

		case op::udiv_64:
		{
		#ifdef DEEPINLINE
			__dil_udiv<val64<>>(state, stop);
		#else
			__udiv<val64<>>();
		#endif
			break;
		}

		case op::udiv_32:
		{
		#ifdef DEEPINLINE
			__dil_udiv<val32<>>(state, stop);
		#else
			__udiv<val32<>>();
		#endif
			break;
		}

		case op::udiv_16:
		{
		#ifdef DEEPINLINE
			__dil_udiv<val16<>>(state, stop);
		#else
			__udiv<val16<>>();
		#endif
			break;
		}

		case op::udiv_8:
		{
		#ifdef DEEPINLINE
			__dil_udiv<val8<>>(state, stop);
		#else
			__udiv<val8<>>();
		#endif
			break;
		}

		case op::scmp_64:
		{
		#ifdef DEEPINLINE
			__dil_scmp<val64<>>(state, stop);
		#else
			__scmp<val64<>>();
		#endif
			break;
		}

		case op::scmp_32:
		{
		#ifdef DEEPINLINE
			__dil_scmp<val32<>>(state, stop);
		#else
			__scmp<val32<>>();
		#endif
			break;
		}

		case op::scmp_16:
		{
		#ifdef DEEPINLINE
			__dil_scmp<val16<>>(state, stop);
		#else
			__scmp<val16<>>();
		#endif
			break;
		}

		case op::scmp_8:
		{
		#ifdef DEEPINLINE
			__dil_scmp<val8<>>(state, stop);
		#else
			__scmp<val8<>>();
		#endif
			break;
		}

		case op::ucmp_64:
		{
		#ifdef DEEPINLINE
			__dil_ucmp<val64<>>(state, stop);
		#else
			__ucmp<val64<>>();
		#endif
			break;
		}

		case op::ucmp_32:
		{
		#ifdef DEEPINLINE
			__dil_ucmp<val32<>>(state, stop);
		#else
			__ucmp<val32<>>();
		#endif
			break;
		}

		case op::ucmp_16:
		{
		#ifdef DEEPINLINE
			__dil_ucmp<val16<>>(state, stop);
		#else
			__ucmp<val16<>>();
		#endif
			break;
		}

		case op::ucmp_8:
		{
		#ifdef DEEPINLINE
			__dil_ucmp<val8<>>(state, stop);
		#else
			__ucmp<val8<>>();
		#endif
			break;
		}

		case op::rotl_64:
		{
		#ifdef DEEPINLINE
			__dil_rotl<val64<>>(stop);
		#else
			__rotl<val64<>>();
		#endif
			break;
		}

		case op::rotl_32:
		{
		#ifdef DEEPINLINE
			__dil_rotl<val32<>>(stop);
		#else
			__rotl<val32<>>();
		#endif
			break;
		}

		case op::rotl_16:
		{
		#ifdef DEEPINLINE
			__dil_rotl<val16<>>(stop);
		#else
			__rotl<val16<>>();
		#endif
			break;
		}

		case op::rotl_8:
		{
		#ifdef DEEPINLINE
			__dil_rotl<val8<>>(stop);
		#else
			__rotl<val8<>>();
		#endif
			break;
		}

		case op::rotr_64:
		{
		#ifdef DEEPINLINE
			__dil_rotr<val64<>>(stop);
		#else
			__rotr<val64<>>();
		#endif
			break;
		}

		case op::rotr_32:
		{
		#ifdef DEEPINLINE
			__dil_rotr<val32<>>(stop);
		#else
			__rotr<val32<>>();
		#endif
			break;
		}

		case op::rotr_16:
		{
		#ifdef DEEPINLINE
			__dil_rotr<val16<>>(stop);
		#else
			__rotr<val16<>>();
		#endif
			break;
		}

		case op::rotr_8:
		{
		#ifdef DEEPINLINE
			__dil_rotr<val8<>>(stop);
		#else
			__rotr<val8<>>();
		#endif
			break;
		}

		case op::sshl_64:
		{
		#ifdef DEEPINLINE
			__dil_sshl<val64<>>(stop);
		#else
			__sshl<val64<>>();
		#endif
			break;
		}

		case op::sshl_32:
		{
		#ifdef DEEPINLINE
			__dil_sshl<val32<>>(stop);
		#else
			__sshl<val32<>>();
		#endif
			break;
		}

		case op::sshl_16:
		{
		#ifdef DEEPINLINE
			__dil_sshl<val16<>>(stop);
		#else
			__sshl<val16<>>();
		#endif
			break;
		}

		case op::sshl_8:
		{
		#ifdef DEEPINLINE
			__dil_sshl<val8<>>(stop);
		#else
			__sshl<val8<>>();
		#endif
			break;
		}

		case op::ushl_64:
		{
		#ifdef DEEPINLINE
			__dil_ushl<val64<>>(stop);
		#else
			__ushl<val64<>>();
		#endif
			break;
		}

		case op::ushl_32:
		{
		#ifdef DEEPINLINE
			__dil_ushl<val32<>>(stop);
		#else
			__ushl<val32<>>();
		#endif
			break;
		}

		case op::ushl_16:
		{
		#ifdef DEEPINLINE
			__dil_ushl<val16<>>(stop);
		#else
			__ushl<val16<>>();
		#endif
			break;
		}

		case op::ushl_8:
		{
		#ifdef DEEPINLINE
			__dil_ushl<val8<>>(stop);
		#else
			__ushl<val8<>>();
		#endif
			break;
		}

		case op::sshr_64:
		{
		#ifdef DEEPINLINE
			__dil_sshr<val64<>>(stop);
		#else
			__sshr<val64<>>();
		#endif
			break;
		}

		case op::sshr_32:
		{
		#ifdef DEEPINLINE
			__dil_sshr<val32<>>(stop);
		#else
			__sshr<val32<>>();
		#endif
			break;
		}

		case op::sshr_16:
		{
		#ifdef DEEPINLINE
			__dil_sshr<val16<>>(stop);
		#else
			__sshr<val16<>>();
		#endif
			break;
		}

		case op::sshr_8:
		{
		#ifdef DEEPINLINE
			__dil_sshr<val8<>>(stop);
		#else
			__sshr<val8<>>();
		#endif
			break;
		}

		case op::ushr_64:
		{
		#ifdef DEEPINLINE
			__dil_ushr<val64<>>(stop);
		#else
			__ushr<val64<>>();
		#endif
			break;
		}

		case op::ushr_32:
		{
		#ifdef DEEPINLINE
			__dil_ushr<val32<>>(stop);
		#else
			__ushr<val32<>>();
		#endif
			break;
		}

		case op::ushr_16:
		{
		#ifdef DEEPINLINE
			__dil_ushr<val16<>>(stop);
		#else
			__ushr<val16<>>();
		#endif
			break;
		}

		case op::ushr_8:
		{
		#ifdef DEEPINLINE
			__dil_ushr<val8<>>(stop);
		#else
			__ushr<val8<>>();
		#endif
			break;
		}

		case op::fneg_64:
		{
		#ifdef DEEPINLINE
			__dil_fneg<val64<>>(stop);
		#else
			__fneg<val64<>>();
		#endif
			break;
		}

		case op::fneg_32:
		{
		#ifdef DEEPINLINE
			__dil_fneg<val32<>>(stop);
		#else
			__fneg<val32<>>();
		#endif
			break;
		}

		case op::fevl_64:
		{
		#ifdef DEEPINLINE
			__dil_fevl<val64<>>(state, stop);
		#else
			__fevl<val64<>>();
		#endif
			break;
		}

		case op::fevl_32:
		{
		#ifdef DEEPINLINE
			__dil_fevl<val32<>>(state, stop);
		#else
			__fevl<val32<>>();
		#endif
			break;
		}

		case op::fadd_64:
		{
		#ifdef DEEPINLINE
			__dil_fadd<val64<>>(stop);
		#else
			__fadd<val64<>>();
		#endif
			break;
		}

		case op::fadd_32:
		{
		#ifdef DEEPINLINE
			__dil_fadd<val32<>>(stop);
		#else
			__fadd<val32<>>();
		#endif
			break;
		}

		case op::fsub_64:
		{
		#ifdef DEEPINLINE
			__dil_fsub<val64<>>(stop);
		#else
			__fsub<val64<>>();
		#endif
			break;
		}

		case op::fsub_32:
		{
		#ifdef DEEPINLINE
			__dil_fsub<val32<>>(stop);
		#else
			__fsub<val32<>>();
		#endif
			break;
		}

		case op::fmul_64:
		{
		#ifdef DEEPINLINE
			__dil_fmul<val64<>>(stop);
		#else
			__fmul<val64<>>();
		#endif
			break;
		}

		case op::fmul_32:
		{
		#ifdef DEEPINLINE
			__dil_fmul<val32<>>(stop);
		#else
			__fmul<val32<>>();
		#endif
			break;
		}

		case op::fmod_64:
		{
		#ifdef DEEPINLINE
			__dil_fmod<val64<>>(stop);
		#else
			__fmod<val64<>>();
		#endif
			break;
		}

		case op::fmod_32:
		{
		#ifdef DEEPINLINE
			__dil_fmod<val32<>>(stop);
		#else
			__fmod<val32<>>();
		#endif
			break;
		}

		case op::fdiv_64:
		{
		#ifdef DEEPINLINE
			__dil_fdiv<val64<>>(stop);
		#else
			__fdiv<val64<>>();
		#endif
			break;
		}

		case op::fdiv_32:
		{
		#ifdef DEEPINLINE
			__dil_fdiv<val32<>>(stop);
		#else
			__fdiv<val32<>>();
		#endif
			break;
		}

		case op::fcmp_64:
		{
		#ifdef DEEPINLINE
			__dil_fcmp<val64<>>(state, stop);
		#else
			__fcmp<val64<>>();
		#endif
			break;
		}

		case op::fcmp_32:
		{
		#ifdef DEEPINLINE
			__dil_fcmp<val32<>>(state, stop);
		#else
			__fcmp<val32<>>();
		#endif
			break;
		}

		case op::si64_2_f64:
		{
		#ifdef DEEPINLINE
			val64<>& v = stack_type::dil_top<val64<>>(stop);
			int64_t si = v.si;

			v.f = (double)si;
		#else
			val64<>& v = _stack.top<val64<>>();
			int64_t si = v.si;

			v.f = (double)si;
		#endif
			break;
		}

		case op::si64_2_f32:
		{
		#ifdef DEEPINLINE
			val64<>& v = stack_type::dil_top<val64<>>(stop);
			int64_t si = v.si;

			stack_type::dil_dealloc(stop, (uint64_t)sizeof(val32<>));

			v.half.hi.f = (float)si;
		#else
			val64<>& v = _stack.top<val64<>>();
			int64_t si = v.si;

			_stack.dealloc((uint64_t)sizeof(val32<>));

			v.half.hi.f = (float)si;
		#endif
			break;
		}

		case op::si32_2_f64:
		{
		#ifdef DEEPINLINE
			stack_type::dil_alloc(stop, (uint64_t)sizeof(val32<>));

			val64<>& v = stack_type::dil_top<val64<>>(stop);
			int32_t si = v.half.hi.si;

			v.f = (double)si;
		#else
			_stack.alloc((uint64_t)sizeof(val32<>));

			val64<>& v = _stack.top<val64<>>();
			int32_t si = v.half.hi.si;

			v.f = (double)si;
		#endif
			break;
		}

		case op::si32_2_f32:
		{
		#ifdef DEEPINLINE
			val32<>& v = stack_type::dil_top<val32<>>(stop);
			int32_t si = v.si;

			v.f = (float)si;
		#else
			val32<>& v = _stack.top<val32<>>();
			int32_t si = v.si;

			v.f = (float)si;
		#endif
			break;
		}

		case op::ui64_2_f64:
		{
		#ifdef DEEPINLINE
			val64<>& v = stack_type::dil_top<val64<>>(stop);
			uint64_t ui = v.ui;

			v.f = (double)ui;
		#else
			val64<>& v = _stack.top<val64<>>();
			uint64_t ui = v.ui;

			v.f = (double)ui;
		#endif
			break;
		}

		case op::ui64_2_f32:
		{
		#ifdef DEEPINLINE
			val64<>& v = stack_type::dil_top<val64<>>(stop);
			uint64_t ui = v.ui;

			stack_type::dil_dealloc(stop, (uint64_t)sizeof(val32<>));

			v.half.hi.f = (float)ui;
		#else
			val64<>& v = _stack.top<val64<>>();
			uint64_t ui = v.ui;

			_stack.dealloc((uint64_t)sizeof(val32<>));

			v.half.hi.f = (float)ui;
		#endif
			break;
		}

		case op::ui32_2_f64:
		{
		#ifdef DEEPINLINE
			stack_type::dil_alloc(stop, (uint64_t)sizeof(val32<>));

			val64<>& v = stack_type::dil_top<val64<>>(stop);
			uint32_t ui = v.half.hi.ui;

			v.f = (double)ui;
		#else
			_stack.alloc((uint64_t)sizeof(val32<>));

			val64<>& v = _stack.top<val64<>>();
			uint32_t ui = v.half.hi.ui;

			v.f = (double)ui;
		#endif
			break;
		}

		case op::ui32_2_f32:
		{
		#ifdef DEEPINLINE
			val32<>& v = stack_type::dil_top<val32<>>(stop);
			uint32_t ui = v.ui;

			v.f = (float)ui;
		#else
			val32<>& v = _stack.top<val32<>>();
			uint32_t ui = v.ui;

			v.f = (float)ui;
		#endif
			break;
		}

		case op::f64_2_si64:
		{
		#ifdef DEEPINLINE
			val64<>& v = stack_type::dil_top<val64<>>(stop);
			double f = v.f;

			bool ovf = !(f < (double)__2_POW_63);
			bool unf = !(f >= -(double)__2_POW_63);

			state.eval.bits.ierr = ovf && unf;
			state.eval.bits.iovf = ovf && !unf;
			state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.si = (int64_t)f;
		#else
			val64<>& v = _stack.top<val64<>>();
			double f = v.f;

			bool ovf = !(f < (double)__2_POW_63);
			bool unf = !(f >= -(double)__2_POW_63);

			_state.eval.bits.ierr = ovf && unf;
			_state.eval.bits.iovf = ovf && !unf;
			_state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.si = (int64_t)f;
		#endif
			break;
		}

		case op::f64_2_si32:
		{
		#ifdef DEEPINLINE
			val64<>& v = stack_type::dil_top<val64<>>(stop);
			double f = v.f;

			stack_type::dil_dealloc(stop, (uint64_t)sizeof(val32<>));

			bool ovf = !(f < (double)__2_POW_31);
			bool unf = !(f >= -(double)__2_POW_31);

			state.eval.bits.ierr = ovf && unf;
			state.eval.bits.iovf = ovf && !unf;
			state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.half.hi.si = (int32_t)f;
		#else
			val64<>& v = _stack.top<val64<>>();
			double f = v.f;

			_stack.dealloc((uint64_t)sizeof(val32<>));

			bool ovf = !(f < (double)__2_POW_31);
			bool unf = !(f >= -(double)__2_POW_31);

			_state.eval.bits.ierr = ovf && unf;
			_state.eval.bits.iovf = ovf && !unf;
			_state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.half.hi.si = (int32_t)f;
		#endif
			break;
		}

		case op::f32_2_si64:
		{
		#ifdef DEEPINLINE
			stack_type::dil_alloc(stop, (uint64_t)sizeof(val32<>));

			val64<>& v = stack_type::dil_top<val64<>>(stop);
			float f = v.half.hi.f;

			bool ovf = !(f < (float)__2_POW_63);
			bool unf = !(f >= -(float)__2_POW_63);

			state.eval.bits.ierr = ovf && unf;
			state.eval.bits.iovf = ovf && !unf;
			state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.si = (int64_t)f;
		#else
			_stack.alloc((uint64_t)sizeof(val32<>));

			val64<>& v = _stack.top<val64<>>();
			float f = v.half.hi.f;

			bool ovf = !(f < (float)__2_POW_63);
			bool unf = !(f >= -(float)__2_POW_63);

			_state.eval.bits.ierr = ovf && unf;
			_state.eval.bits.iovf = ovf && !unf;
			_state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.si = (int64_t)f;
		#endif
			break;
		}

		case op::f32_2_si32:
		{
		#ifdef DEEPINLINE
			val32<>& v = stack_type::dil_top<val32<>>(stop);
			float f = v.f;

			bool ovf = !(f < (float)__2_POW_31);
			bool unf = !(f >= -(float)__2_POW_31);

			state.eval.bits.ierr = ovf && unf;
			state.eval.bits.iovf = ovf && !unf;
			state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.si = (int32_t)f;
		#else
			val32<>& v = _stack.top<val32<>>();
			float f = v.f;

			bool ovf = !(f < (float)__2_POW_31);
			bool unf = !(f >= -(float)__2_POW_31);

			_state.eval.bits.ierr = ovf && unf;
			_state.eval.bits.iovf = ovf && !unf;
			_state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.si = (int32_t)f;
		#endif
			break;
		}

		case op::f64_2_ui64:
		{
		#ifdef DEEPINLINE
			val64<>& v = stack_type::dil_top<val64<>>(stop);
			double f = v.f;

			bool ovf = !(f < (double)__2_POW_64);
			bool unf = !(f >= (double)__0);

			state.eval.bits.ierr = ovf && unf;
			state.eval.bits.iovf = ovf && !unf;
			state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.ui = (uint64_t)f;
		#else
			val64<>& v = _stack.top<val64<>>();
			double f = v.f;

			bool ovf = !(f < (double)__2_POW_64);
			bool unf = !(f >= (double)__0);

			_state.eval.bits.ierr = ovf && unf;
			_state.eval.bits.iovf = ovf && !unf;
			_state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.ui = (uint64_t)f;
		#endif
			break;
		}

		case op::f64_2_ui32:
		{
		#ifdef DEEPINLINE
			val64<>& v = stack_type::dil_top<val64<>>(stop);
			double f = v.f;

			stack_type::dil_dealloc(stop, (uint64_t)sizeof(val32<>));

			bool ovf = !(f < (double)__2_POW_32);
			bool unf = !(f >= (double)__0);

			state.eval.bits.ierr = ovf && unf;
			state.eval.bits.iovf = ovf && !unf;
			state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.half.hi.ui = (uint32_t)f;
		#else
			val64<>& v = _stack.top<val64<>>();
			double f = v.f;

			_stack.dealloc((uint64_t)sizeof(val32<>));

			bool ovf = !(f < (double)__2_POW_32);
			bool unf = !(f >= (double)__0);

			_state.eval.bits.ierr = ovf && unf;
			_state.eval.bits.iovf = ovf && !unf;
			_state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.half.hi.ui = (uint32_t)f;
		#endif
			break;
		}

		case op::f32_2_ui64:
		{
		#ifdef DEEPINLINE
			stack_type::dil_alloc(stop, (uint64_t)sizeof(val32<>));

			val64<>& v = stack_type::dil_top<val64<>>(stop);
			float f = v.half.hi.f;

			bool ovf = !(f < (float)__2_POW_64);
			bool unf = !(f >= (float)__0);

			state.eval.bits.ierr = ovf && unf;
			state.eval.bits.iovf = ovf && !unf;
			state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.ui = (uint64_t)f;
		#else
			_stack.alloc((uint64_t)sizeof(val32<>));

			val64<>& v = _stack.top<val64<>>();
			float f = v.half.hi.f;

			bool ovf = !(f < (float)__2_POW_64);
			bool unf = !(f >= (float)__0);

			_state.eval.bits.ierr = ovf && unf;
			_state.eval.bits.iovf = ovf && !unf;
			_state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.ui = (uint64_t)f;
		#endif
			break;
		}

		case op::f32_2_ui32:
		{
		#ifdef DEEPINLINE
			val32<>& v = stack_type::dil_top<val32<>>(stop);
			float f = v.f;

			bool ovf = !(f < (float)__2_POW_32);
			bool unf = !(f >= (float)__0);

			state.eval.bits.ierr = ovf && unf;
			state.eval.bits.iovf = ovf && !unf;
			state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.ui = (uint32_t)f;
		#else
			val32<>& v = _stack.top<val32<>>();
			float f = v.f;

			bool ovf = !(f < (float)__2_POW_32);
			bool unf = !(f >= (float)__0);

			_state.eval.bits.ierr = ovf && unf;
			_state.eval.bits.iovf = ovf && !unf;
			_state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.ui = (uint32_t)f;
		#endif
			break;
		}

		default:
		#ifdef OPRNDCHECK
			throw runtime_error(_err_msg_wrong_opnd);
		#else
			__assume(false);
		#endif
			break;
		}
	}

#ifdef DEEPINLINE
	_stack.set_data(data);
	_stack.set_size(size);
	_stack.set_sbeg(sbeg);
	_stack.set_send(send);
	_stack.set_ftop(ftop);
	_stack.set_stop(stop);

	_opptr = opptr;
	_state = state;
#endif
}

void dispatcher::push_ptr(uintptr_t value)
{
	_stack.push_ptr(value);
}

uintptr_t dispatcher::pop_ptr()
{
	return _stack.pop_ptr();
}

dispatcher& dispatcher::operator=(dispatcher&& o) noexcept
{
#ifdef FETCHCHECK
	_code_beg = move(o._code_beg);
	_code_end = move(o._code_end);
#endif
	_nativehub = move(o._nativehub);
	_stack = move(o._stack);
	_opptr = move(o._opptr);
	_state = move(o._state);

#ifdef FETCHCHECK
	o._code_beg = (const uint8_t*)nullptr;
	o._code_end = (const uint8_t*)nullptr;
#endif
	o._opptr = (const uint8_t*)nullptr;
	o._state = move(state());

	return *this;
}