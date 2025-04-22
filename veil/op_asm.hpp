#pragma once

#include <array>
#include <bit>
#include <cstdint>

#include "interpreter_type.hpp"
#include "op.hpp"

#define ENDIAN_TEMPLATE_NO_PARAMETERS(NAME) template <std::endian endianness = std::endian::native> inline std::array<uint8_t, (uint64_t)sizeof(interpreter::op)> __##NAME() noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op)>{ (uint8_t)interpreter::op::NAME }; }
#define ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS(NAME, BITS) template <interpreter::VALUE V, std::endian endianness> inline std::enable_if_t<std::is_same_v<V, interpreter::val##BITS<endianness>>, std::array<uint8_t, (uint64_t)sizeof(interpreter::op)>> __##NAME() noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op)>{ (uint8_t)interpreter::op::NAME##_##BITS }; }

#define ENDIAN_TEMPLATE_L_PARAMETER(NAME) template <std::endian endianness = std::endian::native> inline std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val16<endianness>)> __##NAME(interpreter::val16<endianness> value) noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val16<endianness>)>{ (uint8_t)interpreter::op::NAME, value.half.lo.ui, value.half.hi.ui }; }
#define ENDIAN_TEMPLATE_S_PARAMETER(NAME) template <std::endian endianness = std::endian::native> inline std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val8<endianness>)> __##NAME(interpreter::val8<endianness> value) noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val8<endianness>)>{ (uint8_t)interpreter::op::NAME, value.ui }; }

#define ONE_VALUE_ENDIAN_TEMPLATE_L_PARAMETER(NAME, BITS) template <interpreter::VALUE V, std::endian endianness> inline std::enable_if_t<std::is_same_v<V, interpreter::val##BITS<endianness>>, std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val16<endianness>)>> __##NAME(interpreter::val16<endianness> value) noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val16<endianness>)>{ (uint8_t)interpreter::op::NAME##_##BITS, value.half.lo.ui, value.half.hi.ui }; }
#define ONE_VALUE_ENDIAN_TEMPLATE_S_PARAMETER(NAME, BITS) template <interpreter::VALUE V, std::endian endianness> inline std::enable_if_t<std::is_same_v<V, interpreter::val##BITS<endianness>>, std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val8<endianness>)>> __##NAME(interpreter::val8<endianness> value) noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val8<endianness>)>{ (uint8_t)interpreter::op::NAME##_##BITS, value.ui }; }

#define ONE_VALUE_ENDIAN_TEMPLATE_64_BIT_PARAMETER(NAME) template <interpreter::VALUE V, std::endian endianness> inline std::enable_if_t<std::is_same_v<V, interpreter::val64<endianness>>, std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val64<endianness>)>> __##NAME(interpreter::val64<endianness> value) noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val64<endianness>)>{ (uint8_t)interpreter::op::NAME##_64, value.half.lo.half.lo.half.lo.ui, value.half.lo.half.lo.half.hi.ui, value.half.lo.half.hi.half.lo.ui, value.half.lo.half.hi.half.hi.ui, value.half.hi.half.lo.half.lo.ui, value.half.hi.half.lo.half.hi.ui, value.half.hi.half.hi.half.lo.ui, value.half.hi.half.hi.half.hi.ui }; }
#define ONE_VALUE_ENDIAN_TEMPLATE_32_BIT_PARAMETER(NAME) template <interpreter::VALUE V, std::endian endianness> inline std::enable_if_t<std::is_same_v<V, interpreter::val32<endianness>>, std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val32<endianness>)>> __##NAME(interpreter::val32<endianness> value) noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val32<endianness>)>{ (uint8_t)interpreter::op::NAME##_32, value.half.lo.half.lo.ui, value.half.lo.half.hi.ui, value.half.hi.half.lo.ui, value.half.hi.half.hi.ui }; }
#define ONE_VALUE_ENDIAN_TEMPLATE_16_BIT_PARAMETER(NAME) template <interpreter::VALUE V, std::endian endianness> inline std::enable_if_t<std::is_same_v<V, interpreter::val16<endianness>>, std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val16<endianness>)>> __##NAME(interpreter::val16<endianness> value) noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val16<endianness>)>{ (uint8_t)interpreter::op::NAME##_16, value.half.lo.ui, value.half.hi.ui }; }
#define ONE_VALUE_ENDIAN_TEMPLATE_8_BIT_PARAMETER(NAME) template <interpreter::VALUE V, std::endian endianness> inline std::enable_if_t<std::is_same_v<V, interpreter::val8<endianness>>, std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val8<endianness>)>> __##NAME(interpreter::val8<endianness> value) noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::val8<endianness>)>{ (uint8_t)interpreter::op::NAME##_8, value.ui }; }

