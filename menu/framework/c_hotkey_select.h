#pragma once

#include "c_flow_layout.h"
#include "../../renderer/c_renderer.h"

class c_hotkey_select : public c_flow_layout, std::enable_shared_from_this<c_hotkey_select>
{
public:
	explicit c_hotkey_select(std::pair<std::string, char> title, int& selected);
	~c_hotkey_select();

	void draw(c_vector2d position) override;

protected:
	int& selected;

private:
	static auto constexpr font_flags = c_font::font_flags::drop_shadow | c_font::font_flags::centered_x;
};
