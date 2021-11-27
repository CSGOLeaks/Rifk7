#pragma once

#include "../sdk/c_prediction.h"

class c_prediction_
{
	typedef bool(__thiscall* in_prediction_t)(c_prediction*);
	typedef void(__thiscall* run_command_t)(c_prediction*, c_cs_player*, c_user_cmd*, c_move_helper*);
	
public:
	static void hook();

private:
	inline static in_prediction_t _in_prediction;
	inline static run_command_t _run_command;

	static bool __fastcall in_prediction(c_prediction* prediction, uint32_t);
	static void __fastcall run_command(c_prediction* prediction, uint32_t, c_cs_player* player, c_user_cmd* cmd, c_move_helper* helper);
};
