#pragma once
#include "../utils/c_memory.h"
#include "c_base_combat_weapon.h"

class c_weapon_system
{
protected:
	~c_weapon_system() = default;

public:
	static c_weapon_system* get()
	{
		static const auto weapon_system = *reinterpret_cast<c_weapon_system**>(
			sig("client_panorama.dll", "8B 35 ? ? ? ? FF 10 0F B7 C0") + 2);

		return weapon_system;
	}

private:
	virtual void pad0() = 0;
	virtual void pad1() = 0;

public:
	virtual c_base_combat_weapon::weapon_data* get_weapon_data(uint32_t item_definition_index) = 0;
};

#define weapon_system c_weapon_system::get()
