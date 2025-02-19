#pragma once

#include <bit>
#include <cstdint>
#include <stdexcept>

#include "interpreter_type.hpp"

namespace interpreter
{
	class stack_base
	{
	protected:
		static constexpr const char _err_msg_frame_ovf[] = "Stack frame overflow";
		static constexpr const char _err_msg_frame_unf[] = "Stack frame underflow";
		static constexpr const char _err_msg_stack_ovf[] = "Stack overflow";
		static constexpr const char _err_msg_stack_unf[] = "Stack underflow";
		static constexpr const char _err_msg_zero_size[] = "Stack size is zero";

		uint8_t* _data;
		uint64_t _size;

		uint8_t* _sbeg;
		uint8_t* _send;

		uint8_t* _ftop;
		uint8_t* _stop;

		stack_base() = delete;

		stack_base(uint64_t size)
		{
			if (size == 0ui64)
				throw std::runtime_error(_err_msg_zero_size);
			else
			{
				_data = new uint8_t[size];
				_size = size;
			}
		}

		stack_base(const stack_base& o) = delete;

		stack_base(stack_base&& o) noexcept
		{
			__mov_obj(std::move(o));
		}

		virtual ~stack_base() noexcept
		{
			__del_obj();
		}

		inline void __del_obj() noexcept
		{
			if (_data != (uint8_t*)nullptr)
				delete[] _data;
		}

		inline void __mov_obj(stack_base&& o) noexcept
		{
			_data = o._data;
			_size = o._size;

			_sbeg = o._sbeg;
			_send = o._send;

			_ftop = o._ftop;
			_stop = o._stop;

			o._data = (uint8_t*)nullptr;
			o._size = 0ui64;
		}

		stack_base& operator=(const stack_base& o) = delete;

		stack_base& operator=(stack_base&& o) noexcept
		{
			__del_obj();
			__mov_obj(std::move(o));

			return *this;
		}
	};

	template <std::endian endianness>
	class stack : public stack_base
	{
	private:
		inline uint64_t __top_offset() noexcept;
		inline uint64_t __bot_offset() noexcept;

		inline void __push_ptr(uint8_t* value);
		inline uint8_t* __pop_ptr();

		template <VALUE T>
		inline uint8_t* __get_vptr(int64_t offset);

		template <VALUE T>
		inline void __push_val(T value);

		template <VALUE T>
		inline T& __top_val();

		template <VALUE T>
		inline T __pop_val();

	public:
		stack() = delete;

		stack(uint64_t size);

		stack(const stack& o) = delete;

		stack(stack&& o) noexcept : stack_base(std::move(o)) {}

		virtual ~stack() noexcept {}

		void push_frame(uint8_t* value);
		uint8_t* pop_frame();

		void push_ptr(uint8_t* value);
		uint8_t* pop_ptr();

		void alloc(uint64_t size);
		void allocz(uint64_t size);
		void dealloc(uint64_t size);

		template <VALUE T>
		void load(int64_t offset);

		template <VALUE T>
		void store(int64_t offset);

		template <VALUE T>
		void push(T value);

		template <VALUE T>
		void dup();

		template <VALUE T>
		void rem();

		template <VALUE T>
		T& top();

		template <VALUE T>
		T pop();

		stack& operator=(const stack& o) = delete;

		stack& operator=(stack&& o) noexcept
		{
			return (stack&)stack_base::operator=(std::move(o));
		}
	};
}