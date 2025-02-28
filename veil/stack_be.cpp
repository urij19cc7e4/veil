#include "stack_be.hpp"

using namespace interpreter;
using namespace std;

inline ptrdiff_t stack<std::endian::big>::__top_offset() noexcept
{
	return (ptrdiff_t)(_send - _stop);
}

inline ptrdiff_t stack<std::endian::big>::__bot_offset() noexcept
{
	return (ptrdiff_t)(_stop - _ftop);
}

inline void stack<std::endian::big>::__push_ptr(const uint8_t* value)
{
	*((const uint8_t**)_stop) = value;
	_stop += (ptrdiff_t)sizeof(uint64_t);
}

inline uint8_t* stack<std::endian::big>::__pop_ptr()
{
	_stop -= (ptrdiff_t)sizeof(uint64_t);
	uint8_t* value = *((uint8_t**)_stop);

	return value;
}

stack<std::endian::big>::stack(uint64_t size) : stack_base(size)
{
	_sbeg = _data;
	_send = _data + _size;

	_ftop = _sbeg;
	_stop = _sbeg;
}

void stack<std::endian::big>::push_frame(const uint8_t* value)
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

uint8_t* stack<std::endian::big>::pop_frame()
{
	if ((ptrdiff_t)0 != __bot_offset())
		throw std::runtime_error(_err_msg_stack_unf);
	{
		_stop = _ftop;
		_ftop = __pop_ptr();
		return __pop_ptr();
	}
}

void stack<std::endian::big>::push_ptr(const uint8_t* value)
{
	if ((ptrdiff_t)sizeof(uint64_t) > __top_offset())
		throw std::runtime_error(_err_msg_stack_ovf);
	else
		__push_ptr(value);
}

uint8_t* stack<std::endian::big>::pop_ptr()
{
	if ((ptrdiff_t)sizeof(uint64_t) > __bot_offset())
		throw std::runtime_error(_err_msg_stack_unf);
	else
		return __pop_ptr();
}

void stack<std::endian::big>::load_frame()
{
	if ((ptrdiff_t)sizeof(uint64_t) > __bot_offset())
		throw std::runtime_error(_err_msg_stack_unf);
	else
		_ftop = __pop_ptr();
}

void stack<std::endian::big>::load_stack()
{
	if ((ptrdiff_t)sizeof(uint64_t) > __bot_offset())
		throw std::runtime_error(_err_msg_stack_unf);
	else
		_stop = __pop_ptr();
}

void stack<std::endian::big>::store_frame()
{
	if ((ptrdiff_t)sizeof(uint64_t) > __top_offset())
		throw std::runtime_error(_err_msg_stack_ovf);
	else
		__push_ptr(_ftop);
}

void stack<std::endian::big>::store_stack()
{
	if ((ptrdiff_t)sizeof(uint64_t) > __top_offset())
		throw std::runtime_error(_err_msg_stack_ovf);
	else
		__push_ptr(_stop);
}

void stack<std::endian::big>::alloc(uint64_t size)
{
	if (size != 0ui64)
	{
		if (size > __top_offset())
			throw std::runtime_error(_err_msg_stack_ovf);
		else
			_stop += size;
	}
}

void stack<std::endian::big>::allocz(uint64_t size)
{
	if (size != 0ui64)
	{
		if (size > __top_offset())
			throw std::runtime_error(_err_msg_stack_ovf);
		else
		{
			memset((void*)_stop, (int)0ui8, (size_t)size);
			_stop += size;
		}
	}
}

void stack<std::endian::big>::dealloc(uint64_t size)
{
	if (size != 0ui64)
	{
		if (size > __bot_offset())
			throw std::runtime_error(_err_msg_stack_unf);
		else
			_stop -= size;
	}
}