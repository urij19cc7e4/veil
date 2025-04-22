#include "native.hpp"

#include "arch_define.hpp"

using namespace interpreter;
using namespace std;

inline void nativecall::__cpy_obj(const nativecall& o)
{
	_abi_type = o._abi_type;
	_arg_count = o._arg_count;

	_arg_io_type = (arg_type*)new arg_type[_arg_count];
	memcpy(_arg_io_type, o._arg_io_type, (uint64_t)sizeof(arg_type) * _arg_count);
	_ret_io_type = o._ret_io_type;

	_arg_type = (ffi_type**)new ffi_type*[_arg_count];
	memcpy(_arg_type, o._arg_type, (uint64_t)sizeof(ffi_type*) * _arg_count);
	_ret_type = o._ret_type;

	_arg_val = (void**)new void*[_arg_count];
	for (uint64_t i = 0ui64; i < _arg_count; ++i)
		if (o._arg_val[i] == (void*)nullptr)
			_arg_val[i] = (void*)nullptr;
		else
		{
			_arg_val[i] = get_val(_arg_io_type[i]);
			memcpy(_arg_val[i], o._arg_val[i], get_size(_arg_io_type[i]));
		}
	_ret_val = o._ret_val;

	_cif = o._cif;
}

inline void nativecall::__def_obj() noexcept
{
	_abi_type = abi_type::NONE;
	_arg_count = 0ui64;

	_arg_io_type = (arg_type*)nullptr;
	_ret_io_type = arg_type();

	_arg_type = (ffi_type**)nullptr;
	_ret_type = (ffi_type*)nullptr;

	_arg_val = (void**)nullptr;
	_ret_val = (ffi_arg)0;

	memset(&_cif, 0ui8, (uint64_t)sizeof(ffi_cif));
}

inline void nativecall::__del_obj() noexcept
{
	if (_arg_io_type != (arg_type*)nullptr)
		delete[] _arg_io_type;

	if (_arg_type != (ffi_type**)nullptr)
		delete[] _arg_type;

	if (_arg_val != (void**)nullptr)
	{
		for (uint64_t i = 0ui64; i < _arg_count; ++i)
			if (_arg_val[i] != (void*)nullptr)
				delete _arg_val[i];

		delete[] _arg_val;
	}
}

inline void nativecall::__mov_obj(nativecall&& o) noexcept
{
	_abi_type = move(o._abi_type);
	_arg_count = move(o._arg_count);

	_arg_io_type = move(o._arg_io_type);
	_ret_io_type = move(o._ret_io_type);

	_arg_type = move(o._arg_type);
	_ret_type = move(o._ret_type);

	_arg_val = move(o._arg_val);
	_ret_val = move(o._ret_val);

	_cif = move(o._cif);
}

