#include "stack_be.hpp"

using namespace interpreter;
using namespace std;

stack<endian::big>::stack(uint64_t size) : stack_base(size)
{
	_sbeg = (uintptr_t)_data;
	_send = (uintptr_t)((uintptr_t)_data + (ptrdiff_t)_size);

	_ftop = _sbeg;
	_stop = _sbeg;
}

stack<endian::big>::stack(stack&& o) noexcept : stack_base(move(o)) {}

stack<endian::big>& stack<endian::big>::operator=(stack&& o) noexcept
{
	return (stack&)stack_base::operator=(move(o));
}