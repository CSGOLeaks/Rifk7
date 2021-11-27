#pragma once

#include "class_id.h"
#include "macros.h"
#include "recv.h"

enum clientframestage {
	frame_undefined = -1,
	frame_start,
	frame_net_update_start,
	frame_net_update_postdataupdate_start,
	frame_net_update_postdataupdate_end,
	frame_net_update_end,
	frame_render_start,
	frame_render_end
};

class c_global_vars_base;

class c_client_class
{
public:
    void* create_fn;
    void* create_event_fn;
    char* network_name;
    recv_table* recv_table;
    c_client_class* next;
    class_id id;
};

class c_base_client
{
protected:
	~c_base_client() = default;
public:
	virtual int connect(void* factory, c_global_vars_base* globals) = 0;
	virtual int disconnect() = 0;
	virtual int init(void* factory, c_global_vars_base* globals) = 0;
	virtual void post_init() = 0;
	virtual void shutdown() = 0;
	virtual void level_init_pre_entity(char const* map_name) = 0;
	virtual void level_init_post_entity() = 0;
	virtual void level_shutdown() = 0;
	virtual c_client_class* get_all_classes() = 0;
};

interface_var(c_base_client, base_client, "client_panorama.dll", "VClient")