inline ffi_abi nativecall::get_abi(abi_type type)
{
	switch (type)
	{
	case abi_type::NONE: return FFI_DEFAULT_ABI;
	#ifdef __FFI_AIX
	case abi_type::AIX: return FFI_AIX;
	#endif
	#ifdef __FFI_ARC64
	case abi_type::ARC64: return FFI_ARC64;
	#endif
	#ifdef __FFI_ARCOMPACT
	case abi_type::ARCOMPACT: return FFI_ARCOMPACT;
	#endif
	#ifdef __FFI_COMPAT_GCC_SYSV
	case abi_type::COMPAT_GCC_SYSV: return FFI_COMPAT_GCC_SYSV;
	#endif
	#ifdef __FFI_COMPAT_LINUX
	case abi_type::COMPAT_LINUX: return FFI_COMPAT_LINUX;
	#endif
	#ifdef __FFI_COMPAT_LINUX64
	case abi_type::COMPAT_LINUX64: return FFI_COMPAT_LINUX64;
	#endif
	#ifdef __FFI_COMPAT_LINUX_SOFT_FLOAT
	case abi_type::COMPAT_LINUX_SOFT_FLOAT: return FFI_COMPAT_LINUX_SOFT_FLOAT;
	#endif
	#ifdef __FFI_COMPAT_SYSV
	case abi_type::COMPAT_SYSV: return FFI_COMPAT_SYSV;
	#endif
	#ifdef __FFI_DARWIN
	case abi_type::DARWIN: return FFI_DARWIN;
	#endif
	#ifdef __FFI_EABI
	case abi_type::EABI: return FFI_EABI;
	#endif
	#ifdef __FFI_EFI64
	case abi_type::EFI64: return FFI_EFI64;
	#endif
	#ifdef __FFI_ELFBSD
	case abi_type::ELFBSD: return FFI_ELFBSD;
	#endif
	#ifdef __FFI_FASTCALL
	case abi_type::FASTCALL: return FFI_FASTCALL;
	#endif
	#ifdef __FFI_GNUW64
	case abi_type::GNUW64: return FFI_GNUW64;
	#endif
	#ifdef __FFI_LINUX
	case abi_type::LINUX: return FFI_LINUX;
	#endif
	#ifdef __FFI_LP64D
	case abi_type::LP64D: return FFI_LP64D;
	#endif
	#ifdef __FFI_LP64F
	case abi_type::LP64F: return FFI_LP64F;
	#endif
	#ifdef __FFI_LP64S
	case abi_type::LP64S: return FFI_LP64S;
	#endif
	#ifdef __FFI_MS_CDECL
	case abi_type::MS_CDECL: return FFI_MS_CDECL;
	#endif
	#ifdef __FFI_N32
	case abi_type::N32: return FFI_N32;
	#endif
	#ifdef __FFI_N32_SOFT_FLOAT
	case abi_type::N32_SOFT_FLOAT: return FFI_N32_SOFT_FLOAT;
	#endif
	#ifdef __FFI_N64
	case abi_type::N64: return FFI_N64;
	#endif
	#ifdef __FFI_N64_SOFT_FLOAT
	case abi_type::N64_SOFT_FLOAT: return FFI_N64_SOFT_FLOAT;
	#endif
	#ifdef __FFI_O32
	case abi_type::O32: return FFI_O32;
	#endif
	#ifdef __FFI_O32_SOFT_FLOAT
	case abi_type::O32_SOFT_FLOAT: return FFI_O32_SOFT_FLOAT;
	#endif
	#ifdef __FFI_OBSD
	case abi_type::OBSD: return FFI_OBSD;
	#endif
	#ifdef __FFI_OSF
	case abi_type::OSF: return FFI_OSF;
	#endif
	#ifdef __FFI_PA32
	case abi_type::PA32: return FFI_PA32;
	#endif
	#ifdef __FFI_PA64
	case abi_type::PA64: return FFI_PA64;
	#endif
	#ifdef __FFI_PASCAL
	case abi_type::PASCAL: return FFI_PASCAL;
	#endif
	#ifdef __FFI_REGISTER
	case abi_type::REGISTER: return FFI_REGISTER;
	#endif
	#ifdef __FFI_STDCALL
	case abi_type::STDCALL: return FFI_STDCALL;
	#endif
	#ifdef __FFI_SYSV
	case abi_type::SYSV: return FFI_SYSV;
	#endif
	#ifdef __FFI_THISCALL
	case abi_type::THISCALL: return FFI_THISCALL;
	#endif
	#ifdef __FFI_UNIX
	case abi_type::UNIX: return FFI_UNIX;
	#endif
	#ifdef __FFI_UNIX64
	case abi_type::UNIX64: return FFI_UNIX64;
	#endif
	#ifdef __FFI_V8
	case abi_type::V8: return FFI_V8;
	#endif
	#ifdef __FFI_V9
	case abi_type::V9: return FFI_V9;
	#endif
	#ifdef __FFI_VFP
	case abi_type::VFP: return FFI_VFP;
	#endif
	#ifdef __FFI_WASM32
	case abi_type::WASM32: return FFI_WASM32;
	#endif
	#ifdef __FFI_WASM32_EMSCRIPTEN
	case abi_type::WASM32_EMSCRIPTEN: return FFI_WASM32_EMSCRIPTEN;
	#endif
	#ifdef __FFI_WIN64
	case abi_type::WIN64: return FFI_WIN64;
	#endif
	default: throw runtime_error(_err_msg_wrong_abit);
	}
}

inline ffi_type* nativecall::get_type(arg_type type)
{
	switch (type.outer)
	{
	case arg_o_type::__void: return &ffi_type_void;
	case arg_o_type::__flt64: return &ffi_type_double;
	case arg_o_type::__sint64: return &ffi_type_sint64;
	case arg_o_type::__uint64: return &ffi_type_uint64;
	case arg_o_type::__flt32: return &ffi_type_float;
	case arg_o_type::__sint32: return &ffi_type_sint32;
	case arg_o_type::__uint32: return &ffi_type_uint32;
	case arg_o_type::__sint16: return &ffi_type_sint16;
	case arg_o_type::__uint16: return &ffi_type_uint16;
	case arg_o_type::__sint8: return &ffi_type_sint8;
	case arg_o_type::__uint8: return &ffi_type_uint8;
	case arg_o_type::__ptr: return &ffi_type_pointer;
	default: throw runtime_error(_err_msg_wrong_type);
	}
}

inline uint64_t nativecall::get_size(arg_type type)
{
	switch (type.outer)
	{
	case arg_o_type::__void: return 0ui64;
	case arg_o_type::__flt64: return (uint64_t)sizeof(double);
	case arg_o_type::__sint64: return (uint64_t)sizeof(int64_t);
	case arg_o_type::__uint64: return (uint64_t)sizeof(uint64_t);
	case arg_o_type::__flt32: return (uint64_t)sizeof(float);
	case arg_o_type::__sint32: return (uint64_t)sizeof(int32_t);
	case arg_o_type::__uint32: return (uint64_t)sizeof(uint32_t);
	case arg_o_type::__sint16: return (uint64_t)sizeof(int16_t);
	case arg_o_type::__uint16: return (uint64_t)sizeof(uint16_t);
	case arg_o_type::__sint8: return (uint64_t)sizeof(int8_t);
	case arg_o_type::__uint8: return (uint64_t)sizeof(uint8_t);
	case arg_o_type::__ptr: return (uint64_t)sizeof(void*);
	default: throw runtime_error(_err_msg_wrong_type);
	}
}

