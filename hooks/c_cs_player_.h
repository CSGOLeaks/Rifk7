#pragma once

#include "../sdk/c_cs_player.h"
#include "../utils/c_hook.h"

class c_cs_player_
{
	typedef c_qangle*(__thiscall* eye_angles_t)(c_cs_player*);
	typedef void(__thiscall* build_transformations_t)(c_cs_player*, c_studio_hdr*, c_vector3d*, quaternion*, const matrix3x4&, int32_t, byte*);
	typedef void(__thiscall* standard_blending_rules_t)(c_cs_player*, c_studio_hdr*, c_vector3d*, quaternion*, float, int);
	typedef void(__thiscall* do_extra_bone_processing_t)(c_cs_player*, c_studio_hdr*, c_vector3d*, quaternion*, const matrix3x4&, uint8_t*, void*);
	typedef void(__thiscall* update_client_side_animation_t)(c_cs_player*);

public:
    static void hook();

	static void apply_to_player(c_cs_player* player);

	static int __stdcall proxy_is_player(c_cs_player* player, void* return_address, void* eax);
private:
	inline static std::unique_ptr<c_hook<uint32_t>> hk;

	inline static eye_angles_t _eye_angles;
	inline static build_transformations_t _build_transformations;
	inline static standard_blending_rules_t _standard_blending_rules;
	inline static do_extra_bone_processing_t _do_extra_bone_processing;
	inline static update_client_side_animation_t _update_client_side_animation;

	static c_qangle* __fastcall eye_angles(c_cs_player* player, uint32_t);
	static void __fastcall build_transformations(c_cs_player* player, uint32_t, c_studio_hdr* hdr, c_vector3d* pos,
		quaternion* q, const matrix3x4& transform, int32_t mask, byte* computed);
	static void __fastcall standard_blending_rules(c_cs_player* player, uint32_t, c_studio_hdr* hdr, c_vector3d* pos,
		quaternion* q, float time, int mask);
	static void __fastcall do_extra_bone_processing(c_cs_player* player, uint32_t, c_studio_hdr* hdr, c_vector3d* pos, quaternion* q,
		const matrix3x4& matrix, uint8_t* bone_computed, void* context);
	static void __fastcall update_client_side_animation(c_cs_player* player, uint32_t);
};

static bool is_player(void* eax, void* edx);
