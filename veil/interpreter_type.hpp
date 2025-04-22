#pragma once

#include <bit>
#include <concepts>
#include <cstdint>

namespace interpreter
{
	struct comp_bits
	{
	public:
		bool above : 1ui64;
		bool below : 1ui64;

		inline comp_bits(bool above = false, bool below = false) noexcept : above(above), below(below) {}
		inline comp_bits(const comp_bits& o) noexcept : above(o.above), below(o.below) {}
		inline comp_bits(comp_bits&& o) noexcept : above(o.above), below(o.below) {}
		inline ~comp_bits() noexcept = default;

		inline comp_bits& operator=(const comp_bits& o) noexcept
		{
			above = o.above;
			below = o.below;

			return *this;
		}

		inline comp_bits& operator=(comp_bits&& o) noexcept
		{
			above = (bool&&)o.above;
			below = (bool&&)o.below;

			return *this;
		}
	};

	struct comp_byte
	{
	public:
		uint8_t value;

		inline comp_byte(uint8_t value = 0ui8) noexcept : value(value) {}
		inline comp_byte(const comp_byte& o) noexcept : value(o.value) {}
		inline comp_byte(comp_byte&& o) noexcept : value(std::move(o.value)) {}
		inline ~comp_byte() noexcept = default;

		inline comp_byte& operator=(const comp_byte& o) noexcept
		{
			value = o.value;

			return *this;
		}

		inline comp_byte& operator=(comp_byte&& o) noexcept
		{
			value = std::move(o.value);

			return *this;
		}
	};

	union comp_union
	{
	public:
		comp_bits bits;
		comp_byte byte;

		inline comp_union() noexcept : byte() {}
		inline comp_union(comp_bits bits) noexcept : bits(bits) {}
		inline comp_union(comp_byte byte) noexcept : byte(byte) {}
		inline comp_union(const comp_union& o) noexcept : byte(o.byte) {}
		inline comp_union(comp_union&& o) noexcept : byte(std::move(o.byte)) {}
		inline ~comp_union() noexcept = default;

		inline comp_union& operator=(const comp_union& o) noexcept
		{
			byte = o.byte;

			return *this;
		}

		inline comp_union& operator=(comp_union&& o) noexcept
		{
			byte = std::move(o.byte);

			return *this;
		}
	};

	struct eval_bits
	{
	public:
		bool ierr : 1ui64;
		bool iovf : 1ui64;
		bool iunf : 1ui64;
		bool fzer : 1ui64;
		bool fsub : 1ui64;
		bool finf : 1ui64;
		bool fnan : 1ui64;
		bool fnrm : 1ui64;

		inline eval_bits
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
			fzer(fzer), fsub(fsub), finf(finf), fnan(fnan), fnrm(fnrm) {}

		inline eval_bits(const eval_bits& o) noexcept : ierr(o.ierr), iovf(o.iovf), iunf(o.iunf),
			fzer(o.fzer), fsub(o.fsub), finf(o.finf), fnan(o.fnan), fnrm(o.fnrm) {}

		inline eval_bits(eval_bits&& o) noexcept : ierr(o.ierr), iovf(o.iovf), iunf(o.iunf),
			fzer(o.fzer), fsub(o.fsub), finf(o.finf), fnan(o.fnan), fnrm(o.fnrm) {}

		inline ~eval_bits() noexcept = default;

		inline eval_bits& operator=(const eval_bits& o) noexcept
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

		inline eval_bits& operator=(eval_bits&& o) noexcept
		{
			ierr = (bool&&)o.ierr;
			iovf = (bool&&)o.iovf;
			iunf = (bool&&)o.iunf;
			fzer = (bool&&)o.fzer;
			fsub = (bool&&)o.fsub;
			finf = (bool&&)o.finf;
			fnan = (bool&&)o.fnan;
			fnrm = (bool&&)o.fnrm;

			return *this;
		}
	};

	struct eval_byte
	{
	public:
		uint8_t value;

		inline eval_byte(uint8_t value = 0ui8) noexcept : value(value) {}
		inline eval_byte(const eval_byte& o) noexcept : value(o.value) {}
		inline eval_byte(eval_byte&& o) noexcept : value(std::move(o.value)) {}
		inline ~eval_byte() noexcept = default;

