#pragma once
#include "../utils/c_memory.h"
#include "c_base_entity.h"

class c_move_helper
{
protected:
	~c_move_helper() = default;
public:
	static c_move_helper* get()
	{
		static const auto move_helper = **reinterpret_cast<c_move_helper***>(
			reinterpret_cast<uint32_t>(sig("client_panorama.dll", "8B 0D ? ? ? ? 8B 46 08 68")) + 2);
		return move_helper;
	}

private:
	virtual void unknown() = 0;
public:
	virtual void set_host(c_base_entity* host) = 0;
private:
	virtual void unknown1() = 0;
	virtual void unknown2() = 0;
public:
	virtual bool unknown_func() = 0;
};

#define move_helper c_move_helper::get()
