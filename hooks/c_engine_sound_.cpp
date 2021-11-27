#include "c_engine_sound_.h"
#include "../utils/c_hook.h"

void c_engine_sound_::hook()
{
	static c_hook<c_engine_sound> hook(engine_sound());
	_emit_sound = hook.apply<emit_sound_t>(5, emit_sound);
}

void __fastcall c_engine_sound_::emit_sound(c_engine_sound* engine_sound, uint32_t, void* filter, int ent_index, int channel, const char* sound_entry,
	unsigned int sound_entry_hash, const char* sample, float volume, float attenuation, int seed, int flags, int pitch, const c_vector3d* origin, const c_vector3d* direction,
	void* vec_origins, bool update_positions, float sound_time, int speaker_entity, int test)
{
	if (strstr(sample, _("weapon")) && (strstr(sample, _("draw")) || strstr(sample, _("deploy"))))
	{
		static uint32_t prev_hash = 0;
		const uint32_t hash = fnv1a_rt(sample);

		if (prev_hash == hash)
			return;

		prev_hash = hash;
	}

	_emit_sound(engine_sound, filter, ent_index, channel, sound_entry, sound_entry_hash, sample, volume, attenuation, seed, flags,
		pitch, origin, direction, vec_origins, update_positions, sound_time, speaker_entity, test);
}