#define ONE_VALUE_ENDIAN_TEMPLATE_TYPE_64_BIT_PARAMETERS(NAME) template <interpreter::VALUE V, std::endian endianness> inline std::enable_if_t<std::is_same_v<V, interpreter::val64<endianness>>, std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::type) + (uint64_t)sizeof(interpreter::val64<endianness>) * 2ui64>> __##NAME(interpreter::val64<endianness> value, interpreter::val64<endianness> _mask, interpreter::type _type) noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::type) + (uint64_t)sizeof(interpreter::val64<endianness>) * 2ui64>{ (uint8_t)interpreter::op::NAME##_64, (uint8_t)_type, _mask.half.lo.half.lo.half.lo.ui, _mask.half.lo.half.lo.half.hi.ui, _mask.half.lo.half.hi.half.lo.ui, _mask.half.lo.half.hi.half.hi.ui, _mask.half.hi.half.lo.half.lo.ui, _mask.half.hi.half.lo.half.hi.ui, _mask.half.hi.half.hi.half.lo.ui, _mask.half.hi.half.hi.half.hi.ui, value.half.lo.half.lo.half.lo.ui, value.half.lo.half.lo.half.hi.ui, value.half.lo.half.hi.half.lo.ui, value.half.lo.half.hi.half.hi.ui, value.half.hi.half.lo.half.lo.ui, value.half.hi.half.lo.half.hi.ui, value.half.hi.half.hi.half.lo.ui, value.half.hi.half.hi.half.hi.ui }; }
#define ONE_VALUE_ENDIAN_TEMPLATE_TYPE_32_BIT_PARAMETERS(NAME) template <interpreter::VALUE V, std::endian endianness> inline std::enable_if_t<std::is_same_v<V, interpreter::val32<endianness>>, std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::type) + (uint64_t)sizeof(interpreter::val32<endianness>) * 2ui64>> __##NAME(interpreter::val32<endianness> value, interpreter::val32<endianness> _mask, interpreter::type _type) noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::type) + (uint64_t)sizeof(interpreter::val32<endianness>) * 2ui64>{ (uint8_t)interpreter::op::NAME##_32, (uint8_t)_type, _mask.half.lo.half.lo.ui, _mask.half.lo.half.hi.ui, _mask.half.hi.half.lo.ui, _mask.half.hi.half.hi.ui, value.half.lo.half.lo.ui, value.half.lo.half.hi.ui, value.half.hi.half.lo.ui, value.half.hi.half.hi.ui }; }
#define ONE_VALUE_ENDIAN_TEMPLATE_TYPE_16_BIT_PARAMETERS(NAME) template <interpreter::VALUE V, std::endian endianness> inline std::enable_if_t<std::is_same_v<V, interpreter::val16<endianness>>, std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::type) + (uint64_t)sizeof(interpreter::val16<endianness>) * 2ui64>> __##NAME(interpreter::val16<endianness> value, interpreter::val16<endianness> _mask, interpreter::type _type) noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::type) + (uint64_t)sizeof(interpreter::val16<endianness>) * 2ui64>{ (uint8_t)interpreter::op::NAME##_16, (uint8_t)_type, _mask.half.lo.ui, _mask.half.hi.ui, value.half.lo.ui, value.half.hi.ui }; }
#define ONE_VALUE_ENDIAN_TEMPLATE_TYPE_8_BIT_PARAMETERS(NAME) template <interpreter::VALUE V, std::endian endianness> inline std::enable_if_t<std::is_same_v<V, interpreter::val8<endianness>>, std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::type) + (uint64_t)sizeof(interpreter::val8<endianness>) * 2ui64>> __##NAME(interpreter::val8<endianness> value, interpreter::val8<endianness> _mask, interpreter::type _type) noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op) + (uint64_t)sizeof(interpreter::type) + (uint64_t)sizeof(interpreter::val8<endianness>) * 2ui64>{ (uint8_t)interpreter::op::NAME##_8, (uint8_t)_type, _mask.ui, value.ui }; }

