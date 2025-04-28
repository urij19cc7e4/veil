#pragma once

#include <bit>
#include <concepts>
#include <cstdint>
#include <list>
#include <map>
#include <stdexcept>
#include <utility>

#include "interpreter_type.hpp"
#include "stack_be.hpp"
#include "stack_le.hpp"
#include "ffi.h"

namespace interpreter
{
	template <typename T, std::endian endianness = std::endian::native>
	concept STACK = std::same_as<T, stack<endianness>&> || std::same_as<T, uintptr_t&>;

	class nativecall
	{
	private:
		static constexpr const char _err_msg_fetch_code[] = "Fetch out of code range";
		static constexpr const char _err_msg_wrong_abit[] = "Wrong ABI type";
		static constexpr const char _err_msg_wrong_type[] = "Wrong argument type";

		abi_type _abi_type;
		uint64_t _arg_count;

		arg_type* _arg_io_type;
		arg_type _ret_io_type;

		ffi_type** _arg_type;
		ffi_type* _ret_type;

		void** _arg_val;
		void* _ret_val;

		ffi_cif _cif;

		template <VALUE V>
		static inline V fetch_with_check(const uint8_t*& opptr, const uint8_t* bcode, const uint8_t* ecode)
		{
			if (opptr < bcode || opptr + (ptrdiff_t)sizeof(V) > ecode)
				throw std::runtime_error(_err_msg_fetch_code);
			else
			{
				V t = *((V*)opptr);
				opptr += (ptrdiff_t)sizeof(V);

				return t;
			}
		}

		template <VALUE V>
		static inline void fetch_with_check(const uint8_t*& opptr, uint8_t* data, uint64_t size, const uint8_t* bcode, const uint8_t* ecode)
		{
			size *= (uint64_t)sizeof(V);

			if (opptr < bcode || opptr + (ptrdiff_t)size > ecode)
				throw std::runtime_error(_err_msg_fetch_code);
			else
			{
				memcpy(data, opptr, size);
				opptr += (ptrdiff_t)size;
			}
		}

		template <VALUE V>
		static inline V fetch_without_check(const uint8_t*& opptr)
		{
			V t = *((V*)opptr);
			opptr += (ptrdiff_t)sizeof(V);

			return t;
		}

		template <VALUE V>
		static inline void fetch_without_check(const uint8_t*& opptr, uint8_t* data, uint64_t size)
		{
			size *= (uint64_t)sizeof(V);

			memcpy(data, opptr, size);
			opptr += (ptrdiff_t)size;
		}

		static inline ffi_abi get_abi(abi_type type);
		static inline uint64_t get_asize(arg_type type);
		static inline uint64_t get_rsize(arg_type type);
		static inline ffi_type* get_type(arg_type type);
		static inline void* get_val(arg_type type);

		template <typename T>
		static inline void convert(void* mem, arg_o_type type, T value)
		{
			switch (type)
			{
			case arg_o_type::__void: break;
			case arg_o_type::__flt64: *((double*)mem) = (double)value; break;
			case arg_o_type::__sint64: *((int64_t*)mem) = (int64_t)value; break;
			case arg_o_type::__uint64: *((uint64_t*)mem) = (uint64_t)value; break;
			case arg_o_type::__flt32: *((float*)mem) = (float)value; break;
			case arg_o_type::__sint32: *((int32_t*)mem) = (int32_t)value; break;
			case arg_o_type::__uint32: *((uint32_t*)mem) = (uint32_t)value; break;
			case arg_o_type::__sint16: *((int16_t*)mem) = (int16_t)value; break;
			case arg_o_type::__uint16: *((uint16_t*)mem) = (uint16_t)value; break;
			case arg_o_type::__sint8: *((int8_t*)mem) = (int8_t)value; break;
			case arg_o_type::__uint8: *((uint8_t*)mem) = (uint8_t)value; break;
			case arg_o_type::__ptr: // fall to default: throw
			default: throw std::runtime_error(_err_msg_wrong_type);
			}
		}

