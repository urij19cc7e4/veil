#include "stack_be.hpp"

using namespace interpreter;
using namespace std;

inline void stack<endian::big>::__dil_push_ptr(uintptr_t& stop, uintptr_t value)
{
	*((uintptr_t*)stop) = value;
	stop += (ptrdiff_t)sizeof(uint64_t);
}

inline uintptr_t stack<endian::big>::__dil_pop_ptr(uintptr_t& stop)
{
	stop -= (ptrdiff_t)sizeof(uint64_t);
	uintptr_t value = *((uintptr_t*)stop);

	return value;
}

inline ptrdiff_t stack<endian::big>::__top_off() noexcept
{
	return (ptrdiff_t)(_send - _stop);
}

inline ptrdiff_t stack<endian::big>::__bot_off() noexcept
{
	return (ptrdiff_t)(_stop - _ftop);
}

inline void stack<endian::big>::__push_ptr(uintptr_t value)
{
	*((uintptr_t*)_stop) = value;
	_stop += (ptrdiff_t)sizeof(uint64_t);
}

inline uintptr_t stack<endian::big>::__pop_ptr()
{
	_stop -= (ptrdiff_t)sizeof(uint64_t);
	uintptr_t value = *((uintptr_t*)_stop);

	return value;
}

inline void stack<endian::big>::dil_push_frame(uintptr_t& ftop, uintptr_t& stop, uintptr_t value)
{
	__dil_push_ptr(stop, value);
	__dil_push_ptr(stop, ftop);
	ftop = stop;
}

inline uintptr_t stack<endian::big>::dil_pop_frame(uintptr_t& ftop, uintptr_t& stop)
{
	stop = ftop;
	ftop = __dil_pop_ptr(stop);
	return __dil_pop_ptr(stop);
}

inline void stack<endian::big>::dil_push_ptr(uintptr_t& stop, uintptr_t value)
{
	__dil_push_ptr(stop, value);
}

inline uintptr_t stack<endian::big>::dil_pop_ptr(uintptr_t& stop)
{
	return __dil_pop_ptr(stop);
}

inline void stack<endian::big>::dil_load_frame(uintptr_t& ftop, uintptr_t& stop)
{
	ftop = __dil_pop_ptr(stop);
}

inline void stack<endian::big>::dil_load_stack(uintptr_t& stop)
{
	stop = __dil_pop_ptr(stop);
}

inline void stack<endian::big>::dil_store_frame(const uintptr_t& ftop, uintptr_t& stop)
{
	__dil_push_ptr(stop, ftop);
}

inline void stack<endian::big>::dil_store_stack(uintptr_t& stop)
{
	__dil_push_ptr(stop, stop);
}

inline void stack<endian::big>::dil_alloc(uintptr_t& stop, uint64_t size)
{
	if (size != 0ui64)
		stop += (ptrdiff_t)size;
}

inline void stack<endian::big>::dil_allocz(uintptr_t& stop, uint64_t size)
{
	if (size != 0ui64)
	{
		memset((void*)stop, (int)0ui8, (size_t)size);
		stop += (ptrdiff_t)size;
	}
}

inline void stack<endian::big>::dil_dealloc(uintptr_t& stop, uint64_t size)
{
	if (size != 0ui64)
		stop -= (ptrdiff_t)size;
}

stack<endian::big>::stack(uint64_t size) : stack_base(size)
{
	_sbeg = (uintptr_t)_data;
	_send = (uintptr_t)((uintptr_t)_data + (ptrdiff_t)_size);

	_ftop = _sbeg;
	_stop = _sbeg;
}

stack<endian::big>::stack(stack&& o) noexcept : stack_base(move(o)) {}

void stack<endian::big>::push_frame(uintptr_t value)
{
#ifdef STACKCHECK
	if ((ptrdiff_t)(sizeof(uint64_t) * 2ui64) > __top_off())
		throw runtime_error(_err_msg_stack_ovf);
	else
	{
		__push_ptr(value);
		__push_ptr(_ftop);
		_ftop = _stop;
	}
#else
	__push_ptr(value);
	__push_ptr(_ftop);
	_ftop = _stop;
#endif
}

