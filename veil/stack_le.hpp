#pragma once

#include "stack.hpp"

namespace interpreter
{
	template <>
	class stack<std::endian::little> : public stack_base
	{
	private:
		static inline void __dil_push_ptr(uintptr_t& stop, uintptr_t value)
		{
			stop -= (ptrdiff_t)sizeof(uint64_t);
			*((uintptr_t*)stop) = value;
		}

		static inline uintptr_t __dil_pop_ptr(uintptr_t& stop)
		{
			uintptr_t value = *((uintptr_t*)stop);
			stop += (ptrdiff_t)sizeof(uint64_t);

			return value;
		}

		template <VALUE V>
		static inline uintptr_t __dil_get_vptr(const uintptr_t& ftop, ptrdiff_t offset)
		{
			if (offset >= (ptrdiff_t)0)
				return ftop + offset + (ptrdiff_t)(sizeof(uint64_t) * 2ui64);
			else
				return ftop - (ptrdiff_t)(~((uintptr_t)offset)) - (ptrdiff_t)sizeof(V);
		}

		template <VALUE V>
		static inline void __dil_push_val(uintptr_t& stop, V value)
		{
			stop -= (ptrdiff_t)sizeof(V);
			*((V*)stop) = value;
		}

		template <VALUE V>
		static inline V& __dil_top_val(const uintptr_t& stop)
		{
			return *((V*)stop);
		}

		template <VALUE V>
		static inline V __dil_pop_val(uintptr_t& stop)
		{
			V value = *((V*)stop);
			stop += (ptrdiff_t)sizeof(V);

			return value;
		}

		inline ptrdiff_t __top_off() noexcept
		{
			return (ptrdiff_t)(_stop - _send);
		}

		inline ptrdiff_t __bot_off() noexcept
		{
			return (ptrdiff_t)(_ftop - _stop);
		}

		inline void __push_ptr(uintptr_t value)
		{
			_stop -= (ptrdiff_t)sizeof(uint64_t);
			*((uintptr_t*)_stop) = value;
		}

		inline uintptr_t __pop_ptr()
		{
			uintptr_t value = *((uintptr_t*)_stop);
			_stop += (ptrdiff_t)sizeof(uint64_t);

			return value;
		}

		template <VALUE V>
		inline uintptr_t __get_vptr(ptrdiff_t offset)
		{
			if (offset >= (ptrdiff_t)0)
			{
				uintptr_t vptr = _ftop + offset + (ptrdiff_t)(sizeof(uint64_t) * 2ui64);

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
				uintptr_t vptr = _ftop - (ptrdiff_t)(~((uintptr_t)offset)) - (ptrdiff_t)sizeof(V);

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
		static inline void dil_push_frame(uintptr_t& ftop, uintptr_t& stop, uintptr_t value)
		{
			__dil_push_ptr(stop, value);
			__dil_push_ptr(stop, ftop);
			ftop = stop;
		}

		static inline uintptr_t dil_pop_frame(uintptr_t& ftop, uintptr_t& stop)
		{
			stop = ftop;
			ftop = __dil_pop_ptr(stop);
			return __dil_pop_ptr(stop);
		}

		static inline void dil_push_ptr(uintptr_t& stop, uintptr_t value)
		{
			__dil_push_ptr(stop, value);
		}

		static inline uintptr_t dil_pop_ptr(uintptr_t& stop)
		{
			return __dil_pop_ptr(stop);
		}

		static inline void dil_load_frame(uintptr_t& ftop, uintptr_t& stop)
		{
			ftop = __dil_pop_ptr(stop);
		}

		static inline void dil_load_stack(uintptr_t& stop)
		{
			stop = __dil_pop_ptr(stop);
		}

		static inline void dil_store_frame(const uintptr_t& ftop, uintptr_t& stop)
		{
			__dil_push_ptr(stop, ftop);
		}

		static inline void dil_store_stack(uintptr_t& stop)
		{
			__dil_push_ptr(stop, stop);
		}

		static inline void dil_alloc(uintptr_t& stop, uint64_t size)
		{
			if (size != 0ui64)
				stop -= (ptrdiff_t)size;
		}

		static inline void dil_allocz(uintptr_t& stop, uint64_t size)
		{
			if (size != 0ui64)
			{
				stop -= (ptrdiff_t)size;
				memset((void*)stop, (int)0ui8, (size_t)size);
			}
		}

		static inline void dil_dealloc(uintptr_t& stop, uint64_t size)
		{
			if (size != 0ui64)
				stop += (ptrdiff_t)size;
		}

		template <VALUE V>
		static inline void dil_load(const uintptr_t& ftop, uintptr_t& stop, ptrdiff_t offset)
		{
			__dil_push_val<V>(stop, *((V*)__dil_get_vptr<V>(ftop, offset)));
		}

		template <VALUE V>
		static inline void dil_store(const uintptr_t& ftop, uintptr_t& stop, ptrdiff_t offset)
		{
			*((V*)__dil_get_vptr<V>(ftop, offset)) = __dil_pop_val<V>(stop);
		}

		template <VALUE V>
		static inline void dil_push(uintptr_t& stop, V value)
		{
			__dil_push_val<V>(stop, value);
		}

		template <VALUE V>
		static inline void dil_dup(uintptr_t& stop)
		{
			__dil_push_val<V>(stop, __dil_top_val<V>(stop));
		}

		template <VALUE V>
		static inline void dil_rem(uintptr_t& stop)
		{
			stop += (ptrdiff_t)sizeof(V);
		}

		template <VALUE V>
		static inline V& dil_top(const uintptr_t& stop)
		{
			return __dil_top_val<V>(stop);
		}

		template <VALUE V>
		static inline V dil_pop(uintptr_t& stop)
		{
			return __dil_pop_val<V>(stop);
		}

		stack() = delete;
		stack(uint64_t size);
		stack(const stack& o) = delete;
		stack(stack&& o) noexcept;
		~stack() noexcept = default;

		void push_frame(uintptr_t value)
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)(sizeof(uint64_t) * 2ui64) > __top_off())
				throw std::runtime_error(_err_msg_stack_ovf);
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

