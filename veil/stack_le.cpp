#include "stack_le.hpp"

using namespace interpreter;
using namespace std;

inline ptrdiff_t stack<std::endian::little>::__top_offset() noexcept
{
	return (ptrdiff_t)(_stop - _send);
}

inline ptrdiff_t stack<std::endian::little>::__bot_offset() noexcept
{
	return (ptrdiff_t)(_ftop - _stop);
}

inline void stack<std::endian::little>::__push_ptr(const uint8_t* value)
{
	_stop -= (ptrdiff_t)sizeof(uint64_t);
	*((const uint8_t**)_stop) = value;
}

inline uint8_t* stack<std::endian::little>::__pop_ptr()
{
	uint8_t* value = *((uint8_t**)_stop);
	_stop += (ptrdiff_t)sizeof(uint64_t);

	return value;
}

stack<std::endian::little>::stack(uint64_t size) : stack_base(size)
{
	_sbeg = _data + _size;
	_send = _data;

	_ftop = _sbeg;
	_stop = _sbeg;
}

void stack<std::endian::little>::push_frame(const uint8_t* value)
{
	if ((ptrdiff_t)(sizeof(uint64_t) * 2ui64) > __top_offset())
		throw std::runtime_error(_err_msg_stack_ovf);
	else
	{
		__push_ptr(value);
		__push_ptr(_ftop);
		_ftop = _stop;
	}
}

uint8_t* stack<std::endian::little>::pop_frame()
{
	if ((ptrdiff_t)0 != __bot_offset())
		throw std::runtime_error(_err_msg_stack_unf);
	{
		_stop = _ftop;
		_ftop = __pop_ptr();
		return __pop_ptr();
	}
}

void stack<std::endian::little>::push_ptr(const uint8_t* value)
{
	if ((ptrdiff_t)sizeof(uint64_t) > __top_offset())
		throw std::runtime_error(_err_msg_stack_ovf);
	else
		__push_ptr(value);
}

uint8_t* stack<std::endian::little>::pop_ptr()
{
	if ((ptrdiff_t)sizeof(uint64_t) > __bot_offset())
		throw std::runtime_error(_err_msg_stack_unf);
	else
		return __pop_ptr();
}

void stack<std::endian::little>::load_frame()
{
	if ((ptrdiff_t)sizeof(uint64_t) > __bot_offset())
		throw std::runtime_error(_err_msg_stack_unf);
	else
		_ftop = __pop_ptr();
}

void stack<std::endian::little>::load_stack()
{
	if ((ptrdiff_t)sizeof(uint64_t) > __bot_offset())
		throw std::runtime_error(_err_msg_stack_unf);
	else
		_stop = __pop_ptr();
}

void stack<std::endian::little>::store_frame()
{
	if ((ptrdiff_t)sizeof(uint64_t) > __top_offset())
		throw std::runtime_error(_err_msg_stack_ovf);
	else
		__push_ptr(_ftop);
}

void stack<std::endian::little>::store_stack()
{
	if ((ptrdiff_t)sizeof(uint64_t) > __top_offset())
		throw std::runtime_error(_err_msg_stack_ovf);
	else
		__push_ptr(_stop);
}

void stack<std::endian::little>::alloc(uint64_t size)
{
	if (size != 0ui64)
	{
		if (size > __top_offset())
			throw std::runtime_error(_err_msg_stack_ovf);
		else
			_stop -= size;
	}
}

void stack<std::endian::little>::allocz(uint64_t size)
{
	if (size != 0ui64)
	{
		if (size > __top_offset())
			throw std::runtime_error(_err_msg_stack_ovf);
		else
		{
			_stop -= size;
			memset((void*)_stop, (int)0ui8, (size_t)size);
		}
	}
}

void stack<std::endian::little>::dealloc(uint64_t size)
{
	if (size != 0ui64)
	{
		if (size > __bot_offset())
			throw std::runtime_error(_err_msg_stack_unf);
		else
			_stop += size;
	}
}