		template <typename T>
		static inline T convert(void* mem, arg_o_type type)
		{
			switch (type)
			{
			case arg_o_type::__void: return (T)0ui8;
			case arg_o_type::__flt64: return (T) * ((double*)mem);
			case arg_o_type::__sint64: return (T) * ((int64_t*)mem);
			case arg_o_type::__uint64: return (T) * ((uint64_t*)mem);
			case arg_o_type::__flt32: return (T) * ((float*)mem);
			case arg_o_type::__sint32: return (T) * ((int32_t*)mem);
			case arg_o_type::__uint32: return (T) * ((uint32_t*)mem);
			case arg_o_type::__sint16: return (T) * ((int16_t*)mem);
			case arg_o_type::__uint16: return (T) * ((uint16_t*)mem);
			case arg_o_type::__sint8: return (T) * ((int8_t*)mem);
			case arg_o_type::__uint8: return (T) * ((uint8_t*)mem);
			case arg_o_type::__ptr: // fall to default: throw
			default: throw std::runtime_error(_err_msg_wrong_type);
			}
		}

		inline void __cpy_obj(const nativecall& o);
		inline void __def_obj() noexcept;
		inline void __del_obj() noexcept;
		inline void __mov_obj(nativecall&& o) noexcept;

		inline void init();

	public:
		nativecall() = delete;
		nativecall(const uint8_t*& opptr);
		nativecall(const uint8_t*& opptr, const uint8_t* bcode, const uint8_t* ecode);
		nativecall(const nativecall& o);
		nativecall(nativecall&& o) noexcept;
		~nativecall() noexcept;

