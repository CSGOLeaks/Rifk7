#pragma once

#include "../sdk/c_engine_sound.h"

class c_engine_sound_
{
	typedef void(__thiscall* emit_sound_t)(c_engine_sound*, void*, int, int, const char *, unsigned int, const char *, float, float, int, int,
		int, const void *, const void *, void *, bool, float, int, int);

public:
	static void hook();

private:
	inline static emit_sound_t _emit_sound;

	static  void __fastcall emit_sound(c_engine_sound* thisptr, uint32_t, void* filter, int ent_index, int channel, const char* sound_entry, unsigned int sound_entry_hash,
		const char* sample, float volume, float attenuation, int seed, int flags, int pitch, const c_vector3d* origin, const c_vector3d* direction,
		void* vec_origins, bool update_positions, float sound_time, int speaker_entity, int test);
};
