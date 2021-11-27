#pragma once

#include "c_text.h"

class c_button : public c_text
{
public:
	explicit c_button(std::pair<std::string, char> text, c_vector2d padding = c_vector2d(8, 2),
					  std::function<void()> on_click = []() -> void { }, std::function<void()> on_hold = []() -> void { });

	void draw(c_vector2d position) override;

	void animate() override;

	void on_click() override;
	void on_hold() override;

	c_vector2d get_size() override;

private:
	static auto constexpr button_flags = c_font::font_flags::drop_shadow | c_font::font_flags::centered_x | c_font::font_flags::centered_y;
	static constexpr auto fade_frequency = 255 / 0.15f;

	int fade;
	c_vector2d padding;
	std::function<void()> click;
	std::function<void()> hold;
};
