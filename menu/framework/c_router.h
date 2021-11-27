#pragma once

#include "c_flow_layout.h"

class c_router : public c_flow_layout
{
public:
	explicit c_router(std::initializer_list<std::shared_ptr<c_drawable>> children, uint8_t& active_item, bool primary = false,
					  c_vector2d padding = c_vector2d(20, 20), c_vector2d size = c_vector2d(600, 400));

	void draw(c_vector2d position) override;
	void animate() override;

	std::optional<c_drawable*> get_foreground_element_by_position(c_vector2d position) override;

private:
	static auto constexpr fade_frequency = 1000 / 12.f;

	uint8_t& active_item;
	c_vector2d padding;
	bool primary;
	int fade{};
};
