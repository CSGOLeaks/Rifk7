#pragma once

#include "macros.h"

struct player_info {
private:
	__int64 unknown = 0;
public:
	union
	{
		__int64 steam_id64;
		struct
		{
			__int32 xuid_low;
			__int32 xuid_high;
		};
	};

	char name[128];
	int user_id;
	char steam_id[20];
private:
	char pad[0x10]{};
	unsigned long steam_id_2 = 0;
public:
	char friends_name[128];
	bool fakeplayer;
	bool ishltv;
	unsigned int customfiles[4];
	unsigned char filesdownloaded;
};

class c_event_info
{
public:
	uint16_t class_id; //0x0000 0 implies not in use
	char pad_0002[2]; //0x0002 
	float fire_delay; //0x0004 If non-zero, the delay time when the event should be fired ( fixed up on the client )
	char pad_0008[4]; //0x0008
	void* client_class; //0x000C
	void* data; //0x0010 Raw event data
	char pad_0014[48]; //0x0014
}; //Size: 0x0044

class c_engine_client {
protected:
	~c_engine_client() = default;
	vfunc(18, get_view_angles(c_qangle& angle), void(__thiscall*)(c_engine_client*, c_qangle&))(angle)
public:
	vfunc(8, get_player_info(const uint32_t index, player_info* info), bool(__thiscall*)(c_engine_client*, uint32_t, player_info*))(index, info)
	vfunc(9, get_player_for_user_id(const int index), int(__thiscall*)(c_engine_client*, int))(index)
	vfunc(12, get_local_player(), uint32_t(__thiscall*)(c_engine_client*))()
	vfunc(19, set_view_angles(c_qangle& angle), void(__thiscall*)(c_engine_client*, c_qangle&))(angle)
	vfunc(20, get_max_clients(), uint32_t(__thiscall*)(c_engine_client*))()
	vfunc(26, is_ingame(), bool(__thiscall*)(c_engine_client*))()
	vfunc(27, is_connected(), bool(__thiscall*)(c_engine_client*))()
	vfunc(114, clientcmd_unrestricted(const char* str, const char* flag), void(__thiscall*)(c_engine_client*, const char*, const char*))(str, flag)

	c_qangle get_view_angles()
	{
		c_qangle angle;
		get_view_angles(angle);
		return angle;
	}
};

interface_var(c_engine_client, engine_client, "engine.dll", "VEngineClient")
