#include <iostream>

#include "dispatcher.hpp"

using namespace interpreter;
using namespace std;

uint8_t code[] =
{
	(uint8_t)op::s_load_64,
	(uint8_t)8i8,
	(uint8_t)op::s_load_64,
	(uint8_t)0i8,
	(uint8_t)op::udiv_64,
	(uint8_t)op::s_store_64,
	(uint8_t)0i8,
	(uint8_t)op::exit,
};

int main()
{
	uint64_t a, b, c;
	cin >> a >> b;
	cout << "\n";

	dispatcher dis(code, (1ui64 << 20ui64));

	dis.push_val(val64(a));
	dis.push_val(val64(b));

	dis.init();
	dis.loop();
	dis.exit();

	c = dis.pop_val<val64>().ui;
	cout << c << "\n";
}