#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <utility>

#include "dispatcher.hpp"

namespace interpreter
{
	class interpreter
	{
	private:
		static constexpr const char _err_msg_argt_void[] = "Argument type void";
		static constexpr const char _err_msg_wrong_argt[] = "Wrong argument type";
		static constexpr const char _err_msg_wrong_end[] = "Wrong endian type";
		static constexpr const char _err_msg_zero_size[] = "Code size is zero";

		const uint8_t* _code;
		std::optional<dispatcher> _disp;

		inline void __def_obj() noexcept;
		inline void __del_obj() noexcept;
		inline void __mov_obj(interpreter&& o) noexcept;

		template <typename T, uint64_t ...index>
		inline void __push(const T& arg_t, std::integer_sequence<uint64_t, index...>)
		{
			(..., push(std::get<sizeof...(index) - index - 1ui64>(arg_t)));
		}

		template <IPTR P>
		inline void push(P value)
		{
			_disp.value().push_ptr((uintptr_t)value);
		}

		template <VALUE V>
		inline void push(V value)
		{
			_disp.value().push_val(value);
		}

		template <IPTR P>
		inline P pop()
		{
			return (P)_disp.value().pop_ptr();
		}

		template <VALUE V>
		inline V pop()
		{
			return _disp.value().pop_val<V>();
		}

	public:
		interpreter() = delete;
		interpreter(const uint8_t* code, uint64_t code_size, uint64_t cache_size, uint64_t stack_size, std::endian endianness);
		interpreter(const interpreter& o) = delete;
		interpreter(interpreter&& o) noexcept;
		~interpreter() noexcept;

		template <VALUE_IPTR_VOID RET, VALUE_IPTR ...ARG>
		RET execute(ARG... arg)
		{
			if constexpr (!std::same_as<RET, VOID>)
				push(RET());

			__push(std::make_tuple(arg...), std::make_integer_sequence<uint64_t, sizeof...(ARG)>{});

			_disp.value().init();
			_disp.value().loop();
			_disp.value().exit();

			(..., pop<decltype(arg)>());

			if constexpr (std::same_as<RET, VOID>)
				return VOID{};
			else
				return pop<RET>();
		}

		void execute(const vtype* atype, const void** aval, uint64_t count, vtype rtype, void* rval);

		interpreter& operator=(const interpreter& o) = delete;
		interpreter& operator=(interpreter&& o) noexcept;
	};
}