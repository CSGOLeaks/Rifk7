#pragma once

#include "c_singleton.h"
#include "../security/fnv1a.h"
#include "../security/string_obfuscation.h"
#include "../sdk/c_color.h"
#include "base64.h"
#include <fstream>

using json = nlohmann::json;

#define make_key(var) std::to_string(fnv1a(var))

class c_config : public c_singleton<c_config>
{
public:
	struct config_conf
	{
		struct esp_conf
		{
			struct player
			{
				c_color color, impacts_color;
				bool box, info, impacts;
				int radar, bar, skeleton;
			};

			player team, enemy;

			int nade_type;
			c_color nade_color;

			bool hitmarker;
			int hitsound;
			float hitsound_volume;
		};

		struct chams_conf
		{
			struct chams
			{
				int type;
				c_color color;
				bool xqz;
			};

			chams team, enemy, backtrack, local, arms, weapon;
			bool remove_sleeves;
		};

		struct legit_conf
		{
			bool assist;
			float fov, smooth, backtrack;
			bool only_backtrack;
		};

		struct rage_conf
		{
			struct weapon_conf
			{
				float hitchance, min_dmg,
					head_scale, body_scale;
			};

			bool enabled, experimental_aa, infinite_duck;
			int slow_walk;
			weapon_conf auto_snipe, scout, awp,
				pistol, heavy, misc;
		};

		struct misc_conf
		{
			int radio_channel;
			float radio_volume;
			int radio_mute;

			float spike_ping;
			int ping_switch;

			float thirdperson_dist;
			int thirdperson_switch;
			int buy_bot;

			bool engine_radar, no_smoke, no_flash, no_scope, indicators, arrows;

			float fov, fov_view;
		};

		esp_conf esp;
		chams_conf chams;
		legit_conf legit;
		rage_conf rage;
		misc_conf misc;
	};

	c_config() = default;

	config_conf& get()
	{
		static config_conf config {
			// esp
			{
				// esp.team
				{
					c_color(120, 120, 255, 255),
					c_color(120, 120, 255, 255),
					false,
					false,
					false,
					0,
					0,
					0
				},
				// esp.enemy
				{
					c_color(230, 70, 140, 255),
					c_color(230, 70, 140, 255),
					false,
					false,
					false,
					0,
					0,
					0
				},
				0,
				c_color::primary(),
				false,
				0,
				100.f
			},
			// chams
			{
				// chams.team
				{
					0,
					c_color(120, 120, 255, 255),
					false
				},
				// chams.enemy
				{
					0,
					c_color(230, 70, 140, 255),
					false
				},
				// chams.backtrack
				{
					0,
					c_color(70, 225, 230, 255),
					false
				},
				// chams.local
				{
					0,
					c_color::gradient2()
				},
				// chams.arms
				{
					0,
					c_color::primary()
				},
				// chams.weapons
				{
					0,
					c_color::accent()
				},
				false
			},
			// legit
			{
				false,
				0.f,
				0.f,
				0.f
			},
			// rage
			{
				false,
				false,
				false,
				0,
				// rage.auto_snipe
				{
					0.f,
					0.f,
					20.f,
					20.f
				},
				// rage.scout
				{
					0.f,
					0.f,
					20.f,
					20.f
				},
				// rage.awp
				{
					0.f,
					0.f,
					20.f,
					20.f
				},
				// rage.pistol
				{
					0.f,
					0.f,
					20.f,
					20.f
				},
				// rage.heavy
				{
					0.f,
					0.f,
					20.f,
					20.f
				},
				// rage.misc
				{
					0.f,
					0.f,
					20.f,
					20.f
				}
			},
			// misc
			{
				0,
				100.f,
				0,

				0.f,
				0,

				0.f,
				0,

				false,
				false,
				false,
				false,
				false,
				false,

				0,
				50.f,
				50.f
			}
		};

		return config;
	}

	static void load_from_file(const std::string& config_path);
	static void save_to_file(const std::string& config_path);
};

inline void to_json(nlohmann::json& j, const c_config::config_conf::esp_conf::player& value)
{
	j = json {
		{ _("color"), value.color },
		{ _("impacts_color"), value.impacts_color },
		{ _("box"), value.box },
		{ _("info"), value.info },
		{ _("impacts"), value.impacts },
		{ _("radar"), value.radar },
		{ _("bar"), value.bar },
		{ _("skeleton"), value.skeleton },
	};
}

