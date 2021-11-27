#pragma once

#include "c_flow_layout.h"
#include "../../renderer/c_font.h"

class c_group_box : public c_flow_layout
{
public:
	explicit c_group_box(std::pair<std::string, char> title, std::initializer_list<std::shared_ptr<c_drawable>> children = {},
						 c_vector2d size = c_vector2d(275, 174));

	void draw(c_vector2d position) override;
	c_vector2d get_size() override;

private:
	static constexpr auto title_flags = c_font::font_flags::centered_x | c_font::font_flags::centered_y | c_font::font_flags::drop_shadow;
};
