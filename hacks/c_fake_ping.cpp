#include "c_fake_ping.h"
#include "../sdk/c_client_state.h"
#include "../utils/math.h"
#include "../sdk/c_global_vars_base.h"
#include "../utils/c_config.h"
#include "../utils/c_hotkeys.h"
#include "../menu/c_menu.h"
#include "c_animation_system.h"

c_fake_ping::c_fake_ping()
{
	hotkeys->register_callback([&](const uint32_t code) -> void
	{
		if (code != 0 && code == config.misc.ping_switch
			&& config.misc.spike_ping && !menu->is_open())
			in_ping_spike = !in_ping_spike;
	});
}

void c_fake_ping::set_suitable_in_sequence(c_net_channel* channel)
{
	if (flipped_state)
	{
		flipped_state = false;
		return;
	}
	
	const auto spike = time_to_ticks(calculate_wanted_ping(channel));
	if (channel->in_sequence_nr > spike)
		channel->in_sequence_nr -= spike;
}

float c_fake_ping::calculate_wanted_ping(c_net_channel* channel)
{
	auto wanted_ping = 0.f;

	if (in_ping_spike)
		wanted_ping = config.misc.spike_ping / 1000.f;
	else if (config.rage.enabled)
		wanted_ping = 200.f / 1000.f - calculate_lerp();
	else
		return 0.f;

	return std::max(0.f, wanted_ping - channel->get_latency(flow_outgoing) * 2.f);
}

void c_fake_ping::flip_state(c_net_channel* channel)
{
	static auto last_reliable_state = -1;

	if (channel->in_reliable_state != last_reliable_state)
		flipped_state = true;

	last_reliable_state = channel->in_reliable_state;
}
