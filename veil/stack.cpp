#include "stack.hpp"

using namespace interpreter;
using namespace std;

inline void stack_base::__def_obj() noexcept
{
	_data = (uint8_t*)nullptr;
	_size = 0ui64;

	_sbeg = (uintptr_t)nullptr;
	_send = (uintptr_t)nullptr;

	_ftop = (uintptr_t)nullptr;
	_stop = (uintptr_t)nullptr;
}

inline void stack_base::__del_obj() noexcept
{
	if (_data != (uint8_t*)nullptr)
		delete[] _data;
}

inline void stack_base::__mov_obj(stack_base&& o) noexcept
{
	_data = o._data;
	_size = o._size;

	_sbeg = o._sbeg;
	_send = o._send;

	_ftop = o._ftop;
	_stop = o._stop;
}

stack_base::stack_base(uint64_t size)
{
	if (size == 0ui64)
		throw runtime_error(_err_msg_zero_size);
	else
	{
		_data = (uint8_t*)new uint8_t[size];
		_size = size;
	}
}

stack_base::stack_base(stack_base&& o) noexcept
{
	__mov_obj(move(o));
	o.__def_obj();
}

stack_base::~stack_base() noexcept
{
	__del_obj();
	__def_obj();
}

stack_base& stack_base::operator=(stack_base&& o) noexcept
{
	__del_obj();
	__mov_obj(move(o));
	o.__def_obj();

	return *this;
}