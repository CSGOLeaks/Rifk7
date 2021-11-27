#pragma once

#include "../includes.h"
#include "framework/c_flow_layout.h"

class c_menu : public c_singleton<c_menu>, public c_flow_layout
{
public:
	c_menu();

	void draw(c_vector2d position = c_vector2d()) override;
	std::optional<c_drawable*> get_foreground_element_by_position(c_vector2d position) override;

	void animate() override;

	void update_cursor_pos(uint32_t param);
	void register_click(uint32_t param);
	void register_unclick(uint32_t param);

	bool is_open() const;

private:
	static auto constexpr alpha_frequency = 255 / 0.15f;

	uint8_t welcome_placeholder;
	bool open;
	int alpha;
	c_vector2d cursor_pos;
	std::optional<c_drawable*> last_active;
	std::optional<c_drawable*> last_clicked;
};

#define menu c_menu::instance()
