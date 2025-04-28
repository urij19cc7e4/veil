#include "interpreter.hpp"

using namespace interpreter;
using namespace std;

inline void ::interpreter::interpreter::__def_obj() noexcept
{
	_code = (const uint8_t*)nullptr;
}

inline void ::interpreter::interpreter::__del_obj() noexcept
{
	if (_code != (const uint8_t*)nullptr)
		delete[] _code;
}

inline void ::interpreter::interpreter::__mov_obj(interpreter&& o) noexcept
{
	_code = move(o._code);
	_disp = move(o._disp);
}

::interpreter::interpreter::interpreter(const uint8_t* code, uint64_t code_size, uint64_t cache_size, uint64_t stack_size, endian endianness)
{
	if (code_size == 0ui64)
		throw runtime_error(_err_msg_zero_size);
	else
	{
		try
		{
			switch (endianness)
			{
			case endian::big:
			case endian::little:
			{
				uint8_t* code_copy = (uint8_t*)new uint8_t * [code_size];
				memcpy(code_copy, code, (uint64_t)sizeof(uint8_t) * code_size);

				if (endianness != endian::native)
					dispatcher::rev_endian(code_copy, code_size);

				_code = (const uint8_t*)code_copy;

				break;
			}

			default:
				throw runtime_error(_err_msg_wrong_end);
			}

		#ifdef FETCHCHECK
			_disp.emplace(_code, code_size, cache_size, stack_size);
		#else
			_disp.emplace(_code, cache_size, stack_size);
		#endif
		}
		catch (const exception& e)
		{
			__del_obj();
			__def_obj();

			throw e;
		}
	}
}

::interpreter::interpreter::interpreter(interpreter&& o) noexcept
{
	__mov_obj(move(o));
	o.__def_obj();
}

::interpreter::interpreter::~interpreter() noexcept
{
	__del_obj();
	__def_obj();
}

void ::interpreter::interpreter::execute(const vtype* atype, const void** aval, uint64_t count, vtype rtype, void* rval)
{
	switch (rtype)
	{
	case vtype::__void: break;
	case vtype::__val64: push(val64<>()); break;
	case vtype::__val32: push(val32<>()); break;
	case vtype::__val16: push(val16<>()); break;
	case vtype::__val8: push(val8<>()); break;
	case vtype::__ptr: push(uintptr_t()); break;
	default: throw runtime_error(_err_msg_wrong_argt);
	}

	if (count != 0ui64)
		for (uint64_t i = count - 1ui64;;)
		{
			switch (atype[i])
			{
			case vtype::__void: throw runtime_error(_err_msg_argt_void);
			case vtype::__val64: push(*((const val64<>*)(aval[i]))); break;
			case vtype::__val32: push(*((const val32<>*)(aval[i]))); break;
			case vtype::__val16: push(*((const val16<>*)(aval[i]))); break;
			case vtype::__val8: push(*((const val8<>*)(aval[i]))); break;
			case vtype::__ptr: push(*((const uintptr_t*)(aval[i]))); break;
			default: throw runtime_error(_err_msg_wrong_argt);
			}

			if (i == 0ui64)
				break;
			else
				--i;
		}

	_disp.value().init();
	_disp.value().loop();
	_disp.value().exit();

	for (uint64_t i = 0ui64; i < count; ++i)
		switch (atype[i])
		{
		case vtype::__void: throw runtime_error(_err_msg_argt_void);
		case vtype::__val64: pop<val64<>>(); break;
		case vtype::__val32: pop<val32<>>(); break;
		case vtype::__val16: pop<val16<>>(); break;
		case vtype::__val8: pop<val8<>>(); break;
		case vtype::__ptr: pop<uintptr_t>(); break;
		default: throw runtime_error(_err_msg_wrong_argt);
		}

	switch (rtype)
	{
	case vtype::__void: break;
	case vtype::__val64: *((val64<>*)rval) = pop<val64<>>(); break;
	case vtype::__val32: *((val32<>*)rval) = pop<val32<>>(); break;
	case vtype::__val16: *((val16<>*)rval) = pop<val16<>>(); break;
	case vtype::__val8: *((val8<>*)rval) = pop<val8<>>(); break;
	case vtype::__ptr: *((uintptr_t*)rval) = pop<uintptr_t>(); break;
	default: throw runtime_error(_err_msg_wrong_argt);
	}
}

::interpreter::interpreter& ::interpreter::interpreter::operator=(interpreter&& o) noexcept
{
	__del_obj();
	__mov_obj(move(o));
	o.__def_obj();

	return *this;
}