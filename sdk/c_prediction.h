#pragma once

#include "macros.h"
#include "c_move_helper.h"
#include "c_game_movement.h"
#include "c_global_vars_base.h"

class c_prediction
{
public:
	vfunc(13, set_local_view_angles(c_qangle& angle), void(__thiscall*)(c_prediction*, c_qangle&))(angle)
	vfunc(14, in_prediction(), bool(__thiscall*)(c_prediction*))()
	vfunc(20, setup_move(c_base_entity* player, c_user_cmd* cmd, c_move_helper* helper, c_move_data* data),
		void(__thiscall*)(c_prediction*, c_base_entity*, c_user_cmd*, c_move_helper*, c_move_data*))(player, cmd, helper, data)
	vfunc(21, finish_move(c_base_entity* player, c_user_cmd* cmd, c_move_data* data),
		void(__thiscall*)(c_prediction*, c_base_entity*, c_user_cmd*, c_move_data*))(player, cmd, data)

	inline c_global_vars_base* get_unpredicted_globals() {
		if (in_prediction())
			return reinterpret_cast<c_global_vars_base*>(uint32_t(this) + 0x4c);

		return global_vars_base;
	}
};

interface_var(c_prediction, prediction, "client_panorama.dll", "VClientPrediction")