#define TWO_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_CONVERT(NAME1, NAME2, BITS1, BITS2) template <interpreter::VALUE V1, interpreter::VALUE V2, std::endian endianness> inline std::enable_if_t<std::is_same_v<V1, interpreter::val##BITS1<endianness>> && std::is_same_v<V2, interpreter::val##BITS2<endianness>>, std::array<uint8_t, (uint64_t)sizeof(interpreter::op)>> __##NAME1##_2_##NAME2() noexcept { return std::array<uint8_t, (uint64_t)sizeof(interpreter::op)>{ (uint8_t)interpreter::op::NAME1##BITS1_2_##NAME2##BITS2 }; }

#define ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32(NAME)						\
ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS(NAME, 64)								\
ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS(NAME, 32)								\

#define ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(NAME)				\
ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS(NAME, 64)								\
ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS(NAME, 32)								\
ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS(NAME, 16)								\
ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS(NAME, 8)								\

#define ONE_VALUE_ENDIAN_TEMPLATE_64_32_16_8_BIT_PARAMETER(NAME)				\
ONE_VALUE_ENDIAN_TEMPLATE_64_BIT_PARAMETER(NAME)								\
ONE_VALUE_ENDIAN_TEMPLATE_32_BIT_PARAMETER(NAME)								\
ONE_VALUE_ENDIAN_TEMPLATE_16_BIT_PARAMETER(NAME)								\
ONE_VALUE_ENDIAN_TEMPLATE_8_BIT_PARAMETER(NAME)									\

#define ONE_VALUE_ENDIAN_TEMPLATE_L_PARAMETER_64_32_16_8(NAME)					\
ONE_VALUE_ENDIAN_TEMPLATE_L_PARAMETER(NAME, 64)									\
ONE_VALUE_ENDIAN_TEMPLATE_L_PARAMETER(NAME, 32)									\
ONE_VALUE_ENDIAN_TEMPLATE_L_PARAMETER(NAME, 16)									\
ONE_VALUE_ENDIAN_TEMPLATE_L_PARAMETER(NAME, 8)									\

#define ONE_VALUE_ENDIAN_TEMPLATE_S_PARAMETER_64_32_16_8(NAME)					\
ONE_VALUE_ENDIAN_TEMPLATE_S_PARAMETER(NAME, 64)									\
ONE_VALUE_ENDIAN_TEMPLATE_S_PARAMETER(NAME, 32)									\
ONE_VALUE_ENDIAN_TEMPLATE_S_PARAMETER(NAME, 16)									\
ONE_VALUE_ENDIAN_TEMPLATE_S_PARAMETER(NAME, 8)									\

#define ONE_VALUE_ENDIAN_TEMPLATE_TYPE_64_32_16_8_BIT_PARAMETERS(NAME)			\
ONE_VALUE_ENDIAN_TEMPLATE_TYPE_64_BIT_PARAMETERS(NAME)							\
ONE_VALUE_ENDIAN_TEMPLATE_TYPE_32_BIT_PARAMETERS(NAME)							\
ONE_VALUE_ENDIAN_TEMPLATE_TYPE_16_BIT_PARAMETERS(NAME)							\
ONE_VALUE_ENDIAN_TEMPLATE_TYPE_8_BIT_PARAMETERS(NAME)							\

#define TWO_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_CONVERT_64_X_32(NAME1, NAME2)	\
TWO_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_CONVERT(NAME1, NAME2, 64, 64)			\
TWO_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_CONVERT(NAME1, NAME2, 64, 32)			\
TWO_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_CONVERT(NAME1, NAME2, 32, 64)			\
TWO_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_CONVERT(NAME1, NAME2, 32, 32)			\

