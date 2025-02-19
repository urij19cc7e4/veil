#pragma once

#include <cstdint>
#include <concepts>

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

	union val8
	{
		int8_t si;
		uint8_t ui;
	};

	union val16
	{
		int16_t si;
		uint16_t ui;

		val8 v8[2ui64];
	};

	union val32
	{
		float f;

		int32_t si;
		uint32_t ui;

		val8 v8[4ui64];
		val16 v16[2ui64];
	};

	union val64
	{
		double f;

		int64_t si;
		uint64_t ui;

		val8 v8[8ui64];
		val16 v16[4ui64];
		val32 v32[2ui64];
	};

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