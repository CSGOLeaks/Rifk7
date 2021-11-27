#pragma once

#include "c_drawable.h"

class c_flow_layout : public c_drawable
{
public:
	explicit c_flow_layout(c_vector2d size, c_vector2d padding = c_vector2d(0, 0), bool splitted = false);

	void draw(c_vector2d position) override;

	std::optional<c_drawable*> get_foreground_element_by_position(c_vector2d position) override;

	static void set_current_held(std::optional<c_drawable*> held);
protected:
	c_vector2d padding;
	bool splitted;

	static std::optional<c_drawable*> current_held;
	static float held_propagation_delay;
	static constexpr auto default_held_propagation_delay = .75f;
};
