#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>

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

	public:
		inline uint8_t* get_data() const
		{
			return _data;
		}

		inline uint64_t get_size() const
		{
			return _size;
		}

		inline uintptr_t get_sbeg() const
		{
			return _sbeg;
		}

		inline uintptr_t get_send() const
		{
			return _send;
		}

		inline uintptr_t get_ftop() const
		{
			return _ftop;
		}

		inline uintptr_t get_stop() const
		{
			return _stop;
		}

		inline void set_data(uint8_t* data)
		{
			_data = data;
		}

		inline void set_size(uint64_t size)
		{
			_size = size;
		}

		inline void set_sbeg(uintptr_t sbeg)
		{
			_sbeg = sbeg;
		}

		inline void set_send(uintptr_t send)
		{
			_send = send;
		}

		inline void set_ftop(uintptr_t ftop)
		{
			_ftop = ftop;
		}

		inline void set_stop(uintptr_t stop)
		{
			_stop = stop;
		}
	};

	template <std::endian endianness = std::endian::native>
	class stack : public stack_base {};
}