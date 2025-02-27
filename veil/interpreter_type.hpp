#pragma once

#include <bit>
#include <concepts>
#include <cstdint>

namespace interpreter
{
	struct comp_bits
	{
		bool above : 1ui64;
		bool below : 1ui64;
	};

	struct comp_byte
	{
		uint8_t value;
	};

	union comp_union
	{
		comp_bits bits;
		comp_byte byte;
	};

	struct eval_bits
	{
		bool ierr : 1ui64;
		bool iovf : 1ui64;
		bool iunf : 1ui64;
		bool fzer : 1ui64;
		bool fsub : 1ui64;
		bool finf : 1ui64;
		bool fnan : 1ui64;
		bool fnrm : 1ui64;
	};

	struct eval_byte
	{
		uint8_t value;
	};

	union eval_union
	{
		eval_bits bits;
		eval_byte byte;
	};

	struct state
	{
		comp_union comp;
		eval_union eval;
	};

	union state_val
	{
		state state;
		val16 value;
	};

	enum class type : uint8_t
	{
		v0,
		v1
	};

	union val8
	{
		int8_t si;
		uint8_t ui;
	};

	template <std::endian endianness>
	struct val16half;

	template <>
	struct val16half<std::endian::big>
	{
		val8 hi;
		val8 lo;
	};

	template <>
	struct val16half<std::endian::little>
	{
		val8 lo;
		val8 hi;
	};

	union val16
	{
		static constexpr uint64_t v8_count = 2ui64;

		int16_t si;
		uint16_t ui;

		val8 v8[v8_count];

		val16half<std::endian::native> half;
	};

	template <std::endian endianness>
	struct val32half;

	template <>
	struct val32half<std::endian::big>
	{
		val16 hi;
		val16 lo;
	};

	template <>
	struct val32half<std::endian::little>
	{
		val16 lo;
		val16 hi;
	};

	union val32
	{
		static constexpr uint64_t v8_count = 4ui64;
		static constexpr uint64_t v16_count = 2ui64;

		float f;

		int32_t si;
		uint32_t ui;

		val8 v8[v8_count];
		val16 v16[v16_count];

		val32half<std::endian::native> half;
	};

	template <std::endian endianness>
	struct val64half;

	template <>
	struct val64half<std::endian::big>
	{
		val32 hi;
		val32 lo;
	};

	template <>
	struct val64half<std::endian::little>
	{
		val32 lo;
		val32 hi;
	};

	union val64
	{
		static constexpr uint64_t v8_count = 8ui64;
		static constexpr uint64_t v16_count = 4ui64;
		static constexpr uint64_t v32_count = 2ui64;

		double f;

		int64_t si;
		uint64_t ui;

		val8 v8[v8_count];
		val16 v16[v16_count];
		val32 v32[v32_count];

		val64half<std::endian::native> half;
	};

	template <typename T>
	concept FLOAT
		= std::same_as<T, double>
		|| std::same_as<T, float>;

	template <typename T>
	concept SINT
		= std::same_as<T, int64_t>
		|| std::same_as<T, int32_t>
		|| std::same_as<T, int16_t>
		|| std::same_as<T, int8_t>;

	template <typename T>
	concept UINT
		= std::same_as<T, uint64_t>
		|| std::same_as<T, uint32_t>
		|| std::same_as<T, uint16_t>
		|| std::same_as<T, uint8_t>;

	template <typename T>
	concept VALUE
		= std::same_as<T, val64>
		|| std::same_as<T, val32>
		|| std::same_as<T, val16>
		|| std::same_as<T, val8>;
}