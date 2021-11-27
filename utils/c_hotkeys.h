#pragma once

#include "../includes.h"
#include "../menu/framework/c_hotkey_select.h"

class c_hotkeys : public c_singleton<c_hotkeys>
{
public:
	static std::pair<std::string, char> keycode_to_string(uint32_t code);

	c_hotkeys();

	void register_update(c_hotkey_select* selector, std::function<void(uint32_t)> update);
	void register_callback(std::function<void(uint32_t)> callback);
	void register_stop_callback(std::function<void(uint32_t)> callback);
	void clear_updates(c_hotkey_select* selector);
	void process_input(uint32_t code);
	void process_stop_input(uint32_t code);

private:
	std::unordered_map<c_hotkey_select*, std::function<void(uint32_t)>> updates;
	std::vector<std::function<void(uint32_t)>> callbacks, stop_callbacks;
};

#define hotkeys c_hotkeys::instance()
