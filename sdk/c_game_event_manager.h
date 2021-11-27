#pragma once

#include <cstdint>
#include "macros.h"

class c_game_event
{
public:
	virtual ~c_game_event() = default;
	virtual const char* get_name() const = 0;

	virtual bool is_reliable() const = 0;
	virtual bool is_local() const = 0;
	virtual bool is_empty(const char* keyname = nullptr) = 0;

	virtual bool get_bool(const char* keyname = nullptr, bool default_value = false) = 0;
	virtual int get_int(const char* keyname = nullptr, int default_value = 0) = 0;
	virtual uint64_t get_uint64(const char* keyname = nullptr, uint64_t default_value = 0) = 0;
	virtual float get_float(const char* keyname = nullptr, float default_value = 0.0f) = 0;
	virtual const char* get_string(const char* keyname = nullptr, const char* default_value = _("")) = 0;
};

class c_game_event_listener
{
public:
	virtual ~c_game_event_listener() = default;

	virtual void fire_game_event(c_game_event *event) = 0;
	virtual int get_event_debug_id() = 0;
};

class c_game_event_manager
{
public:
	virtual ~c_game_event_manager() = 0;
	virtual int load_events_from_file(const char* filename) = 0;
	virtual void reset() = 0;
	virtual bool add_listener(c_game_event_listener* listener, const char* name, bool serverside) = 0;
	virtual bool find_listener(c_game_event_listener* listener, const char* name) = 0;
	virtual void remove_listener(c_game_event_listener* listener) = 0;
	virtual void add_listener_global(c_game_event_listener* listener, bool serverside) = 0;
};

interface_var(c_game_event_manager, game_event_manager, "engine.dll", "GAMEEVENTSMANAGER")
