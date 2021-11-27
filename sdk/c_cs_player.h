#pragma once

#include "c_base_player.h"
#include "c_engine_client.h"
#include <optional>
#include "c_csgo_player_animstate.h"
#include "c_user_cmd.h"

class c_studio_hdr;

class c_cs_player : public c_base_player
{
	vfunc(279, weapon_shoot_position(c_vector3d& pos), void(__thiscall*)(c_cs_player*, c_vector3d&))(pos)

		inline static c_cs_player* local_player = 0;
public:
	enum class hitbox
	{
		head,
		neck,
		pelvis,
		body,
		thorax,
		chest,
		upper_chest,
		left_thigh,
		right_thigh,
		left_calf,
		right_calf,
		left_foot,
		right_foot,
		left_hand,
		right_hand,
		left_upper_arm,
		left_forearm,
		right_upper_arm,
		right_forearm,
		max
	};

	inline static const hitbox hitboxes[] = {
		hitbox::head,
		hitbox::neck,
		hitbox::pelvis,
		hitbox::body,
		hitbox::thorax,
		hitbox::chest,
		hitbox::upper_chest,
		hitbox::right_thigh,
		hitbox::left_thigh,
		hitbox::right_calf,
		hitbox::left_calf,
		hitbox::right_foot,
		hitbox::left_foot,
		hitbox::right_hand,
		hitbox::left_hand,
		hitbox::right_upper_arm,
		hitbox::right_forearm,
		hitbox::left_upper_arm,
		hitbox::left_forearm
	};

	inline static const hitbox hitboxes_aiming[] = {
		hitbox::head,
		hitbox::neck,
		hitbox::pelvis,
		hitbox::upper_chest,
		hitbox::right_calf,
		hitbox::left_calf,
		hitbox::right_foot,
		hitbox::left_foot
	};

	inline static const hitbox hitboxes_baim[] = {
		hitbox::pelvis,
		hitbox::right_calf,
		hitbox::left_calf,
		hitbox::right_foot,
		hitbox::left_foot
	};

	static uint32_t* get_vtable()
	{
		static const auto table = reinterpret_cast<uint32_t>(sig("client_panorama.dll",
			"55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C")) + 0x47;
		return *reinterpret_cast<uint32_t**>(table);
	}

	static void set_local_player(c_cs_player* player);
	static c_cs_player* get_local_player();

	inline static auto in_call_to_weapon_shoot = false;
	inline static auto can_not_shoot_due_to_cock = false;

	netvar(get_view_punch_angle(), c_vector3d, "CCSPlayer", "m_viewPunchAngle")
		netvar(get_punch_angle(), c_vector3d, "CCSPlayer", "m_aimPunchAngle")
		netvar(get_punch_angle_vel(), c_vector3d, "CCSPlayer", "m_aimPunchAngleVel")
		netvar(get_view_offset(), c_vector3d, "CCSPlayer", "m_vecViewOffset_0")
		netvar(get_health(), int32_t, "CCSPlayer", "m_iHealth")
		netvar(get_armor(), int32_t, "CCSPlayer", "m_ArmorValue")
		netvar(get_eye_angles(), c_qangle, "CCSPlayer", "m_angEyeAngles")
		netvar(get_lby(), float, "CCSPlayer", "m_flLowerBodyYawTarget")
		netvar(is_scoped(), bool, "CCSPlayer", "m_bIsScoped")
		netvar(has_helmet(), bool, "CCSPlayer", "m_bHasHelmet")
		netvar(has_heavy_armor(), bool, "CCSPlayer", "m_bHasHeavyArmor")
		netvar(get_observer_mode(), int32_t, "CCSPlayer", "m_iObserverMode")
		netvar(get_flash_alpha(), float, "CCSPlayer", "m_flFlashMaxAlpha")
		netvar(get_gun_game_immunity(), bool, "CCSPlayer", "m_bGunGameImmunity")
		netprop(get_lby_prop(), "CCSPlayer", "m_flLowerBodyYawTarget")

		datamap(get_abs_rotation(), c_qangle, "m_angAbsRotation")

		offset(get_think_tick(), int32_t, 0x40)
		// find next 2 in cs player setup bones
		offset(get_occlusion_flags(), uint32_t, 0xA28)
		offset(get_occlusion_framecount(), uint32_t, 0xA30)
		// find next 5 in base entity setup bones
		offset(get_bone_array_for_write(), matrix3x4*, 0x26A8)
		offset(get_readable_bones(), uint32_t, 0x26AC)
		offset(get_writable_bones(), uint32_t, 0x26B0)
		offset(get_most_recent_model_bone_counter(), uint32_t, 0x2690)
		offset(get_last_bone_setup_time(), float, 0x2924)
		// those are easy to spot in cs player spawn (call to reset and set to curtime)
		offset(get_anim_state(), c_csgo_player_anim_state*, 0x3900)
		offset(get_spawn_time(), float, 0xA350)
		// next two are easy to find in almost any subroutine of setup bones
		offset(get_model_hdr(), c_studio_hdr*, 0x294C)
		offset(get_bone_cache(), matrix3x4**, 0x2910)

		vfunc(138, think(), void(__thiscall*)(void*))()
		vfunc(221, update_clientside_anim(), void(__thiscall*)(c_cs_player*))()
		vfunc(314, pre_think(), void(__thiscall*)(void*))()
		vfunc(315, post_think(), void(__thiscall*)(void*))()

		bool is_local_player() const;
	player_info get_info() const;

	void run_pre_think();
	void run_think();

	std::optional<int> get_hitbox_bone_attachment(hitbox id) const;
	std::optional<c_vector3d> get_hitbox_position(hitbox id, matrix3x4* bones) const;
	c_vector3d get_shoot_position();
	bool is_shooting(c_user_cmd* cmd, float time);
	bool can_shoot(c_user_cmd* cmd, float time, bool manual_shot = false);

	bool is_enemy();
	bool is_visible(matrix3x4* bones) const;

	void draw_hitboxes(matrix3x4* matrix) const;
};
