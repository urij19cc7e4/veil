#include "stack_le.hpp"

using namespace interpreter;
using namespace std;

inline void stack<endian::little>::__dil_push_ptr(uintptr_t& stop, uintptr_t value)
{
	stop -= (ptrdiff_t)sizeof(uint64_t);
	*((uintptr_t*)stop) = value;
}

inline uintptr_t stack<endian::little>::__dil_pop_ptr(uintptr_t& stop)
{
	uintptr_t value = *((uintptr_t*)stop);
	stop += (ptrdiff_t)sizeof(uint64_t);

	return value;
}

inline ptrdiff_t stack<endian::little>::__top_off() noexcept
{
	return (ptrdiff_t)(_stop - _send);
}

inline ptrdiff_t stack<endian::little>::__bot_off() noexcept
{
	return (ptrdiff_t)(_ftop - _stop);
}

inline void stack<endian::little>::__push_ptr(uintptr_t value)
{
	_stop -= (ptrdiff_t)sizeof(uint64_t);
	*((uintptr_t*)_stop) = value;
}

inline uintptr_t stack<endian::little>::__pop_ptr()
{
	uintptr_t value = *((uintptr_t*)_stop);
	_stop += (ptrdiff_t)sizeof(uint64_t);

	return value;
}

inline void stack<endian::little>::dil_push_frame(uintptr_t& ftop, uintptr_t& stop, uintptr_t value)
{
	__dil_push_ptr(stop, value);
	__dil_push_ptr(stop, ftop);
	ftop = stop;
}

inline uintptr_t stack<endian::little>::dil_pop_frame(uintptr_t& ftop, uintptr_t& stop)
{
	stop = ftop;
	ftop = __dil_pop_ptr(stop);
	return __dil_pop_ptr(stop);
}

inline void stack<endian::little>::dil_push_ptr(uintptr_t& stop, uintptr_t value)
{
	__dil_push_ptr(stop, value);
}

inline uintptr_t stack<endian::little>::dil_pop_ptr(uintptr_t& stop)
{
	return __dil_pop_ptr(stop);
}

inline void stack<endian::little>::dil_load_frame(uintptr_t& ftop, uintptr_t& stop)
{
	ftop = __dil_pop_ptr(stop);
}

inline void stack<endian::little>::dil_load_stack(uintptr_t& stop)
{
	stop = __dil_pop_ptr(stop);
}

inline void stack<endian::little>::dil_store_frame(const uintptr_t& ftop, uintptr_t& stop)
{
	__dil_push_ptr(stop, ftop);
}

inline void stack<endian::little>::dil_store_stack(uintptr_t& stop)
{
	__dil_push_ptr(stop, stop);
}

inline void stack<endian::little>::dil_alloc(uintptr_t& stop, uint64_t size)
{
	if (size != 0ui64)
		stop -= (ptrdiff_t)size;
}

inline void stack<endian::little>::dil_allocz(uintptr_t& stop, uint64_t size)
{
	if (size != 0ui64)
	{
		stop -= (ptrdiff_t)size;
		memset((void*)stop, (int)0ui8, (size_t)size);
	}
}

inline void stack<endian::little>::dil_dealloc(uintptr_t& stop, uint64_t size)
{
	if (size != 0ui64)
		stop += (ptrdiff_t)size;
}

stack<endian::little>::stack(uint64_t size) : stack_base(size)
{
	_sbeg = (uintptr_t)((uintptr_t)_data + (ptrdiff_t)_size);
	_send = (uintptr_t)_data;

	_ftop = _sbeg;
	_stop = _sbeg;
}

stack<endian::little>::stack(stack&& o) noexcept : stack_base(move(o)) {}

void stack<endian::little>::push_frame(uintptr_t value)
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

uintptr_t stack<endian::little>::pop_frame()
{
#ifdef STACKCHECK
	if (_stop > *((uintptr_t*)_ftop))
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

void stack<endian::little>::push_ptr(uintptr_t value)
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

uintptr_t stack<endian::little>::pop_ptr()
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

void stack<endian::little>::load_frame()
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

void stack<endian::little>::load_stack()
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

void stack<endian::little>::store_frame()
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

void stack<endian::little>::store_stack()
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

void stack<endian::little>::alloc(uint64_t size)
{
	if (size != 0ui64)
	{
	#ifdef STACKCHECK
		if (size > __top_off())
			throw runtime_error(_err_msg_stack_ovf);
		else
			_stop -= (ptrdiff_t)size;
	#else
		_stop -= (ptrdiff_t)size;
	#endif
	}
}

void stack<endian::little>::allocz(uint64_t size)
{
	if (size != 0ui64)
	{
	#ifdef STACKCHECK
		if (size > __top_off())
			throw runtime_error(_err_msg_stack_ovf);
		else
		{
			_stop -= (ptrdiff_t)size;
			memset((void*)_stop, (int)0ui8, (size_t)size);
		}
	#else
		_stop -= (ptrdiff_t)size;
		memset((void*)_stop, (int)0ui8, (size_t)size);
	#endif
	}
}

void stack<endian::little>::dealloc(uint64_t size)
{
	if (size != 0ui64)
	{
	#ifdef STACKCHECK
		if (size > __bot_off())
			throw runtime_error(_err_msg_stack_unf);
		else
			_stop += (ptrdiff_t)size;
	#else
		_stop += (ptrdiff_t)size;
	#endif
	}
}

stack<endian::little>& stack<endian::little>::operator=(stack&& o) noexcept
{
	return (stack&)stack_base::operator=(move(o));
}