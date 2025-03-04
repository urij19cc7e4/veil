#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include "define.hpp"
#include "interpreter_type.hpp"

namespace interpreter
{
	class stack_base
	{
	private:
		inline void __def_obj() noexcept;
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

		uintptr_t _sbeg;
		uintptr_t _send;

		uintptr_t _ftop;
		uintptr_t _stop;

		stack_base() = delete;
		stack_base(uint64_t size);
		stack_base(const stack_base& o) = delete;
		stack_base(stack_base&& o) noexcept;
		~stack_base() noexcept;

		stack_base& operator=(const stack_base& o) = delete;
		stack_base& operator=(stack_base&& o) noexcept;
	};

	template <std::endian endianness>
	class stack : public stack_base {};
}