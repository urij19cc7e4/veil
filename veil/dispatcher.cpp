#include "dispatcher.hpp"

#include <cmath>

using namespace interpreter;
using namespace std;

#define __2_POW_64 18446744073709551616.0
#define __2_POW_63 9223372036854775808.0
#define __2_POW_32 4294967296.0
#define __2_POW_31 2147483648.0
#define __0 0.0

#ifdef FETCHCHECK
dispatcher::dispatcher(const uint8_t* bcode, const uint8_t* ecode, uint64_t size)
	: _code_beg(bcode), _code_end(ecode), _stack(size), _opptr(bcode), _state() {}

dispatcher::dispatcher(dispatcher&& o) noexcept
	: _code_beg(move(o._code_beg)), _code_end(move(o._code_end)),
	_stack(move(o._stack)), _opptr(move(o._opptr)), _state(move(o._state))
{
	o._code_beg = (const uint8_t*)nullptr;
	o._code_end = (const uint8_t*)nullptr;
	o._opptr = (const uint8_t*)nullptr;
	o._state = move(state());
}
#else
dispatcher::dispatcher(const uint8_t* code, uint64_t size)
	: _stack(size), _opptr(code), _state() {}

dispatcher::dispatcher(dispatcher&& o) noexcept
	: _stack(move(o._stack)), _opptr(move(o._opptr)), _state(move(o._state))
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
	while (true)
		switch (fetch<op>())
		{
		case op::nop:
			break;

		case op::exit:
			return;

		case op::nativecall:
		{
			throw;
			break;
		}

		case op::ret:
		{
			_opptr = (const uint8_t*)_stack.pop_frame();
			break;
		}

		case op::call_64:
		{
			__call<val64<>>();
			break;
		}

		case op::call_32:
		{
			__call<val32<>>();
			break;
		}

		case op::call_16:
		{
			__call<val16<>>();
			break;
		}

		case op::call_8:
		{
			__call<val8<>>();
			break;
		}

		case op::memcpy:
		{
			uint8_t* dst = (uint8_t*)_stack.pop_ptr();
			uint8_t* src = (uint8_t*)_stack.pop_ptr();
			uint64_t c = _stack.pop<val64<>>().ui;

			memcpy(dst, src, c);

			break;
		}

		case op::memset:
		{
			uint8_t* dst = (uint8_t*)_stack.pop_ptr();
			uint8_t val = _stack.pop<val8<>>().ui;
			uint64_t c = _stack.pop<val64<>>().ui;

			memset(dst, val, c);

			break;
		}

		case op::l_alloc:
		{
			_stack.alloc((uint64_t)fetch<uint16_t>());
			break;
		}

		case op::s_alloc:
		{
			_stack.alloc((uint64_t)fetch<uint8_t>());
			break;
		}

		case op::l_allocz:
		{
			_stack.allocz((uint64_t)fetch<uint16_t>());
			break;
		}

		case op::s_allocz:
		{
			_stack.allocz((uint64_t)fetch<uint8_t>());
			break;
		}

		case op::l_dealloc:
		{
			_stack.dealloc((uint64_t)fetch<uint16_t>());
			break;
		}

		case op::s_dealloc:
		{
			_stack.dealloc((uint64_t)fetch<uint8_t>());
			break;
		}

		case op::push_flow:
		{
			_stack.push_ptr((uintptr_t)_opptr);
			break;
		}

		case op::push_frame:
		{
			_stack.store_frame();
			break;
		}

		case op::push_stack:
		{
			_stack.store_stack();
			break;
		}

		case op::push_state:
		{
			_stack.push((state_val(_state)).__value);
			break;
		}

		case op::pop_flow:
		{
			_opptr = (const uint8_t*)_stack.pop_ptr();
			break;
		}

		case op::pop_frame:
		{
			_stack.load_frame();
			break;
		}

		case op::pop_stack:
		{
			_stack.load_stack();
			break;
		}

		case op::pop_state:
		{
			_state = (state_val(_stack.pop<val16<>>())).__state;
			break;
		}

		case op::jmp_64:
		{
			__jmp<val64<>>();
			break;
		}

		case op::jmp_32:
		{
			__jmp<val32<>>();
			break;
		}

		case op::jmp_16:
		{
			__jmp<val16<>>();
			break;
		}

		case op::jmp_8:
		{
			__jmp<val8<>>();
			break;
		}

		case op::jmp_a_64:
		{
			__jmp_a<val64<>>();
			break;
		}

		case op::jmp_a_32:
		{
			__jmp_a<val32<>>();
			break;
		}

		case op::jmp_a_16:
		{
			__jmp_a<val16<>>();
			break;
		}

		case op::jmp_a_8:
		{
			__jmp_a<val8<>>();
			break;
		}

		case op::jmp_ae_64:
		{
			__jmp_ae<val64<>>();
			break;
		}

		case op::jmp_ae_32:
		{
			__jmp_ae<val32<>>();
			break;
		}

		case op::jmp_ae_16:
		{
			__jmp_ae<val16<>>();
			break;
		}

		case op::jmp_ae_8:
		{
			__jmp_ae<val8<>>();
			break;
		}

		case op::jmp_b_64:
		{
			__jmp_b<val64<>>();
			break;
		}

		case op::jmp_b_32:
		{
			__jmp_b<val32<>>();
			break;
		}

		case op::jmp_b_16:
		{
			__jmp_b<val16<>>();
			break;
		}

		case op::jmp_b_8:
		{
			__jmp_b<val8<>>();
			break;
		}

		case op::jmp_be_64:
		{
			__jmp_be<val64<>>();
			break;
		}

		case op::jmp_be_32:
		{
			__jmp_be<val32<>>();
			break;
		}

		case op::jmp_be_16:
		{
			__jmp_be<val16<>>();
			break;
		}

		case op::jmp_be_8:
		{
			__jmp_be<val8<>>();
			break;
		}

		case op::jmp_e_64:
		{
			__jmp_e<val64<>>();
			break;
		}

		case op::jmp_e_32:
		{
			__jmp_e<val32<>>();
			break;
		}

		case op::jmp_e_16:
		{
			__jmp_e<val16<>>();
			break;
		}

		case op::jmp_e_8:
		{
			__jmp_e<val8<>>();
			break;
		}

		case op::jmp_ne_64:
		{
			__jmp_ne<val64<>>();
			break;
		}

		case op::jmp_ne_32:
		{
			__jmp_ne<val32<>>();
			break;
		}

		case op::jmp_ne_16:
		{
			__jmp_ne<val16<>>();
			break;
		}

		case op::jmp_ne_8:
		{
			__jmp_ne<val8<>>();
			break;
		}

		case op::jmp_un_64:
		{
			__jmp_un<val64<>>();
			break;
		}

		case op::jmp_un_32:
		{
			__jmp_un<val32<>>();
			break;
		}

		case op::jmp_un_16:
		{
			__jmp_un<val16<>>();
			break;
		}

		case op::jmp_un_8:
		{
			__jmp_un<val8<>>();
			break;
		}

		case op::jmp_msk_64:
		{
			__jmp_msk<val64<>>();
			break;
		}

		case op::jmp_msk_32:
		{
			__jmp_msk<val32<>>();
			break;
		}

		case op::jmp_msk_16:
		{
			__jmp_msk<val16<>>();
			break;
		}

		case op::jmp_msk_8:
		{
			__jmp_msk<val8<>>();
			break;
		}

		case op::l_load_64:
		{
			__l_load<val64<>>();
			break;
		}

		case op::l_load_32:
		{
			__l_load<val32<>>();
			break;
		}

		case op::l_load_16:
		{
			__l_load<val16<>>();
			break;
		}

		case op::l_load_8:
		{
			__l_load<val8<>>();
			break;
		}

		case op::l_store_64:
		{
			__l_store<val64<>>();
			break;
		}

		case op::l_store_32:
		{
			__l_store<val32<>>();
			break;
		}

		case op::l_store_16:
		{
			__l_store<val16<>>();
			break;
		}

		case op::l_store_8:
		{
			__l_store<val8<>>();
			break;
		}

		case op::s_load_64:
		{
			__s_load<val64<>>();
			break;
		}

		case op::s_load_32:
		{
			__s_load<val32<>>();
			break;
		}

		case op::s_load_16:
		{
			__s_load<val16<>>();
			break;
		}

		case op::s_load_8:
		{
			__s_load<val8<>>();
			break;
		}

		case op::s_store_64:
		{
			__s_store<val64<>>();
			break;
		}

		case op::s_store_32:
		{
			__s_store<val32<>>();
			break;
		}

		case op::s_store_16:
		{
			__s_store<val16<>>();
			break;
		}

		case op::s_store_8:
		{
			__s_store<val8<>>();
			break;
		}

		case op::push_64:
		{
			__push<val64<>>();
			break;
		}

		case op::push_32:
		{
			__push<val32<>>();
			break;
		}

		case op::push_16:
		{
			__push<val16<>>();
			break;
		}

		case op::push_8:
		{
			__push<val8<>>();
			break;
		}

		case op::pop_64:
		{
			_stack.rem<val64<>>();
			break;
		}

		case op::pop_32:
		{
			_stack.rem<val32<>>();
			break;
		}

		case op::pop_16:
		{
			_stack.rem<val16<>>();
			break;
		}

		case op::pop_8:
		{
			_stack.rem<val8<>>();
			break;
		}

		case op::dup_64:
		{
			_stack.dup<val64<>>();
			break;
		}

		case op::dup_32:
		{
			_stack.dup<val32<>>();
			break;
		}

		case op::dup_16:
		{
			_stack.dup<val16<>>();
			break;
		}

		case op::dup_8:
		{
			_stack.dup<val8<>>();
			break;
		}

		case op::ptrcpy_64:
		{
			__ptrcpy<val64<>>();
			break;
		}

		case op::ptrcpy_32:
		{
			__ptrcpy<val32<>>();
			break;
		}

		case op::ptrcpy_16:
		{
			__ptrcpy<val16<>>();
			break;
		}

		case op::ptrcpy_8:
		{
			__ptrcpy<val8<>>();
			break;
		}

		case op::ptrget_64:
		{
			__ptrget<val64<>>();
			break;
		}

		case op::ptrget_32:
		{
			__ptrget<val32<>>();
			break;
		}

		case op::ptrget_16:
		{
			__ptrget<val16<>>();
			break;
		}

		case op::ptrget_8:
		{
			__ptrget<val8<>>();
			break;
		}

		case op::ptrset_64:
		{
			__ptrset<val64<>>();
			break;
		}

		case op::ptrset_32:
		{
			__ptrset<val32<>>();
			break;
		}

		case op::ptrset_16:
		{
			__ptrset<val16<>>();
			break;
		}

		case op::ptrset_8:
		{
			__ptrset<val8<>>();
			break;
		}

		case op::neg_64:
		{
			__neg<val64<>>();
			break;
		}

		case op::neg_32:
		{
			__neg<val32<>>();
			break;
		}

		case op::neg_16:
		{
			__neg<val16<>>();
			break;
		}

		case op::neg_8:
		{
			__neg<val8<>>();
			break;
		}

		case op::not_64:
		{
			__not<val64<>>();
			break;
		}

		case op::not_32:
		{
			__not<val32<>>();
			break;
		}

		case op::not_16:
		{
			__not<val16<>>();
			break;
		}

		case op::not_8:
		{
			__not<val8<>>();
			break;
		}

		case op::sinc_64:
		{
			__sinc<val64<>>();
			break;
		}

		case op::sinc_32:
		{
			__sinc<val32<>>();
			break;
		}

		case op::sinc_16:
		{
			__sinc<val16<>>();
			break;
		}

		case op::sinc_8:
		{
			__sinc<val8<>>();
			break;
		}

		case op::uinc_64:
		{
			__uinc<val64<>>();
			break;
		}

		case op::uinc_32:
		{
			__uinc<val32<>>();
			break;
		}

		case op::uinc_16:
		{
			__uinc<val16<>>();
			break;
		}

		case op::uinc_8:
		{
			__uinc<val8<>>();
			break;
		}

		case op::sdec_64:
		{
			__sdec<val64<>>();
			break;
		}

		case op::sdec_32:
		{
			__sdec<val32<>>();
			break;
		}

		case op::sdec_16:
		{
			__sdec<val16<>>();
			break;
		}

		case op::sdec_8:
		{
			__sdec<val8<>>();
			break;
		}

		case op::udec_64:
		{
			__udec<val64<>>();
			break;
		}

		case op::udec_32:
		{
			__udec<val32<>>();
			break;
		}

		case op::udec_16:
		{
			__udec<val16<>>();
			break;
		}

		case op::udec_8:
		{
			__udec<val8<>>();
			break;
		}

		case op::and_64:
		{
			__and<val64<>>();
			break;
		}

		case op::and_32:
		{
			__and<val32<>>();
			break;
		}

		case op::and_16:
		{
			__and<val16<>>();
			break;
		}

		case op::and_8:
		{
			__and<val8<>>();
			break;
		}

		case op::or_64:
		{
			__or<val64<>>();
			break;
		}

		case op::or_32:
		{
			__or<val32<>>();
			break;
		}

		case op::or_16:
		{
			__or<val16<>>();
			break;
		}

		case op::or_8:
		{
			__or<val8<>>();
			break;
		}

		case op::xor_64:
		{
			__xor<val64<>>();
			break;
		}

		case op::xor_32:
		{
			__xor<val32<>>();
			break;
		}

		case op::xor_16:
		{
			__xor<val16<>>();
			break;
		}

		case op::xor_8:
		{
			__xor<val8<>>();
			break;
		}

		case op::sadd_64:
		{
			__sadd<val64<>>();
			break;
		}

		case op::sadd_32:
		{
			__sadd<val32<>>();
			break;
		}

		case op::sadd_16:
		{
			__sadd<val16<>>();
			break;
		}

		case op::sadd_8:
		{
			__sadd<val8<>>();
			break;
		}

		case op::uadd_64:
		{
			__uadd<val64<>>();
			break;
		}

		case op::uadd_32:
		{
			__uadd<val32<>>();
			break;
		}

		case op::uadd_16:
		{
			__uadd<val16<>>();
			break;
		}

		case op::uadd_8:
		{
			__uadd<val8<>>();
			break;
		}

		case op::ssub_64:
		{
			__ssub<val64<>>();
			break;
		}

		case op::ssub_32:
		{
			__ssub<val32<>>();
			break;
		}

		case op::ssub_16:
		{
			__ssub<val16<>>();
			break;
		}

		case op::ssub_8:
		{
			__ssub<val8<>>();
			break;
		}

		case op::usub_64:
		{
			__usub<val64<>>();
			break;
		}

		case op::usub_32:
		{
			__usub<val32<>>();
			break;
		}

		case op::usub_16:
		{
			__usub<val16<>>();
			break;
		}

		case op::usub_8:
		{
			__usub<val8<>>();
			break;
		}

		case op::smul_64:
		{
			__smul<val64<>>();
			break;
		}

		case op::smul_32:
		{
			__smul<val32<>>();
			break;
		}

		case op::smul_16:
		{
			__smul<val16<>>();
			break;
		}

		case op::smul_8:
		{
			__smul<val8<>>();
			break;
		}

		case op::umul_64:
		{
			__umul<val64<>>();
			break;
		}

		case op::umul_32:
		{
			__umul<val32<>>();
			break;
		}

		case op::umul_16:
		{
			__umul<val16<>>();
			break;
		}

		case op::umul_8:
		{
			__umul<val8<>>();
			break;
		}

		case op::smod_64:
		{
			__smod<val64<>>();
			break;
		}

		case op::smod_32:
		{
			__smod<val32<>>();
			break;
		}

		case op::smod_16:
		{
			__smod<val16<>>();
			break;
		}

		case op::smod_8:
		{
			__smod<val8<>>();
			break;
		}

		case op::umod_64:
		{
			__umod<val64<>>();
			break;
		}

		case op::umod_32:
		{
			__umod<val32<>>();
			break;
		}

		case op::umod_16:
		{
			__umod<val16<>>();
			break;
		}

		case op::umod_8:
		{
			__umod<val8<>>();
			break;
		}

		case op::sdiv_64:
		{
			__sdiv<val64<>>();
			break;
		}

		case op::sdiv_32:
		{
			__sdiv<val32<>>();
			break;
		}

		case op::sdiv_16:
		{
			__sdiv<val16<>>();
			break;
		}

		case op::sdiv_8:
		{
			__sdiv<val8<>>();
			break;
		}

		case op::udiv_64:
		{
			__udiv<val64<>>();
			break;
		}

		case op::udiv_32:
		{
			__udiv<val32<>>();
			break;
		}

		case op::udiv_16:
		{
			__udiv<val16<>>();
			break;
		}

		case op::udiv_8:
		{
			__udiv<val8<>>();
			break;
		}

		case op::scmp_64:
		{
			__scmp<val64<>>();
			break;
		}

		case op::scmp_32:
		{
			__scmp<val32<>>();
			break;
		}

		case op::scmp_16:
		{
			__scmp<val16<>>();
			break;
		}

		case op::scmp_8:
		{
			__scmp<val8<>>();
			break;
		}

		case op::ucmp_64:
		{
			__ucmp<val64<>>();
			break;
		}

		case op::ucmp_32:
		{
			__ucmp<val32<>>();
			break;
		}

		case op::ucmp_16:
		{
			__ucmp<val16<>>();
			break;
		}

		case op::ucmp_8:
		{
			__ucmp<val8<>>();
			break;
		}

		case op::rotl_64:
		{
			__rotl<val64<>>();
			break;
		}

		case op::rotl_32:
		{
			__rotl<val32<>>();
			break;
		}

		case op::rotl_16:
		{
			__rotl<val16<>>();
			break;
		}

		case op::rotl_8:
		{
			__rotl<val8<>>();
			break;
		}

		case op::rotr_64:
		{
			__rotr<val64<>>();
			break;
		}

		case op::rotr_32:
		{
			__rotr<val32<>>();
			break;
		}

		case op::rotr_16:
		{
			__rotr<val16<>>();
			break;
		}

		case op::rotr_8:
		{
			__rotr<val8<>>();
			break;
		}

		case op::sshl_64:
		{
			__sshl<val64<>>();
			break;
		}

		case op::sshl_32:
		{
			__sshl<val32<>>();
			break;
		}

		case op::sshl_16:
		{
			__sshl<val16<>>();
			break;
		}

		case op::sshl_8:
		{
			__sshl<val8<>>();
			break;
		}

		case op::ushl_64:
		{
			__ushl<val64<>>();
			break;
		}

		case op::ushl_32:
		{
			__ushl<val32<>>();
			break;
		}

		case op::ushl_16:
		{
			__ushl<val16<>>();
			break;
		}

		case op::ushl_8:
		{
			__ushl<val8<>>();
			break;
		}

		case op::sshr_64:
		{
			__sshr<val64<>>();
			break;
		}

		case op::sshr_32:
		{
			__sshr<val32<>>();
			break;
		}

		case op::sshr_16:
		{
			__sshr<val16<>>();
			break;
		}

		case op::sshr_8:
		{
			__sshr<val8<>>();
			break;
		}

		case op::ushr_64:
		{
			__ushr<val64<>>();
			break;
		}

		case op::ushr_32:
		{
			__ushr<val32<>>();
			break;
		}

		case op::ushr_16:
		{
			__ushr<val16<>>();
			break;
		}

		case op::ushr_8:
		{
			__ushr<val8<>>();
			break;
		}

		case op::fneg_64:
		{
			__fneg<val64<>>();
			break;
		}

		case op::fneg_32:
		{
			__fneg<val32<>>();
			break;
		}

		case op::fevl_64:
		{
			__fevl<val64<>>();
			break;
		}

		case op::fevl_32:
		{
			__fevl<val32<>>();
			break;
		}

		case op::fadd_64:
		{
			__fadd<val64<>>();
			break;
		}

		case op::fadd_32:
		{
			__fadd<val32<>>();
			break;
		}

		case op::fsub_64:
		{
			__fsub<val64<>>();
			break;
		}

		case op::fsub_32:
		{
			__fsub<val32<>>();
			break;
		}

		case op::fmul_64:
		{
			__fmul<val64<>>();
			break;
		}

		case op::fmul_32:
		{
			__fmul<val32<>>();
			break;
		}

		case op::fmod_64:
		{
			__fmod<val64<>>();
			break;
		}

		case op::fmod_32:
		{
			__fmod<val32<>>();
			break;
		}

		case op::fdiv_64:
		{
			__fdiv<val64<>>();
			break;
		}

		case op::fdiv_32:
		{
			__fdiv<val32<>>();
			break;
		}

		case op::fcmp_64:
		{
			__fcmp<val64<>>();
			break;
		}

		case op::fcmp_32:
		{
			__fcmp<val32<>>();
			break;
		}

		case op::si64_2_f64:
		{
			val64<>& v = _stack.top<val64<>>();
			int64_t si = v.si;

			v.f = (double)si;

			break;
		}

		case op::si64_2_f32:
		{
			val64<>& v = _stack.top<val64<>>();
			int64_t si = v.si;

			_stack.dealloc((uint64_t)sizeof(val32<>));

			v.half.hi.f = (float)si;

			break;
		}

		case op::si32_2_f64:
		{
			_stack.alloc((uint64_t)sizeof(val32<>));

			val64<>& v = _stack.top<val64<>>();
			int32_t si = v.half.hi.si;

			v.f = (double)si;

			break;
		}

		case op::si32_2_f32:
		{
			val32<>& v = _stack.top<val32<>>();
			int32_t si = v.si;

			v.f = (float)si;

			break;
		}

		case op::ui64_2_f64:
		{
			val64<>& v = _stack.top<val64<>>();
			uint64_t ui = v.ui;

			v.f = (double)ui;

			break;
		}

		case op::ui64_2_f32:
		{
			val64<>& v = _stack.top<val64<>>();
			uint64_t ui = v.ui;

			_stack.dealloc((uint64_t)sizeof(val32<>));

			v.half.hi.f = (float)ui;

			break;
		}

		case op::ui32_2_f64:
		{
			_stack.alloc((uint64_t)sizeof(val32<>));

			val64<>& v = _stack.top<val64<>>();
			uint32_t ui = v.half.hi.ui;

			v.f = (double)ui;

			break;
		}

		case op::ui32_2_f32:
		{
			val32<>& v = _stack.top<val32<>>();
			uint32_t ui = v.ui;

			v.f = (float)ui;

			break;
		}

		case op::f64_2_si64:
		{
			val64<>& v = _stack.top<val64<>>();
			double f = v.f;

			bool ovf = !(f < (double)__2_POW_63);
			bool unf = !(f >= -(double)__2_POW_63);

			_state.eval.bits.ierr = ovf && unf;
			_state.eval.bits.iovf = ovf && !unf;
			_state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.si = (int64_t)f;

			break;
		}

		case op::f64_2_si32:
		{
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

			break;
		}

		case op::f32_2_si64:
		{
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

			break;
		}

		case op::f32_2_si32:
		{
			val32<>& v = _stack.top<val32<>>();
			float f = v.f;

			bool ovf = !(f < (float)__2_POW_31);
			bool unf = !(f >= -(float)__2_POW_31);

			_state.eval.bits.ierr = ovf && unf;
			_state.eval.bits.iovf = ovf && !unf;
			_state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.si = (int32_t)f;

			break;
		}

		case op::f64_2_ui64:
		{
			val64<>& v = _stack.top<val64<>>();
			double f = v.f;

			bool ovf = !(f < (double)__2_POW_64);
			bool unf = !(f >= (double)__0);

			_state.eval.bits.ierr = ovf && unf;
			_state.eval.bits.iovf = ovf && !unf;
			_state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.ui = (uint64_t)f;

			break;
		}

		case op::f64_2_ui32:
		{
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

			break;
		}

		case op::f32_2_ui64:
		{
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

			break;
		}

		case op::f32_2_ui32:
		{
			val32<>& v = _stack.top<val32<>>();
			float f = v.f;

			bool ovf = !(f < (float)__2_POW_32);
			bool unf = !(f >= (float)__0);

			_state.eval.bits.ierr = ovf && unf;
			_state.eval.bits.iovf = ovf && !unf;
			_state.eval.bits.iunf = !ovf && unf;

			if (!ovf && !unf)
				v.ui = (uint32_t)f;

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