inline void from_json(const nlohmann::json& j, c_config::config_conf::esp_conf::player& value)
{
	value.color = j.value(_("color"), c_color(120, 120, 255, 255));
	value.impacts_color = j.value(_("impacts_color"), c_color(120, 120, 255, 255));
	value.box = j.value(_("box"), false);
	value.info = j.value(_("info"), false);
	value.impacts = j.value(_("impacts"), false);
	value.radar = j.value(_("radar"), 0);
	value.bar = j.value(_("bar"), 0);
	value.skeleton = j.value(_("skeleton"), 0);
}

inline void to_json(nlohmann::json& j, const c_config::config_conf::esp_conf& value)
{
	j = json {
		{ _("team"), value.team },
		{ _("enemy"), value.enemy },
		{ _("nade_type"), value.nade_type },
		{ _("nade_color"), value.nade_color },
		{ _("hitmarker"), value.hitmarker },
		{ _("hitsound"), value.hitsound },
		{ _("hitsound_volume"), value.hitsound_volume }
	};
}

inline void from_json(const nlohmann::json& j, c_config::config_conf::esp_conf& value)
{
	value.team = j.value(_("team"), c_config::config_conf::esp_conf::player{});
	value.enemy = j.value(_("enemy"), c_config::config_conf::esp_conf::player{});
	value.nade_type = j.value(_("nade_type"), 0);
	value.nade_color = j.value(_("nade_color"), c_color::primary());
	value.hitmarker = j.value(_("hitmarker"), false);
	value.hitsound = j.value(_("hitsound"), 0);
	value.hitsound_volume = j.value(_("hitsound_volume"), 100.f);
}

inline void to_json(nlohmann::json& j, const c_config::config_conf::chams_conf::chams& value)
{
	j = json {
		{ _("type"), value.type },
		{ _("color"), value.color },
		{ _("xqz"), value.xqz }
	};
}

inline void from_json(const nlohmann::json& j, c_config::config_conf::chams_conf::chams& value)
{
	value.type = j.value(_("type"), 0);
	value.color = j.value(_("color"), c_color(120, 120, 255, 255));
	value.xqz = j.value(_("xqz"), false);
}

inline void to_json(nlohmann::json& j, const c_config::config_conf::chams_conf& value)
{
	j = json {
		{ _("team"), value.team },
		{ _("enemy"), value.enemy },
		{ _("backtrack"), value.backtrack },
		{ _("local"), value.local },
		{ _("arms"), value.arms },
		{ _("weapon"), value.weapon },
		{ _("remove_sleeves"), value.remove_sleeves }
	};
}

inline void from_json(const nlohmann::json& j, c_config::config_conf::chams_conf& value)
{
	value.team = j.value(_("team"), c_config::config_conf::chams_conf::chams{});
	value.enemy = j.value(_("enemy"), c_config::config_conf::chams_conf::chams{});
	value.backtrack = j.value(_("backtrack"), c_config::config_conf::chams_conf::chams{});
	value.local = j.value(_("local"), c_config::config_conf::chams_conf::chams{});
	value.arms = j.value(_("arms"), c_config::config_conf::chams_conf::chams{});
	value.weapon = j.value(_("weapon"), c_config::config_conf::chams_conf::chams{});
	value.remove_sleeves = j.value(_("remove_sleeves"), false);
}

inline void to_json(nlohmann::json& j, const c_config::config_conf::legit_conf& value)
{
	j = json {
		{ _("assist"), value.assist },
		{ _("fov"), value.fov },
		{ _("smooth"), value.smooth },
		{ _("backtrack"), value.backtrack }
	};
}

inline void from_json(const nlohmann::json& j, c_config::config_conf::legit_conf& value)
{
	value.assist = j.value(_("assist"), false);
	value.fov = j.value(_("fov"), 0.f);
	value.smooth = j.value(_("smooth"), 0.f);
	value.backtrack = j.value(_("backtrack"), 0.f);
}

inline void to_json(nlohmann::json& j, const c_config::config_conf::rage_conf::weapon_conf& value)
{
	j = json {
		{ _("hitchance"), value.hitchance },
		{ _("min_dmg"), value.min_dmg },
		{ _("head_scale"), value.head_scale },
		{ _("body_scale"), value.body_scale }
	};
}

inline void from_json(const nlohmann::json& j, c_config::config_conf::rage_conf::weapon_conf& value)
{
	value.hitchance = j.value(_("hitchance"), 0.f);
	value.min_dmg = j.value(_("min_dmg"), 0.f);
	value.head_scale = j.value(_("head_scale"), 20.f);
	value.body_scale = j.value(_("body_scale"), 20.f);
}

inline void to_json(nlohmann::json& j, const c_config::config_conf::rage_conf& value)
{
	j = json {
		{ _("enabled"), value.enabled },
		{ _("experimental_aa"), value.experimental_aa },
		{ _("infinite_duck"), value.infinite_duck },
		{ _("slow_walk"), value.slow_walk },
		{ _("auto_snipe"), value.auto_snipe },
		{ _("scout"), value.scout },
		{ _("awp"), value.awp },
		{ _("pistol"), value.pistol },
		{ _("heavy"), value.heavy },
		{ _("misc"), value.misc }
	};
}