		uintptr_t pop_frame()
		{
		#ifdef STACKCHECK
			if (_stop > *((uintptr_t*)_ftop))
				throw std::runtime_error(_err_msg_stack_unf);
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

		void push_ptr(uintptr_t value)
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(uint64_t) > __top_off())
				throw std::runtime_error(_err_msg_stack_ovf);
			else
				__push_ptr(value);
		#else
			__push_ptr(value);
		#endif
		}

		uintptr_t pop_ptr()
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(uint64_t) > __bot_off())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				return __pop_ptr();
		#else
			return __pop_ptr();
		#endif
		}

		void load_frame()
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(uint64_t) > __bot_off())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				_ftop = __pop_ptr();
		#else
			_ftop = __pop_ptr();
		#endif
		}

		void load_stack()
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(uint64_t) > __bot_off())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				_stop = __pop_ptr();
		#else
			_stop = __pop_ptr();
		#endif
		}

		void store_frame()
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(uint64_t) > __top_off())
				throw std::runtime_error(_err_msg_stack_ovf);
			else
				__push_ptr(_ftop);
		#else
			__push_ptr(_ftop);
		#endif
		}

		void store_stack()
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(uint64_t) > __top_off())
				throw std::runtime_error(_err_msg_stack_ovf);
			else
				__push_ptr(_stop);
		#else
			__push_ptr(_stop);
		#endif
		}

		void alloc(uint64_t size)
		{
			if (size != 0ui64)
			{
			#ifdef STACKCHECK
				if (size > __top_off())
					throw std::runtime_error(_err_msg_stack_ovf);
				else
					_stop -= (ptrdiff_t)size;
			#else
				_stop -= (ptrdiff_t)size;
			#endif
			}
		}

		void allocz(uint64_t size)
		{
			if (size != 0ui64)
			{
			#ifdef STACKCHECK
				if (size > __top_off())
					throw std::runtime_error(_err_msg_stack_ovf);
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

		void dealloc(uint64_t size)
		{
			if (size != 0ui64)
			{
			#ifdef STACKCHECK
				if (size > __bot_off())
					throw std::runtime_error(_err_msg_stack_unf);
				else
					_stop += (ptrdiff_t)size;
			#else
				_stop += (ptrdiff_t)size;
			#endif
			}
		}

		template <VALUE V>
		void load(ptrdiff_t offset)
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(V) > __top_off())
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
			if ((ptrdiff_t)sizeof(V) > __bot_off())
				throw std::runtime_error(_err_msg_stack_unf);
			else
				*((V*)__get_vptr<V>(offset)) = __pop_val<V>();
		#else
			*((V*)__get_vptr<V>(offset)) = __pop_val<V>();
		#endif
		}

		template <VALUE V>
		void push(V value)
		{
		#ifdef STACKCHECK
			if ((ptrdiff_t)sizeof(V) > __top_off())
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
			if ((ptrdiff_t)sizeof(V) > __top_off())
				throw std::runtime_error(_err_msg_stack_ovf);
			else if ((ptrdiff_t)sizeof(V) > __bot_off())
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
			if ((ptrdiff_t)sizeof(V) > __bot_off())
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
			if ((ptrdiff_t)sizeof(V) > __bot_off())
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
			if ((ptrdiff_t)sizeof(V) > __bot_off())
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