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

		comp_bits(bool above = false, bool below = false) noexcept : above(above), below(below) {}
		comp_bits(const comp_bits& o) noexcept : above(o.above), below(o.below) {}
		comp_bits(comp_bits&& o) noexcept : above(o.above), below(o.below) {}
		~comp_bits() noexcept = default;

		comp_bits& operator=(const comp_bits& o) noexcept
		{
			above = o.above;
			below = o.below;

			return *this;
		}

		comp_bits& operator=(comp_bits&& o) noexcept
		{
			above = o.above;
			below = o.below;

			return *this;
		}
	};

	struct comp_byte
	{
		uint8_t value;

		comp_byte(uint8_t value = 0ui8) noexcept : value(value) {}
		comp_byte(const comp_byte& o) noexcept : value(o.value) {}
		comp_byte(comp_byte&& o) noexcept : value(std::move(o.value)) {}
		~comp_byte() noexcept = default;

		comp_byte& operator=(const comp_byte& o) noexcept
		{
			value = o.value;
			return *this;
		}

		comp_byte& operator=(comp_byte&& o) noexcept
		{
			value = std::move(o.value);
			return *this;
		}
	};

	union comp_union
	{
		comp_bits bits;
		comp_byte byte;

		comp_union() noexcept : byte() {}
		comp_union(comp_bits bits) noexcept : bits(bits) {}
		comp_union(comp_byte byte) noexcept : byte(byte) {}
		comp_union(const comp_union& o) noexcept : byte(o.byte) {}
		comp_union(comp_union&& o) noexcept : byte(std::move(o.byte)) {}
		~comp_union() noexcept = default;

		comp_union& operator=(const comp_union& o) noexcept
		{
			byte = o.byte;
			return *this;
		}

		comp_union& operator=(comp_union&& o) noexcept
		{
			byte = std::move(o.byte);
			return *this;
		}
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

		eval_bits
		(
			bool ierr = false,
			bool iovf = false,
			bool iunf = false,
			bool fzer = false,
			bool fsub = false,
			bool finf = false,
			bool fnan = false,
			bool fnrm = false
		) noexcept : ierr(ierr), iovf(iovf), iunf(iunf),
			fzer(fzer), fsub(fsub), finf(finf), fnan(fnan), fnrm(fnrm)
		{}
		eval_bits(const eval_bits& o) noexcept : ierr(o.ierr), iovf(o.iovf), iunf(o.iunf),
			fzer(o.fzer), fsub(o.fsub), finf(o.finf), fnan(o.fnan), fnrm(o.fnrm)
		{}
		eval_bits(eval_bits&& o) noexcept : ierr(o.ierr), iovf(o.iovf), iunf(o.iunf),
			fzer(o.fzer), fsub(o.fsub), finf(o.finf), fnan(o.fnan), fnrm(o.fnrm)
		{}
		~eval_bits() noexcept = default;

		eval_bits& operator=(const eval_bits& o) noexcept
		{
			ierr = o.ierr;
			iovf = o.iovf;
			iunf = o.iunf;
			fzer = o.fzer;
			fsub = o.fsub;
			finf = o.finf;
			fnan = o.fnan;
			fnrm = o.fnrm;

			return *this;
		}

		eval_bits& operator=(eval_bits&& o) noexcept
		{
			ierr = o.ierr;
			iovf = o.iovf;
			iunf = o.iunf;
			fzer = o.fzer;
			fsub = o.fsub;
			finf = o.finf;
			fnan = o.fnan;
			fnrm = o.fnrm;

			return *this;
		}
	};

	struct eval_byte
	{
		uint8_t value;

		eval_byte(uint8_t value = 0ui8) noexcept : value(value) {}
		eval_byte(const eval_byte& o) noexcept : value(o.value) {}
		eval_byte(eval_byte&& o) noexcept : value(std::move(o.value)) {}
		~eval_byte() noexcept = default;

		eval_byte& operator=(const eval_byte& o) noexcept
		{
			value = o.value;
			return *this;
		}

		eval_byte& operator=(eval_byte&& o) noexcept
		{
			value = std::move(o.value);
			return *this;
		}
	};

	union eval_union
	{
		eval_bits bits;
		eval_byte byte;

		eval_union() noexcept : byte() {}
		eval_union(eval_bits bits) noexcept : bits(bits) {}
		eval_union(eval_byte byte) noexcept : byte(byte) {}
		eval_union(const eval_union& o) noexcept : byte(o.byte) {}
		eval_union(eval_union&& o) noexcept : byte(std::move(o.byte)) {}
		~eval_union() noexcept = default;

		eval_union& operator=(const eval_union& o) noexcept
		{
			byte = o.byte;
			return *this;
		}

		eval_union& operator=(eval_union&& o) noexcept
		{
			byte = std::move(o.byte);
			return *this;
		}
	};

	struct state
	{
		comp_union comp;
		eval_union eval;

		state(comp_union comp = comp_union(), eval_union eval = eval_union()) noexcept : comp(comp), eval(eval) {}
		state(const state& o) noexcept : comp(o.comp), eval(o.eval) {}
		state(state&& o) noexcept : comp(std::move(o.comp)), eval(std::move(o.eval)) {}
		~state() noexcept = default;

		state& operator=(const state& o) noexcept
		{
			comp = o.comp;
			eval = o.eval;
			return *this;
		}

		state& operator=(state&& o) noexcept
		{
			comp = std::move(o.comp);
			eval = std::move(o.eval);
			return *this;
		}
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

		val8() noexcept {}
		val8(const int8_t& si) noexcept : si(si) {}
		val8(const uint8_t& ui) noexcept : ui(ui) {}
		val8(const val8& o) noexcept : ui(o.ui) {}
		val8(val8&& o) noexcept : ui(std::move(o.ui)) {}
		~val8() noexcept = default;

		val8& operator=(const val8& o) noexcept
		{
			ui = o.ui;
			return *this;
		}

		val8& operator=(val8&& o) noexcept
		{
			ui = std::move(o.ui);
			return *this;
		}
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

		val16() noexcept {}
		val16(const int16_t& si) noexcept : si(si) {}
		val16(const uint16_t& ui) noexcept : ui(ui) {}
		val16(const val16& o) noexcept : ui(o.ui) {}
		val16(val16&& o) noexcept : ui(std::move(o.ui)) {}
		~val16() noexcept = default;

		val16& operator=(const val16& o) noexcept
		{
			ui = o.ui;
			return *this;
		}

		val16& operator=(val16&& o) noexcept
		{
			ui = std::move(o.ui);
			return *this;
		}
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

		val32() noexcept {}
		val32(const float& f) noexcept : f(f) {}
		val32(const int32_t& si) noexcept : si(si) {}
		val32(const uint32_t& ui) noexcept : ui(ui) {}
		val32(const val32& o) noexcept : ui(o.ui) {}
		val32(val32&& o) noexcept : ui(std::move(o.ui)) {}
		~val32() noexcept = default;

		val32& operator=(const val32& o) noexcept
		{
			ui = o.ui;
			return *this;
		}

		val32& operator=(val32&& o) noexcept
		{
			ui = std::move(o.ui);
			return *this;
		}
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
	public:
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

		val64() noexcept {}
		val64(const double& f) noexcept : f(f) {}
		val64(const int64_t& si) noexcept : si(si) {}
		val64(const uint64_t& ui) noexcept : ui(ui) {}
		val64(const val64& o) noexcept : ui(o.ui) {}
		val64(val64&& o) noexcept : ui(std::move(o.ui)) {}
		~val64() noexcept = default;

		val64& operator=(const val64& o) noexcept
		{
			ui = o.ui;
			return *this;
		}

		val64& operator=(val64&& o) noexcept
		{
			ui = std::move(o.ui);
			return *this;
		}
	};

	union state_val
	{
		state __state;
		val16 __value;

		state_val() noexcept {}
		state_val(const state& s) noexcept : __state(s) {}
		state_val(const val16& v) noexcept : __value(v) {}
		state_val(const state_val& o) noexcept : __value(o.__value) {}
		state_val(state_val&& o) noexcept : __value(std::move(o.__value)) {}
		~state_val() noexcept = default;

		state_val& operator=(const state_val& o) noexcept
		{
			__value = o.__value;
			return *this;
		}

		state_val& operator=(state_val&& o) noexcept
		{
			__value = std::move(o.__value);
			return *this;
		}
	};

	template <typename T>
	concept FLOAT = std::same_as<T, double> || std::same_as<T, float>;

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

	template <typename T>
	concept VALUE_PTR = VALUE<T> || std::same_as<T, uint8_t*>;
}