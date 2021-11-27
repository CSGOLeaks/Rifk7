#pragma once

#include <unordered_map>
#include "c_singleton.h"
#include "../sdk/recv.h"

class c_netvar : public c_singleton<c_netvar>
{
	struct netvar
	{
		size_t offset;
		recv_prop* prop;
	};

public:
	c_netvar();

	std::unordered_map<uint32_t, std::unordered_map<uint32_t, netvar>> netvars;
private:
	void write_prop_to_map(const char* name, recv_table* table, size_t offset = 0);
};

#define netvars c_netvar::instance()->netvars
