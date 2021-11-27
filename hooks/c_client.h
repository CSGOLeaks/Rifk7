#pragma once

#include "../sdk/c_base_client.h"

class c_client
{
	typedef void(__thiscall* shutdown_t)(c_base_client*);
	typedef void(__thiscall* level_init_pre_entity_t)(c_base_client*, const char*);
	typedef void(__thiscall* create_move_t)(c_base_client*, int, float, bool);
	typedef void(__thiscall* frame_stage_notify_t)(c_base_client*, clientframestage);
	
public:
    static void hook();

	inline static struct {
		c_qangle aim_punch;
		c_vector3d aim_punch_vel, view_offset;
	} recoil_info[64] = { };

	static void __stdcall create_move(int sequence_number, float input_sample_frametime, bool active, bool& sendpacket);
private:
	inline static shutdown_t _shutdown;
	inline static level_init_pre_entity_t _level_init_pre_entity;
	inline static create_move_t _create_move;
	inline static frame_stage_notify_t _frame_stage_notify;

	static void __fastcall shutdown(c_base_client* client, uint32_t);
	static void __fastcall level_init_pre_entity(c_base_client* client, uint32_t, const char* map_name);
	static void __fastcall frame_stage_notify(c_base_client* client, uint32_t, clientframestage stage);
};

static void create_move(int sequence_number, float input_sample_frametime, bool active);
