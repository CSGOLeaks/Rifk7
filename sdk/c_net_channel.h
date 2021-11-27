#pragma once

#include "macros.h"

#define flow_outgoing 0		
#define flow_incoming 1
#define net_frames_mask 63

class c_net_msg
{
public:
	virtual	~c_net_msg() = default;

	virtual void set_net_channel(void* netchan) = 0;
	virtual void set_reliable(bool state) = 0;
	virtual bool process() = 0;
	virtual	bool read_from_buffer(void* buffer) = 0;
	virtual	bool write_to_buffer(void* buffer) = 0;
	virtual bool is_reliable() const = 0;
	virtual int	get_type() const = 0;
	virtual int	get_group() const = 0;
	virtual const char* get_name() const = 0;
	virtual void* get_net_channel() const = 0;
	virtual const char* to_string() const = 0;
};

class c_net_channel {
public:
	static uint32_t* get_vtable()
	{
		
		static const auto table = reinterpret_cast<uint32_t>(sig("engine.dll",
			"68 ? ? ? ? C7 07")) + 7;
		return *reinterpret_cast<uint32_t**>(table);
	}

	vfunc(9, get_latency(const int flow), float(__thiscall*)(c_net_channel*, int))(flow)
	vfunc(40, send_netmsg(c_net_msg* msg, bool reliable, bool voice), bool(__thiscall*)(c_net_channel*, c_net_msg*, bool, bool))(msg, reliable, voice)
	vfunc(46, send_datagram(), int(__thiscall*)(c_net_channel*, void*))(nullptr)
	vfunc(59, has_pending_reliable_data(), bool(__thiscall*)(c_net_channel*))()

	char pad_0000[20];
	bool processing_messages;
	bool should_delete;
	char pad_0016[2];
	int out_sequence_nr;
	int in_sequence_nr;
	int out_sequence_nr_ack;
	int out_reliable_state;
	int in_reliable_state;
	int choked_packets;
	char pad_0030[1044];
};