inline void from_json(const nlohmann::json& j, c_config::config_conf::rage_conf& value)
{
	value.enabled = j.value(_("enabled"), false);
	value.experimental_aa = j.value(_("experimental_aa"), false);
	value.infinite_duck = j.value(_("infinite_duck"), false);
	value.slow_walk = j.value(_("slow_walk"), 0);
	value.auto_snipe = j.value(_("auto_snipe"), c_config::config_conf::rage_conf::weapon_conf{});
	value.scout = j.value(_("scout"), c_config::config_conf::rage_conf::weapon_conf{});
	value.awp = j.value(_("awp"), c_config::config_conf::rage_conf::weapon_conf{});
	value.pistol = j.value(_("pistol"), c_config::config_conf::rage_conf::weapon_conf{});
	value.heavy = j.value(_("heavy"), c_config::config_conf::rage_conf::weapon_conf{});
	value.misc = j.value(_("misc"), c_config::config_conf::rage_conf::weapon_conf{});
}

inline void to_json(nlohmann::json& j, const c_config::config_conf::misc_conf& value)
{
	j = json {
		{ _("radio_channel"), value.radio_channel },
		{ _("radio_volume"), value.radio_volume },
		{ _("radio_mute"), value.radio_mute },

		{ _("spike_ping"), value.spike_ping },
		{ _("ping_switch"), value.ping_switch },

		{ _("thirdperson_dist"), value.thirdperson_dist },
		{ _("thirdperson_switch"), value.thirdperson_switch },

		{ _("buy_bot"), value.buy_bot },

		{ _("engine_radar"), value.engine_radar },
		{ _("no_smoke"), value.no_smoke },
		{ _("no_flash"), value.no_flash },
		{ _("no_scope"), value.no_scope },
		{ _("indicators"), value.indicators },
		{ _("arrows"), value.arrows },

		{ _("fov"), value.fov },
		{ _("fov_view"), value.fov_view }
	};
}

inline void from_json(const nlohmann::json& j, c_config::config_conf::misc_conf& value)
{
	value.radio_channel = j.value(_("radio_channel"), 0);
	value.radio_volume = j.value(_("radio_volume"), 100.f);
	value.radio_mute = j.value(_("radio_mute"), 0);

	value.spike_ping = j.value(_("spike_ping"), 0.f);
	value.ping_switch = j.value(_("ping_switch"), 0);

	value.thirdperson_dist = j.value(_("thirdperson_dist"), 0.f);
	value.thirdperson_switch = j.value(_("thirdperson_switch"), 0);

	value.buy_bot = j.value(_("buy_bot"), 0);

	value.engine_radar = j.value(_("engine_radar"), 0);
	value.no_smoke = j.value(_("no_smoke"), 0);
	value.no_flash = j.value(_("no_flash"), 0);
	value.no_scope = j.value(_("no_scope"), 0);
	value.indicators = j.value(_("indicators"), 0);
	value.arrows = j.value(_("arrows"), 0);

	value.fov = j.value(_("fov"), 50.f);
	value.fov_view = j.value(_("fov_view"), 50.f);
}

inline void to_json(nlohmann::json& j, const c_config::config_conf& value)
{
	j = json {
		{ _("esp"), value.esp },
		{ _("chams"), value.chams },
		{ _("legit"), value.legit },
		{ _("rage"), value.rage },
		{ _("misc"), value.misc }
	};
}

inline void from_json(const nlohmann::json& j, c_config::config_conf& value)
{
	value.esp = j.value(_("esp"), c_config::config_conf::esp_conf{});
	value.chams = j.value(_("chams"), c_config::config_conf::chams_conf{});
	value.legit = j.value(_("legit"), c_config::config_conf::legit_conf{});
	value.rage = j.value(_("rage"), c_config::config_conf::rage_conf{});
	value.misc = j.value(_("misc"), c_config::config_conf::misc_conf{});
}

inline void c_config::load_from_file(const std::string& config_path)
{
	std::ifstream file(config_path);
	std::stringstream rage;
	rage << file.rdbuf();

	if (rage.str().empty())
		return;

	auto cfg = base64::decode(rage.str());
	from_json(json::parse(cfg), instance()->get());
}

inline void c_config::save_to_file(const std::string& config_path)
{
	json exp;
	to_json(exp, instance()->get());
	const auto str = base64::encode(exp.dump());

	std::ofstream file(config_path);
	file << str;
}

#define config c_config::instance()->get()
