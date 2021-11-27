#include "c_conf.h"
#include "../c_rifk.h"
#include "../security/string_obfuscation.h"
#include "framework/c_group_box.h"
#include "framework/c_button.h"
#include "../utils/c_config.h"

c_conf::c_conf(const c_vector2d size) : c_flow_layout(size, c_vector2d(12, 12))
{
	static const auto legit = __("%s\\legit.cfg");
	static const auto rage = __("%s\\rage.cfg");

	add_child(std::make_shared<c_group_box>(__("Rage"), std::initializer_list<std::shared_ptr<c_drawable>> {
		make_child(c_button, __("Load Rage"), c_vector2d(8, 4), []() -> void
		{
			_rt(ra, rage);
			char path[MAX_PATH];
			sprintf_s(path, ra, loader_info.loader_path);
			c_config::load_from_file(path);
		}),
			make_child(c_button, __("Save Rage"), c_vector2d(8, 4), []() -> void
		{
			_rt(ra, rage);
			char path[MAX_PATH];
			sprintf_s(path, ra, loader_info.loader_path);
			c_config::save_to_file(path);
		})
	}));

	add_child(std::make_shared<c_group_box>(__("Legit"), std::initializer_list<std::shared_ptr<c_drawable>> {
			make_child(c_button, __("Load Legit"), c_vector2d(8, 4), []() -> void
		{
			_rt(le, legit);
			char path[MAX_PATH];
			sprintf_s(path, le, loader_info.loader_path);
			c_config::load_from_file(path);
		}),
			make_child(c_button, __("Save Legit"), c_vector2d(8, 4), []() -> void
		{
			_rt(le, legit);
			char path[MAX_PATH];
			sprintf_s(path, le, loader_info.loader_path);
			c_config::save_to_file(path);
		})
	}));
}
