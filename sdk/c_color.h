#pragma once

#include <optional>
#include <d3d9.h>
#include "../libs/json.hpp"
#include "../security/string_obfuscation.h"

#define color(name, red, green, blue, alpha) static c_color name() { return c_color(red, green, blue, alpha); }
#define var_color(name, red, green, blue) static c_color name(int alpha) { return c_color(red, green, blue, alpha); }

class c_color
{
public:
	color(background, 33, 33, 33, 255)
	color(background_alternate, 55, 55, 55, 255)
	color(foreground, 238, 238, 238, 255)
	color(primary, 64, 190, 59, 255)
	color(accent, 122, 42, 151, 255)
	color(border, 186, 186, 186, 120)

	color(gradient1, 59, 177, 190, 255)
	color(gradient2, 190, 164, 59, 255)
	color(gradient3, 190, 59, 59, 255)
	color(gradient4, 217, 101, 2, 255)

	var_color(shadow, 0, 0, 0)
	color(none, 0, 0, 0, 0)

	int red, green, blue, alpha;

	c_color();
	c_color(int red, int green, int blue);
	c_color(int red, int green, int blue, int alpha);
	~c_color();

	D3DCOLOR direct() const;
	c_color fade_to(c_color color, float percent = 0.f, bool only_alpha = false) const;
	c_color adjust_hue(float delta) const;

	static int get_alpha_override();
	static void set_alpha_override(std::optional<int> alpha);

private:
	std::tuple<float, float, float> to_hsv() const;
	c_color from_hsv(std::tuple<float, float, float>& hsv) const;

	static std::optional<int> alpha_override;
};

inline void to_json(nlohmann::json& j, const c_color& value)
{
	j = nlohmann::json {
		{ _("red"), value.red },
		{ _("green"), value.green },
		{ _("blue"), value.blue },
		{ _("alpha"), value.alpha }
	};
}

inline void from_json(const nlohmann::json& j, c_color& value)
{
	value.red = j.at(_("red")).get<int>();
	value.green = j.at(_("green")).get<int>();
	value.blue = j.at(_("blue")).get<int>();
	value.alpha = j.at(_("alpha")).get<int>();
}

#undef color
