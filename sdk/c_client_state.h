#pragma once

#include <cstdint>
#include "c_net_channel.h"
#include "c_vector3d.h"
#include "../utils/c_memory.h"

class c_client_state {
	class c_clock_drift_mgr {
	public:
		float clock_offsets[17];
		uint32_t cur_clock_offset;
		uint32_t server_tick;
		uint32_t client_tick;
	};

public:
	static c_client_state* get()
	{
		static const auto state = **reinterpret_cast<c_client_state***>(sig("engine.dll", "8B 3D ? ? ? ? 8A F9") + 2);
		return state;
	}

	void request_full_update()
	{
		delta_tick = -1;
	}

	char pad_0000[0x9C];
	c_net_channel* net_channel;
	uint32_t m_nChallengeNr;
	char pad_00A4[0x64];
	uint32_t m_nSignonState;
	char pad_010C[0x8];
	float m_flNextCmdTime;
	uint32_t m_nServerCount;
	uint32_t current_sequence;
	char pad_0120[4];
	c_clock_drift_mgr clock_drift_mgr;
	int32_t delta_tick;
	bool m_bPaused;
	char pad_0179[7];
	uint32_t m_nViewEntity;
	uint32_t m_nPlayerSlot;
	char m_szLevelName[260];
	char m_szLevelNameShort[40];
	char m_szGroupName[40];
	char pad_02DC[52];
	uint32_t m_nMaxClients;
	char pad_0314[18820];
	float m_flLastServerTickTime;
	bool in_simulation;
	char pad_4C9D[3];
	uint32_t oldtickcount;
	float m_tickRemainder;
	float m_frameTime;
	uint32_t last_command;
	uint32_t choked_commands;
	uint32_t last_command_ack;
	uint32_t command_ack;
	uint32_t m_nSoundSequence;
	char pad_4CC0[80];
	c_vector3d viewangles;
	char pad_4D14[0xD0];
	uint32_t events;
};

#define client_state c_client_state::get()
#define net_channel client_state->net_channel