uintptr_t stack<endian::big>::pop_frame()
{
#ifdef STACKCHECK
	if (_stop < *((uintptr_t*)(_ftop - (ptrdiff_t)sizeof(uintptr_t))))
		throw runtime_error(_err_msg_stack_unf);
	else
	{
		_stop = _ftop;
		_ftop = __pop_ptr();
		return __pop_ptr();
	}
#else
	_stop = _ftop;
	_ftop = __pop_ptr();
	return __pop_ptr();
#endif
}

void stack<endian::big>::push_ptr(uintptr_t value)
{
#ifdef STACKCHECK
	if ((ptrdiff_t)sizeof(uint64_t) > __top_off())
		throw runtime_error(_err_msg_stack_ovf);
	else
		__push_ptr(value);
#else
	__push_ptr(value);
#endif
}

uintptr_t stack<endian::big>::pop_ptr()
{
#ifdef STACKCHECK
	if ((ptrdiff_t)sizeof(uint64_t) > __bot_off())
		throw runtime_error(_err_msg_stack_unf);
	else
		return __pop_ptr();
#else
	return __pop_ptr();
#endif
}

void stack<endian::big>::load_frame()
{
#ifdef STACKCHECK
	if ((ptrdiff_t)sizeof(uint64_t) > __bot_off())
		throw runtime_error(_err_msg_stack_unf);
	else
		_ftop = __pop_ptr();
#else
	_ftop = __pop_ptr();
#endif
}

void stack<endian::big>::load_stack()
{
#ifdef STACKCHECK
	if ((ptrdiff_t)sizeof(uint64_t) > __bot_off())
		throw runtime_error(_err_msg_stack_unf);
	else
		_stop = __pop_ptr();
#else
	_stop = __pop_ptr();
#endif
}

void stack<endian::big>::store_frame()
{
#ifdef STACKCHECK
	if ((ptrdiff_t)sizeof(uint64_t) > __top_off())
		throw runtime_error(_err_msg_stack_ovf);
	else
		__push_ptr(_ftop);
#else
	__push_ptr(_ftop);
#endif
}

void stack<endian::big>::store_stack()
{
#ifdef STACKCHECK
	if ((ptrdiff_t)sizeof(uint64_t) > __top_off())
		throw runtime_error(_err_msg_stack_ovf);
	else
		__push_ptr(_stop);
#else
	__push_ptr(_stop);
#endif
}

void stack<endian::big>::alloc(uint64_t size)
{
	if (size != 0ui64)
	{
	#ifdef STACKCHECK
		if (size > __top_off())
			throw runtime_error(_err_msg_stack_ovf);
		else
			_stop += (ptrdiff_t)size;
	#else
		_stop += (ptrdiff_t)size;
	#endif
	}
}

void stack<endian::big>::allocz(uint64_t size)
{
	if (size != 0ui64)
	{
	#ifdef STACKCHECK
		if (size > __top_off())
			throw runtime_error(_err_msg_stack_ovf);
		else
		{
			memset((void*)_stop, (int)0ui8, (size_t)size);
			_stop += (ptrdiff_t)size;
		}
	#else
		memset((void*)_stop, (int)0ui8, (size_t)size);
		_stop += (ptrdiff_t)size;
	#endif
	}
}

void stack<endian::big>::dealloc(uint64_t size)
{
	if (size != 0ui64)
	{
	#ifdef STACKCHECK
		if (size > __bot_off())
			throw runtime_error(_err_msg_stack_unf);
		else
			_stop -= (ptrdiff_t)size;
	#else
		_stop -= (ptrdiff_t)size;
	#endif
	}
}

stack<endian::big>& stack<endian::big>::operator=(stack&& o) noexcept
{
	return (stack&)stack_base::operator=(move(o));
}