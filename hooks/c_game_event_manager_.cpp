#include "c_game_event_manager_.h"
#include "../utils/c_hook.h"
#include "../sdk/c_cs_player.h"
#include "c_cs_player_.h"
#include <future>

void c_game_event_manager_::hook()
{
	static c_hook<c_game_event_manager> hook(game_event_manager());
	_add_listener = hook.apply<add_listener_t>(3, add_listener);
}

bool c_game_event_manager_::add_listener(c_game_event_listener* listener, const char* name, const bool serverside)
{
	static const auto return_to_c_csplayer_ctor = reinterpret_cast<void*>(
		reinterpret_cast<uint32_t>(sig("client_panorama.dll", "FF 50 0C C7 87")) + 3);
	
	if (_ReturnAddress() == return_to_c_csplayer_ctor)
	{
		const auto player = reinterpret_cast<c_cs_player*>(
			reinterpret_cast<uint32_t>(listener) - 0x3870);

		c_cs_player_::apply_to_player(player);
	}

	return _add_listener(game_event_manager(), listener, name, serverside);
}
