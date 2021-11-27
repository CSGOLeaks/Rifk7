#include "c_mdl_cache.h"
#include "../utils/c_memory.h"
#include "../utils/c_hook.h"
#include "../hacks/c_animation_system.h"
#include <intrin.h>

void c_mdl_cache::hook()
{
	static c_hook<void> hook(**reinterpret_cast<uint32_t***>(uint32_t(sig("client_panorama.dll", "8B 3D ? ? ? ? 8B D9 8B CF")) + 2));
	_unknown_fn = hook.apply<unknown_fn_t>(33, unknown_fn);
}

bool __fastcall c_mdl_cache::unknown_fn(void* mdl_cache, uint32_t)
{
	static auto return_to_setup_velocity = sig("client_panorama.dll", "8B 0D ? ? ? ? F3 0F 7E 86");
	static auto return_to_setup_aim_matrix = sig("client_panorama.dll", "F3 0F 10 8B ? ? ? ? 0F 57 C0 0F 2F C1 F3 0F 10 2D");
	
	if (engine_client()->is_ingame())
	{
		if (_ReturnAddress() == return_to_setup_velocity && animation_system->last_process_state)
			animation_system->last_process_state->delta_time = 0.f;
		
		if (_ReturnAddress() == return_to_setup_aim_matrix && animation_system->last_process_state)
		{
			animation_system->last_process_state->delta_time
				= fmaxf(global_vars_base->curtime - animation_system->last_process_state->last_client_side_animation_update_time, 0.f);

			if (animation_system->last_process_state->speed <= 0.f)
			{
				animation_system->last_process_state->time_since_started_moving = 0.f;
				animation_system->last_process_state->time_since_stopped_moving
					= animation_system->last_process_state->delta_time + animation_system->last_process_state->time_since_stopped_moving;
			}
			else
			{
				animation_system->last_process_state->time_since_stopped_moving = 0.f;
				animation_system->last_process_state->time_since_started_moving
					= animation_system->last_process_state->time_since_started_moving + animation_system->last_process_state->delta_time;
			}
		}
	}

	return _unknown_fn(mdl_cache);
}
