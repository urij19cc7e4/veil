#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include "interpreter_type.hpp"

namespace interpreter
{
	class stack_base
	{
	private:
		inline void __del_obj() noexcept;
		inline void __mov_obj(stack_base&& o) noexcept;

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
		stack_base(uint64_t size);
		stack_base(const stack_base& o) = delete;
		stack_base(stack_base&& o) noexcept;
		~stack_base() noexcept;

		stack_base& operator=(const stack_base& o) = delete;
		stack_base& operator=(stack_base&& o) noexcept;
	};

	template <std::endian endianness>
	class stack : public stack_base
	{
	private:
		inline ptrdiff_t __top_offset() noexcept;
		inline ptrdiff_t __bot_offset() noexcept;

		inline void __push_ptr(const uint8_t* value);
		inline uint8_t* __pop_ptr();

		template <VALUE V>
		inline uint8_t* __get_vptr(ptrdiff_t offset);

		template <VALUE V>
		inline void __push_val(V value);

		template <VALUE V>
		inline V& __top_val();

		template <VALUE V>
		inline V __pop_val();

	public:
		stack() = delete;
		stack(uint64_t size);
		stack(const stack& o) = delete;
		stack(stack&& o) noexcept : stack_base(std::move(o)) {}
		~stack() noexcept = default;

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

		template <VALUE V>
		void load(ptrdiff_t offset);

		template <VALUE V>
		void store(ptrdiff_t offset);

		template <VALUE V>
		void push(V value);

		template <VALUE V>
		void dup();

		template <VALUE V>
		void rem();

		template <VALUE V>
		V& top();

		template <VALUE V>
		V pop();

		stack& operator=(const stack& o) = delete;

		stack& operator=(stack&& o) noexcept
		{
			return (stack&)stack_base::operator=(std::move(o));
		}
	};
}