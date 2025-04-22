#include "stack_le.hpp"

using namespace interpreter;
using namespace std;

stack<endian::little>::stack(uint64_t size) : stack_base(size)
{
	_sbeg = (uintptr_t)((uintptr_t)_data + (ptrdiff_t)_size);
	_send = (uintptr_t)_data;

	_ftop = _sbeg;
	_stop = _sbeg;
}

stack<endian::little>::stack(stack&& o) noexcept : stack_base(move(o)) {}

stack<endian::little>& stack<endian::little>::operator=(stack&& o) noexcept
{
	return (stack&)stack_base::operator=(move(o));
}