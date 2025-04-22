#include <iostream>

#include "dispatcher.hpp"

using namespace interpreter;
using namespace std;

#include <vector>
#include "op_asm.hpp"

template <size_t N>
constexpr void insertArrayToVector(std::vector<uint8_t>& vec, const std::array<uint8_t, N>& arr)
{
	for (const auto& elem : arr)
	{
		vec.push_back(elem);
	}
}

uint64_t fibonacci(uint64_t n)
{
	uint64_t prev1 = 0; // Represents F(n-2)
	uint64_t prev2 = 1; // Represents F(n-1)
	uint64_t current = 0;

	for (uint64_t i = 3ui64; i <= n; ++i)
	{
		current = prev1 + prev2; // F(n) = F(n-1) + F(n-2)
		prev1 = prev2;          // Shift F(n-1) to F(n-2)
		prev2 = current;        // Shift F(n) to F(n-1)
	}

	return current;
}

#include <chrono>

int main()
{
	auto start_1 = chrono::high_resolution_clock::now();
	uint64_t t = fibonacci(1000000000ui64);
	auto end_1 = chrono::high_resolution_clock::now();

	uint64_t a, c;
	cin >> a;
	cout << "\n";
	cout << t;

	vector<uint8_t> code;
	code.reserve(1ui64 << 16ui64);

	insertArrayToVector(code, op_asm::__push<val64<>, endian::native>(val64<>(0ui64)));
	insertArrayToVector(code, op_asm::__push<val64<>, endian::native>(val64<>(1ui64)));

	uint64_t loop_start = code.size();

	// check
	insertArrayToVector(code, op_asm::__s_load<val64<>, endian::native>(val8<>((uint8_t)(0ui8))));
	insertArrayToVector(code, op_asm::__push<val64<>, endian::native>(val64<>(2ui64)));
	insertArrayToVector(code, op_asm::__ucmp<val64<>, endian::native>());
	insertArrayToVector(code, op_asm::__jmp_ne<val8<>, endian::native>(val8((uint8_t)4ui8)));

	// end
	insertArrayToVector(code, op_asm::__s_store<val64<>, endian::native>(val8<>((uint8_t)(0ui8))));
	insertArrayToVector(code, op_asm::__pop<val64<>, endian::native>());
	insertArrayToVector(code, op_asm::__exit<endian::native>());

	// summ
	insertArrayToVector(code, op_asm::__s_load<val64<>, endian::native>(val8<>((uint8_t)(~0ui8))));
	insertArrayToVector(code, op_asm::__s_load<val64<>, endian::native>(val8<>((uint8_t)(~8ui8))));
	insertArrayToVector(code, op_asm::__uadd<val64<>, endian::native>());

	// -2 = -1 = 0
	insertArrayToVector(code, op_asm::__s_load<val64<>, endian::native>(val8<>((uint8_t)(~8ui8))));
	insertArrayToVector(code, op_asm::__s_store<val64<>, endian::native>(val8<>((uint8_t)(~0ui8))));
	insertArrayToVector(code, op_asm::__s_store<val64<>, endian::native>(val8<>((uint8_t)(~8ui8))));

	// dec
	insertArrayToVector(code, op_asm::__s_load<val64<>, endian::native>(val8<>((uint8_t)(0ui8))));
	insertArrayToVector(code, op_asm::__udec<val64<>, endian::native>());
	insertArrayToVector(code, op_asm::__s_store<val64<>, endian::native>(val8<>((uint8_t)(0ui8))));

	uint64_t loop_end = code.size();

	// loop start
	insertArrayToVector(code, op_asm::__jmp<val8<>, endian::native>(val8<>((int8_t)((int8_t)loop_start - (int8_t)loop_end - 2i8))));

	dispatcher dis(code.data(), code.data() + code.size(), 16ui64, (1ui64 << 20ui64));

	dis.push_val(val64<>(a));

	auto start_2 = chrono::high_resolution_clock::now();
	dis.init();
	dis.loop();
	dis.exit();
	auto end_2 = chrono::high_resolution_clock::now();

	cout << "1 TIME: " << chrono::duration_cast<chrono::milliseconds>(end_1 - start_1).count() << " ms\n";
	cout << "2 TIME: " << chrono::duration_cast<chrono::milliseconds>(end_2 - start_2).count() << " ms\n";

	c = dis.pop_val<val64<>>().ui;
	cout << c << "\n";
}