#pragma once

#include "c_flow_layout.h"
#include "../../sdk/c_color.h"

class c_color_select : public c_flow_layout
{
	class c_color_select_preview : public c_drawable
	{
	public:
		c_color_select_preview(c_color& color, c_vector2d size);

		void draw(c_vector2d position) override;

	private:
		c_color& color;
	};

public:
	explicit c_color_select(std::pair<std::string, char> title, c_color& color);

private:
	static constexpr auto speed = .5f;
};
