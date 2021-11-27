#pragma once

class c_global_vars_base {
public:
	static c_global_vars_base* get();

	float realtime;
	int framecount;
	float absoluteframetime;
	float absoluteframestarttimestddev;
	float curtime;
	float frametime;
	int max_clients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
	int sim_ticks_this_frame;
	int network_protocol;
	void* save_data;
	bool client;
	bool remote_client;
};

#define global_vars_base c_global_vars_base::get()