		inline eval_byte& operator=(const eval_byte& o) noexcept
		{
			value = o.value;

			return *this;
		}

		inline eval_byte& operator=(eval_byte&& o) noexcept
		{
			value = std::move(o.value);

			return *this;
		}
	};

	union eval_union
	{
	public:
		eval_bits bits;
		eval_byte byte;

		inline eval_union() noexcept : byte() {}
		inline eval_union(eval_bits bits) noexcept : bits(bits) {}
		inline eval_union(eval_byte byte) noexcept : byte(byte) {}
		inline eval_union(const eval_union& o) noexcept : byte(o.byte) {}
		inline eval_union(eval_union&& o) noexcept : byte(std::move(o.byte)) {}
		inline ~eval_union() noexcept = default;

		inline eval_union& operator=(const eval_union& o) noexcept
		{
			byte = o.byte;

			return *this;
		}

		inline eval_union& operator=(eval_union&& o) noexcept
		{
			byte = std::move(o.byte);

			return *this;
		}
	};

	struct state
	{
	public:
		comp_union comp;
		eval_union eval;

		inline state(comp_union comp = comp_union(), eval_union eval = eval_union()) noexcept
			: comp(comp), eval(eval) {}
		inline state(const state& o) noexcept : comp(o.comp), eval(o.eval) {}
		inline state(state&& o) noexcept : comp(std::move(o.comp)), eval(std::move(o.eval)) {}
		inline ~state() noexcept = default;

		inline state& operator=(const state& o) noexcept
		{
			comp = o.comp;
			eval = o.eval;

			return *this;
		}

		inline state& operator=(state&& o) noexcept
		{
			comp = std::move(o.comp);
			eval = std::move(o.eval);

			return *this;
		}
	};

	enum class type : uint8_t
	{
		v0 = 0ui8,
		v1 = 1ui8
	};

	template <std::endian endianness = std::endian::native>
	union val8
	{
	public:
		int8_t si;
		uint8_t ui;

		inline val8() noexcept : ui(0ui8) {}
		inline val8(const int8_t& si) noexcept : si(si) {}
		inline val8(const uint8_t& ui) noexcept : ui(ui) {}
		inline val8(const val8& o) noexcept : ui(o.ui) {}
		inline val8(val8&& o) noexcept : ui(std::move(o.ui)) {}
		inline ~val8() noexcept = default;

		inline val8& operator=(const val8& o) noexcept
		{
			ui = o.ui;

			return *this;
		}

		inline val8& operator=(val8&& o) noexcept
		{
			ui = std::move(o.ui);

			return *this;
		}
	};

	template <std::endian endianness = std::endian::native>
	struct val16half;

	template <>
	struct val16half<std::endian::big>
	{
	public:
		val8<std::endian::big> hi;
		val8<std::endian::big> lo;
	};

	template <>
	struct val16half<std::endian::little>
	{
	public:
		val8<std::endian::little> lo;
		val8<std::endian::little> hi;
	};

	template <std::endian endianness = std::endian::native>
	union val16
	{
	public:
		static constexpr uint64_t v8_count = 2ui64;

		int16_t si;
		uint16_t ui;

		val8<endianness> v8[v8_count];

		val16half<endianness> half;

		inline val16() noexcept : ui(0ui16) {}
		inline val16(const int16_t& si) noexcept : si(si) {}
		inline val16(const uint16_t& ui) noexcept : ui(ui) {}
		inline val16(const val16& o) noexcept : ui(o.ui) {}
		inline val16(val16&& o) noexcept : ui(std::move(o.ui)) {}
		inline ~val16() noexcept = default;

		inline val16& operator=(const val16& o) noexcept
		{
			ui = o.ui;

			return *this;
		}

		inline val16& operator=(val16&& o) noexcept
		{
			ui = std::move(o.ui);

			return *this;
		}
	};

	template <std::endian endianness = std::endian::native>
	struct val32half;

	template <>
	struct val32half<std::endian::big>
	{
	public:
		val16<std::endian::big> hi;
		val16<std::endian::big> lo;
	};

	template <>
	struct val32half<std::endian::little>
	{
	public:
		val16<std::endian::little> lo;
		val16<std::endian::little> hi;
	};

