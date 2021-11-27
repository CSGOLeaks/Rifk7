#pragma once

#include "macros.h"
#include "c_color.h"

class convar
{
protected:
	~convar() = default;
public:
	vfunc(14, set_value(const char* v), void( __thiscall*)(convar*, const char*))(v)
	vfunc(15, set_value(const float v), void( __thiscall*)(convar*, float))(v)
	vfunc(16, set_value(const int32_t v), void( __thiscall*)(convar*, int32_t))(v)

	float get_float()
	{
		const auto val = *reinterpret_cast<uint32_t*>(&value);
		auto xored = static_cast<uint32_t>(val ^ reinterpret_cast<uint32_t>(this));
		return *reinterpret_cast<float*>(&xored);
	}

	int32_t get_int()
	{
		const auto val = *reinterpret_cast<uint32_t*>(&n_val);
		auto xored = static_cast<uint32_t>(val ^ reinterpret_cast<uint32_t>(this));
		return *reinterpret_cast<int32_t*>(&xored);
	}

	// ReSharper disable CppUninitializedNonStaticDataMember
	char pad0[0x4]{};
	convar* next{};
	int32_t registered{};
	char* name{};
	char* help_string{};
	int32_t flags{};
	char pad1[0x4]{};
	convar* parent{};
	char* default_value{};
	char* string{};
	int32_t string_length{};
	float value{};
	int32_t n_val{};
	int32_t has_min{};
	float min_val{};
	int32_t has_max{};
	float max_val{};
	void* change_callbacks{};
	// ReSharper restore CppUninitializedNonStaticDataMember
};

class concommand
{
public:
	vfunc(14, dispatch(), void(__thiscall*)(concommand*, void*))(nullptr)
};

struct c_console_color
{
	unsigned char r, g, b, a;
};

class c_cvar
{
protected:
	~c_cvar() = default;
public:
	template <typename... args>
	void console_color_printf(const bool developer, const c_color& color, const char* format, args... arg)
	{
		static const auto print_developer = reinterpret_cast<void(__stdcall*)(const c_console_color&, char const*)>(
			sig("engine.dll", "55 8B EC 81 EC 10 08 00 00 80 3D"));

		const c_console_color cl = { static_cast<unsigned char>(color.red), static_cast<unsigned char>(color.green),
			static_cast<unsigned char>(color.blue), static_cast<unsigned char>(color.alpha) };

		if (developer)
		{
			char buf[100];
			snprintf(buf, sizeof(buf), format, arg...);

			print_developer(cl, buf);
		}

		reinterpret_cast<void(*)(c_cvar*, const c_console_color&, const char*, ...)>(
			(*reinterpret_cast<uint32_t**>(this))[25])(this, cl, format, arg...);
	}

	vfunc(16, find_var(const char* name), convar*(__thiscall*)(c_cvar*, const char*))(name)
	vfunc(18, find_command(const char* name), concommand*(__thiscall*)(c_cvar*, const char*))(name)
};

interface_var(c_cvar, cvar, "vstdlib.dll", "VEngineCvar")
