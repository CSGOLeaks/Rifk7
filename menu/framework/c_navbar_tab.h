#pragma once

#include "c_text.h"
#include "../../renderer/c_font.h"

class c_navbar_tab : public c_text
{
public:
	explicit c_navbar_tab(std::pair<std::string, char> text, std::function<void()> activate,
						  bool first = false, bool last = false, c_vector2d size = c_vector2d(100, 40));

	void draw(c_vector2d position) override;

	void animate() override;

	void on_click() override;

	void set_active(bool is_active);
private:
	static constexpr auto flags = c_font::font_flags::centered_x | c_font::font_flags::centered_y | c_font::font_flags::drop_shadow;

	static auto constexpr highlight_frequency = 255 / 0.15f;

	std::function<void()> activate;

	int32_t highlight;

	bool first, last, is_active;
};