inline void* nativecall::get_val(arg_type type)
{
	switch (type.outer)
	{
	case arg_o_type::__void: return (void*)nullptr;
	case arg_o_type::__flt64: return (void*)new double;
	case arg_o_type::__sint64: return (void*)new int64_t;
	case arg_o_type::__uint64: return (void*)new uint64_t;
	case arg_o_type::__flt32: return (void*)new float;
	case arg_o_type::__sint32: return (void*)new int32_t;
	case arg_o_type::__uint32: return (void*)new uint32_t;
	case arg_o_type::__sint16: return (void*)new int16_t;
	case arg_o_type::__uint16: return (void*)new uint16_t;
	case arg_o_type::__sint8: return (void*)new int8_t;
	case arg_o_type::__uint8: return (void*)new uint8_t;
	case arg_o_type::__ptr: return (void*)new void*;
	default: throw runtime_error(_err_msg_wrong_type);
	}
}

inline void nativecall::init()
{
	_arg_type = (ffi_type**)new ffi_type*[_arg_count];
	_arg_val = (void**)new void*[_arg_count];

	for (uint64_t i = 0ui64; i < _arg_count; ++i)
	{
		_arg_type[i] = get_type(_arg_io_type[i]);
		_arg_val[i] = get_val(_arg_io_type[i]);
	}

	_ret_type = get_type(_ret_io_type);

	ffi_prep_cif(&_cif, get_abi(_abi_type), (unsigned int)_arg_count, _ret_type, _arg_type);
}

nativecall::nativecall(const uint8_t*& opptr)
{
	_abi_type = (abi_type)fetch_without_check<val8<>>(opptr).ui;
	_arg_count = (uint64_t)fetch_without_check<val8<>>(opptr).ui;

	_arg_io_type = (arg_type*)new arg_type[_arg_count];
	fetch_without_check<val8<>>(opptr, (uint8_t*)_arg_io_type, _arg_count);
	_ret_io_type = arg_type_val<>(fetch_without_check<val8<>>(opptr)).__arg_type;
}

nativecall::nativecall(const uint8_t*& opptr, const uint8_t* bcode, const uint8_t* ecode)
{
	_abi_type = (abi_type)fetch_with_check<val8<>>(opptr, bcode, ecode).ui;
	_arg_count = (uint64_t)fetch_with_check<val8<>>(opptr, bcode, ecode).ui;

	_arg_io_type = (arg_type*)new arg_type[_arg_count];
	fetch_with_check<val8<>>(opptr, (uint8_t*)_arg_io_type, _arg_count, bcode, ecode);
	_ret_io_type = arg_type_val<>(fetch_with_check<val8<>>(opptr, bcode, ecode)).__arg_type;
}

nativecall::nativecall(const nativecall& o)
{
	__cpy_obj(o);
}

nativecall::nativecall(nativecall&& o) noexcept
{
	__mov_obj(move(o));
	o.__def_obj();
}

nativecall::~nativecall()
{
	__del_obj();
	__def_obj();
}

nativecall& nativecall::operator=(const nativecall& o)
{
	__del_obj();
	__cpy_obj(o);

	return *this;
}

nativecall& nativecall::operator=(nativecall&& o) noexcept
{
	__del_obj();
	__mov_obj(move(o));
	o.__def_obj();

	return *this;
}

void nativecall::prep(const uint8_t*& opptr)
{
	fetch_without_check<val8<>>(opptr).ui;
	fetch_without_check<val8<>>(opptr).ui;

	fetch_without_check<val8<>>(opptr, (uint8_t*)_arg_io_type, _arg_count);
	_ret_io_type = arg_type_val<>(fetch_without_check<val8<>>(opptr)).__arg_type;
}

void nativecall::prep(const uint8_t*& opptr, const uint8_t* bcode, const uint8_t* ecode)
{
	fetch_with_check<val8<>>(opptr, bcode, ecode).ui;
	fetch_with_check<val8<>>(opptr, bcode, ecode).ui;

	fetch_with_check<val8<>>(opptr, (uint8_t*)_arg_io_type, _arg_count, bcode, ecode);
	_ret_io_type = arg_type_val<>(fetch_with_check<val8<>>(opptr, bcode, ecode)).__arg_type;
}

nativehub::nativehub(uint64_t size) : _cache(), _queue(), _count(0ui64), _size(size) {}

nativehub::nativehub(nativehub&& o) noexcept
	: _cache(move(o._cache)), _queue(move(o._queue)), _count(move(o._count)), _size(move(o._size)) {}

nativehub& nativehub::operator=(nativehub&& o) noexcept
{
	_cache = move(o._cache);
	_queue = move(o._queue);

	_count = move(o._count);
	_size = move(o._size);

	return *this;
}