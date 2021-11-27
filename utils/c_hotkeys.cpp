#include "c_hotkeys.h"

std::pair<std::string, char> c_hotkeys::keycode_to_string(const uint32_t code)
{
	auto scanCode = MapVirtualKeyA(code, MAPVK_VK_TO_VSC);
	CHAR name[128];
	
	switch (code) {
	case 0:
		return __("< None >");
	case VK_XBUTTON1:
		return __("< Mouse 5 >");
	case VK_XBUTTON2:
		return __("< Mouse 4 >");
	case VK_MBUTTON:
		return __("< Mouse 3 >");
	case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
	case VK_PRIOR: case VK_NEXT:
	case VK_END: case VK_HOME:
	case VK_INSERT: case VK_DELETE:
	case VK_DIVIDE:
	case VK_NUMLOCK:
		scanCode |= KF_EXTENDED;
	default:
		GetKeyNameTextA(scanCode << 16, name, 128);
	}

	constexpr auto key = random::_char<__COUNTER__>::value;

	auto str = "< " + std::string(name) + " >";
	
	for (auto& c : str)
		c ^= key;

	return std::make_pair(str, key);
}

c_hotkeys::c_hotkeys() = default;

void c_hotkeys::register_update(c_hotkey_select* selector, std::function<void(uint32_t)> update)
{
	updates.insert_or_assign(selector, update);
}

void c_hotkeys::register_callback(const std::function<void(uint32_t)> callback)
{
	callbacks.push_back(callback);
}

void c_hotkeys::register_stop_callback(const std::function<void(uint32_t)> callback)
{
	stop_callbacks.push_back(callback);
}

void c_hotkeys::clear_updates(c_hotkey_select* selector)
{
	updates.erase(selector);
}

void c_hotkeys::process_input(const uint32_t code)
{
	for (const auto& update : updates)
		update.second(code);

	for (const auto& callback : callbacks)
		callback(code);

	updates.clear();
}

void c_hotkeys::process_stop_input(const uint32_t code)
{
	for (const auto& callback : stop_callbacks)
		callback(code);
}
