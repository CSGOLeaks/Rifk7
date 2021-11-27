#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "../utils/c_netvar.h"
#include "../utils/c_memory.h"
#include "datamap.h"
#include "../utils/c_log.h"

typedef void* (*create_t)();

class interface_reg {
public:
	create_t create;
	const char* name;
	interface_reg* next;
};

static std::string get_first_number(std::string const & str)
{
	std::size_t const n = str.find_first_of(_("0123456789"));
	if (n != std::string::npos)
	{
		std::size_t const m = str.find_first_not_of(_("0123456789"), n);
		return str.substr(n, m != std::string::npos ? m - n : m);
	}
	return std::string();
}

static uintptr_t get_jmp_target(uintptr_t ptr)
{
	const auto relative = *reinterpret_cast<int*>(ptr + 1);
	ptr += 5;
	ptr += relative;
	return ptr;
}

template <typename t> t* get_interface(const uint32_t module, const uint32_t inter_face)
{
	const auto jmp = memory::get_proc_address(memory::get_module_handle(module), fnv1a("CreateInterface")) + 4;
	const auto offset = *reinterpret_cast<int32_t*>(jmp + 1);
	const auto internal = jmp + offset + 5;
	const auto interface_list = **reinterpret_cast<interface_reg***>(internal + 6);

	interface_reg* highest_interface_ptr = nullptr;
	auto highest_interface_version = -1;

	for (interface_reg* current = interface_list; current; current = current->next)
	{
		char versionless_name[64] = {};
		size_t size;

		// copy until end / until we hit a digit
		for (size = 0; current->name[size] != '\0' && !isdigit(current->name[size]); ++size)
			versionless_name[size] = current->name[size];
		versionless_name[size] = '\0';

		// found it
		if (fnv1a_rt(versionless_name) == inter_face)
		{
			// lets always try to get the latest version
			// thats why the previous code did not work with stuff like "GAMEEVENTSMANAGER"
			// since there are multiple versions of it

			const auto version = atoi(get_first_number(current->name).c_str()); // NOLINT(cert-err34-c)
			if (version > highest_interface_version)
			{
				highest_interface_version = version;
				highest_interface_ptr = current;
			}
		}
	}

	if (!highest_interface_ptr)
		return nullptr;

	return reinterpret_cast<t*>(highest_interface_ptr->create());
}

inline unsigned int find_in_data_map(datamap_t* map, const uint32_t hash)
{
	while (map)
	{
		for (auto i = 0; i < map->data_num_fields; i++)
		{
			if (map->data_desc[i].field_name == nullptr)
				continue;

			if (fnv1a_rt(map->data_desc[i].field_name) == hash)
				return map->data_desc[i].field_offset[td_offset_normal];				

			if (map->data_desc[i].field_type == field_embedded)
			{
				if (map->data_desc[i].td)
				{
					unsigned int offset;
 
					if ((offset = find_in_data_map(map->data_desc[i].td, hash)) != 0)
						return offset;
				}
			}
		}
		map = map->base_map;
	}
	return 0;
}

#define interface_var(type, name, module, version) static type* name() { \
	static auto i = get_interface<type>(fnv1a(module), fnv1a(version)); \
	return i; \
}

#define member_func_args(...) (this, __VA_ARGS__ ); }
#define vfunc(index, func, sig) auto func { return reinterpret_cast<sig>((*(uint32_t**)this)[index]) member_func_args
#define netvar(func, type, table, entry) type& func { static const auto offset = netvars[fnv1a(table)][fnv1a(entry)].offset; return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset); }  // NOLINT(misc-macro-parentheses)
#define pnetvar(func, type, table, entry) type* func { static const auto offset = netvars[fnv1a(table)][fnv1a(entry)].offset; return reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset); }  // NOLINT(misc-macro-parentheses)
#define netvar_offset(func, type, table, entry, off) type& func { static const auto offset = netvars[fnv1a(table)][fnv1a(entry)].offset + off; return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset); }  // NOLINT(misc-macro-parentheses)
#define netvar_offset_ptr(func, type, table, entry, off) type& func { static const auto offset = netvars[fnv1a(table)][fnv1a(entry)].offset + off; return **reinterpret_cast<type**>(reinterpret_cast<uintptr_t>(this) + offset); }  // NOLINT(misc-macro-parentheses)
#define netprop(func, table, entry) static recv_prop* func { static const auto prop = netvars[fnv1a(table)][fnv1a(entry)].prop; return prop; }  // NOLINT(misc-macro-parentheses)
#define datamap(func, type, entry) type& func { static const auto offset = find_in_data_map(get_pred_desc_map(), fnv1a(entry)); return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset); }
#define offset(func, type, offset) type& func { return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset); }
