
#include "c_visuals.h"
#include "framework/c_group_box.h"
#include "framework/c_checkbox.h"
#include "framework/c_button.h"
#include "framework/c_color_select.h"
#include "framework/c_select.h"
#include "framework/c_float_select.h"
#include "../security/string_obfuscation.h"
#include "../utils/c_config.h"
#include "../utils/c_log.h"

c_visuals::c_visuals(const c_vector2d size) : c_flow_layout(size, c_vector2d(12, 12))
{
	add_child(std::make_shared<c_group_box>(__("Team"), std::initializer_list<std::shared_ptr<c_drawable>> {
		make_child(c_checkbox, __("ESP Box"), config.esp.team.box),
		make_child(c_select, __("ESP Bar"), std::vector<std::pair<std::string, char>> {
			__("Off"),
			__("Health"),
			__("Full"),
		}, config.esp.team.bar),
		make_child(c_checkbox, __("ESP Info"), config.esp.team.info),
		make_child(c_select, __("ESP Skeleton"), std::vector<std::pair<std::string, char>> {
			__("Off"),
			__("Classic"),
			__("Retro"),
		}, config.esp.team.skeleton),
		make_child(c_select, __("ESP Radar"), std::vector<std::pair<std::string, char>> {
			__("Off"),
			__("Out-of-fov"),
			__("Always on"),
		}, config.esp.team.radar),
		make_child(c_color_select, __("ESP Color"), config.esp.team.color),
		make_child(c_select, __("Chams Style"), std::vector<std::pair<std::string, char>> {
			__("Off"),
			__("Oldschool"),
			__("Material"),
			__("Classic"),
			__("Retro")
		}, config.chams.team.type),
		make_child(c_color_select, __("Chams Color"), config.chams.team.color),
		make_child(c_checkbox, __("Chams XQZ"), config.chams.team.xqz)
	}));
	add_child(std::make_shared<c_group_box>(__("Enemy"), std::initializer_list<std::shared_ptr<c_drawable>> {
		make_child(c_checkbox, __("ESP Box"), config.esp.enemy.box),
		make_child(c_select, __("ESP Bar"), std::vector<std::pair<std::string, char>> {
			__("Off"),
			__("Health"),
			__("Full"),
		}, config.esp.enemy.bar),
		make_child(c_checkbox, __("ESP Info"), config.esp.enemy.info),
		make_child(c_select, __("ESP Skeleton"), std::vector<std::pair<std::string, char>> {
			__("Off"),
			__("Classic"),
			__("Retro"),
		}, config.esp.enemy.skeleton),
		make_child(c_select, __("ESP Radar"), std::vector<std::pair<std::string, char>> {
			__("Off"),
			__("Out-of-fov"),
			__("Always on"),
		}, config.esp.enemy.radar),
		make_child(c_color_select, __("ESP Color"), config.esp.enemy.color),
		make_child(c_select, __("Chams Style"), std::vector<std::pair<std::string, char>> {
			__("Off"),
			__("Oldschool"),
			__("Material"),
			__("Classic"),
			__("Retro")
		}, config.chams.enemy.type),
		make_child(c_color_select, __("Chams Color"), config.chams.enemy.color),
		make_child(c_checkbox, __("Chams XQZ"), config.chams.enemy.xqz)
	}));
	add_child(std::make_shared<c_group_box>(__("World"), std::initializer_list<std::shared_ptr<c_drawable>> {
		make_child(c_select, __("Backtrack Style"), std::vector<std::pair<std::string, char>> {
			__("Off"),
			__("Oldschool"),
			__("Material"),
			__("Classic"),
			__("Retro")
		}, config.chams.backtrack.type),
		make_child(c_color_select, __("Backtrack Color"), config.chams.backtrack.color),
		make_child(c_checkbox, __("Backtrack XQZ"), config.chams.backtrack.xqz),
		make_child(c_checkbox, __("Local Impacts"), config.esp.team.impacts),
		make_child(c_color_select, __("Local Impacts Color"), config.esp.team.impacts_color),
		make_child(c_checkbox, __("Enemy Impacts"), config.esp.enemy.impacts),
		make_child(c_color_select, __("Enemy Impacts Color"), config.esp.enemy.impacts_color),
		make_child(c_select, __("Grenade ESP"), std::vector<std::pair<std::string, char>> {
			__("Off"),
			__("Text"),
			__("Retro")
		}, config.esp.nade_type),
		make_child(c_color_select, __("Grenade ESP Color"), config.esp.nade_color)
	}));
	add_child(std::make_shared<c_group_box>(__("Misc"), std::initializer_list<std::shared_ptr<c_drawable>> {
		make_child(c_select, __("Local Chams Style"), std::vector<std::pair<std::string, char>> {
			__("Off"),
			__("Oldschool"),
			__("Material"),
			__("Classic"),
			__("Retro")
		}, config.chams.local.type),
		make_child(c_color_select, __("Local Chams Color"), config.chams.local.color),
		make_child(c_select, __("Arm Chams Style"), std::vector<std::pair<std::string, char>> {
			__("Off"),
			__("Oldschool"),
			__("Classic"),
			__("Retro"),
			__("Ultimate")
		}, config.chams.arms.type),
		make_child(c_color_select, __("Arm Chams Color"), config.chams.arms.color),
		make_child(c_select, __("Weapon Chams Style"), std::vector<std::pair<std::string, char>> {
			__("Off"),
			__("Oldschool"),
			__("Classic"),
			__("Retro"),
			__("Ultimate")
		}, config.chams.weapon.type),
		make_child(c_color_select, __("Weapon Chams Color"), config.chams.weapon.color),
		make_child(c_checkbox, __("Remove sleeves"), config.chams.remove_sleeves),
		make_child(c_checkbox, __("Hitmarker"), config.esp.hitmarker),
		make_child(c_select, __("Hitsound"), std::vector<std::pair<std::string, char>> {
			__("Off"),
			__("COD"),
			__("UT"),
			__("Quake"),
			__("Roblox"),
			__("Uff"),
			__("Laser")
		}, config.esp.hitsound),
		make_child(c_float_select, __("Hitsound volume"), config.esp.hitsound_volume, 100.f, 0.f, 2.f)
	}));
}