namespace op_asm
{
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(and)
	ONE_VALUE_ENDIAN_TEMPLATE_64_32_16_8_BIT_PARAMETER(call)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(dup)
	ENDIAN_TEMPLATE_NO_PARAMETERS(exit)
	TWO_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_CONVERT_64_X_32(f, si)
	TWO_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_CONVERT_64_X_32(f, ui)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32(fadd)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32(fcmp)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32(fdiv)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32(fevl)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32(fmod)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32(fmul)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32(fneg)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32(fsub)
	ONE_VALUE_ENDIAN_TEMPLATE_64_32_16_8_BIT_PARAMETER(jmp)
	ONE_VALUE_ENDIAN_TEMPLATE_64_32_16_8_BIT_PARAMETER(jmp_a)
	ONE_VALUE_ENDIAN_TEMPLATE_64_32_16_8_BIT_PARAMETER(jmp_ae)
	ONE_VALUE_ENDIAN_TEMPLATE_64_32_16_8_BIT_PARAMETER(jmp_b)
	ONE_VALUE_ENDIAN_TEMPLATE_64_32_16_8_BIT_PARAMETER(jmp_be)
	ONE_VALUE_ENDIAN_TEMPLATE_64_32_16_8_BIT_PARAMETER(jmp_e)
	ONE_VALUE_ENDIAN_TEMPLATE_64_32_16_8_BIT_PARAMETER(jmp_ne)
	ONE_VALUE_ENDIAN_TEMPLATE_64_32_16_8_BIT_PARAMETER(jmp_un)
	ONE_VALUE_ENDIAN_TEMPLATE_TYPE_64_32_16_8_BIT_PARAMETERS(jmp_msk)
	ENDIAN_TEMPLATE_L_PARAMETER(l_alloc)
	ENDIAN_TEMPLATE_L_PARAMETER(l_allocz)
	ENDIAN_TEMPLATE_L_PARAMETER(l_dealloc)
	ONE_VALUE_ENDIAN_TEMPLATE_L_PARAMETER_64_32_16_8(l_load)
	ONE_VALUE_ENDIAN_TEMPLATE_L_PARAMETER_64_32_16_8(l_store)
	ENDIAN_TEMPLATE_NO_PARAMETERS(memcpy)
	ENDIAN_TEMPLATE_NO_PARAMETERS(memset)
	//template <std::endian endianness = std::endian::native> inline std::array<uint8_t, (uint64_t)sizeof(interpreter::op)> __nativecall() {}
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(neg)
	ENDIAN_TEMPLATE_NO_PARAMETERS(nop)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(not)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(or)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(pop)
	ENDIAN_TEMPLATE_NO_PARAMETERS(pop_flow)
	ENDIAN_TEMPLATE_NO_PARAMETERS(pop_frame)
	ENDIAN_TEMPLATE_NO_PARAMETERS(pop_stack)
	ENDIAN_TEMPLATE_NO_PARAMETERS(pop_state)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(ptrcpy)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(ptrget)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(ptrset)
	ONE_VALUE_ENDIAN_TEMPLATE_64_32_16_8_BIT_PARAMETER(push)
	ENDIAN_TEMPLATE_NO_PARAMETERS(push_flow)
	ENDIAN_TEMPLATE_NO_PARAMETERS(push_frame)
	ENDIAN_TEMPLATE_NO_PARAMETERS(push_stack)
	ENDIAN_TEMPLATE_NO_PARAMETERS(push_state)
	ENDIAN_TEMPLATE_NO_PARAMETERS(ret)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(rotl)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(rotr)
	ENDIAN_TEMPLATE_S_PARAMETER(s_alloc)
	ENDIAN_TEMPLATE_S_PARAMETER(s_allocz)
	ENDIAN_TEMPLATE_S_PARAMETER(s_dealloc)
	ONE_VALUE_ENDIAN_TEMPLATE_S_PARAMETER_64_32_16_8(s_load)
	ONE_VALUE_ENDIAN_TEMPLATE_S_PARAMETER_64_32_16_8(s_store)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(sadd)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(scmp)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(sdec)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(sdiv)
	TWO_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_CONVERT_64_X_32(si, f)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(sinc)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(smod)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(smul)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(sshl)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(sshr)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(ssub)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(uadd)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(ucmp)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(udec)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(udiv)
	TWO_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_CONVERT_64_X_32(ui, f)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(uinc)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(umod)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(umul)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(ushl)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(ushr)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(usub)
	ONE_VALUE_ENDIAN_TEMPLATE_NO_PARAMETERS_64_32_16_8(xor)
}