	template <std::endian endianness = std::endian::native>
	union val32
	{
	public:
		static constexpr uint64_t v8_count = 4ui64;
		static constexpr uint64_t v16_count = 2ui64;

		float f;

		int32_t si;
		uint32_t ui;

		val8<endianness> v8[v8_count];
		val16<endianness> v16[v16_count];

		val32half<endianness> half;

		inline val32() noexcept : ui(0ui32) {}
		inline val32(const float& f) noexcept : f(f) {}
		inline val32(const int32_t& si) noexcept : si(si) {}
		inline val32(const uint32_t& ui) noexcept : ui(ui) {}
		inline val32(const val32& o) noexcept : ui(o.ui) {}
		inline val32(val32&& o) noexcept : ui(std::move(o.ui)) {}
		inline ~val32() noexcept = default;

		inline val32& operator=(const val32& o) noexcept
		{
			ui = o.ui;

			return *this;
		}

		inline val32& operator=(val32&& o) noexcept
		{
			ui = std::move(o.ui);

			return *this;
		}
	};

	template <std::endian endianness = std::endian::native>
	struct val64half;

	template <>
	struct val64half<std::endian::big>
	{
	public:
		val32<std::endian::big> hi;
		val32<std::endian::big> lo;
	};

	template <>
	struct val64half<std::endian::little>
	{
	public:
		val32<std::endian::little> lo;
		val32<std::endian::little> hi;
	};

	template <std::endian endianness = std::endian::native>
	union val64
	{
	public:
		static constexpr uint64_t v8_count = 8ui64;
		static constexpr uint64_t v16_count = 4ui64;
		static constexpr uint64_t v32_count = 2ui64;

		double f;

		int64_t si;
		uint64_t ui;

		val8<endianness> v8[v8_count];
		val16<endianness> v16[v16_count];
		val32<endianness> v32[v32_count];

		val64half<endianness> half;

		inline val64() noexcept : ui(0ui64) {}
		inline val64(const double& f) noexcept : f(f) {}
		inline val64(const int64_t& si) noexcept : si(si) {}
		inline val64(const uint64_t& ui) noexcept : ui(ui) {}
		inline val64(const val64& o) noexcept : ui(o.ui) {}
		inline val64(val64&& o) noexcept : ui(std::move(o.ui)) {}
		inline ~val64() noexcept = default;

		inline val64& operator=(const val64& o) noexcept
		{
			ui = o.ui;

			return *this;
		}

		inline val64& operator=(val64&& o) noexcept
		{
			ui = std::move(o.ui);

			return *this;
		}
	};

	template <std::endian endianness = std::endian::native>
	union state_val
	{
	public:
		state __state;
		val16<endianness> __value;

		inline state_val() noexcept : __value() {}
		inline state_val(const state& s) noexcept : __state(s) {}
		inline state_val(const val16<endianness>& v) noexcept : __value(v) {}
		inline state_val(const state_val& o) noexcept : __value(o.__value) {}
		inline state_val(state_val&& o) noexcept : __value(std::move(o.__value)) {}
		inline ~state_val() noexcept = default;

		inline state_val& operator=(const state_val& o) noexcept
		{
			__value = o.__value;

			return *this;
		}

		inline state_val& operator=(state_val&& o) noexcept
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
		= std::same_as<T, val64<std::endian::big>> || std::same_as<T, val64<std::endian::little>>
		|| std::same_as<T, val32<std::endian::big>> || std::same_as<T, val32<std::endian::little>>
		|| std::same_as<T, val16<std::endian::big>> || std::same_as<T, val16<std::endian::little>>
		|| std::same_as<T, val8<std::endian::big>> || std::same_as<T, val8<std::endian::little>>;

	template <typename T>
	concept DIPTR = std::same_as<T, ptrdiff_t>;

	template <typename T>
	concept SIPTR = std::same_as<T, intptr_t>;

	template <typename T>
	concept UIPTR = std::same_as<T, uintptr_t>;

	template <typename T>
	concept IPTR = SIPTR<T> || UIPTR<T>;

	template <typename T>
	concept VALUE_IPTR = VALUE<T> || IPTR<T>;
}