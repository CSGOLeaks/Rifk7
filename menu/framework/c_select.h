#pragma once

#include <mutex>
#include "c_flow_layout.h"
#include "../../renderer/c_renderer.h"

class c_select : public c_flow_layout
{
public:
	explicit c_select(std::pair<std::string, char> title, std::vector<std::pair<std::string, char>> options,
					  int& selected, bool holdable = false, c_vector2d size = c_vector2d(90, 10));

	void draw(c_vector2d position) override;

	virtual void on_change() {}

protected:
	std::vector<std::pair<std::string, char>> options;

	// Don't ask.
	std::mutex select_mutex;

	int& selected;

private:
	static auto constexpr font_flags = c_font::font_flags::drop_shadow | c_font::font_flags::centered_x;

	bool holdable;
};
