#pragma once

#include "../sdk/c_client_state.h"

class c_client_state_
{
	typedef void(__thiscall* packet_start_t)(c_client_state*, int, int);

public:
	static void hook();

	inline static std::vector<int32_t> cmds = {};
private:
	inline static packet_start_t _packet_start;

	static void __fastcall packet_start(c_client_state* state, uint32_t, int incoming_sequence, int outgoing_acknowledged);
};
