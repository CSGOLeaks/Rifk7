#pragma once

#include "c_flow_layout.h"
#include "../../renderer/c_font.h"

class c_checkbox : public c_flow_layout
{
	class c_checkbox_inner : public c_drawable
	{
	public:
		explicit c_checkbox_inner(bool& value);

		void draw(c_vector2d position) override;

		void animate() override;

		void on_click() override;

	private:
		static constexpr auto fade_frequency = 255 / 0.15f;
		static constexpr auto enabled_frequency = 255 / 0.15f;

		int fade, enabled;
		bool& value;
	};

public:
	explicit c_checkbox(std::pair<std::string, char> title, bool& value);
};
