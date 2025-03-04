#include "stack_le.hpp"

using namespace interpreter;
using namespace std;

inline ptrdiff_t stack<endian::little>::__top_offset() noexcept
{
	return (ptrdiff_t)(_stop - _send);
}

inline ptrdiff_t stack<endian::little>::__bot_offset() noexcept
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

uintptr_t stack<endian::little>::pop_frame()
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

void stack<endian::little>::push_ptr(uintptr_t value)
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

uintptr_t stack<endian::little>::pop_ptr()
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

void stack<endian::little>::load_frame()
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

void stack<endian::little>::load_stack()
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

void stack<endian::little>::store_frame()
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

void stack<endian::little>::store_stack()
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

void stack<endian::little>::alloc(uint64_t size)
{
	if (size != 0ui64)
	{
	#ifdef STACKCHECK
		if (size > __top_offset())
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
		if (size > __top_offset())
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
		if (size > __bot_offset())
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