		template <std::endian endianness = std::endian::native>
		void call(uintptr_t& stop, uintptr_t native_ptr)
		{
			using stack_type = stack<endianness>;

			for (uint64_t i = 0ui64; i < _arg_count; ++i)
				switch (_arg_io_type[i].inner)
				{
				case arg_i_type::__void: convert(_arg_val[i], _arg_io_type[i].outer, 0ui8); break;
				case arg_i_type::__val64f: convert(_arg_val[i], _arg_io_type[i].outer, stack_type::template dil_pop<val64<>>(stop).f); break;
				case arg_i_type::__val64s: convert(_arg_val[i], _arg_io_type[i].outer, stack_type::template dil_pop<val64<>>(stop).si); break;
				case arg_i_type::__val64u: convert(_arg_val[i], _arg_io_type[i].outer, stack_type::template dil_pop<val64<>>(stop).ui); break;
				case arg_i_type::__val32f: convert(_arg_val[i], _arg_io_type[i].outer, stack_type::template dil_pop<val32<>>(stop).f); break;
				case arg_i_type::__val32s: convert(_arg_val[i], _arg_io_type[i].outer, stack_type::template dil_pop<val32<>>(stop).si); break;
				case arg_i_type::__val32u: convert(_arg_val[i], _arg_io_type[i].outer, stack_type::template dil_pop<val32<>>(stop).ui); break;
				case arg_i_type::__val16s: convert(_arg_val[i], _arg_io_type[i].outer, stack_type::template dil_pop<val16<>>(stop).si); break;
				case arg_i_type::__val16u: convert(_arg_val[i], _arg_io_type[i].outer, stack_type::template dil_pop<val16<>>(stop).ui); break;
				case arg_i_type::__val8s: convert(_arg_val[i], _arg_io_type[i].outer, stack_type::template dil_pop<val8<>>(stop).si); break;
				case arg_i_type::__val8u: convert(_arg_val[i], _arg_io_type[i].outer, stack_type::template dil_pop<val8<>>(stop).ui); break;
				case arg_i_type::__ptr:
					if (_arg_io_type[i].outer == arg_o_type::__ptr)
					{
						*((void**)(_arg_val[i])) = (void*)stack_type::template dil_pop_ptr(stop);
						break;
					}
					// else fall to default: throw
				default: throw std::runtime_error(_err_msg_wrong_type);
				}

			ffi_call(&_cif, (void (*)())native_ptr, _ret_val, _arg_val);

			switch (_ret_io_type.inner)
			{
			case arg_i_type::__void: break;
			case arg_i_type::__val64f: stack_type::template dil_push(stop, val64<>(convert<double>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val64s: stack_type::template dil_push(stop, val64<>(convert<int64_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val64u: stack_type::template dil_push(stop, val64<>(convert<uint64_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val32f: stack_type::template dil_push(stop, val32<>(convert<float>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val32s: stack_type::template dil_push(stop, val32<>(convert<int32_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val32u: stack_type::template dil_push(stop, val32<>(convert<uint32_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val16s: stack_type::template dil_push(stop, val16<>(convert<int16_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val16u: stack_type::template dil_push(stop, val16<>(convert<uint16_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val8s: stack_type::template dil_push(stop, val8<>(convert<int8_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val8u: stack_type::template dil_push(stop, val8<>(convert<uint8_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__ptr:
				if (_ret_io_type.outer == arg_o_type::__ptr)
				{
					stack_type::template dil_push_ptr(stop, (uintptr_t) * ((void**)_ret_val));
					break;
				}
				// else fall to default: throw
			default: throw std::runtime_error(_err_msg_wrong_type);
			}
		}

		template <std::endian endianness = std::endian::native>
		void call(stack<endianness>& stack, uintptr_t native_ptr)
		{
			for (uint64_t i = 0ui64; i < _arg_count; ++i)
				switch (_arg_io_type[i].inner)
				{
				case arg_i_type::__void: convert(_arg_val[i], _arg_io_type[i].outer, 0ui8); break;
				case arg_i_type::__val64f: convert(_arg_val[i], _arg_io_type[i].outer, stack.pop<val64<>>().f); break;
				case arg_i_type::__val64s: convert(_arg_val[i], _arg_io_type[i].outer, stack.pop<val64<>>().si); break;
				case arg_i_type::__val64u: convert(_arg_val[i], _arg_io_type[i].outer, stack.pop<val64<>>().ui); break;
				case arg_i_type::__val32f: convert(_arg_val[i], _arg_io_type[i].outer, stack.pop<val32<>>().f); break;
				case arg_i_type::__val32s: convert(_arg_val[i], _arg_io_type[i].outer, stack.pop<val32<>>().si); break;
				case arg_i_type::__val32u: convert(_arg_val[i], _arg_io_type[i].outer, stack.pop<val32<>>().ui); break;
				case arg_i_type::__val16s: convert(_arg_val[i], _arg_io_type[i].outer, stack.pop<val16<>>().si); break;
				case arg_i_type::__val16u: convert(_arg_val[i], _arg_io_type[i].outer, stack.pop<val16<>>().ui); break;
				case arg_i_type::__val8s: convert(_arg_val[i], _arg_io_type[i].outer, stack.pop<val8<>>().si); break;
				case arg_i_type::__val8u: convert(_arg_val[i], _arg_io_type[i].outer, stack.pop<val8<>>().ui); break;
				case arg_i_type::__ptr:
					if (_arg_io_type[i].outer == arg_o_type::__ptr)
					{
						*((void**)(_arg_val[i])) = (void*)stack.pop_ptr();
						break;
					}
					// else fall to default: throw
				default: throw std::runtime_error(_err_msg_wrong_type);
				}

			ffi_call(&_cif, (void (*)())native_ptr, _ret_val, _arg_val);

			switch (_ret_io_type.inner)
			{
			case arg_i_type::__void: break;
			case arg_i_type::__val64f: stack.push(val64<>(convert<double>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val64s: stack.push(val64<>(convert<int64_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val64u: stack.push(val64<>(convert<uint64_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val32f: stack.push(val32<>(convert<float>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val32s: stack.push(val32<>(convert<int32_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val32u: stack.push(val32<>(convert<uint32_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val16s: stack.push(val16<>(convert<int16_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val16u: stack.push(val16<>(convert<uint16_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val8s: stack.push(val8<>(convert<int8_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__val8u: stack.push(val8<>(convert<uint8_t>(_ret_val, _ret_io_type.outer))); break;
			case arg_i_type::__ptr:
				if (_ret_io_type.outer == arg_o_type::__ptr)
				{
					stack.push_ptr((uintptr_t) * ((void**)_ret_val));
					break;
				}
				// else fall to default: throw
			default: throw std::runtime_error(_err_msg_wrong_type);
			}
		}

		void prep(const uint8_t*& opptr);
		void prep(const uint8_t*& opptr, const uint8_t* bcode, const uint8_t* ecode);

		nativecall& operator=(const nativecall& o);
		nativecall& operator=(nativecall&& o) noexcept;
	};

	class nativehub
	{
	private:
		using nref = std::map<uintptr_t, nativecall>::iterator;

		static constexpr const char _err_msg_cache_error[] = "Native Hub internal cache error";

		std::map<uintptr_t, nativecall> _cache;
		std::list<nref> _queue;

		uint64_t _count;
		uint64_t _size;

		template <std::endian endianness = std::endian::native>
		static inline uintptr_t stack_pop_native_ptr(uintptr_t& stop)
		{
			return stack<endianness>::dil_pop_ptr(stop);
		}

		template <std::endian endianness = std::endian::native>
		static inline uintptr_t stack_pop_native_ptr(stack<endianness>& stack)
		{
			return stack.pop_ptr();
		}

		inline std::pair<nref, bool> add(uintptr_t k, nativecall&& v)
		{
			std::pair<nativehub::nref, bool> r = _cache.insert({ k, std::move(v) });
			_queue.push_back(r.first);
			++_count;
			return r;
		}

		inline nref get(uintptr_t k)
		{
			return _cache.find(k);
		}

		inline void rem()
		{
			_cache.erase(_queue.front());
			_queue.pop_front();
			--_count;
		}

		template <STACK S, std::endian endianness = std::endian::native, typename ...L>
		inline void __call(const uint8_t*& opptr, S stack, L... limit)
		{
			uintptr_t native_ptr = stack_pop_native_ptr<endianness>(stack);

			if (_count != 0ui64)
			{
				nref iter = get(native_ptr);

				if (iter != _cache.end())
				{
					iter->second.prep(opptr, limit...);
					iter->second.call(stack, native_ptr);
					return;
				}
			}

			nativecall call_obj(opptr, limit...);
			nativecall* call_ptr;

			if (_size == 0ui64)
				call_ptr = &call_obj;
			else
			{
				if (_count == _size)
					rem();

				std::pair<nref, bool> r = add(native_ptr, std::move(call_obj));

				if (!r.second)
					throw std::runtime_error(_err_msg_cache_error);

				call_ptr = &(r.first->second);
			}

			call_ptr->call(stack, native_ptr);
		}

	public:
		nativehub() = delete;
		nativehub(uint64_t size);
		nativehub(const nativehub& o) = delete;
		nativehub(nativehub&& o) noexcept;
		~nativehub() noexcept = default;

		template <std::endian endianness = std::endian::native>
		void call(const uint8_t*& opptr, uintptr_t& stop)
		{
			__call<uintptr_t&, endianness>(opptr, stop);
		}

		template <std::endian endianness = std::endian::native>
		void call(const uint8_t*& opptr, stack<endianness>& stack)
		{
			__call<interpreter::stack<endianness>&, endianness>(opptr, stack);
		}

		template <std::endian endianness = std::endian::native>
		void call(const uint8_t*& opptr, uintptr_t& stop, const uint8_t* bcode, const uint8_t* ecode)
		{
			__call<uintptr_t&, endianness>(opptr, stop, bcode, ecode);
		}

		template <std::endian endianness = std::endian::native>
		void call(const uint8_t*& opptr, stack<endianness>& stack, const uint8_t* bcode, const uint8_t* ecode)
		{
			__call<interpreter::stack<endianness>&, endianness>(opptr, stack, bcode, ecode);
		}

		nativehub& operator=(const nativehub& o) = delete;
		nativehub& operator=(nativehub&& o) noexcept;
	};
}