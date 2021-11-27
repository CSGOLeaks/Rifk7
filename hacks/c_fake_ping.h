#pragma once

#include <cstdint>
#include "../sdk/c_net_channel.h"

class c_fake_ping : public c_singleton<c_fake_ping>
{
public:
	c_fake_ping();

	void set_suitable_in_sequence(c_net_channel* channel);
	float calculate_wanted_ping(c_net_channel* channel);
	void flip_state(c_net_channel* channel);

private:
	bool in_ping_spike{}, flipped_state{};
};

#define fake_ping c_fake_ping::instance()
