#pragma once

#include "c_drawable.h"
#include "../../sdk/c_color.h"
#include "../../renderer/c_font.h"
#include "../../security/fnv1a.h"
#include <string>

class c_text : public c_drawable
{
public:
	explicit c_text(std::pair<std::string, char> text, c_color color,
					uint32_t font = fnv1a("pro13"), uint8_t flags = c_font::font_flags::drop_shadow, bool centered = false);

	void draw(c_vector2d position) override;

	void set_text(std::pair<std::string, char> text);
	void set_color(c_color color);

protected:
	std::pair<std::string, char> text;
	c_color color;
	uint32_t font;
	uint8_t flags;
	bool centered;
};
