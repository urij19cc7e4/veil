#pragma once

#include "stack.hpp"

namespace interpreter
{
	template <>
	class stack<std::endian::big> : public stack_base
	{
	private:
		inline ptrdiff_t __top_offset() noexcept;
		inline ptrdiff_t __bot_offset() noexcept;

		inline void __push_ptr(const uint8_t* value);
		inline uint8_t* __pop_ptr();

		template <VALUE T>
		inline uint8_t* __get_vptr(ptrdiff_t offset)
		{
			if (offset >= (ptrdiff_t)0)
			{
				uint8_t* vptr = _ftop - offset - (ptrdiff_t)(sizeof(uint64_t) * 2ui64) - (ptrdiff_t)sizeof(T);

				if (vptr < *((uint8_t**)(_ftop - (ptrdiff_t)sizeof(uint8_t*))))
					throw std::runtime_error(_err_msg_frame_unf);
				else
					return vptr;
			}
			else
			{
				// uint8_t* vptr = _ftop + (ptrdiff_t)(-((int64_t)offset + 1i64));
				uint8_t* vptr = _ftop + (ptrdiff_t)(~((uint64_t)(int64_t)offset));

				if (vptr + (ptrdiff_t)sizeof(T) > _stop)
					throw std::runtime_error(_err_msg_frame_ovf);
				else
					return vptr;
			}
		}

		template <VALUE T>
		inline void __push_val(T value)
		{
			*((T*)_stop) = value;
			_stop += (ptrdiff_t)sizeof(T);
		}

		template <VALUE T>
		inline T& __top_val()
		{
			return *((T*)(_stop - (ptrdiff_t)sizeof(T)));
		}

		template <VALUE T>
		inline T __pop_val()
		{
			_stop -= (ptrdiff_t)sizeof(T);
			T value = *((T*)_stop);

			return value;
		}

	public:
		stack(uint64_t size);

		void push_frame(const uint8_t* value);
		uint8_t* pop_frame();

		void push_ptr(const uint8_t* value);
		uint8_t* pop_ptr();

		void load_frame();
		void load_stack();

		void store_frame();
		void store_stack();

		void alloc(uint64_t size);
		void allocz(uint64_t size);
		void dealloc(uint64_t size);

		template <VALUE T>
		void load(ptrdiff_t offset)
		{
			if ((ptrdiff_t)sizeof(T) > __top_offset())
				throw std::runtime_error(_err_msg_stack_ovf);
			else
				__push_val<T>(*((T*)__get_vptr<T>(offset)));
		}

		template <VALUE T>
		void store(ptrdiff_t offset)
		{
			if ((ptrdiff_t)sizeof(T) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				*((T*)__get_vptr<T>(offset)) = __pop_val<T>();
		}

		template <VALUE T>
		void push(T value)
		{
			if ((ptrdiff_t)sizeof(T) > __top_offset())
				throw std::runtime_error(_err_msg_stack_ovf);
			else
				__push_val<T>(value);
		}

		template <VALUE T>
		void dup()
		{
			if ((ptrdiff_t)sizeof(T) > __top_offset())
				throw std::runtime_error(_err_msg_stack_ovf);
			else if ((ptrdiff_t)sizeof(T) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				__push_val<T>(__top_val<T>());
		}

		template <VALUE T>
		void rem()
		{
			if ((ptrdiff_t)sizeof(T) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				_stop -= (ptrdiff_t)sizeof(T);
		}

		template <VALUE T>
		T& top()
		{
			if ((ptrdiff_t)sizeof(T) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				return __top_val<T>();
		}

		template <VALUE T>
		T pop()
		{
			if ((ptrdiff_t)sizeof(T) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				return __pop_val<T>();
		}
	};
}