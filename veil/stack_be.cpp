#include "stack_be.hpp"

using namespace interpreter;
using namespace std;

inline ptrdiff_t stack<endian::big>::__top_offset() noexcept
{
	return (ptrdiff_t)(_send - _stop);
}

inline ptrdiff_t stack<endian::big>::__bot_offset() noexcept
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
	if ((ptrdiff_t)(sizeof(uint64_t) * 2ui64) > __top_offset())
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
	if ((ptrdiff_t)0 != __bot_offset())
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
	if ((ptrdiff_t)sizeof(uint64_t) > __top_offset())
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
	if ((ptrdiff_t)sizeof(uint64_t) > __bot_offset())
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
	if ((ptrdiff_t)sizeof(uint64_t) > __bot_offset())
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
	if ((ptrdiff_t)sizeof(uint64_t) > __bot_offset())
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
	if ((ptrdiff_t)sizeof(uint64_t) > __top_offset())
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
	if ((ptrdiff_t)sizeof(uint64_t) > __top_offset())
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
		if (size > __top_offset())
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
		if (size > __top_offset())
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
		if (size > __bot_offset())
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