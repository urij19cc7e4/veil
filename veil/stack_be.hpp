#pragma once

#include "stack.hpp"

namespace interpreter
{
	template <>
	class stack<std::endian::big> : public stack_base
	{
	private:
		inline uint64_t __top_offset() noexcept
		{
			return (uint64_t)(_send - _stop);
		}

		inline uint64_t __bot_offset() noexcept
		{
			return (uint64_t)(_stop - _ftop);
		}

		inline void __push_ptr(uint8_t* value)
		{
			*((uint8_t**)_stop) = value;
			_stop += (uint64_t)sizeof(uint64_t);
		}

		inline uint8_t* __pop_ptr()
		{
			_stop -= (uint64_t)sizeof(uint64_t);
			uint8_t* value = *((uint8_t**)_stop);

			return value;
		}

		template <VALUE T>
		inline uint8_t* __get_vptr(int64_t offset)
		{
			if (offset >= 0i64)
			{
				uint8_t* vptr = _ftop - (uint64_t)offset - (uint64_t)sizeof(uint64_t) * 2ui64 - (uint64_t)sizeof(T);

				if (vptr < *((uint8_t**)(_ftop - (uint64_t)sizeof(uint8_t*))))
					throw std::runtime_error(_err_msg_frame_unf);
				else
					return vptr;
			}
			else
			{
				// uint8_t* vptr = _ftop + (uint64_t)(-(offset + 1i64));
				uint8_t* vptr = _ftop + (~((uint64_t)offset));

				if (vptr + (uint64_t)sizeof(T) > _stop)
					throw std::runtime_error(_err_msg_frame_ovf);
				else
					return vptr;
			}
		}

		template <VALUE T>
		inline void __push_val(T value)
		{
			*((T*)_stop) = value;
			_stop += (uint64_t)sizeof(T);
		}

		template <VALUE T>
		inline T& __top_val()
		{
			return *((T*)(_stop - (uint64_t)sizeof(T)));
		}

		template <VALUE T>
		inline T __pop_val()
		{
			_stop -= (uint64_t)sizeof(T);
			T value = *((T*)_stop);

			return value;
		}

	public:
		stack(uint64_t size) : stack_base(size)
		{
				_sbeg = _data;
				_send = _data + _size;

				_ftop = _sbeg;
				_stop = _sbeg;
		}

		void push_frame(uint8_t* value)
		{
			if ((uint64_t)sizeof(uint64_t) * 2ui64 > __top_offset())
				throw std::runtime_error(_err_msg_stack_ovf);
			else
			{
				__push_ptr(value);
				__push_ptr(_ftop);
				_ftop = _stop;
			}
		}

		uint8_t* pop_frame()
		{
			if ((uint64_t)sizeof(uint64_t) * 2ui64 > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			{
				_stop = _ftop;
				_ftop = __pop_ptr();
				return __pop_ptr();
			}
		}

		void push_ptr(uint8_t* value)
		{
			if ((uint64_t)sizeof(uint64_t) > __top_offset())
				throw std::runtime_error(_err_msg_stack_ovf);
			else
				__push_ptr(value);
		}

		uint8_t* pop_ptr()
		{
			if ((uint64_t)sizeof(uint64_t) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				return __pop_ptr();
		}

		void alloc(uint64_t size)
		{
			if (size != 0ui64)
			{
				if (size > __top_offset())
					throw std::runtime_error(_err_msg_stack_ovf);
				else
					_stop += size;
			}
		}

		void allocz(uint64_t size)
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

		void dealloc(uint64_t size)
		{
			if (size != 0ui64)
			{
				if (size > __bot_offset())
					throw std::runtime_error(_err_msg_stack_unf);
				else
					_stop -= size;
			}
		}

		template <VALUE T>
		void load(int64_t offset)
		{
			if ((uint64_t)sizeof(T) > __top_offset())
				throw std::runtime_error(_err_msg_stack_ovf);
			else
				__push_val<T>(*((T*)__get_vptr<T>(offset)));
		}

		template <VALUE T>
		void store(int64_t offset)
		{
			if ((uint64_t)sizeof(T) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				*((T*)__get_vptr<T>(offset)) = __pop_val<T>();
		}

		template <VALUE T>
		void push(T value)
		{
			if ((uint64_t)sizeof(T) > __top_offset())
				throw std::runtime_error(_err_msg_stack_ovf);
			else
				__push_val<T>(value);
		}

		template <VALUE T>
		void dup()
		{
			if ((uint64_t)sizeof(T) > __top_offset())
				throw std::runtime_error(_err_msg_stack_ovf);
			else if ((uint64_t)sizeof(T) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				__push_val<T>(__top_val<T>());
		}

		template <VALUE T>
		void rem()
		{
			if ((uint64_t)sizeof(T) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				_stop -= (uint64_t)sizeof(T);
		}

		template <VALUE T>
		T& top()
		{
			if ((uint64_t)sizeof(T) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				return __top_val<T>();
		}

		template <VALUE T>
		T pop()
		{
			if ((uint64_t)sizeof(T) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				return __pop_val<T>();
		}
	};
}