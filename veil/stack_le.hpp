#pragma once

#include "stack.hpp"

namespace interpreter
{
	template <>
	class stack<std::endian::little> : public stack_base
	{
	private:
		inline ptrdiff_t __top_offset() noexcept;
		inline ptrdiff_t __bot_offset() noexcept;

		inline void __push_ptr(uintptr_t value);
		inline uintptr_t __pop_ptr();

		template <VALUE V>
		inline uintptr_t __get_vptr(ptrdiff_t offset)
		{
			if (offset >= (ptrdiff_t)0)
			{
				uintptr_t vptr = _ftop + (ptrdiff_t)offset + (ptrdiff_t)(sizeof(uint64_t) * 2ui64);

			#ifdef STACKCHECK
				if ((uintptr_t)(vptr + (ptrdiff_t)sizeof(V)) > *((uintptr_t*)_ftop))
					throw std::runtime_error(_err_msg_frame_unf);
				else
					return vptr;
			#else
				return vptr;
			#endif
			}
			else
			{
				// uintptr_t vptr = _ftop - (ptrdiff_t)(-((int64_t)offset + 1i64)) - (ptrdiff_t)sizeof(V);
				uintptr_t vptr = _ftop - (ptrdiff_t)(~((uint64_t)(int64_t)offset)) - (ptrdiff_t)sizeof(V);

			#ifdef STACKCHECK
				if (vptr < _stop)
					throw std::runtime_error(_err_msg_frame_ovf);
				else
					return vptr;
			#else
				return vptr;
			#endif
			}
		}

		template <VALUE V>
		inline void __push_val(V value)
		{
			_stop -= (ptrdiff_t)sizeof(V);
			*((V*)_stop) = value;
		}

		template <VALUE V>
		inline V& __top_val()
		{
			return *((V*)_stop);
		}

		template <VALUE V>
		inline V __pop_val()
		{
			V value = *((V*)_stop);
			_stop += (ptrdiff_t)sizeof(V);

			return value;
		}

	public:
		stack() = delete;
		stack(uint64_t size);
		stack(const stack& o) = delete;
		stack(stack&& o) noexcept;
		~stack() noexcept = default;

		void push_frame(uintptr_t value);
		uintptr_t pop_frame();

		void push_ptr(uintptr_t value);
		uintptr_t pop_ptr();

		void load_frame();
		void load_stack();

		void store_frame();
		void store_stack();

		void alloc(uint64_t size);
		void allocz(uint64_t size);
		void dealloc(uint64_t size);

		template <VALUE V>
		void load(ptrdiff_t offset)
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(V) > __top_offset())
				throw std::runtime_error(_err_msg_stack_ovf);
			else
				__push_val<V>(*((V*)__get_vptr<V>(offset)));
		#else
			__push_val<V>(*((V*)__get_vptr<V>(offset)));
		#endif
		}

		template <VALUE V>
		void store(ptrdiff_t offset)
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(V) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				*((V*)__get_vptr<V>(offset)) = __pop_val<V>();
		#else
			* ((V*)__get_vptr<V>(offset)) = __pop_val<V>();
		#endif
		}

		template <VALUE V>
		void push(V value)
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(V) > __top_offset())
				throw std::runtime_error(_err_msg_stack_ovf);
			else
				__push_val<V>(value);
		#else
			__push_val<V>(value);
		#endif
		}

		template <VALUE V>
		void dup()
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(V) > __top_offset())
				throw std::runtime_error(_err_msg_stack_ovf);
			else if ((ptrdiff_t)sizeof(V) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				__push_val<V>(__top_val<V>());
		#else
			__push_val<V>(__top_val<V>());
		#endif
		}

		template <VALUE V>
		void rem()
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(V) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				_stop += (ptrdiff_t)sizeof(V);
		#else
			_stop += (ptrdiff_t)sizeof(V);
		#endif
		}

		template <VALUE V>
		V& top()
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(V) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				return __top_val<V>();
		#else
			return __top_val<V>();
		#endif
		}

		template <VALUE V>
		V pop()
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(V) > __bot_offset())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				return __pop_val<V>();
		#else
			return __pop_val<V>();
		#endif
		}

		stack& operator=(const stack& o) = delete;
		stack& operator=(stack&& o) noexcept;
	};
}